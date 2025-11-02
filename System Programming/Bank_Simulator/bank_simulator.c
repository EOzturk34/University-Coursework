/*
 * CSE 344 - Midterm Project
 * A simple Bank Simulator 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <errno.h>
#include <time.h>


/* =========================================================
 * COMMON DEFINITIONS
 * ========================================================= */
#define MAX_CLIENTS 20
#define MAX_ACCOUNT_ID_LEN 16
#define MAX_FIFO_NAME_LEN 256
#define MAX_LOG_LINE_LEN 1024
#define MAX_CLIENT_LINE_LEN 256
#define BUFFER_SIZE 1024

// Operation types
#define OP_DEPOSIT 1
#define OP_WITHDRAW 2

// Error codes
#define ERR_INVALID_OPERATION -1
#define ERR_INSUFFICIENT_FUNDS -2
#define ERR_ACCOUNT_NOT_FOUND -3
#define ERR_OPERATION_FAILED -4

// Semaphore operations
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// Message structure for communication between client and teller
typedef struct {
    int operation;           // OP_DEPOSIT or OP_WITHDRAW
    int amount;              // Amount of credits
    char account_id[MAX_ACCOUNT_ID_LEN]; // Account ID
    pid_t client_pid;        // Client process ID
    char client_fifo[MAX_FIFO_NAME_LEN]; // Client FIFO name for response
} ClientMessage;

// Message structure for response from teller to client
typedef struct {
    int status;              // 0 for success, negative for error
    int balance;             // Current balance after operation
    char account_id[MAX_ACCOUNT_ID_LEN]; // Account ID 
    char message[BUFFER_SIZE]; // Additional information or error message
} TellerResponse;

// Account structure for bank database
typedef struct {
    char account_id[MAX_ACCOUNT_ID_LEN];
    int balance;             // Current balance
    int total_deposits;      // Total amount deposited
    int total_withdrawals;   // Total amount withdrawn
    time_t last_operation;   // Timestamp of last operation
    int is_active;           // 1 if account is active, 0 if closed
} Account;

// Transaction structure for logging
typedef struct {
    char account_id[MAX_ACCOUNT_ID_LEN];
    int operation;           // OP_DEPOSIT or OP_WITHDRAW
    int amount;              // Amount of operation
    int result_balance;      // Balance after operation
    time_t timestamp;        // When the transaction happened
} Transaction;

// Shared memory structure for teller-server communication
typedef struct {
    ClientMessage request;   // Request from client
    TellerResponse response; // Response to be sent back
    int operation_status;    // Status of the operation (0 = success, negative = error)
} SharedMemory;

/* =========================================================
 * BANK SERVER DEFINITIONS
 * ========================================================= */
// Bank database
#define MAX_ACCOUNTS 100
#define BANK_LOG_FILE "AdaBank.bankLog"

// Semaphore indices
#define SEM_REQUEST_READY 0  // Teller signals server that request is ready
#define SEM_RESPONSE_READY 1 // Server signals teller that response is ready
#define SEM_MUTEX 2          // Mutual exclusion for shared memory access

// Server state
typedef struct {
    char server_fifo[MAX_FIFO_NAME_LEN];
    Account accounts[MAX_ACCOUNTS];
    int account_count;
    char bank_name[32];
    volatile sig_atomic_t running;
    Transaction *transactions;
    int transaction_count;
    int max_transactions;
    int shm_id;              // Shared memory ID
    int sem_id;              // Semaphore ID
    SharedMemory *shm_ptr;   // Pointer to shared memory
} BankServer;

// Function type for teller operations
typedef int (*TellerFunc)(void*);

// Server function prototypes
int init_server(BankServer *server, const char *bank_name, const char *server_fifo_name);
int create_server_fifo(BankServer *server);
void wait_for_clients(BankServer *server);
pid_t create_teller_process(BankServer *server, int client_fd);
int process_client_request(BankServer *server, ClientMessage *request, TellerResponse *response);
int create_account(BankServer *server, int amount, char *account_id);
int close_account(BankServer *server, const char *account_id);
int update_account(BankServer *server, const char *account_id, int operation, int amount);
void update_log_file(BankServer *server);
void cleanup_resources_server(BankServer *server);
void handle_signals_server(int signum);
void load_from_log_file(BankServer *server);
int find_account_index(BankServer *server, const char *account_id);
void record_transaction(BankServer *server, const char *account_id, int operation, int amount, int balance);
int setup_shared_memory(BankServer *server);
int setup_semaphores(BankServer *server);
int cleanup_ipc_resources(BankServer *server);
int sem_operation(int sem_id, int sem_num, int op);
int server_listener_process(BankServer* server);
void graceful_shutdown(BankServer *server);

// Teller operation functions as required 
int deposit(void* arg);
int withdraw(void* arg);

// implementation of Teller process creation
pid_t Teller(void* func, void* arg_func);
int waitTeller(pid_t pid, int* status);

/* =========================================================
 * BANK CLIENT DEFINITIONS
 * ========================================================= */
// Client request structure
typedef struct {
    char account_id[MAX_ACCOUNT_ID_LEN]; 
    char action[10]; 
    int amount;
} ClientRequest;

// Client state
typedef struct {
    char server_fifo[MAX_FIFO_NAME_LEN];
    char client_fifo[MAX_FIFO_NAME_LEN];
    pid_t pid;
    volatile sig_atomic_t running;
    ClientRequest *requests;
    int request_count;
} BankClient;

// Client function prototypes
int init_client(BankClient *client, const char *server_fifo);
int read_client_file(BankClient *client, const char *filename);
int create_client_fifo(BankClient *client);
int connect_to_server(BankClient *client);
int send_request(BankClient *client, ClientRequest *request);
int receive_response(BankClient *client, TellerResponse *response);
void cleanup_resources_client(BankClient *client);
void handle_signals_client(int signum);

/* =========================================================
 * GLOBAL VARIABLES
 * ========================================================= */
BankServer server;
BankClient client;

/* =========================================================
 * COMMON FUNCTIONS
 * ========================================================= */
// Helper function to handle errors
void handle_error(const char *msg) {
    perror(msg);
}

// Helper function to get formatted timestamp
char *get_timestamp(void) {
    static char buffer[64];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    strftime(buffer, sizeof(buffer), "%H:%M:%S", tm_info);
    return buffer;
}

// Semaphore operations helper
int sem_operation(int sem_id, int sem_num, int op) {
    struct sembuf sem_op;
    
    sem_op.sem_num = sem_num;
    sem_op.sem_op = op;
    sem_op.sem_flg = 0;
    
    return semop(sem_id, &sem_op, 1);
}

/* =========================================================
 * BANK SERVER IMPLEMENTATION
 * ========================================================= */
// Signal handler for server
void handle_signals_server(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        printf("\nSignal received, shutting down gracefully...\n");
         
        signal(SIGINT, SIG_IGN);
        signal(SIGTERM, SIG_IGN);
        
        update_log_file(&server);
        
        cleanup_ipc_resources(&server);
        
        if (server.server_fifo[0] != '\0') {
            unlink(server.server_fifo);
        }
        
        if (server.transactions) {
            free(server.transactions);
            server.transactions = NULL;
        }
        
        printf("%s says \"Bye\"...\n", server.bank_name);
      
        _exit(0);
    }
    
    server.running = 0;
}
// Find account by ID
int find_account_index(BankServer *server, const char *account_id) {
    for (int i = 0; i < server->account_count; i++) {
        if (strcmp(server->accounts[i].account_id, account_id) == 0 && 
            server->accounts[i].is_active) {
            return i;
        }
    }
    return -1; // Not found
}

// Record a transaction
void record_transaction(BankServer *server, const char *account_id, int operation, 
                        int amount, int balance) {
    // Ensure we have space for a new transaction
    if (server->transaction_count >= server->max_transactions) {
        server->max_transactions *= 2;
        server->transactions = realloc(server->transactions, 
                                      server->max_transactions * sizeof(Transaction));
        if (server->transactions == NULL) {
            handle_error("Failed to reallocate memory for transactions");
            return;
        }
    }
    
    // Record the transaction
    Transaction *t = &server->transactions[server->transaction_count];
    strncpy(t->account_id, account_id, MAX_ACCOUNT_ID_LEN - 1);
    t->operation = operation;
    t->amount = amount;
    t->result_balance = balance;
    t->timestamp = time(NULL);
    
    server->transaction_count++;
    
    // Update log file after each transaction
    update_log_file(server);
}

// Load accounts from log file
void load_from_log_file(BankServer *server) {
    FILE *log_file = fopen(BANK_LOG_FILE, "r");
    if (log_file == NULL) {
        return; // File doesn't exist, nothing to load
    }
    
    char line[MAX_LOG_LINE_LEN];
    int line_number = 0;
    server->account_count = 0; // Reset account count
    
    while (fgets(line, sizeof(line), log_file) != NULL) {
        line_number++;
        
        // Skip header and footer lines
        if (line[0] == '#' && line[1] == ' ') {
            continue;
        }
        if (strncmp(line, "## end", 6) == 0) {
            continue;
        }
        
        // Check for closed accounts (starting with #)
        int is_closed = 0;
        char *line_ptr = line;
        
        if (line[0] == '#') {
            is_closed = 1;
            line_ptr++; // Skip the '#' character
        }
        
        // Parse account information line: "BankID_XX D XXX W XXX XXX"
        char account_id[MAX_ACCOUNT_ID_LEN];
        int deposits, withdrawals, balance;
        
        if (sscanf(line_ptr, "%s D %d W %d %d", 
                account_id, &deposits, &withdrawals, &balance) == 4) {
            // Check for valid account ID
            if (strlen(account_id) < 2 || account_id[0] == ' ' || account_id[0] == '\t') {
                printf("Warning: Invalid account ID in log file at line %d\n", line_number);
                continue;
            }
            
            // Create or update account
            if (server->account_count < MAX_ACCOUNTS) {
                Account *account = &server->accounts[server->account_count];
                memset(account, 0, sizeof(Account));
                
                strncpy(account->account_id, account_id, MAX_ACCOUNT_ID_LEN - 1);
                account->balance = balance;
                account->total_deposits = deposits;
                account->total_withdrawals = withdrawals;
                account->last_operation = time(NULL);
                account->is_active = !is_closed;
                
                server->account_count++;
                printf("Loaded account %s with balance %d\n", account_id, balance);
            } else {
                printf("Warning: Maximum number of accounts reached, skipping %s\n", account_id);
            }
        } else {
            printf("Warning: Invalid format in log file at line %d\n", line_number);
        }
    }
    
    fclose(log_file);
    printf("Loaded %d accounts from log file.\n", server->account_count);
}

// Set up shared memory for teller-server communication
int setup_shared_memory(BankServer *server) {
    key_t key = ftok("/tmp", 'A');
    if (key == -1) {
        handle_error("ftok failed");
        return -1;
    }
    
    // Create shared memory segment
    server->shm_id = shmget(key, sizeof(SharedMemory), IPC_CREAT | 0666);
    if (server->shm_id == -1) {
        handle_error("shmget failed");
        return -1;
    }
    
    // Attach to shared memory
    server->shm_ptr = (SharedMemory*)shmat(server->shm_id, NULL, 0);
    if (server->shm_ptr == (void*)-1) {
        handle_error("shmat failed");
        return -1;
    }
    
    // Initialize shared memory
    memset(server->shm_ptr, 0, sizeof(SharedMemory));
    
    return 0;
}

// Set up semaphores for synchronization
int setup_semaphores(BankServer *server) {
    key_t key = ftok("/tmp", 'B');
    if (key == -1) {
        handle_error("ftok failed for semaphores");
        return -1;
    }
    
    // Create semaphore set 
    server->sem_id = semget(key, 3, IPC_CREAT | 0666);
    if (server->sem_id == -1) {
        handle_error("semget failed");
        return -1;
    }
    
    // Initialize semaphores
    union semun arg;
    arg.val = 0; // Initial value for request_ready
    if (semctl(server->sem_id, SEM_REQUEST_READY, SETVAL, arg) == -1) {
        handle_error("semctl failed for request_ready");
        return -1;
    }
    
    arg.val = 0; // Initial value for response_ready
    if (semctl(server->sem_id, SEM_RESPONSE_READY, SETVAL, arg) == -1) {
        handle_error("semctl failed for response_ready");
        return -1;
    }
    
    arg.val = 1; // Initial value for mutex (unlocked)
    if (semctl(server->sem_id, SEM_MUTEX, SETVAL, arg) == -1) {
        handle_error("semctl failed for mutex");
        return -1;
    }
    
    return 0;
}

// Clean up IPC resources (shared memory and semaphores)
int cleanup_ipc_resources(BankServer *server) {
    int result = 0;
    
    // Detach from shared memory
    if (server->shm_ptr != NULL && server->shm_ptr != (void*)-1) {
        if (shmdt(server->shm_ptr) == -1) {
            handle_error("shmdt failed");
            result = -1;
        }
    }
    
    // Remove shared memory
    if (server->shm_id != -1) {
        if (shmctl(server->shm_id, IPC_RMID, NULL) == -1) {
            handle_error("shmctl failed");
            result = -1;
        }
    }
    
    // Remove semaphore set
    if (server->sem_id != -1) {
        if (semctl(server->sem_id, 0, IPC_RMID) == -1) {
            handle_error("semctl failed for removal");
            result = -1;
        }
    }
    
    return result;
}

// Initialize server
int init_server(BankServer *server, const char *bank_name, const char *server_fifo_name) {
    memset(server, 0, sizeof(BankServer));
    
    strncpy(server->bank_name, bank_name, sizeof(server->bank_name) - 1);
    strncpy(server->server_fifo, server_fifo_name, sizeof(server->server_fifo) - 1);
    
    server->running = 1;
    server->account_count = 0;
    server->shm_id = -1;
    server->sem_id = -1;
    server->shm_ptr = NULL;
    
    // Initialize transaction log
    server->max_transactions = 1000; // Start with space for 1000 transactions
    server->transactions = malloc(server->max_transactions * sizeof(Transaction));
    if (server->transactions == NULL) {
        handle_error("Failed to allocate memory for transactions");
        return -1;
    }
    server->transaction_count = 0;
    
    // Set up signal handlers
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signals_server;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    
    // Set up shared memory and semaphores
    if (setup_shared_memory(server) != 0 || setup_semaphores(server) != 0) {
        cleanup_ipc_resources(server);
        free(server->transactions);
        return -1;
    }
    
    printf("%s is active....\n", server->bank_name);
    
    // Check if log file exists
    if (access(BANK_LOG_FILE, F_OK) != 0) {
        printf("No previous logs.. Creating the bank database\n");
    } else {
        printf("Loading previous logs from %s\n", BANK_LOG_FILE);
        load_from_log_file(server);
    }
    
    return 0;
}

// Create server FIFO
int create_server_fifo(BankServer *server) {
    // Remove FIFO if it already exists
    unlink(server->server_fifo);
    
    // Create server FIFO
    if (mkfifo(server->server_fifo, 0666) == -1) {
        handle_error("Failed to create server FIFO");
        return -1;
    }
    
    return 0;
}

// Wait for client connections
void wait_for_clients(BankServer *server) {
    int server_fd;
    
    printf("Waiting for clients @%s...\n", server->server_fifo);
    
    // Open server FIFO for reading
    server_fd = open(server->server_fifo, O_RDONLY);
    if (server_fd == -1) {
        handle_error("Failed to open server FIFO");
        return;
    }
    
    pid_t client_pid;
    int client_count = 0;
    
    // Read client information once
    ssize_t bytes_read = read(server_fd, &client_pid, sizeof(pid_t));
    if (bytes_read <= 0) {
        close(server_fd);
        return;
    }
    
    bytes_read = read(server_fd, &client_count, sizeof(int));
    if (bytes_read <= 0) {
        close(server_fd);
        return;
    }
    
    printf(" - Received %d clients from PID%d..\n", client_count, (int)client_pid);
    
    // Close and reopen the FIFO for each client to get a fresh file descriptor
    close(server_fd);
    
    // Process each client one by one
    for (int i = 0; i < client_count && server->running; i++) {
        // Open a new FIFO connection for each client message
        server_fd = open(server->server_fifo, O_RDONLY);
        if (server_fd == -1) {
            handle_error("Failed to reopen server FIFO");
            continue;
        }
        
        // Create a teller process for this client
        pid_t teller_pid = create_teller_process(server, server_fd);
        if (teller_pid > 0) {
            printf(" -- Teller PID%d is active serving Client%02d...\n", 
                   (int)teller_pid, i + 1);
            
            // Wait for teller to finish using our custom waitTeller function
            int status;
            waitTeller(teller_pid, &status);
        } else {
            printf("Failed to create teller for Client%02d\n", i + 1);
        }
        
        // Close the server_fd in the parent since the teller will handle it
        close(server_fd);
    }
}

// Create a teller process using our custom Teller function
pid_t create_teller_process(BankServer *server, int client_fd) {
    // Read client request first to determine operation type
    ClientMessage request;
    ssize_t bytes_read = read(client_fd, &request, sizeof(ClientMessage));
    if (bytes_read != sizeof(ClientMessage)) {
        printf("Failed to read client request\n");
        return -1;
    }
    
    // Create operation argument structure (we'll pass this to the teller process)
    int *arg = malloc(sizeof(int) * 2);
    if (!arg) {
        handle_error("Failed to allocate memory for teller argument");
        return -1;
    }
    
    // Store client_fd and request operation in arg
    arg[0] = client_fd;
    arg[1] = request.operation;
    
    // Copy request to shared memory for teller to access
    sem_operation(server->sem_id, SEM_MUTEX, -1); // Lock mutex
    memcpy(&server->shm_ptr->request, &request, sizeof(ClientMessage));
    sem_operation(server->sem_id, SEM_MUTEX, 1); // Unlock mutex
    
    // Create teller process based on operation type
    pid_t teller_pid;
    if (request.operation == OP_DEPOSIT) {
        teller_pid = Teller(deposit, arg);
    } else if (request.operation == OP_WITHDRAW) {
        teller_pid = Teller(withdraw, arg);
    } else {
        printf("Invalid operation type: %d\n", request.operation);
        free(arg);
        return -1;
    }
    
    return teller_pid;
}

// Process client request (called by the deposit/withdraw functions)
int process_client_request(BankServer *server, ClientMessage *request, TellerResponse *response) {
    // Initialize response
    memset(response, 0, sizeof(TellerResponse));
    
    // Copy account ID to response
    strncpy(response->account_id, request->account_id, MAX_ACCOUNT_ID_LEN - 1);
    
    // Process based on operation type
    if (request->operation == OP_DEPOSIT) {
        // Handle deposit
        if (strncmp(request->account_id, "N", 1) == 0) {
            // New account
            if (create_account(server, request->amount, response->account_id) == 0) {
                int index = find_account_index(server, response->account_id);
                if (index >= 0) {
                    response->balance = server->accounts[index].balance;
                    strcpy(response->message, "New account created successfully");
                    record_transaction(server, response->account_id, OP_DEPOSIT, 
                                       request->amount, response->balance);
                    printf("Client deposited %d credits... updating log\n", request->amount);
                } else {
                    response->status = ERR_OPERATION_FAILED;
                    strcpy(response->message, "Failed to create account");
                }
            } else {
                response->status = ERR_OPERATION_FAILED;
                strcpy(response->message, "Failed to create account");
            }
        } else {
            // Existing account
            int result = update_account(server, request->account_id, OP_DEPOSIT, request->amount);
            if (result == 0) {
                int index = find_account_index(server, request->account_id);
                if (index >= 0) {
                    response->balance = server->accounts[index].balance;
                    strcpy(response->message, "Deposit successful");
                    record_transaction(server, request->account_id, OP_DEPOSIT, 
                                       request->amount, response->balance);
                    printf("Client deposited %d credits updating log\n", request->amount);
                } else {
                    response->status = ERR_ACCOUNT_NOT_FOUND;
                    strcpy(response->message, "Account not found after deposit");
                }
            } else if (result == ERR_ACCOUNT_NOT_FOUND) {
                response->status = ERR_ACCOUNT_NOT_FOUND;
                strcpy(response->message, "Account not found");
            } else {
                response->status = ERR_OPERATION_FAILED;
                strcpy(response->message, "Deposit operation failed");
            }
        }
    } else if (request->operation == OP_WITHDRAW) {
        // Handle withdrawal
        int result = update_account(server, request->account_id, OP_WITHDRAW, request->amount);
        if (result == 0) {
            int index = find_account_index(server, request->account_id);
            if (index >= 0) {
                response->balance = server->accounts[index].balance;
                strcpy(response->message, "Withdrawal successful");
                record_transaction(server, request->account_id, OP_WITHDRAW, 
                                   request->amount, response->balance);
                printf("Client withdraws %d credits.. updating log\n", request->amount);
                
                // Check if account is now empty
                if (response->balance == 0) {
                    close_account(server, request->account_id);
                    strcpy(response->message, "account closed");
                    printf("Bye Client %s\n", request->account_id);
                }
            } else {
                response->status = ERR_ACCOUNT_NOT_FOUND;
                strcpy(response->message, "Account not found after withdrawal");
            }
        } else if (result == ERR_INSUFFICIENT_FUNDS) {
            response->status = ERR_INSUFFICIENT_FUNDS;
            strcpy(response->message, "Insufficient funds");
            printf("Client withdraws %d credit.. operation not permitted.\n", request->amount);
        } else if (result == ERR_ACCOUNT_NOT_FOUND) {
            response->status = ERR_ACCOUNT_NOT_FOUND;
            strcpy(response->message, "Account not found");
        } else {
            response->status = ERR_OPERATION_FAILED;
            strcpy(response->message, "Withdrawal operation failed");
        }
    } else {
        // Invalid operation
        response->status = ERR_INVALID_OPERATION;
        strcpy(response->message, "Invalid operation");
    }
    
    return response->status;
}

// Create a new account
int create_account(BankServer *server, int amount, char *account_id) {
    if (server->account_count >= MAX_ACCOUNTS) {
        return -1; // No space for new accounts
    }
    
    // Generate a new account ID if not provided
    if (strncmp(account_id, "N", 1) == 0) {
        // Find the highest account ID number and increment by 1
        int max_id = 0;
        for (int i = 0; i < server->account_count; i++) {
            if (strncmp(server->accounts[i].account_id, "BankID_", 7) == 0) {
                int current_id;
                if (sscanf(server->accounts[i].account_id + 7, "%d", &current_id) == 1) {
                    if (current_id > max_id) {
                        max_id = current_id;
                    }
                }
            }
        }
        
        sprintf(account_id, "BankID_%02d", max_id + 1);
    }
    
    // Check if account already exists but is inactive
    for (int i = 0; i < server->account_count; i++) {
        if (strcmp(server->accounts[i].account_id, account_id) == 0) {
            if (!server->accounts[i].is_active) {
                // Reactivate account
                server->accounts[i].is_active = 1;
                server->accounts[i].balance += amount;
                server->accounts[i].total_deposits += amount;
                server->accounts[i].last_operation = time(NULL);
                return 0;
            }
            // Account already exists and is active
            return -1;
        }
    }
    
    // Create new account
    Account *account = &server->accounts[server->account_count];
    strncpy(account->account_id, account_id, MAX_ACCOUNT_ID_LEN - 1);
    account->balance = amount;
    account->total_deposits = amount;
    account->total_withdrawals = 0;
    account->last_operation = time(NULL);
    account->is_active = 1;
    
    server->account_count++;
    
    return 0;
}

// Close an account
int close_account(BankServer *server, const char *account_id) {
    for (int i = 0; i < server->account_count; i++) {
        if (strcmp(server->accounts[i].account_id, account_id) == 0) {
            // Mark account as inactive instead of removing
            server->accounts[i].is_active = 0;
            server->accounts[i].last_operation = time(NULL);
            return 0;
        }
    }
    
    return ERR_ACCOUNT_NOT_FOUND;
}

// Update account balance
int update_account(BankServer *server, const char *account_id, int operation, int amount) {
    for (int i = 0; i < server->account_count; i++) {
        if (strcmp(server->accounts[i].account_id, account_id) == 0 && 
            server->accounts[i].is_active) {
            
            if (operation == OP_DEPOSIT) {
                server->accounts[i].balance += amount;
                server->accounts[i].total_deposits += amount;
                server->accounts[i].last_operation = time(NULL);
                return 0;
            } else if (operation == OP_WITHDRAW) {
                if (server->accounts[i].balance >= amount) {
                    server->accounts[i].balance -= amount;
                    server->accounts[i].total_withdrawals += amount;
                    server->accounts[i].last_operation = time(NULL);
                    return 0;
                } else {
                    return ERR_INSUFFICIENT_FUNDS;
                }
            }
        }
    }
    
    return ERR_ACCOUNT_NOT_FOUND;
}


// Update log file
void update_log_file(BankServer *server) {
    
    char temp_file[256];
    sprintf(temp_file, "%s.tmp", BANK_LOG_FILE);
    
    FILE *log_file = fopen(temp_file, "w");
    if (log_file == NULL) {
        handle_error("Failed to open temporary log file for writing");
        return;
    }
    
    // Write header
    char timestamp[64];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%H:%M %B %d %Y", tm_info);
    
    fprintf(log_file, "# %s Log file updated @%s \n", server->bank_name, timestamp);
    
    // Write account info
    for (int i = 0; i < server->account_count; i++) {
        Account *account = &server->accounts[i];
        if (account->is_active || account->total_deposits > 0) {
            // Ensure account_id is not empty
            if (account->account_id[0] == '\0' || strlen(account->account_id) < 2) {
                // Skip entries with invalid account IDs
                continue;
            }
            
            // Check if account is closed (balance is 0 but there were transactions)
            if (!account->is_active && account->balance == 0) {
                // Closed account: add '#' at the beginning of the line
                fprintf(log_file, "#%s D %d W %d %d\n", 
                        account->account_id, 
                        account->total_deposits, 
                        account->total_withdrawals, 
                        account->balance);
            } else {
                // Active account: write normally
                fprintf(log_file, "%s D %d W %d %d\n", 
                        account->account_id, 
                        account->total_deposits, 
                        account->total_withdrawals, 
                        account->balance);
            }
        }
    }
    
    fprintf(log_file, "## end of log.\n");
    
    // Flush buffers and ensure write completes
    fflush(log_file);
    fsync(fileno(log_file));
    fclose(log_file);
  
    if (rename(temp_file, BANK_LOG_FILE) != 0) {
        handle_error("Failed to rename temporary log file");
        unlink(temp_file);
        return;
    }
    
    printf("Log file updated successfully.\n");
}

// Clean up resources for server
void cleanup_resources_server(BankServer *server) {
    
    cleanup_ipc_resources(server);
  
    printf("Removing ServerFIFO...\n");
    unlink(server->server_fifo);
    
    if (server->transactions) {
        free(server->transactions);
        server->transactions = NULL;
    }
}

void graceful_shutdown(BankServer *server) {
  
    printf("Updating log file before shutdown...\n");
    update_log_file(server);
    
    printf("Cleaning up resources...\n");
   
    cleanup_ipc_resources(server);
    
    printf("Removing ServerFIFO...\n");
    unlink(server->server_fifo);
    
    if (server->transactions) {
        free(server->transactions);
        server->transactions = NULL;
    }
    
    printf("%s says \"Bye\"...\n", server->bank_name);
}

/* =========================================================
 * TELLER OPERATIONS IMPLEMENTATION (for full credit)
 * ========================================================= */

// Deposit operation - called by teller process
int deposit(void* arg) {
    
    // Get shared memory and semaphore IDs
    key_t shm_key = ftok("/tmp", 'A');
    key_t sem_key = ftok("/tmp", 'B');
    
    if (shm_key == -1 || sem_key == -1) {
        perror("ftok failed in teller");
        free(arg);
        return -1;
    }
    
    // Attach to shared memory
    int shm_id = shmget(shm_key, sizeof(SharedMemory), 0);
    if (shm_id == -1) {
        perror("shmget failed in teller");
        free(arg);
        return -1;
    }
    
    SharedMemory *shm_ptr = (SharedMemory*)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void*)-1) {
        perror("shmat failed in teller");
        free(arg);
        return -1;
    }
    
    // Get semaphore set
    int sem_id = semget(sem_key, 3, 0);
    if (sem_id == -1) {
        perror("semget failed in teller");
        shmdt(shm_ptr);
        free(arg);
        return -1;
    }
    
    // Process has started, print info
    printf("Teller (deposit) process started. Waiting for server to process request...\n");
    
    // Signal server to process the request
    sem_operation(sem_id, SEM_REQUEST_READY, 1);
    
    // Wait for server to process the request
    sem_operation(sem_id, SEM_RESPONSE_READY, -1);
    
    // Get the response from shared memory
    TellerResponse response;
    sem_operation(sem_id, SEM_MUTEX, -1); // Lock mutex
    memcpy(&response, &shm_ptr->response, sizeof(TellerResponse));
    sem_operation(sem_id, SEM_MUTEX, 1); // Unlock mutex
    
    // Open client FIFO for response
    int client_fifo_fd = open(shm_ptr->request.client_fifo, O_WRONLY);
    if (client_fifo_fd == -1) {
        perror("Failed to open client FIFO for response");
        shmdt(shm_ptr);
        free(arg);
        return -1;
    }
    
    // Send response to client
    if (write(client_fifo_fd, &response, sizeof(TellerResponse)) != sizeof(TellerResponse)) {
        perror("Failed to send response to client");
    } else {
        printf("Teller: Response sent to client PID%d\n", shm_ptr->request.client_pid);
    }
    
    close(client_fifo_fd);
    shmdt(shm_ptr);
    free(arg);
    
    printf("Teller (deposit) process completed successfully\n");
    return 0;
}

// Withdraw operation - called by teller process
int withdraw(void* arg) {
    // Get shared memory and semaphore IDs
    key_t shm_key = ftok("/tmp", 'A');
    key_t sem_key = ftok("/tmp", 'B');
    
    if (shm_key == -1 || sem_key == -1) {
        perror("ftok failed in teller");
        free(arg);
        return -1;
    }
    
    // Attach to shared memory
    int shm_id = shmget(shm_key, sizeof(SharedMemory), 0);
    if (shm_id == -1) {
        perror("shmget failed in teller");
        free(arg);
        return -1;
    }
    
    SharedMemory *shm_ptr = (SharedMemory*)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void*)-1) {
        perror("shmat failed in teller");
        free(arg);
        return -1;
    }
    
    // Get semaphore set
    int sem_id = semget(sem_key, 3, 0);
    if (sem_id == -1) {
        perror("semget failed in teller");
        shmdt(shm_ptr);
        free(arg);
        return -1;
    }
    
    // Process has started, print info
    printf("Teller (withdraw) process started. Waiting for server to process request...\n");
    
    // Signal server to process the request
    sem_operation(sem_id, SEM_REQUEST_READY, 1);
    
    // Wait for server to process the request
    sem_operation(sem_id, SEM_RESPONSE_READY, -1);
    
    // Get the response from shared memory
    TellerResponse response;
    sem_operation(sem_id, SEM_MUTEX, -1); // Lock mutex
    memcpy(&response, &shm_ptr->response, sizeof(TellerResponse));
    sem_operation(sem_id, SEM_MUTEX, 1); // Unlock mutex
    
    // Open client FIFO for response
    int client_fifo_fd = open(shm_ptr->request.client_fifo, O_WRONLY);
    if (client_fifo_fd == -1) {
        perror("Failed to open client FIFO for response");
        shmdt(shm_ptr);
        free(arg);
        return -1;
    }
    
    // Send response to client
    if (write(client_fifo_fd, &response, sizeof(TellerResponse)) != sizeof(TellerResponse)) {
        perror("Failed to send response to client");
    } else {
        printf("Teller: Response sent to client PID%d\n", shm_ptr->request.client_pid);
    }
    
    close(client_fifo_fd);
    shmdt(shm_ptr);
    free(arg);
    
    printf("Teller (withdraw) process completed successfully\n");
    return 0;
}

/* =========================================================
 * FULL CREDIT IMPLEMENTATION OF TELLER AND WAITTELLER
 * ========================================================= */

// Custom Teller function for process creation
pid_t Teller(void* func, void* arg_func) {
    // This is the full credit implementation that creates a process
    // to execute only the function specified by func with the argument arg_func
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Fork failed in Teller");
        return -1;
    } else if (pid == 0) {
        // Child process (teller)
        // Cast func to the appropriate function type and call it
        int result = ((TellerFunc)func)(arg_func);
        exit(result); // Exit with the result of the function
    }
    
    // Parent process returns the teller PID
    return pid;
}

// Custom waitTeller function to wait for teller process
int waitTeller(pid_t pid, int* status) {
    // This is the full credit implementation that waits for a specific teller process
    return waitpid(pid, status, 0);
}

/* =========================================================
 * BANK CLIENT IMPLEMENTATION
 * ========================================================= */
// Signal handler for client
void handle_signals_client(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        client.running = 0;
    }
}

// Initialize client
int init_client(BankClient *client, const char *server_fifo) {
    memset(client, 0, sizeof(BankClient));
    
    client->pid = getpid();
    strncpy(client->server_fifo, server_fifo, sizeof(client->server_fifo) - 1);
    
    // Generate client FIFO name
    snprintf(client->client_fifo, sizeof(client->client_fifo), 
            "/tmp/client_%d_fifo", client->pid);
    
    client->running = 1;
    client->request_count = 0;
    
    // Set up signal handlers
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signals_client;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    
    return 0;
}

int read_client_file(BankClient *client, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        handle_error("Failed to open client file");
        return -1;
    }
    
    // Count lines first to allocate memory
    int line_count = 0;
    char line[MAX_CLIENT_LINE_LEN];
    
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strlen(line) > 1) {
            line_count++;
        }
    }
    
    fseek(file, 0, SEEK_SET);
    
    client->requests = malloc(line_count * sizeof(ClientRequest));
    if (client->requests == NULL) {
        handle_error("Failed to allocate memory for requests");
        fclose(file);
        return -1;
    }
    
    client->request_count = 0;
    
    while (fgets(line, sizeof(line), file) != NULL && client->request_count < line_count) {
        if (strlen(line) <= 1) {
            continue;
        }
        
        ClientRequest *req = &client->requests[client->request_count];
        char account_id[MAX_ACCOUNT_ID_LEN];
        char action[10];
        int amount;
        
        if (sscanf(line, "%s %s %d", account_id, action, &amount) == 3) {
            strncpy(req->account_id, account_id, MAX_ACCOUNT_ID_LEN - 1);
            strncpy(req->action, action, sizeof(req->action) - 1);
            req->amount = amount;
            client->request_count++;
        }
    }
    
    fclose(file);
    printf("Reading %s..\n%d clients to connect.. creating clients..\n", 
            filename, client->request_count);
    
    return client->request_count;
}

// Create client FIFO
int create_client_fifo(BankClient *client) {
    // Remove FIFO if it already exists
    unlink(client->client_fifo);
    
    // Create client FIFO
    if (mkfifo(client->client_fifo, 0666) == -1) {
        handle_error("Failed to create client FIFO");
        return -1;
    }
    
    return 0;
}

// Connect to server
int connect_to_server(BankClient *client) {
    int server_fd;
    
    // Open server FIFO for writing
    server_fd = open(client->server_fifo, O_WRONLY);
    if (server_fd == -1) {
        printf("Cannot connect %s...\n", client->server_fifo);
        return -1;
    }
    
    printf("Connected to Adabank..\n");
    
    // Send client PID and request count to server
    if (write(server_fd, &client->pid, sizeof(pid_t)) == -1 ||
        write(server_fd, &client->request_count, sizeof(int)) == -1) {
        close(server_fd);
        handle_error("Failed to send client information to server");
        return -1;
    }
    
    // Keep the server FIFO open for further communication
    return server_fd;
}

int send_request(BankClient *client, ClientRequest *request) {
    int server_fd;
    ClientMessage msg;
    
    server_fd = open(client->server_fifo, O_WRONLY);
    if (server_fd == -1) {
        handle_error("Failed to open server FIFO for sending request");
        return -1;
    }
    
    if (strcmp(request->action, "deposit") == 0) {
        msg.operation = OP_DEPOSIT;
    } else if (strcmp(request->action, "withdraw") == 0) {
        msg.operation = OP_WITHDRAW;
    } else {
        close(server_fd);
        return ERR_INVALID_OPERATION;
    }
    
    msg.amount = request->amount;
    strncpy(msg.account_id, request->account_id, MAX_ACCOUNT_ID_LEN - 1); 
    msg.client_pid = client->pid;
    strncpy(msg.client_fifo, client->client_fifo, MAX_FIFO_NAME_LEN - 1);
    
    printf("Sending request: %s %d credits to account %s\n", 
           (msg.operation == OP_DEPOSIT) ? "depositing" : "withdrawing",
           msg.amount, msg.account_id);
    
    if (write(server_fd, &msg, sizeof(ClientMessage)) == -1) {
        handle_error("Failed to send request to server");
        close(server_fd);
        return -1;
    }
    
    close(server_fd);
    return 0;
}
// Receive response from server
int receive_response(BankClient *client, TellerResponse *response) {
    int client_fd;
    
    // Open client FIFO for reading
    client_fd = open(client->client_fifo, O_RDONLY);
    if (client_fd == -1) {
        handle_error("Failed to open client FIFO for reading response");
        return -1;
    }
    
    // Read response with timeout (using select)
    fd_set readfds;
    struct timeval tv;
    
    FD_ZERO(&readfds);
    FD_SET(client_fd, &readfds);
    
    // Set timeout to 5 seconds
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    
    int select_result = select(client_fd + 1, &readfds, NULL, NULL, &tv);
    
    if (select_result == -1) {
        handle_error("Failed in select call");
        close(client_fd);
        return -1;
    } else if (select_result == 0) {
        // Timeout occurred
        printf("Timeout waiting for server response\n");
        close(client_fd);
        return -1;
    }
    
    // Data is available, read it
    if (read(client_fd, response, sizeof(TellerResponse)) <= 0) {
        handle_error("Failed to read response from server");
        close(client_fd);
        return -1;
    }
    
    close(client_fd);
    return 0;
}

// Clean up resources for client
void cleanup_resources_client(BankClient *client) {
    // Remove client FIFO
    printf("Cleaning up resources...\n");
    unlink(client->client_fifo);
    
    // Free allocated memory
    if (client->requests) {
        free(client->requests);
        client->requests = NULL;
    }
    
    printf("exiting..\n");
}


int server_listener_process(BankServer* server) {
    printf("Server listener process started\n");
    
    while (server->running) {
        // Wait for a request to be ready
        sem_operation(server->sem_id, SEM_REQUEST_READY, -1);
        
        // Check if server is still running 
        if (!server->running) {
            break;
        }
        
        printf("Server: Request received from teller, processing...\n");
        
        // Process the request
        ClientMessage request;
        TellerResponse response;
        
        // Get the request from shared memory
        sem_operation(server->sem_id, SEM_MUTEX, -1); // Lock mutex
        memcpy(&request, &server->shm_ptr->request, sizeof(ClientMessage));
        sem_operation(server->sem_id, SEM_MUTEX, 1); // Unlock mutex
        
        // Process the request
        process_client_request(server, &request, &response);
        
        // Put the response in shared memory
        sem_operation(server->sem_id, SEM_MUTEX, -1); // Lock mutex
        memcpy(&server->shm_ptr->response, &response, sizeof(TellerResponse));
        sem_operation(server->sem_id, SEM_MUTEX, 1); // Unlock mutex
        
        // Signal that response is ready
        sem_operation(server->sem_id, SEM_RESPONSE_READY, 1);
        
        printf("Server: Response sent to teller\n");
    }
    
    printf("Server listener process exiting\n");
    return 0;
}

/* =========================================================
 * MAIN FUNCTIONS
 * ========================================================= */
int server_main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s BankName ServerFIFO_Name\n", argv[0]);
        return 1;
    }
    
    // Initialize server
    if (init_server(&server, argv[1], argv[2]) != 0) {
        return 1;
    }
    
    // Create server FIFO
    if (create_server_fifo(&server) != 0) {
        cleanup_resources_server(&server);
        return 1;
    }
    
    pid_t listener_pid = fork();
    
    if (listener_pid < 0) {
        handle_error("Failed to create listener process");
        cleanup_resources_server(&server);
        return 1;
    } else if (listener_pid == 0) {
        // Child process - this will be our listener
        int result = server_listener_process(&server);
        exit(result); // Exit the listener process
    }
    
    // Parent process continues as main server
    printf("Listener process created with PID: %d\n", (int)listener_pid);
    
    // Main server loop
    while (server.running) {
        wait_for_clients(&server);
    }
    
    printf("Server shutting down...\n");
    
    // Signal the listener process to exit
    server.running = 0;
    sem_operation(server.sem_id, SEM_REQUEST_READY, 1);
    
    // Wait for listener process to exit
    int status;
    waitpid(listener_pid, &status, 0);
    printf("Listener process exited with status: %d\n", WEXITSTATUS(status));
    
    graceful_shutdown(&server);
    
    return 0;
}
int client_main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ClientFile> #ServerFIFO_Name\n", argv[0]);
        return 1;
    }
    
    // Skip the '#' character in server FIFO name if present
    const char *server_fifo = argv[2];
    if (server_fifo[0] == '#') {
        server_fifo++;
    }
    
    // Initialize client
    if (init_client(&client, server_fifo) != 0) {
        return 1;
    }
    
    // Read client file
    if (read_client_file(&client, argv[1]) <= 0) {
        cleanup_resources_client(&client);
        return 1;
    }
    
    // Create client FIFO for receiving responses
    if (create_client_fifo(&client) != 0) {
        cleanup_resources_client(&client);
        return 1;
    }
    
    // Connect to server
    if (connect_to_server(&client) == -1) {
        cleanup_resources_client(&client);
        return 1;
    }
    
    // Process each request
    for (int i = 0; i < client.request_count && client.running; i++) {
        ClientRequest *req = &client.requests[i];
        TellerResponse response;
        
        // Display request info
        if (strcmp(req->account_id, "N") == 0) {
            printf("Client%02d connected..%sing %d credits\n", 
                   i + 1, req->action, req->amount);
        } else {
            printf("Client%02d connected..%sing %d credits\n", 
                   i + 1, req->action, req->amount);
        }
        
        // Send request and receive response
        if (send_request(&client, req) == 0) {
            if (receive_response(&client, &response) == 0) {
                // Process response
                if (response.status == 0) {
                    printf("Client%02d served.. %s\n", i + 1, response.account_id);
                    if (strcmp(response.message, "account closed") == 0) {
                        printf("account closed\n");
                    } else {
                        printf("Balance: %d credits - %s\n", response.balance, response.message);
                    }
                } else {
                    printf("Client%02d something went WRONG: %s\n", i + 1, response.message);
                }
            } else {
                printf("Client%02d failed to receive response\n", i + 1);
            }
        } else {
            printf("Client%02d failed to send request\n", i + 1);
        }
        
        // Small delay between requests to avoid overwhelming the server
        usleep(100000); // 100ms
        
        if (i < client.request_count - 1) {
        printf("------------------------------------------------\n");
    }
    }
    
    // Clean up resources
    cleanup_resources_client(&client);
    
    return 0;
}

/* =========================================================
 * MAIN FUNCTION
 * ========================================================= */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: \n");
        fprintf(stderr, "  %s BankServer BankName ServerFIFO_Name\n", argv[0]);
        fprintf(stderr, "  %s BankClient <ClientFile> #ServerFIFO_Name\n", argv[0]);
        return 1;
    }
    
    // Check if we're running as server or client
    if (strcmp(argv[1], "BankServer") == 0) {
        // Shift arguments to remove the "BankServer" argument
        return server_main(argc - 1, argv + 1);
    } else if (strcmp(argv[1], "BankClient") == 0) {
        // Shift arguments to remove the "BankClient" argument
        return client_main(argc - 1, argv + 1);
    } else {
        fprintf(stderr, "Unknown mode: %s\n", argv[1]);
        return 1;
    }
}

