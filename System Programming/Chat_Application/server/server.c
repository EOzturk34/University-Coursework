#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdarg.h>


#define MAX_CLIENTS 30
#define MAX_ROOMS 10
#define MAX_ROOM_CAPACITY 15
#define MAX_USERNAME_LEN 16
#define MAX_ROOM_NAME_LEN 32
#define MAX_MESSAGE_LEN 1024
#define MAX_FILENAME_LEN 256
#define MAX_FILE_SIZE (3 * 1024 * 1024) // 3MB
#define MAX_UPLOAD_QUEUE 5
#define LOG_FILE "server.log"

// ANSI color codes
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_CYAN "\033[36m"
#define COLOR_RESET "\033[0m"

// Client structure
typedef struct {
    int socket;
    char username[MAX_USERNAME_LEN + 1];
    char current_room[MAX_ROOM_NAME_LEN + 1];
    char previous_rooms[MAX_ROOMS][MAX_ROOM_NAME_LEN + 1]; // Track visited rooms
    int visited_room_count;
    int active;
    pthread_t thread;
} Client;

// Room structure
typedef struct {
    char name[MAX_ROOM_NAME_LEN + 1];
    int client_count;
    int client_ids[MAX_CLIENTS];
    pthread_mutex_t mutex;
} Room;

// File upload queue structure
typedef struct FileUpload {
    char filename[MAX_FILENAME_LEN];
    char sender[MAX_USERNAME_LEN + 1];
    char receiver[MAX_USERNAME_LEN + 1];
    size_t size;
    time_t enqueue_time;  // Track when file was added to queue
    struct FileUpload* next;
} FileUpload;

// Track received files per user (for duplicate detection)
typedef struct ReceivedFile {
    char filename[MAX_FILENAME_LEN];
    char sender[MAX_USERNAME_LEN + 1];
    time_t timestamp;
    struct ReceivedFile* next;
} ReceivedFile;

// Global variables
Client clients[MAX_CLIENTS];
Room rooms[MAX_ROOMS];
FileUpload* upload_queue = NULL;
ReceivedFile* received_files[MAX_CLIENTS]; // Track files received by each client
int upload_queue_size = 0;
int server_running = 1;
int server_socket;

// Synchronization primitives
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rooms_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t upload_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t upload_queue_sem;

// Function prototypes
void* handle_client(void* arg);
void log_activity(const char* format, ...);
void send_message(int socket, const char* message);
void broadcast_room(const char* room_name, const char* message, const char* sender);
void handle_join(Client* client, const char* room_name);
void handle_leave(Client* client);
void handle_broadcast(Client* client, const char* message);
void handle_whisper(Client* client, const char* target_user, const char* message);
void handle_sendfile(Client* client, const char* filename, const char* target_user);
void handle_exit(Client* client);
void signal_handler(int sig);
int validate_username(const char* username);
int validate_room_name(const char* room_name);
Client* find_client_by_username(const char* username);
Room* find_or_create_room(const char* room_name);
void remove_client_from_room(Client* client);
void cleanup_client(Client* client);
void* file_upload_worker(void* arg);
void enqueue_file_upload(const char* filename, const char* sender, const char* receiver, size_t size);

// Initialize server
void init_server() {
    // Initialize clients
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].socket = -1;
        clients[i].active = 0;
        memset(clients[i].username, 0, sizeof(clients[i].username));
        memset(clients[i].current_room, 0, sizeof(clients[i].current_room));
        clients[i].visited_room_count = 0;
        memset(clients[i].previous_rooms, 0, sizeof(clients[i].previous_rooms));
        received_files[i] = NULL; // Initialize received files tracking
    }
    
    // Initialize rooms
    for (int i = 0; i < MAX_ROOMS; i++) {
        memset(rooms[i].name, 0, sizeof(rooms[i].name));
        rooms[i].client_count = 0;
        pthread_mutex_init(&rooms[i].mutex, NULL);
    }
    
    // Initialize semaphore for upload queue (0 means start empty)
    if (sem_init(&upload_queue_sem, 0, 0) != 0) {
        perror("Failed to initialize upload queue semaphore");
        exit(1);
    }
    
    // Start multiple file upload worker threads
    for (int i = 0; i < MAX_UPLOAD_QUEUE; i++) {
        pthread_t upload_worker;
        int* worker_id = malloc(sizeof(int));
        *worker_id = i + 1;
        
        if (pthread_create(&upload_worker, NULL, file_upload_worker, worker_id) == 0) {
            printf("[INFO] File upload worker thread %d started\n", i + 1);
            log_activity("[INFO] File upload worker thread %d started", i + 1);
            pthread_detach(upload_worker);
        } else {
            printf("[ERROR] Failed to start upload worker thread %d\n", i + 1);
            log_activity("[ERROR] Failed to start upload worker thread %d", i + 1);
            free(worker_id);
        }
    }
}

// Main server function
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    
    int port = atoi(argv[1]);
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // Set up signal handler
    signal(SIGINT, signal_handler);
    
    // Initialize server
    init_server();
    
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    // Set socket options
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Bind socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }
    
    // Listen for connections
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(1);
    }
    
    printf("[INFO] Server listening on port %d...\n", port);
    log_activity("[INFO] Server listening on port %d...", port);
    
    // Accept connections
    while (server_running) {
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            if (server_running) {
                perror("Accept failed");
            }
            continue;
        }
        
        // Get client IP
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        
        // Find available slot
        pthread_mutex_lock(&clients_mutex);
        int slot = -1;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i].active) {
                slot = i;
                break;
            }
        }
        
        if (slot == -1) {
            pthread_mutex_unlock(&clients_mutex);
            send_message(client_socket, "[ERROR] Server full. Try again later.\n");
            printf("[REJECTED] Connection rejected from %s - Server full (max %d clients)\n", client_ip, MAX_CLIENTS);
            log_activity("[REJECTED] Connection rejected from %s - Server full", client_ip);
            close(client_socket);
            continue;
        }
        
        // Initialize client
        clients[slot].socket = client_socket;
        clients[slot].active = 1;
        pthread_mutex_unlock(&clients_mutex);
        
        // Create thread for client
        pthread_create(&clients[slot].thread, NULL, handle_client, &clients[slot]);
        pthread_detach(clients[slot].thread); // Detach the client thread
        
        // Don't log connection here, wait for username
    }
    
    // Cleanup
    close(server_socket);
    return 0;
}

// Handle client connection
// Handle client connection
void* handle_client(void* arg) {
    pthread_detach(pthread_self());
    Client* client = (Client*)arg;
    char buffer[MAX_MESSAGE_LEN];
    int username_accepted = 0;
    int bytes;
    char* newline;
    
    // Get client IP address
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    char client_ip[INET_ADDRSTRLEN] = "unknown";
    
    if (getpeername(client->socket, (struct sockaddr*)&addr, &addr_len) == 0) {
        inet_ntop(AF_INET, &addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    }
    
    // Request username - no retry limit
    while (!username_accepted) {
        send_message(client->socket, "Enter your username: ");
        
        // Clear the entire buffer first
        memset(buffer, 0, sizeof(buffer));
        
        // Receive username
        bytes = recv(client->socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            cleanup_client(client);
            return NULL;
        }
        
        buffer[bytes] = '\0';
        
        // Remove newline
        newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';
        
        // Check length
        if (strlen(buffer) > MAX_USERNAME_LEN) {
            send_message(client->socket, COLOR_RED "[ERROR] Username too long. Maximum 16 characters allowed.\n" COLOR_RESET);
            log_activity("[REJECTED] Username too long attempted: %s (length: %zu)", buffer, strlen(buffer));
            
            // IMPORTANT: Clear any remaining data in the socket buffer
            // This prevents the extra characters from being read as the next input
            int flags = fcntl(client->socket, F_GETFL, 0);
            fcntl(client->socket, F_SETFL, flags | O_NONBLOCK);
            
            char discard[1024];
            while (recv(client->socket, discard, sizeof(discard), 0) > 0) {
                // Discard remaining data
            }
            
            fcntl(client->socket, F_SETFL, flags); // Restore blocking mode
            continue;
        }
        
        // Validate username characters
        if (!validate_username(buffer)) {
            send_message(client->socket, COLOR_RED "[ERROR] Invalid username. Use alphanumeric characters only (max 16). Try again.\n" COLOR_RESET);
            continue;
        }
        
        // Check duplicate username
        pthread_mutex_lock(&clients_mutex);
        if (find_client_by_username(buffer) != NULL) {
            pthread_mutex_unlock(&clients_mutex);
            send_message(client->socket, COLOR_RED "[ERROR] Username already taken. Please choose another.\n" COLOR_RESET);
            log_activity("[REJECTED] Duplicate username attempted: %s", buffer);
            continue;
        }
        
        // Username accepted
        strcpy(client->username, buffer);
        username_accepted = 1;
        pthread_mutex_unlock(&clients_mutex);
    }
    
    send_message(client->socket, COLOR_GREEN "[Server]: Welcome! Type /help for commands.\n" COLOR_RESET);
    
    // Now we have both username and IP, so log it properly
    printf("[CONNECT] Client connected: user=%s from %s\n", client->username, client_ip);
    log_activity("[LOGIN] user '%s' connected from %s", client->username, client_ip);
    
    // Main client loop
    while (server_running && client->active) {
        memset(buffer, 0, sizeof(buffer));
        bytes = recv(client->socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes <= 0) {
            break;
        }
        
        buffer[bytes] = '\0';
        // Remove newline
        newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';
        
        // Parse command
        if (buffer[0] == '/') {
            char command[32], param1[256], param2[MAX_MESSAGE_LEN];
            memset(command, 0, sizeof(command));
            memset(param1, 0, sizeof(param1));
            memset(param2, 0, sizeof(param2));
            
            sscanf(buffer, "%31s %255s %[^\n]", command, param1, param2);
            
            if (strcmp(command, "/join") == 0) {
                printf("[COMMAND] %s joined room '%s'\n", client->username, param1);
                handle_join(client, param1);
            } else if (strcmp(command, "/leave") == 0) {
                handle_leave(client);
            } else if (strcmp(command, "/broadcast") == 0) {
                // Reconstruct message
                char* msg_start = strstr(buffer, " ");
                if (msg_start) {
                    printf("[COMMAND] %s broadcasted to '%s'\n", client->username, client->current_room);
                    handle_broadcast(client, msg_start + 1);
                }
            } else if (strcmp(command, "/whisper") == 0) {
                printf("[COMMAND] %s sent whisper to %s\n", client->username, param1);
                handle_whisper(client, param1, param2);
            } else if (strcmp(command, "/sendfile") == 0) {
                printf("[COMMAND] %s initiated file transfer to %s\n", client->username, param2);
                handle_sendfile(client, param1, param2);
            } else if (strcmp(command, "/exit") == 0) {
                handle_exit(client);
                break;
            } else if (strcmp(command, "/help") == 0) {
                send_message(client->socket, 
                    "Commands:\n"
                    "/join <room_name> - Join or create a room\n"
                    "/leave - Leave current room\n"
                    "/broadcast <message> - Send message to room\n"
                    "/whisper <username> <message> - Send private message\n"
                    "/sendfile <filename> <username> - Send file to user\n"
                    "/exit - Disconnect from server\n");
            } else {
                send_message(client->socket, COLOR_RED "[Error]: Unknown command\n" COLOR_RESET);
            }
        } else {
            send_message(client->socket, COLOR_YELLOW "[Info]: Commands start with '/'. Type /help for help.\n" COLOR_RESET);
        }
    }
    
    cleanup_client(client);
    return NULL;
}

// Send message to client
void send_message(int socket, const char* message) {
    send(socket, message, strlen(message), 0);
}

// Log activity to file
void log_activity(const char* format, ...) {
    pthread_mutex_lock(&log_mutex);
    
    FILE* log_file = fopen(LOG_FILE, "a");
    if (!log_file) {
        pthread_mutex_unlock(&log_mutex);
        return;
    }
    
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(log_file, "%s - ", timestamp);
    
    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    
    fprintf(log_file, "\n");
    fclose(log_file);
    
    pthread_mutex_unlock(&log_mutex);
}

// Validate username
int validate_username(const char* username) {
    int len = strlen(username);
    if (len == 0 || len > MAX_USERNAME_LEN) return 0;
    
    for (int i = 0; i < len; i++) {
        if (!isalnum(username[i])) return 0;
    }
    return 1;
}

// Validate room name
int validate_room_name(const char* room_name) {
    int len = strlen(room_name);
    if (len == 0 || len > MAX_ROOM_NAME_LEN) return 0;
    
    for (int i = 0; i < len; i++) {
        if (!isalnum(room_name[i])) return 0;
    }
    return 1;
}

// Find client by username
Client* find_client_by_username(const char* username) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].username, username) == 0) {
            return &clients[i];
        }
    }
    return NULL;
}

// Find or create room
Room* find_or_create_room(const char* room_name) {
    pthread_mutex_lock(&rooms_mutex);
    
    // Find existing room
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (strlen(rooms[i].name) > 0 && strcmp(rooms[i].name, room_name) == 0) {
            pthread_mutex_unlock(&rooms_mutex);
            return &rooms[i];
        }
    }
    
    // Create new room
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (strlen(rooms[i].name) == 0) {
            strcpy(rooms[i].name, room_name);
            rooms[i].client_count = 0;
            pthread_mutex_unlock(&rooms_mutex);
            return &rooms[i];
        }
    }
    
    pthread_mutex_unlock(&rooms_mutex);
    return NULL;
}

// Check if user has been in this room before
int has_visited_room(Client* client, const char* room_name) {
    for (int i = 0; i < client->visited_room_count; i++) {
        if (strcmp(client->previous_rooms[i], room_name) == 0) {
            return 1;
        }
    }
    return 0;
}

// Add room to visited list
void add_to_visited_rooms(Client* client, const char* room_name) {
    // Check if already in list
    if (has_visited_room(client, room_name)) {
        return;
    }
    
    // Add to list if space available
    if (client->visited_room_count < MAX_ROOMS) {
        strcpy(client->previous_rooms[client->visited_room_count], room_name);
        client->visited_room_count++;
    }
}

// Handle join command
void handle_join(Client* client, const char* room_name) {
    if (!validate_room_name(room_name)) {
        send_message(client->socket, COLOR_RED "[Error]: Invalid room name. Use alphanumeric characters only (max 32).\n" COLOR_RESET);
        return;
    }
    
    Room* room = find_or_create_room(room_name);
    if (!room) {
        send_message(client->socket, COLOR_RED "[Error]: No available rooms. Try again later.\n" COLOR_RESET);
        return;
    }
    
    pthread_mutex_lock(&room->mutex);
    
    // Check room capacity
    if (room->client_count >= MAX_ROOM_CAPACITY) {
        pthread_mutex_unlock(&room->mutex);
        send_message(client->socket, COLOR_RED "[Error]: Room is full (max 15 users per room).\n" COLOR_RESET);
        log_activity("[ROOM-FULL] user '%s' tried to join full room '%s'", client->username, room_name);
        return;
    }
    
    // Store previous room for logging
    char previous_room[MAX_ROOM_NAME_LEN + 1] = "";
    int switching_rooms = 0;
    
    // Check if already in a room
    if (strlen(client->current_room) > 0) {
        strcpy(previous_room, client->current_room);
        switching_rooms = 1;
        // Remove from current room but don't log yet
        remove_client_from_room(client);
        
        // Notify others in old room
        char msg[256];
        snprintf(msg, sizeof(msg), "[Server]: %s left the room", client->username);
        broadcast_room(previous_room, msg, NULL);
    }
    
    // Check if rejoining
    int is_rejoin = has_visited_room(client, room_name);
    
    // Add client to room
    int client_id = client - clients;
    room->client_ids[room->client_count++] = client_id;
    strcpy(client->current_room, room_name);
    
    // Add to visited rooms
    add_to_visited_rooms(client, room_name);
    
    pthread_mutex_unlock(&room->mutex);
    
    char msg[256];
    snprintf(msg, sizeof(msg), COLOR_GREEN "[Server]: You %s the room '%s'\n" COLOR_RESET, 
             is_rejoin ? "rejoined" : "joined", room_name);
    send_message(client->socket, msg);
    
    // Notify others in room
    snprintf(msg, sizeof(msg), "[Server]: %s %s the room", client->username, 
             is_rejoin ? "rejoined" : "joined");
    broadcast_room(room_name, msg, NULL);
    
    // Log appropriately
    if (switching_rooms) {
        log_activity("[ROOM] user '%s' left room '%s', joined '%s'", 
                    client->username, previous_room, room_name);
    } else if (is_rejoin) {
        log_activity("[ROOM] user '%s' rejoined '%s'", client->username, room_name);
    } else {
        log_activity("[JOIN] user '%s' joined room '%s'", client->username, room_name);
    }
}

// Handle leave command  
void handle_leave(Client* client) {
    if (strlen(client->current_room) == 0) {
        send_message(client->socket, COLOR_YELLOW "[Info]: You are not in any room.\n" COLOR_RESET);
        return;
    }
    
    char room_name[MAX_ROOM_NAME_LEN + 1];
    strcpy(room_name, client->current_room);
    
    remove_client_from_room(client);
    
    char msg[256];
    snprintf(msg, sizeof(msg), COLOR_GREEN "[Server]: You left the room '%s'\n" COLOR_RESET, room_name);
    send_message(client->socket, msg);
    
    // Notify others
    snprintf(msg, sizeof(msg), "[Server]: %s left the room", client->username);
    broadcast_room(room_name, msg, NULL);
    
    // Log the leave action
    log_activity("[LEAVE] user '%s' left room '%s'", client->username, room_name);
}

// Remove client from current room
void remove_client_from_room(Client* client) {
    if (strlen(client->current_room) == 0) return;
    
    pthread_mutex_lock(&rooms_mutex);
    
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (strcmp(rooms[i].name, client->current_room) == 0) {
            pthread_mutex_lock(&rooms[i].mutex);
            
            int client_id = client - clients;
            // Remove client from room
            for (int j = 0; j < rooms[i].client_count; j++) {
                if (rooms[i].client_ids[j] == client_id) {
                    // Shift remaining clients
                    for (int k = j; k < rooms[i].client_count - 1; k++) {
                        rooms[i].client_ids[k] = rooms[i].client_ids[k + 1];
                    }
                    rooms[i].client_count--;
                    break;
                }
            }
            
            // Delete room if empty
            if (rooms[i].client_count == 0) {
                memset(rooms[i].name, 0, sizeof(rooms[i].name));
            }
            
            pthread_mutex_unlock(&rooms[i].mutex);
            break;
        }
    }
    
    pthread_mutex_unlock(&rooms_mutex);
    
    memset(client->current_room, 0, sizeof(client->current_room));
}

// Broadcast message to room
void broadcast_room(const char* room_name, const char* message, const char* sender) {
    pthread_mutex_lock(&rooms_mutex);
    
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (strcmp(rooms[i].name, room_name) == 0) {
            pthread_mutex_lock(&rooms[i].mutex);
            
            char formatted_msg[MAX_MESSAGE_LEN + 256];
            if (sender) {
                snprintf(formatted_msg, sizeof(formatted_msg), "[%s]: %s\n", sender, message);
            } else {
                snprintf(formatted_msg, sizeof(formatted_msg), "%s\n", message);
            }
            
            for (int j = 0; j < rooms[i].client_count; j++) {
                Client* target = &clients[rooms[i].client_ids[j]];
                if (target->active && (!sender || strcmp(target->username, sender) != 0)) {
                    send_message(target->socket, formatted_msg);
                }
            }
            
            pthread_mutex_unlock(&rooms[i].mutex);
            break;
        }
    }
    
    pthread_mutex_unlock(&rooms_mutex);
}

// Handle broadcast command
void handle_broadcast(Client* client, const char* message) {
    if (strlen(client->current_room) == 0) {
        send_message(client->socket, COLOR_RED "[Error]: You must join a room first.\n" COLOR_RESET);
        return;
    }
    
    if (strlen(message) == 0) {
        send_message(client->socket, COLOR_RED "[Error]: Message cannot be empty.\n" COLOR_RESET);
        return;
    }
    
    send_message(client->socket, COLOR_GREEN "[Server]: Message sent to room\n" COLOR_RESET);
    broadcast_room(client->current_room, message, client->username);
    
    log_activity("[BROADCAST] user '%s': %s", client->username, message);
}

// Handle whisper command
void handle_whisper(Client* client, const char* target_user, const char* message) {
    if (strlen(target_user) == 0 || strlen(message) == 0) {
        send_message(client->socket, COLOR_RED "[Error]: Usage: /whisper <username> <message>\n" COLOR_RESET);
        return;
    }
    
    // Check if trying to whisper to self
    if (strcmp(client->username, target_user) == 0) {
        send_message(client->socket, COLOR_RED "[Error]: You cannot whisper to yourself.\n" COLOR_RESET);
        return;
    }
    
    pthread_mutex_lock(&clients_mutex);
    Client* target = find_client_by_username(target_user);
    
    if (!target) {
        pthread_mutex_unlock(&clients_mutex);
        send_message(client->socket, COLOR_RED "[Error]: User not found.\n" COLOR_RESET);
        return;
    }
    
    char formatted_msg[MAX_MESSAGE_LEN + 256];
    snprintf(formatted_msg, sizeof(formatted_msg), COLOR_BLUE "[Whisper from %s]: %s\n" COLOR_RESET, client->username, message);
    send_message(target->socket, formatted_msg);
    
    pthread_mutex_unlock(&clients_mutex);
    
    send_message(client->socket, COLOR_GREEN "[Server]: Whisper sent\n" COLOR_RESET);
    log_activity("[WHISPER] from '%s' to '%s': %s", client->username, target_user, message);
}

// Format file size for better readability
char* format_file_size(size_t bytes, char* buffer, size_t buf_size) {
    if (bytes < 1024) {
        snprintf(buffer, buf_size, "%zu bytes", bytes);
    } else if (bytes < 1024 * 1024) {
        snprintf(buffer, buf_size, "%.2f KB", bytes / 1024.0);
    } else {
        snprintf(buffer, buf_size, "%.2f MB", bytes / (1024.0 * 1024.0));
    }
    return buffer;
}

// Check if user already received file with same name
int check_duplicate_file(const char* receiver_username, const char* filename, const char* sender) {
    int receiver_index = -1;
    
    // Find receiver's index
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].username, receiver_username) == 0) {
            receiver_index = i;
            break;
        }
    }
    
    if (receiver_index == -1) return 0; // User not found
    
    ReceivedFile* current = received_files[receiver_index];
    while (current != NULL) {
        if (strcmp(current->filename, filename) == 0) {
            // Same filename found - check if from different sender
            if (strcmp(current->sender, sender) != 0) {
                return 1; // Duplicate from different sender
            }
        }
        current = current->next;
    }
    
    return 0;
}

// Add file to received files list
void add_received_file(const char* receiver_username, const char* filename, const char* sender) {
    int receiver_index = -1;
    
    // Find receiver's index
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].username, receiver_username) == 0) {
            receiver_index = i;
            break;
        }
    }
    
    if (receiver_index == -1) return;
    
    ReceivedFile* new_file = malloc(sizeof(ReceivedFile));
    strcpy(new_file->filename, filename);
    strcpy(new_file->sender, sender);
    new_file->timestamp = time(NULL);
    new_file->next = received_files[receiver_index];
    received_files[receiver_index] = new_file;
}

// Handle sendfile command
void handle_sendfile(Client* client, const char* filename, const char* target_user) {
    if (strlen(filename) == 0 || strlen(target_user) == 0) {
        send_message(client->socket, COLOR_RED "[Error]: Usage: /sendfile <filename> <username>\n" COLOR_RESET);
        return;
    }
    
    // Check file extension
    const char* ext = strrchr(filename, '.');
    if (!ext || (strcmp(ext, ".txt") != 0 && strcmp(ext, ".pdf") != 0 && 
                 strcmp(ext, ".jpg") != 0 && strcmp(ext, ".png") != 0)) {
        send_message(client->socket, COLOR_RED "[Error]: Invalid file type. Allowed: .txt, .pdf, .jpg, .png\n" COLOR_RESET);
        return;
    }
    
    // Check if trying to send file to self
    if (strcmp(client->username, target_user) == 0) {
        send_message(client->socket, COLOR_RED "[Error]: You cannot send files to yourself.\n" COLOR_RESET);
        return;
    }
    
    pthread_mutex_lock(&clients_mutex);
    Client* target = find_client_by_username(target_user);
    
    if (!target) {
        pthread_mutex_unlock(&clients_mutex);
        send_message(client->socket, COLOR_RED "[Error]: User not found.\n" COLOR_RESET);
        return;
    }
    pthread_mutex_unlock(&clients_mutex);
    
    // Check for duplicate file from different sender
    if (check_duplicate_file(target_user, filename, client->username)) {
        char msg[512];
        snprintf(msg, sizeof(msg), COLOR_RED "[Error]: %s already received a file named '%s' from another user. Please rename your file.\n" COLOR_RESET, 
                target_user, filename);
        send_message(client->socket, msg);
        log_activity("[FILE] Conflict: '%s' already sent to %s by another user", filename, target_user);
        return;
    }
    
    // Try to get real file stats
    struct stat file_stat;
    size_t file_size;
    char file_info[512];
    char size_buffer[64];
    
    if (stat(filename, &file_stat) == 0) {
        // Real file exists - get actual metadata
        file_size = file_stat.st_size;
        
        // Get file modification time
        char time_str[100];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
        
        // Get file type description
        const char* file_type = "Unknown";
        if (strcmp(ext, ".txt") == 0) file_type = "Text Document";
        else if (strcmp(ext, ".pdf") == 0) file_type = "PDF Document";
        else if (strcmp(ext, ".jpg") == 0) file_type = "JPEG Image";
        else if (strcmp(ext, ".png") == 0) file_type = "PNG Image";
        
        // Send detailed file info to sender
        snprintf(file_info, sizeof(file_info), 
                COLOR_CYAN "[File Info]\n"
                "  Name: %s\n"
                "  Type: %s\n"
                "  Size: %s\n"
                "  Modified: %s\n"
                "  Permissions: %o\n" COLOR_RESET,
                filename, file_type, 
                format_file_size(file_size, size_buffer, sizeof(size_buffer)), 
                time_str, file_stat.st_mode & 0777);
        
        send_message(client->socket, file_info);
        
        // Also notify receiver about incoming file with metadata
        snprintf(file_info, sizeof(file_info),
                COLOR_BLUE "[Incoming File Alert]\n"
                "  From: %s\n"
                "  File: %s\n"
                "  Type: %s\n"
                "  Size: %s\n"
                "  Modified: %s\n"
                "  Status: Waiting in queue...\n" COLOR_RESET,
                client->username, filename, file_type,
                format_file_size(file_size, size_buffer, sizeof(size_buffer)), 
                time_str);
        
        send_message(target->socket, file_info);
        
    } else {
        // File doesn't exist - simulate with random size
        file_size = 100 * 1024 + (rand() % (MAX_FILE_SIZE - 100 * 1024));
        
        char msg[256];
        snprintf(msg, sizeof(msg), COLOR_YELLOW "[Warning]: File not found locally. Using simulated size: %s\n" COLOR_RESET,
                format_file_size(file_size, size_buffer, sizeof(size_buffer)));
        send_message(client->socket, msg);
    }
    
    if (file_size > MAX_FILE_SIZE) {
        send_message(client->socket, COLOR_RED "[Error]: File exceeds 3MB limit.\n" COLOR_RESET);
        log_activity("[ERROR] File '%s' from user '%s' exceeds size limit (%.2f MB).", 
                    filename, client->username, file_size / (1024.0 * 1024.0));
        return;
    }
    
    // Check queue status
    pthread_mutex_lock(&upload_queue_mutex);
    int queue_position = upload_queue_size + 1;
    pthread_mutex_unlock(&upload_queue_mutex);
    
    if (queue_position > MAX_UPLOAD_QUEUE) {
        char msg[512];
        snprintf(msg, sizeof(msg), COLOR_YELLOW "[Server]: Upload queue is full. Your file will be queued at position %d. Estimated wait: %d seconds.\n" COLOR_RESET,
                queue_position, (queue_position - MAX_UPLOAD_QUEUE) * 5);
        send_message(client->socket, msg);
    } else {
        char msg[512];
        snprintf(msg, sizeof(msg), COLOR_GREEN "[Server]: File '%s' (%s) added to upload queue at position %d.\n" COLOR_RESET,
                filename, format_file_size(file_size, size_buffer, sizeof(size_buffer)), queue_position);
        send_message(client->socket, msg);
    }
    
    // Add to upload queue
    enqueue_file_upload(filename, client->username, target_user, file_size);
    
    log_activity("[COMMAND] %s initiated file transfer of '%s' (%.2f MB) to %s", 
                client->username, filename, file_size / (1024.0 * 1024.0), target_user);
}

// Enqueue file upload - IMPROVED VERSION
void enqueue_file_upload(const char* filename, const char* sender, const char* receiver, size_t size) {
    FileUpload* upload = malloc(sizeof(FileUpload));
    strcpy(upload->filename, filename);
    strcpy(upload->sender, sender);
    strcpy(upload->receiver, receiver);
    upload->size = size;
    upload->enqueue_time = time(NULL);  // Record enqueue time
    upload->next = NULL;
    
    pthread_mutex_lock(&upload_queue_mutex);
    
    // Count actual queue size
    int actual_queue_size = 0;
    FileUpload* temp = upload_queue;
    while (temp != NULL) {
        actual_queue_size++;
        temp = temp->next;
    }
    
    if (upload_queue == NULL) {
        upload_queue = upload;
    } else {
        FileUpload* current = upload_queue;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = upload;
    }
    
    upload_queue_size = actual_queue_size + 1;  // Update with actual size
    log_activity("[FILE-QUEUE] Upload '%s' from %s added to queue. Queue size: %d", 
                 filename, sender, upload_queue_size);
    
    pthread_mutex_unlock(&upload_queue_mutex);
    
    // Signal a worker thread to process this file
    sem_post(&upload_queue_sem);
}

// File upload worker thread - IMPROVED VERSION
void* file_upload_worker(void* arg) {
    int worker_id = *((int*)arg);
    free(arg);
    
    pthread_detach(pthread_self());
    log_activity("[INFO] File upload worker thread %d started and waiting for files", worker_id);
    
    while (server_running) {
        if (sem_wait(&upload_queue_sem) != 0) {
            if (!server_running) break;
            log_activity("[ERROR] Worker %d: sem_wait failed", worker_id);
            continue;
        }
        
        if (!server_running) break;
        
        pthread_mutex_lock(&upload_queue_mutex);
        
        if (upload_queue != NULL) {
            FileUpload* upload = upload_queue;
            upload_queue = upload_queue->next;
            
            // Recount queue size
            int remaining = 0;
            FileUpload* temp = upload_queue;
            while (temp != NULL) {
                remaining++;
                temp = temp->next;
            }
            upload_queue_size = remaining;
            
            pthread_mutex_unlock(&upload_queue_mutex);
            
            // Calculate wait time
            time_t wait_time = time(NULL) - upload->enqueue_time;
            if (wait_time > 0) {
                log_activity("[FILE] Worker %d: '%s' from user '%s' started upload after %ld seconds in queue.", 
                            worker_id, upload->filename, upload->sender, wait_time);
            }
            
            // Buffer for size formatting
            char size_buf[64];
            
            // Notify sender about processing
            Client* sender = find_client_by_username(upload->sender);
            if (sender && sender->active) {
                char msg[512];
                snprintf(msg, sizeof(msg), COLOR_YELLOW "[Server]: Worker %d processing file '%s' (%s)...\n" COLOR_RESET,
                        worker_id, upload->filename, format_file_size(upload->size, size_buf, sizeof(size_buf)));
                send_message(sender->socket, msg);
            }
            
            // Notify receiver about incoming file
            Client* receiver = find_client_by_username(upload->receiver);
            if (receiver && receiver->active) {
                char msg[512];
                snprintf(msg, sizeof(msg), COLOR_BLUE "[Server]: Incoming file from %s: '%s' (%s)\n" COLOR_RESET,
                        upload->sender, upload->filename, format_file_size(upload->size, size_buf, sizeof(size_buf)));
                send_message(receiver->socket, msg);
            }
            
            // Simulate file transfer
            log_activity("[FILE] Worker %d: Starting upload of '%s' from %s to %s (%s)", 
                        worker_id, upload->filename, upload->sender, upload->receiver, 
                        format_file_size(upload->size, size_buf, sizeof(size_buf)));
            
            // Show progress
            for (int i = 1; i <= 4; i++) {
                
                if (sender && sender->active) {
                    char progress_msg[256];
                    snprintf(progress_msg, sizeof(progress_msg), 
                            COLOR_YELLOW "[Server]: Upload progress: %d%%\n" COLOR_RESET, i * 25);
                    send_message(sender->socket, progress_msg);
                }
                
                if (receiver && receiver->active) {
                    char progress_msg[256];
                    snprintf(progress_msg, sizeof(progress_msg), 
                            COLOR_BLUE "[Status]: Download progress: %d%%\n" COLOR_RESET, i * 25);
                    send_message(receiver->socket, progress_msg);
                }
            }
            
            // Notify completion
            if (sender && sender->active) {
                char msg[512];
                snprintf(msg, sizeof(msg), COLOR_GREEN "[Server]: File '%s' sent successfully to %s!\n" COLOR_RESET,
                        upload->filename, upload->receiver);
                send_message(sender->socket, msg);
            }
            
            if (receiver && receiver->active) {
                char msg[512];
                snprintf(msg, sizeof(msg), COLOR_GREEN "[Server]: '[Received]' '%s' from '%s' (%s)\n" COLOR_RESET,
                        upload->filename, upload->sender, format_file_size(upload->size, size_buf, sizeof(size_buf)));
                send_message(receiver->socket, msg);
                
                // Show file info
                snprintf(msg, sizeof(msg), COLOR_GREEN "[Server]: File saved as: downloads/%s_%s\n" COLOR_RESET,
                        upload->sender, upload->filename);
                send_message(receiver->socket, msg);
            }
            
            log_activity("[SEND FILE] Worker %d: '%s' sent from %s to %s (success) - %s", 
                        worker_id, upload->filename, upload->sender, upload->receiver, 
                        format_file_size(upload->size, size_buf, sizeof(size_buf)));
            
            // Record the file as received
            add_received_file(upload->receiver, upload->filename, upload->sender);
            
            free(upload);
        } else {
            pthread_mutex_unlock(&upload_queue_mutex);
        }
    }
    
    log_activity("[INFO] File upload worker thread %d exiting", worker_id);
    return NULL;
}

// Handle exit command
void handle_exit(Client* client) {
    send_message(client->socket, COLOR_GREEN "[Server]: Disconnected. Goodbye!\n" COLOR_RESET);
    printf("[DISCONNECT] Client %s disconnected.\n", client->username);
    log_activity("[DISCONNECT] Client %s disconnected.", client->username);
}

// Cleanup client
void cleanup_client(Client* client) {
    pthread_mutex_lock(&clients_mutex);
    
    // Store username before clearing
    char username[MAX_USERNAME_LEN + 1];
    strcpy(username, client->username);
    
    if (strlen(client->current_room) > 0) {
        remove_client_from_room(client);
    }
    
    if (client->socket != -1) {
        close(client->socket);
        client->socket = -1;
    }
    
    client->active = 0;
    memset(client->username, 0, sizeof(client->username));
    
    // Free ReceivedFile structures
    int client_id = client - clients;
    ReceivedFile* current = received_files[client_id];
    while (current != NULL) {
        ReceivedFile* temp = current;
        current = current->next;
        free(temp);
    }
    received_files[client_id] = NULL;
    
    pthread_mutex_unlock(&clients_mutex);
    
    // Log with the stored username
    if (strlen(username) > 0) {
        log_activity("[DISCONNECT] user '%s' lost connection. Cleaned up resources.", username);
    }
}

// Signal handler
void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\n[SHUTDOWN] SIGINT received. Shutting down server...\n");
        
        // Count active clients
        int active_count = 0;
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active) {
                active_count++;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
        
        log_activity("[SHUTDOWN] SIGINT received. Disconnecting %d clients, saving logs.", active_count);
        
        server_running = 0;
        
        // Notify all clients
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active) {
                send_message(clients[i].socket, COLOR_RED "[Server]: Server is shutting down. Goodbye!\n" COLOR_RESET);
                close(clients[i].socket);
            }
        }
        pthread_mutex_unlock(&clients_mutex);
        
        // Close server socket
        close(server_socket);
        
        // Wake up all file upload workers
        for (int i = 0; i < MAX_UPLOAD_QUEUE; i++) {
            sem_post(&upload_queue_sem);
        }
        
        exit(0);
    }
}
