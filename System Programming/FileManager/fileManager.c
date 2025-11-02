

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/file.h>

#define BUFFER_SIZE 1024
#define LOG_FILE "log.txt"
#define MAX_PATH_LEN 256

// Function prototypes
void createDirectory(const char *dirName);
void createFile(const char *fileName);
void listDirectory(const char *dirName);
void listFilesByExtension(const char *dirName, const char *extension);
void readFile(const char *fileName);
void appendToFile(const char *fileName, const char *content);
void deleteFile(const char *fileName);
void deleteDirectory(const char *dirName);
void showLogs();
void writeToLog(const char *message);
void displayHelp();
char *getCurrentTimestamp();
int isDirectoryEmpty(const char *dirName);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        displayHelp();
        return 0;
    }

    if (strcmp(argv[1], "createDir") == 0) {
        if (argc != 3) {
            write(STDERR_FILENO, "Error: Invalid arguments for createDir\n", 39);
            return 1;
        }
        createDirectory(argv[2]);
    } 
    else if (strcmp(argv[1], "createFile") == 0) {
        if (argc != 3) {
            write(STDERR_FILENO, "Error: Invalid arguments for createFile\n", 40);
            return 1;
        }
        createFile(argv[2]);
    } 
    else if (strcmp(argv[1], "listDir") == 0) {
        if (argc != 3) {
            write(STDERR_FILENO, "Error: Invalid arguments for listDir\n", 37);
            return 1;
        }
        listDirectory(argv[2]);
    } 
    else if (strcmp(argv[1], "listFilesByExtension") == 0) {
        if (argc != 4) {
            write(STDERR_FILENO, "Error: Invalid arguments for listFilesByExtension\n", 50);
            return 1;
        }
        listFilesByExtension(argv[2], argv[3]);
    } 
    else if (strcmp(argv[1], "readFile") == 0) {
        if (argc != 3) {
            write(STDERR_FILENO, "Error: Invalid arguments for readFile\n", 38);
            return 1;
        }
        readFile(argv[2]);
    } 
    else if (strcmp(argv[1], "appendToFile") == 0) {
        if (argc != 4) {
            write(STDERR_FILENO, "Error: Invalid arguments for appendToFile\n", 42);
            return 1;
        }
        appendToFile(argv[2], argv[3]);
    } 
    else if (strcmp(argv[1], "deleteFile") == 0) {
        if (argc != 3) {
            write(STDERR_FILENO, "Error: Invalid arguments for deleteFile\n", 40);
            return 1;
        }
        deleteFile(argv[2]);
    } 
    else if (strcmp(argv[1], "deleteDir") == 0) {
        if (argc != 3) {
            write(STDERR_FILENO, "Error: Invalid arguments for deleteDir\n", 39);
            return 1;
        }
        deleteDirectory(argv[2]);
    } 
    else if (strcmp(argv[1], "showLogs") == 0) {
        showLogs();
    } 
    else {
        displayHelp();
    }

    return 0;
}

/**
 * Displays the help message with all available commands
 */
void displayHelp() {
    const char *usage = "Usage: ./fileManager <command> [arguments]\n"
                        "Commands:\n"
                        " createDir \"folderName\"                  - Create a new directory\n"
                        " createFile \"fileName\"                   - Create a new file\n"
                        " listDir \"folderName\"                    - List all files in a directory\n"
                        " listFilesByExtension \"folderName\" \".txt\" - List files with specific extension\n"
                        " readFile \"fileName\"                     - Read a file's content\n"
                        " appendToFile \"fileName\" \"new content\"    - Append content to a file\n"
                        " deleteFile \"fileName\"                   - Delete a file\n"
                        " deleteDir \"folderName\"                  - Delete an empty directory\n"
                        " showLogs                                - Display operation logs\n";
    
    write(STDOUT_FILENO, usage, strlen(usage));
}

/**
 * Returns the current timestamp as a string
 * Caller must free the returned string
 */
char *getCurrentTimestamp() {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    char *timestamp = malloc(20); // YYYY-MM-DD HH:MM:SS
    if (timestamp != NULL) {
        strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    }
    
    return timestamp;
}

/**
 * Writes an entry to the log file
 */
void writeToLog(const char *message) {
    // Get current timestamp
    char *timestamp = getCurrentTimestamp();
    
    // Open log file for appending (or create if it doesn't exist)
    int fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0600);
    if (fd == -1) {
        write(STDERR_FILENO, "Error: Failed to open log file for writing\n", 43);
        free(timestamp);
        return;
    }
    
    // Format and write log entry
    char log_entry[BUFFER_SIZE];
    char bracket_open[] = "[";
    char bracket_close[] = "] ";
    char newline[] = "\n";
    
    // Manually construct log entry without sprintf
    strcpy(log_entry, bracket_open);
    strcat(log_entry, timestamp);
    strcat(log_entry, bracket_close);
    strcat(log_entry, message);
    strcat(log_entry, newline);
    
    write(fd, log_entry, strlen(log_entry));
    
    close(fd);
    free(timestamp);
}

/**
 * Creates a new directory with the given name
 */
void createDirectory(const char *dirName) {
    struct stat st = {0};
    
    // Check if directory already exists
    if (stat(dirName, &st) != -1) {
        char error_msg[BUFFER_SIZE];
        // Manually construct the error message without sprintf
        strcpy(error_msg, "Error: Directory \"");
        strcat(error_msg, dirName);
        strcat(error_msg, "\" already exists.\n");
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }
    
    // Create directory with read, write, execute permissions for owner
    if (mkdir(dirName, 0700) == -1) {
        write(STDERR_FILENO, "Error: Failed to create directory\n", 34);
        return;
    }
    
    char log_msg[BUFFER_SIZE];
    // Manually construct log message without sprintf
    strcpy(log_msg, "Directory \"");
    strcat(log_msg, dirName);
    strcat(log_msg, "\" created successfully.");
    writeToLog(log_msg);
    
    char success_msg[BUFFER_SIZE];
    // Manually construct success message without sprintf
    strcpy(success_msg, "Directory \"");
    strcat(success_msg, dirName);
    strcat(success_msg, "\" created successfully.\n");
    write(STDOUT_FILENO, success_msg, strlen(success_msg));
}

/**
 * Creates a new file with the given name and writes timestamp inside
 */
void createFile(const char *fileName) {
    struct stat st = {0};
    
    // Check if file already exists
    if (stat(fileName, &st) != -1) {
        char error_msg[BUFFER_SIZE];
        // Manually construct the error message without sprintf
        strcpy(error_msg, "Error: File \"");
        strcat(error_msg, fileName);
        strcat(error_msg, "\" already exists.\n");
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }
    
    // Create file with read and write permissions for owner
    int fd = open(fileName, O_CREAT | O_WRONLY, 0600);
    if (fd == -1) {
        write(STDERR_FILENO, "Error: Failed to create file\n", 29);
        return;
    }
    
    // Get and write current timestamp to file
    char *timestamp = getCurrentTimestamp();
    char content[BUFFER_SIZE];
    // Manually construct content without sprintf
    strcpy(content, "File created at: ");
    strcat(content, timestamp);
    strcat(content, "\n");
    write(fd, content, strlen(content));
    
    close(fd);
    free(timestamp);
    
    char log_msg[BUFFER_SIZE];
    // Manually construct log message without sprintf
    strcpy(log_msg, "File \"");
    strcat(log_msg, fileName);
    strcat(log_msg, "\" created successfully.");
    writeToLog(log_msg);
    
    char success_msg[BUFFER_SIZE];
    // Manually construct success message without sprintf
    strcpy(success_msg, "File \"");
    strcat(success_msg, fileName);
    strcat(success_msg, "\" created successfully.\n");
    write(STDOUT_FILENO, success_msg, strlen(success_msg));
}

/**
 * Displays the contents of the log file
 */
void showLogs() {
    // Check if log file exists
    struct stat st = {0};
    if (stat(LOG_FILE, &st) == -1) {
        write(STDERR_FILENO, "Error: Log file does not exist\n", 31);
        return;
    }
    
    // Open log file for reading
    int fd = open(LOG_FILE, O_RDONLY);
    if (fd == -1) {
        write(STDERR_FILENO, "Error: Failed to open log file\n", 31);
        return;
    }
    
    // Read and display log content
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    write(STDOUT_FILENO, "Operation Logs:\n", 16);
    
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    
    close(fd);
    
    // Log the log viewing
    writeToLog("Logs viewed.");
}

/**
 * Reads and displays the content of the specified file
 */
void readFile(const char *fileName) {
    // Check if file exists
    struct stat st = {0};
    if (stat(fileName, &st) == -1) {
        char error_msg[BUFFER_SIZE];
        strcpy(error_msg, "Error: File \"");
        strcat(error_msg, fileName);
        strcat(error_msg, "\" not found.\n");
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }
    
    // Open file for reading
    int fd = open(fileName, O_RDONLY);
    if (fd == -1) {
        write(STDERR_FILENO, "Error: Failed to open file for reading\n", 39);
        return;
    }
    
    // Read and display file content
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    char header[BUFFER_SIZE];
    strcpy(header, "Content of file \"");
    strcat(header, fileName);
    strcat(header, "\":\n");
    write(STDOUT_FILENO, header, strlen(header));
    
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    write(STDOUT_FILENO, "\n", 1);
    
    close(fd);
    
    char log_msg[BUFFER_SIZE];
    strcpy(log_msg, "Read file \"");
    strcat(log_msg, fileName);
    strcat(log_msg, "\".");
    writeToLog(log_msg);
}

/**
 * Appends content to the specified file with file locking
 */
void appendToFile(const char *fileName, const char *content) {
    // Check if file exists
    struct stat st = {0};
    if (stat(fileName, &st) == -1) {
        char error_msg[BUFFER_SIZE];
        strcpy(error_msg, "Error: File \"");
        strcat(error_msg, fileName);
        strcat(error_msg, "\" not found.\n");
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }
    
    // Open file for appending
    int fd = open(fileName, O_WRONLY | O_APPEND);
    if (fd == -1) {
        char error_msg[BUFFER_SIZE];
        strcpy(error_msg, "Error: Cannot write to \"");
        strcat(error_msg, fileName);
        strcat(error_msg, "\". File is locked or read-only.\n");
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        return;
    }
    
    // Try to get an exclusive lock on the file
    if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
        char error_msg[BUFFER_SIZE];
        strcpy(error_msg, "Error: Cannot write to \"");
        strcat(error_msg, fileName);
        strcat(error_msg, "\". File is locked or read-only.\n");
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        close(fd);
        return;
    }
    
    // Append content to file
    write(fd, content, strlen(content));
    write(fd, "\n", 1); // Add newline after content
    
    // Release the lock
    flock(fd, LOCK_UN);
    close(fd);
    
    char log_msg[BUFFER_SIZE];
    strcpy(log_msg, "Appended content to file \"");
    strcat(log_msg, fileName);
    strcat(log_msg, "\".");
    writeToLog(log_msg);
    
    char success_msg[BUFFER_SIZE];
    strcpy(success_msg, "Content appended to \"");
    strcat(success_msg, fileName);
    strcat(success_msg, "\" successfully.\n");
    write(STDOUT_FILENO, success_msg, strlen(success_msg));
}

/**
 * Lists all files and directories in the given directory
 * Uses a separate process (fork())
 */
void listDirectory(const char *dirName) {
    pid_t pid = fork();
    
    if (pid == -1) {
        write(STDERR_FILENO, "Error: Failed to create process\n", 32);
        return;
    } 
    else if (pid == 0) { // Child process
        // Check if directory exists
        DIR *dir = opendir(dirName);
        if (dir == NULL) {
            char error_msg[BUFFER_SIZE];
            strcpy(error_msg, "Error: Directory \"");
            strcat(error_msg, dirName);
            strcat(error_msg, "\" not found.\n");
            write(STDERR_FILENO, error_msg, strlen(error_msg));
            exit(EXIT_FAILURE);
        }
        
        char output[BUFFER_SIZE];
        strcpy(output, "Contents of directory \"");
        strcat(output, dirName);
        strcat(output, "\":\n");
        write(STDOUT_FILENO, output, strlen(output));
        
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            // Skip . and .. directories
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char entry_path[MAX_PATH_LEN];
                strcpy(entry_path, dirName);
                strcat(entry_path, "/");
                strcat(entry_path, entry->d_name);
                
                struct stat st;
                if (stat(entry_path, &st) == 0) {
                    char type = S_ISDIR(st.st_mode) ? 'd' : '-';
                    char details[BUFFER_SIZE];
                    strcpy(details, "[");
                    details[0] = '[';
                    details[1] = type;
                    details[2] = ']';
                    details[3] = ' ';
                    details[4] = '\0';
                    strcat(details, entry->d_name);
                    strcat(details, "\n");
                    write(STDOUT_FILENO, details, strlen(details));
                }
            }
        }
        
        closedir(dir);
        
        char log_msg[BUFFER_SIZE];
        strcpy(log_msg, "Listed contents of directory \"");
        strcat(log_msg, dirName);
        strcat(log_msg, "\".");
        writeToLog(log_msg);
        
        exit(EXIT_SUCCESS);
    } 
    else { // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

/**
 * Lists files with the specified extension in the given directory
 * Uses a separate process (fork())
 */
void listFilesByExtension(const char *dirName, const char *extension) {
    pid_t pid = fork();
    
    if (pid == -1) {
        write(STDERR_FILENO, "Error: Failed to create process\n", 32);
        return;
    } 
    else if (pid == 0) { // Child process
        // Check if directory exists
        DIR *dir = opendir(dirName);
        if (dir == NULL) {
            char error_msg[BUFFER_SIZE];
            strcpy(error_msg, "Error: Directory \"");
            strcat(error_msg, dirName);
            strcat(error_msg, "\" not found.\n");
            write(STDERR_FILENO, error_msg, strlen(error_msg));
            exit(EXIT_FAILURE);
        }
        
        char output[BUFFER_SIZE];
        strcpy(output, "Files with extension \"");
        strcat(output, extension);
        strcat(output, "\" in directory \"");
        strcat(output, dirName);
        strcat(output, "\":\n");
        write(STDOUT_FILENO, output, strlen(output));
        
        struct dirent *entry;
        int found = 0;
        
        while ((entry = readdir(dir)) != NULL) {
            // Check if file ends with the given extension
            const char *file_name = entry->d_name;
            size_t name_len = strlen(file_name);
            size_t ext_len = strlen(extension);
            
            if (name_len > ext_len && 
                strcmp(file_name + name_len - ext_len, extension) == 0) {
                write(STDOUT_FILENO, file_name, strlen(file_name));
                write(STDOUT_FILENO, "\n", 1);
                found = 1;
            }
        }
        
        if (!found) {
            char no_files_msg[BUFFER_SIZE];
            strcpy(no_files_msg, "No files with extension \"");
            strcat(no_files_msg, extension);
            strcat(no_files_msg, "\" found in \"");
            strcat(no_files_msg, dirName);
            strcat(no_files_msg, "\".\n");
            write(STDOUT_FILENO, no_files_msg, strlen(no_files_msg));
        }
        
        closedir(dir);
        
        char log_msg[BUFFER_SIZE];
        strcpy(log_msg, "Listed files with extension \"");
        strcat(log_msg, extension);
        strcat(log_msg, "\" in directory \"");
        strcat(log_msg, dirName);
        strcat(log_msg, "\".");
        writeToLog(log_msg);
        
        exit(EXIT_SUCCESS);
    } 
    else { // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

/**
 * Deletes the specified file using a separate process (fork())
 */
void deleteFile(const char *fileName) {
    pid_t pid = fork();
    
    if (pid == -1) {
        write(STDERR_FILENO, "Error: Failed to create process\n", 32);
        return;
    } 
    else if (pid == 0) { // Child process
        // Check if file exists
        struct stat st = {0};
        if (stat(fileName, &st) == -1) {
            char error_msg[BUFFER_SIZE];
            strcpy(error_msg, "Error: File \"");
            strcat(error_msg, fileName);
            strcat(error_msg, "\" not found.\n");
            write(STDERR_FILENO, error_msg, strlen(error_msg));
            exit(EXIT_FAILURE);
        }
        
        // Delete the file
        if (unlink(fileName) == -1) {
            write(STDERR_FILENO, "Error: Failed to delete file\n", 29);
            exit(EXIT_FAILURE);
        }
        
        char success_msg[BUFFER_SIZE];
        strcpy(success_msg, "File \"");
        strcat(success_msg, fileName);
        strcat(success_msg, "\" deleted successfully.\n");
        write(STDOUT_FILENO, success_msg, strlen(success_msg));
        
        char log_msg[BUFFER_SIZE];
        strcpy(log_msg, "File \"");
        strcat(log_msg, fileName);
        strcat(log_msg, "\" deleted successfully.");
        writeToLog(log_msg);
        
        exit(EXIT_SUCCESS);
    } 
    else { // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

/**
 * Checks if a directory is empty
 * Returns 1 if empty, 0 otherwise
 */
int isDirectoryEmpty(const char *dirName) {
    int isEmpty = 1;
    DIR *dir = opendir(dirName);
    
    if (dir == NULL) {
        return 0; // Error or not a directory
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            isEmpty = 0;
            break;
        }
    }
    
    closedir(dir);
    return isEmpty;
}

/**
 * Deletes the specified empty directory using a separate process (fork())
 */
void deleteDirectory(const char *dirName) {
    pid_t pid = fork();
    
    if (pid == -1) {
        write(STDERR_FILENO, "Error: Failed to create process\n", 32);
        return;
    } 
    else if (pid == 0) { // Child process
        // Check if directory exists
        DIR *dir = opendir(dirName);
        if (dir == NULL) {
            char error_msg[BUFFER_SIZE];
            strcpy(error_msg, "Error: Directory \"");
            strcat(error_msg, dirName);
            strcat(error_msg, "\" not found.\n");
            write(STDERR_FILENO, error_msg, strlen(error_msg));
            exit(EXIT_FAILURE);
        }
        closedir(dir);
        
        // Check if directory is empty
        if (!isDirectoryEmpty(dirName)) {
            char error_msg[BUFFER_SIZE];
            strcpy(error_msg, "Error: Directory \"");
            strcat(error_msg, dirName);
            strcat(error_msg, "\" is not empty.\n");
            write(STDERR_FILENO, error_msg, strlen(error_msg));
            exit(EXIT_FAILURE);
        }
        
        // Delete the directory
        if (rmdir(dirName) == -1) {
            write(STDERR_FILENO, "Error: Failed to delete directory\n", 34);
            exit(EXIT_FAILURE);
        }
        
        char success_msg[BUFFER_SIZE];
        strcpy(success_msg, "Directory \"");
        strcat(success_msg, dirName);
        strcat(success_msg, "\" deleted successfully.\n");
        write(STDOUT_FILENO, success_msg, strlen(success_msg));
        
        char log_msg[BUFFER_SIZE];
        strcpy(log_msg, "Directory \"");
        strcat(log_msg, dirName);
        strcat(log_msg, "\" deleted.");
        writeToLog(log_msg);
        
        exit(EXIT_SUCCESS);
    } 
    else { // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}
