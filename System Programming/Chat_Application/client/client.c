#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_MESSAGE_LEN 1024
#define MAX_USERNAME_LEN 16

// ANSI color codes
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_RESET "\033[0m"

int client_socket;
int running = 1;

// Function prototypes
void* receive_messages(void* arg);
void send_command(const char* command);
void print_help();
void signal_handler(int sig);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        exit(1);
    }
    
    char* server_ip = argv[1];
    int port = atoi(argv[2]);
    
    // Set up signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    // Connect to server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address\n");
        exit(1);
    }
    
    printf("Connecting to %s:%d...\n", server_ip, port);
    
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }
    
    printf(COLOR_GREEN "Connected to server!\n" COLOR_RESET);
    
    // Start receiver thread
    pthread_t receiver_thread;
    pthread_create(&receiver_thread, NULL, receive_messages, NULL);
    pthread_detach(receiver_thread); // Detach the receiver thread
    
    // Main input loop
    char buffer[MAX_MESSAGE_LEN];
    while (running) {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // Remove newline
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len-1] == '\n') {
                buffer[len-1] = '\0';
            }
            
            // Send to server
            if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
                perror("Send failed");
                break;
            }
            
            // Check for exit command
            if (strcmp(buffer, "/exit") == 0) {
                running = 0;
                break;
            }
        }
    }
    
    // Cleanup
    close(client_socket);
    pthread_cancel(receiver_thread);
    pthread_join(receiver_thread, NULL);
    
    return 0;
}

// Receive messages from server
void* receive_messages(void* arg) {
    char buffer[MAX_MESSAGE_LEN];
    
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes <= 0) {
            if (running) {
                printf(COLOR_RED "\nDisconnected from server\n" COLOR_RESET);
                running = 0;
            }
            break;
        }
        
        buffer[bytes] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }
    
    return NULL;
}

// Signal handler
void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\n" COLOR_YELLOW "Disconnecting...\n" COLOR_RESET);
        running = 0;
        if (client_socket > 0) {
            send(client_socket, "/exit", 5, 0);
            close(client_socket);
        }
        exit(0);
    }
}