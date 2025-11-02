
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "buffer.h"

#define MAX_LINE_LENGTH 1024

// Global variables for clean exit
volatile sig_atomic_t exit_flag = 0;
Buffer buffer;
pthread_t *worker_threads = NULL;
pthread_barrier_t barrier;

// Structure to pass arguments to worker threads
typedef struct {
    int id;
    char *search_term;
    int match_count;
} WorkerArgs;

// Signal handler for SIGINT (Ctrl+C)
void handle_sigint(int sig __attribute__((unused))) {
    exit_flag = 1;
}

// Function to print usage
void print_usage(char *program_name) {
    fprintf(stderr, "Usage: %s <buffer_size> <num_workers> <log_file> <search_term>\n", program_name);
}

// Function to check if a line contains the search term
int line_contains_term(const char *line, const char *term) {
    return strstr(line, term) != NULL;
}

// Worker thread function
void *worker_function(void *arg) {
    WorkerArgs *args = (WorkerArgs *)arg;
    char *line;
    args->match_count = 0;
    
    // Process lines until exit_flag is set or no more lines
    while (!exit_flag) {
        line = buffer_get(&buffer);
        
        // If NULL, either buffer is empty and producer is done, or we got an EOF marker
        if (line == NULL) {
            break;
        }
        
        // Check if line contains the search term
        if (line_contains_term(line, args->search_term)) {
            args->match_count++;
            printf("Worker %d found match: %s\n", args->id, line);
        }
        
        // Free the line
        free(line);
    }
    
    // Wait for all workers to finish before printing summary
    pthread_barrier_wait(&barrier);
    
    // First worker prints the summary
    if (args->id == 0) {
        printf("\nSummary Report:\n");
        printf("Worker %d found %d matches\n", args->id, args->match_count);
    } else {
        printf("Worker %d found %d matches\n", args->id, args->match_count);
    }
    
    return NULL;
}

// Manager thread function
void *manager_function(void *arg) {
    char *log_file = (char *)arg;
    FILE *file = fopen(log_file, "r");
    char line[MAX_LINE_LENGTH];
    
    if (file == NULL) {
        perror("Error opening log file");
        exit_flag = 1;
        buffer_set_done(&buffer);
        return NULL;
    }
    
    // Read lines from file and add to buffer
    while (!exit_flag && fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        // Remove trailing newline if present
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        // Add line to buffer
        if (buffer_add(&buffer, line) != 0) {
            fprintf(stderr, "Error adding line to buffer\n");
            break;
        }
    }
    
    // Signal that producer is done
    buffer_set_done(&buffer);
    
    fclose(file);
    return NULL;
}

// Clean up resources
void cleanup() {
    if (worker_threads != NULL) {
        free(worker_threads);
    }
    buffer_destroy(&buffer);
    pthread_barrier_destroy(&barrier);
}

int main(int argc, char *argv[]) {
    int buffer_size, num_workers, i;
    char *log_file, *search_term;
    pthread_t manager_thread;
    WorkerArgs *worker_args = NULL;
    
    // Check command line arguments
    if (argc != 5) {
        print_usage(argv[0]);
        return 1;
    }
    
    // Parse command line arguments
    buffer_size = atoi(argv[1]);
    num_workers = atoi(argv[2]);
    log_file = argv[3];
    search_term = argv[4];
    
    // Validate arguments
    if (buffer_size <= 0 || num_workers <= 0) {
        fprintf(stderr, "Buffer size and number of workers must be positive integers\n");
        print_usage(argv[0]);
        return 1;
    }
    
    // Set up signal handler for SIGINT
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);
    
    // Initialize buffer
    if (buffer_init(&buffer, buffer_size) != 0) {
        fprintf(stderr, "Error initializing buffer\n");
        return 1;
    }
    
    // Initialize barrier
    if (pthread_barrier_init(&barrier, NULL, num_workers) != 0) {
        fprintf(stderr, "Error initializing barrier\n");
        buffer_destroy(&buffer);
        return 1;
    }
    
    // Allocate memory for worker threads and arguments
    worker_threads = (pthread_t *)malloc(num_workers * sizeof(pthread_t));
    worker_args = (WorkerArgs *)malloc(num_workers * sizeof(WorkerArgs));
    
    if (worker_threads == NULL || worker_args == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        cleanup();
        if (worker_args != NULL) free(worker_args);
        return 1;
    }
    
    // Create worker threads
    for (i = 0; i < num_workers; i++) {
        worker_args[i].id = i;
        worker_args[i].search_term = search_term;
        worker_args[i].match_count = 0;
        
        if (pthread_create(&worker_threads[i], NULL, worker_function, &worker_args[i]) != 0) {
            fprintf(stderr, "Error creating worker thread %d\n", i);
            exit_flag = 1;
            break;
        }
    }
    
    // Create manager thread
    if (!exit_flag && pthread_create(&manager_thread, NULL, manager_function, log_file) != 0) {
        fprintf(stderr, "Error creating manager thread\n");
        exit_flag = 1;
    }
    
    // Wait for all threads to finish
    if (!exit_flag) {
        if (pthread_join(manager_thread, NULL) != 0) {
            fprintf(stderr, "Error joining manager thread\n");
        }
    }
    
    // Wait for worker threads to finish
    for (i = 0; i < num_workers; i++) {
        if (pthread_join(worker_threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining worker thread %d\n", i);
        }
    }
    
    // Clean up
    cleanup();
    free(worker_args);
    
    return 0;
}
