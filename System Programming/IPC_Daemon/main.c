#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#define FIFO1_PATH "./fifo1"
#define FIFO2_PATH "./fifo2"
#define COMMAND "FIND_LARGER"
#define LOG_FILE "./daemon.log"
#define CHILD_TIMEOUT 30 

#define NUM_CHILDREN 2

int child_counter = 0;
int daemon_running = 0;
int log_fd = -1;
pid_t child_pids[NUM_CHILDREN] = {0};
time_t child_start_times[NUM_CHILDREN] = {0};
int child_active[NUM_CHILDREN] = {0};

void cleanup_resources();

void error_handler(const char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void log_event(const char* event_type, const char* message) {
    time_t now;
    struct tm *time_info;
    char timestamp[30];
    char log_message[512];
    
    time(&now);
    time_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time_info);
    
    snprintf(log_message, sizeof(log_message), "[%s] %s: %s\n", timestamp, event_type, message);
    
    if (log_fd != -1) {
        write(log_fd, log_message, strlen(log_message));
    }
}

void redirect_std_fds() {
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    int fd = open("/dev/null", O_RDWR);
    if (fd != STDIN_FILENO) {
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    
    if (log_fd != -1) {
        dup2(log_fd, STDOUT_FILENO);
        dup2(log_fd, STDERR_FILENO);
    }
    
    log_event("DAEMON", "Standard file descriptors redirected");
}

void setup_daemon_logger() {
    log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (log_fd == -1) {
        error_handler("Failed to open log file");
    }
    
    log_event("DAEMON", "Logger initialized");
}

void setup_nonblocking_fifo(int fd) {
    int flags;
    
    flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        log_event("ERROR", "Failed to get file descriptor flags");
        return;
    }
    
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        log_event("ERROR", "Failed to set non-blocking mode");
        return;
    }
    
    log_event("DAEMON", "FIFO set to non-blocking mode");
}

void handle_sigusr1(int sig) {
    char log_msg[100];
    
    log_event("SIGNAL", "Received SIGUSR1 signal");
    
    for (int i = 0; i < NUM_CHILDREN; i++) {
        if (child_pids[i] > 0) {
            snprintf(log_msg, sizeof(log_msg), "Child %d (PID: %d) is %s", 
                    i+1, child_pids[i], child_active[i] ? "active" : "inactive");
            log_event("SIGUSR1", log_msg);
        }
    }
}

void handle_sighup(int sig) {
    log_event("SIGNAL", "Received SIGHUP signal, reloading configuration");
    
    if (log_fd != -1) {
        close(log_fd);
        
        log_fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
        if (log_fd == -1) {
            fprintf(stderr, "Failed to reopen log file after SIGHUP\n");
            exit(EXIT_FAILURE);
        }
        
        log_event("SIGHUP", "Log file reopened successfully");
    }
}

void handle_sigterm(int sig) {
    log_event("SIGNAL", "Received SIGTERM signal, shutting down gracefully");
    
    for (int i = 0; i < NUM_CHILDREN; i++) {
        if (child_pids[i] > 0) {
            log_event("SIGTERM", "Sending SIGTERM to child process");
            kill(child_pids[i], SIGTERM);
        }
    }
    
    cleanup_resources();
    
    exit(EXIT_SUCCESS);
}

void setup_daemon_signals() {
    struct sigaction sa;
    
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        log_event("ERROR", "Failed to set SIGUSR1 handler");
        exit(EXIT_FAILURE);
    }
    
    sa.sa_handler = handle_sighup;
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        log_event("ERROR", "Failed to set SIGHUP handler");
        exit(EXIT_FAILURE);
    }
    
    sa.sa_handler = handle_sigterm;
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        log_event("ERROR", "Failed to set SIGTERM handler");
        exit(EXIT_FAILURE);
    }
    
    log_event("DAEMON", "Signal handlers set up successfully");
}

void monitor_child_processes() {
    time_t now;
    char log_msg[100];
    
    time(&now);
    
    for (int i = 0; i < NUM_CHILDREN; i++) {
        if (child_pids[i] > 0 && child_active[i]) {
            time_t elapsed = now - child_start_times[i];
            
            if (elapsed > CHILD_TIMEOUT) {
                snprintf(log_msg, sizeof(log_msg), "Child %d (PID: %d) exceeded timeout (%d seconds), terminating", 
                        i+1, child_pids[i], CHILD_TIMEOUT);
                log_event("MONITOR", log_msg);
                
                kill(child_pids[i], SIGTERM);
                
                sleep(2);
                if (kill(child_pids[i], 0) == 0) {
                    snprintf(log_msg, sizeof(log_msg), "Child %d (PID: %d) did not respond to SIGTERM, sending SIGKILL", 
                            i+1, child_pids[i]);
                    log_event("MONITOR", log_msg);
                    kill(child_pids[i], SIGKILL);
                }
                
                child_active[i] = 0;
            }
        }
    }
}

void daemonize() {
    pid_t pid, sid;
    
    pid = fork();
    if (pid < 0) {
        error_handler("Failed to fork daemon process");
    } else if (pid > 0) {
        printf("Daemon process started with PID: %d\n", pid);
        exit(EXIT_SUCCESS);
    }
    
    umask(0);
    
    setup_daemon_logger();
    
    sid = setsid();
    if (sid < 0) {
        log_event("ERROR", "Failed to create new session for daemon");
        exit(EXIT_FAILURE);
    }
    
    
    redirect_std_fds();
    
    setup_daemon_signals();
    
    daemon_running = 1;
    log_event("DAEMON", "Daemon process initialized successfully");
}

void create_fifos() {
    if (mkfifo(FIFO1_PATH, 0666) == -1) {
        if (errno != EEXIST) {
            if (daemon_running) {
                log_event("ERROR", "FIFO1 creation failed");
            }
            error_handler("FIFO1 creation failed");
        } else {
            if (daemon_running) {
                log_event("INFO", "FIFO1 already exists. Continuing...");
            } else {
                printf("FIFO1 already exists. Continuing...\n");
            }
        }
    } else {
        if (daemon_running) {
            log_event("INFO", "FIFO1 created successfully");
        } else {
            printf("FIFO1 created successfully.\n");
        }
    }

    if (mkfifo(FIFO2_PATH, 0666) == -1) {
        if (errno != EEXIST) {
            if (daemon_running) {
                log_event("ERROR", "FIFO2 creation failed");
            }
            error_handler("FIFO2 creation failed");
        } else {
            if (daemon_running) {
                log_event("INFO", "FIFO2 already exists. Continuing...");
            } else {
                printf("FIFO2 already exists. Continuing...\n");
            }
        }
    } else {
        if (daemon_running) {
            log_event("INFO", "FIFO2 created successfully");
        } else {
            printf("FIFO2 created successfully.\n");
        }
    }
}

void write_to_fifo(int num1, int num2) {
    int fd1;
    
    if (daemon_running) {
        log_event("INFO", "Opening FIFO1 for writing");
    } else {
        printf("Opening FIFO1 for writing...\n");
    }
    
    fd1 = open(FIFO1_PATH, O_WRONLY);
    if (fd1 == -1) {
        if (daemon_running) {
            log_event("ERROR", "Failed to open FIFO1 for writing");
        }
        error_handler("Failed to open FIFO1 for writing");
    }
    
    char log_msg[100];
    snprintf(log_msg, sizeof(log_msg), "Writing numbers %d and %d to FIFO1", num1, num2);
    
    if (daemon_running) {
        log_event("INFO", log_msg);
    } else {
        printf("Writing numbers %d and %d to FIFO1...\n", num1, num2);
    }
    
    if (write(fd1, &num1, sizeof(int)) == -1) {
        if (daemon_running) {
            log_event("ERROR", "Failed to write num1 to FIFO1");
        }
        error_handler("Failed to write num1 to FIFO1");
    }
    if (write(fd1, &num2, sizeof(int)) == -1) {
        if (daemon_running) {
            log_event("ERROR", "Failed to write num2 to FIFO1");
        }
        error_handler("Failed to write num2 to FIFO1");
    }
    
    close(fd1);
    
    if (daemon_running) {
        log_event("INFO", "Data written to FIFO1 successfully");
    } else {
        printf("Data written to FIFO1 successfully.\n");
    }
    
    sleep(1);
}

void child1_process() {
    int fd1, fd2;
    int num1, num2, larger;
    char log_msg[100];
    
    snprintf(log_msg, sizeof(log_msg), "Child 1 (PID: %d) started", getpid());
    if (daemon_running) {
        log_event("CHILD1", log_msg);
    } else {
        printf("Child 1 (PID: %d) started.\n", getpid());
    }
    
    fd1 = open(FIFO1_PATH, O_RDONLY);
    if (fd1 == -1) {
        if (daemon_running) {
            log_event("ERROR", "Child 1: Failed to open FIFO1 for reading");
        }
        error_handler("Child 1: Failed to open FIFO1 for reading");
    }
    
    setup_nonblocking_fifo(fd1);
    
    fd_set readfds;
    struct timeval tv;
    int retval;
    
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(fd1, &readfds);
        
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        
        retval = select(fd1 + 1, &readfds, NULL, NULL, &tv);
        
        if (retval == -1) {
            if (daemon_running) {
                log_event("ERROR", "Child 1: Select failed");
            }
            error_handler("Child 1: Select failed");
        } else if (retval == 0) {
            if (daemon_running) {
                log_event("WARN", "Child 1: Timeout waiting for data on FIFO1");
            } else {
                printf("Child 1: Timeout waiting for data on FIFO1.\n");
            }
            continue;
        }
        
        if (FD_ISSET(fd1, &readfds)) {
            if (read(fd1, &num1, sizeof(int)) <= 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue;
                }
                if (daemon_running) {
                    log_event("ERROR", "Child 1: Failed to read num1 from FIFO1");
                }
                error_handler("Child 1: Failed to read num1 from FIFO1");
            }
            if (read(fd1, &num2, sizeof(int)) <= 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue;
                }
                if (daemon_running) {
                    log_event("ERROR", "Child 1: Failed to read num2 from FIFO1");
                }
                error_handler("Child 1: Failed to read num2 from FIFO1");
            }
            break;
        }
    }
    
    close(fd1);
    
    snprintf(log_msg, sizeof(log_msg), "Child 1: Read numbers %d and %d from FIFO1", num1, num2);
    if (daemon_running) {
        log_event("CHILD1", log_msg);
    } else {
        printf("Child 1: Read numbers %d and %d from FIFO1.\n", num1, num2);
    }
    
    larger = (num1 > num2) ? num1 : num2;
    
    snprintf(log_msg, sizeof(log_msg), "Child 1: Larger number is %d", larger);
    if (daemon_running) {
        log_event("CHILD1", log_msg);
    } else {
        printf("Child 1: Larger number is %d.\n", larger);
    }
    
    fd2 = open(FIFO2_PATH, O_WRONLY);
    if (fd2 == -1) {
        if (daemon_running) {
            log_event("ERROR", "Child 1: Failed to open FIFO2 for writing");
        }
        error_handler("Child 1: Failed to open FIFO2 for writing");
    }
    
    char buffer[36] = {0};
    strncpy(buffer, COMMAND, 32);
    
    memcpy(buffer + 32, &larger, sizeof(int));
    
    if (write(fd2, buffer, sizeof(buffer)) == -1) {
        if (daemon_running) {
            log_event("ERROR", "Child 1: Failed to write data to FIFO2");
        }
        error_handler("Child 1: Failed to write data to FIFO2");
    }
    
    close(fd2);
    
    if (daemon_running) {
        log_event("CHILD1", "Child 1: Wrote command and larger number to FIFO2");
        log_event("CHILD1", "Child 1: Sleeping for 10 seconds");
    } else {
        printf("Child 1: Wrote command and larger number to FIFO2.\n");
        printf("Child 1: Sleeping for 10 seconds...\n");
    }
    
    sleep(10);
    
    if (daemon_running) {
        log_event("CHILD1", "Child 1: Exiting now");
    } else {
        printf("Child 1: Exiting now.\n");
    }
    
    exit(EXIT_SUCCESS);
}

void child2_process() {
    int fd2;
    char command[32];
    int larger;
    char buffer[36] = {0};
    char log_msg[100];
    
    snprintf(log_msg, sizeof(log_msg), "Child 2 (PID: %d) started", getpid());
    if (daemon_running) {
        log_event("CHILD2", log_msg);
    } else {
        printf("Child 2 (PID: %d) started.\n", getpid());
    }
    
    fd2 = open(FIFO2_PATH, O_RDONLY);
    if (fd2 == -1) {
        if (daemon_running) {
            log_event("ERROR", "Child 2: Failed to open FIFO2 for reading");
        }
        error_handler("Child 2: Failed to open FIFO2 for reading");
    }
    
    setup_nonblocking_fifo(fd2);
    
    fd_set readfds;
    struct timeval tv;
    int retval;
    
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(fd2, &readfds);
        
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        
        retval = select(fd2 + 1, &readfds, NULL, NULL, &tv);
        
        if (retval == -1) {
            if (daemon_running) {
                log_event("ERROR", "Child 2: Select failed");
            }
            error_handler("Child 2: Select failed");
        } else if (retval == 0) {
            if (daemon_running) {
                log_event("WARN", "Child 2: Timeout waiting for data on FIFO2");
            } else {
                printf("Child 2: Timeout waiting for data on FIFO2.\n");
            }
            continue;
        }
        
        if (FD_ISSET(fd2, &readfds)) {
            if (read(fd2, buffer, sizeof(buffer)) <= 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue;
                }
                if (daemon_running) {
                    log_event("ERROR", "Child 2: Failed to read data from FIFO2");
                }
                error_handler("Child 2: Failed to read data from FIFO2");
            }
            break;
        }
    }
    
    close(fd2);
    
    strncpy(command, buffer, 32);
    memcpy(&larger, buffer + 32, sizeof(int));
    
    snprintf(log_msg, sizeof(log_msg), "Child 2: Read command '%s' from FIFO2", command);
    if (daemon_running) {
        log_event("CHILD2", log_msg);
    } else {
        printf("Child 2: Read command '%s' from FIFO2.\n", command);
    }
    
    snprintf(log_msg, sizeof(log_msg), "Child 2: The larger number is: %d", larger);
    if (daemon_running) {
        log_event("CHILD2", log_msg);
    } else {
        printf("Child 2: The larger number is: %d\n", larger);
    }
    
    if (daemon_running) {
        log_event("CHILD2", "Child 2: Sleeping for 10 seconds");
    } else {
        printf("Child 2: Sleeping for 10 seconds...\n");
    }
    
    sleep(10);
    
    if (daemon_running) {
        log_event("CHILD2", "Child 2: Exiting now");
    } else {
        printf("Child 2: Exiting now.\n");
    }
    
    exit(EXIT_SUCCESS);
}

void handle_sigchld(int sig) {
    pid_t pid;
    int status;
    char log_msg[100];
    
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        snprintf(log_msg, sizeof(log_msg), "Child process %d terminated", pid);
        if (daemon_running) {
            log_event("PARENT", log_msg);
        } else {
            printf("Parent: Child process %d terminated.\n", pid);
        }
        
        if (WIFEXITED(status)) {
            snprintf(log_msg, sizeof(log_msg), "Child %d exited with status %d", pid, WEXITSTATUS(status));
            if (daemon_running) {
                log_event("PARENT", log_msg);
            } else {
                printf("Parent: Child %d exited with status %d.\n", pid, WEXITSTATUS(status));
            }
        } else if (WIFSIGNALED(status)) {
            snprintf(log_msg, sizeof(log_msg), "Child %d terminated by signal %d", pid, WTERMSIG(status));
            if (daemon_running) {
                log_event("PARENT", log_msg);
            } else {
                printf("Parent: Child %d terminated by signal %d.\n", pid, WTERMSIG(status));
            }
        }
        
        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (child_pids[i] == pid) {
                child_pids[i] = 0;
                child_active[i] = 0;
                break;
            }
        }
        
        child_counter += 2;
        
        snprintf(log_msg, sizeof(log_msg), "Child counter is now %d", child_counter);
        if (daemon_running) {
            log_event("PARENT", log_msg);
        } else {
            printf("Parent: Child counter is now %d.\n", child_counter);
        }
    }
}

void create_child_processes() {
    pid_t pid1, pid2;
    char log_msg[100];
    
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        if (daemon_running) {
            log_event("ERROR", "Failed to set SIGCHLD handler");
        }
        error_handler("Failed to set SIGCHLD handler");
    }
    
    pid1 = fork();
    if (pid1 < 0) {
        if (daemon_running) {
            log_event("ERROR", "Fork failed for child 1");
        }
        error_handler("Fork failed for child 1");
    } else if (pid1 == 0) {
        child1_process();
        exit(EXIT_SUCCESS);
    }
    
    snprintf(log_msg, sizeof(log_msg), "Created child 1 with PID: %d", pid1);
    if (daemon_running) {
        log_event("PARENT", log_msg);
    } else {
        printf("Parent: Created child 1 with PID: %d\n", pid1);
    }
    
    child_pids[0] = pid1;
    time(&child_start_times[0]);
    child_active[0] = 1;
    
    pid2 = fork();
    if (pid2 < 0) {
        if (daemon_running) {
            log_event("ERROR", "Fork failed for child 2");
        }
        error_handler("Fork failed for child 2");
    } else if (pid2 == 0) {
        child2_process();
        exit(EXIT_SUCCESS);
    }
    
    snprintf(log_msg, sizeof(log_msg), "Created child 2 with PID: %d", pid2);
    if (daemon_running) {
        log_event("PARENT", log_msg);
    } else {
        printf("Parent: Created child 2 with PID: %d\n", pid2);
    }
    
    child_pids[1] = pid2;
    time(&child_start_times[1]);
    child_active[1] = 1;
}

void cleanup_resources() {
    unlink(FIFO1_PATH);
    unlink(FIFO2_PATH);
    
    if (daemon_running) {
        log_event("PARENT", "Resources cleaned up");
    } else {
        printf("Resources cleaned up.\n");
    }
    
    if (log_fd != -1) {
        log_event("DAEMON", "Daemon shutting down");
        close(log_fd);
    }
}

void parent_loop() {
    char log_msg[100];
    
    if (daemon_running) {
        log_event("PARENT", "Entering main loop");
    } else {
        printf("Parent: Entering main loop...\n");
    }
    
    int timeout = 0;
    int max_timeout = 60;
    
    while (child_counter < NUM_CHILDREN * 2) {
        snprintf(log_msg, sizeof(log_msg), "Proceeding... (counter = %d)", child_counter);
        if (daemon_running) {
            log_event("PARENT", log_msg);
        } else {
            printf("Parent: Proceeding... (counter = %d)\n", child_counter);
        }
        
        if (daemon_running) {
            monitor_child_processes();
        }
        
        sleep(2);
        
        timeout += 2;
        if (timeout > max_timeout) {
            if (daemon_running) {
                log_event("PARENT", "Timeout reached, forcing exit");
            } else {
                printf("Parent: Timeout reached, forcing exit.\n");
            }
            break;
        }
    }
    
    if (daemon_running) {
        log_event("PARENT", "All children have terminated. Exiting");
    } else {
        printf("Parent: All children have terminated. Exiting.\n");
    }
}

void prevent_zombie() {
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDWAIT;
    
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        if (daemon_running) {
            log_event("ERROR", "Failed to set zombie prevention");
        } else {
            perror("Failed to set zombie prevention");
        }
    } else {
        if (daemon_running) {
            log_event("PARENT", "Zombie prevention enabled");
        } else {
            printf("Parent: Zombie prevention enabled.\n");
        }
    }
}

int main(int argc, char *argv[]) {
   if (argc != 3) {
       fprintf(stderr, "Usage: %s <integer1> <integer2>\n", argv[0]);
       return EXIT_FAILURE;
   }

   int num1 = atoi(argv[1]);
   int num2 = atoi(argv[2]);
   
   printf("Arguments received: %d and %d\n", num1, num2);
   
   prevent_zombie();
   
   daemonize();
   
   create_fifos();
   
   create_child_processes();
   
   write_to_fifo(num1, num2);
   
   parent_loop();
   
   cleanup_resources();
   
   return EXIT_SUCCESS;
}
