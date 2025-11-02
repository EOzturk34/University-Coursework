#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define NUM_SATELLITES 5
#define NUM_ENGINEERS 3
#define MAX_TIMEOUT 6  // seconds

// Structure to represent a satellite request
typedef struct {
    int satelliteId;
    int priority;
    pthread_mutex_t mutex;
    sem_t requestHandled;
    bool isActive;
} SatelliteRequest;

// Shared resources
int availableEngineers = NUM_ENGINEERS;
pthread_mutex_t engineerMutex = PTHREAD_MUTEX_INITIALIZER;
sem_t newRequest;
bool allSatellitesProcessed = false;

// Priority queue for satellite requests
SatelliteRequest* requestQueue[NUM_SATELLITES];
int queueSize = 0;

// Function to add a request to the priority queue
void addToQueue(SatelliteRequest* request) {
    int i = queueSize++;
    
    // Move elements with lower priority down to find the right position
    while (i > 0 && requestQueue[i-1]->priority < request->priority) {
        requestQueue[i] = requestQueue[i-1];
        i--;
    }
    
    // Insert the new request at the right position
    requestQueue[i] = request;
}

// Function to remove the highest priority request from the queue
SatelliteRequest* removeHighestPriority() {
    if (queueSize == 0) {
        return NULL;
    }
    
    // Get the highest priority request (at the front of the queue)
    SatelliteRequest* highestPriorityRequest = requestQueue[0];
    
    // Shift remaining elements up
    for (int i = 0; i < queueSize - 1; i++) {
        requestQueue[i] = requestQueue[i+1];
    }
    
    queueSize--;
    return highestPriorityRequest;
}

// Function to handle timeout for a satellite
void* timeoutHandler(void* arg) {
    SatelliteRequest* request = (SatelliteRequest*)arg;
    
    // Wait for the timeout period
    sleep(MAX_TIMEOUT);
    
    // Check if the satellite is still active (waiting)
    pthread_mutex_lock(&request->mutex);
    if (request->isActive) {
        request->isActive = false;
        printf("[TIMEOUT] Satellite %d timeout %d second.\n", request->satelliteId, MAX_TIMEOUT);
        
        // Remove this request from the queue if it's still there
        pthread_mutex_lock(&engineerMutex);
        for (int i = 0; i < queueSize; i++) {
            if (requestQueue[i]->satelliteId == request->satelliteId) {
                // Shift remaining elements up
                for (int j = i; j < queueSize - 1; j++) {
                    requestQueue[j] = requestQueue[j+1];
                }
                queueSize--;
                break;
            }
        }
        pthread_mutex_unlock(&engineerMutex);
    }
    pthread_mutex_unlock(&request->mutex);
    
    return NULL;
}

// Function representing a satellite thread
void* satellite(void* arg) {
    int id = *((int*)arg);
    free(arg);
    
    // Generate a random priority (1 to 4)
    int priority = rand() % 4 + 1;
    
    // Create a satellite request
    SatelliteRequest* request = (SatelliteRequest*)malloc(sizeof(SatelliteRequest));
    request->satelliteId = id;
    request->priority = priority;
    request->isActive = true;
    pthread_mutex_init(&request->mutex, NULL);
    sem_init(&request->requestHandled, 0, 0);
    
    // Try to get an engineer
    pthread_mutex_lock(&engineerMutex);
    printf("[SATELLITE] Satellite %d requesting (priority %d)\n", id, priority);
    
    // Add the request to the queue
    addToQueue(request);
    
    // Signal that a new request has arrived
    sem_post(&newRequest);
    pthread_mutex_unlock(&engineerMutex);
    
    // Create a timeout thread
    pthread_t timeoutThread;
    pthread_create(&timeoutThread, NULL, timeoutHandler, (void*)request);
    pthread_detach(timeoutThread);
    
    // Wait for the request to be handled or timeout
    sem_wait(&request->requestHandled);
    
    // If we get here, the request was handled (not timed out)
    pthread_mutex_lock(&request->mutex);
    if (request->isActive) {
        request->isActive = false;
    }
    pthread_mutex_unlock(&request->mutex);
    
    // Cleanup
    pthread_mutex_destroy(&request->mutex);
    sem_destroy(&request->requestHandled);
    free(request);
    
    return NULL;
}

// Function representing an engineer thread
void* engineer(void* arg) {
    int id = *((int*)arg);
    free(arg);
    
    while (1) {
        // Wait for a new request
        sem_wait(&newRequest);
        
        // Get the highest priority request
        pthread_mutex_lock(&engineerMutex);
        
        // Check if it's time to exit first
        if (allSatellitesProcessed && availableEngineers == NUM_ENGINEERS && queueSize == 0) {
            printf("[ENGINEER %d] Exiting...\n", id);
            pthread_mutex_unlock(&engineerMutex);
            break;
        }
        
        SatelliteRequest* request = removeHighestPriority();
        
        if (request == NULL) {
            pthread_mutex_unlock(&engineerMutex);
            continue;
        }
        
        // Mark that an engineer is busy
        availableEngineers--;
        pthread_mutex_unlock(&engineerMutex);
        
        // Check if the satellite is still active
        pthread_mutex_lock(&request->mutex);
        if (request->isActive) {
            printf("[ENGINEER %d] Handling Satellite %d (Priority %d)\n", id, request->satelliteId, request->priority);
            pthread_mutex_unlock(&request->mutex);
            
            // Simulate work
            sleep(1 + rand() % 3);  // Random time between 1-3 seconds
            
            // Signal that the request has been handled
            pthread_mutex_lock(&request->mutex);
            if (request->isActive) {
                sem_post(&request->requestHandled);
                printf("[ENGINEER %d] Finished Satellite %d\n", id, request->satelliteId);
            }
            pthread_mutex_unlock(&request->mutex);
        } else {
            pthread_mutex_unlock(&request->mutex);
        }
        
        // Mark that an engineer is available again
        pthread_mutex_lock(&engineerMutex);
        availableEngineers++;
        
        // Check if there are more requests in the queue
        if (queueSize > 0) {
            // Signal that an engineer is available to handle another request
            sem_post(&newRequest);
        }
        
        pthread_mutex_unlock(&engineerMutex);
    }
    
    return NULL;
}

int main() {
    // Seed the random number generator
    srand(time(NULL));
    
    // Initialize semaphore
    sem_init(&newRequest, 0, 0);
    
    // Create engineer threads
    pthread_t engineerThreads[NUM_ENGINEERS];
    for (int i = 0; i < NUM_ENGINEERS; i++) {
        int* id = (int*)malloc(sizeof(int));
        *id = i;
        pthread_create(&engineerThreads[i], NULL, engineer, (void*)id);
    }
    
    // Create satellite threads with some delay between them
    pthread_t satelliteThreads[NUM_SATELLITES];
    for (int i = 0; i < NUM_SATELLITES; i++) {
        int* id = (int*)malloc(sizeof(int));
        *id = i;
        pthread_create(&satelliteThreads[i], NULL, satellite, (void*)id);
        usleep(500000);  // 0.5 second delay between satellite creation
    }
    
     
    sleep(NUM_SATELLITES * 3);
    
  
    pthread_mutex_lock(&engineerMutex);
    allSatellitesProcessed = true;
    pthread_mutex_unlock(&engineerMutex);
    
    
    for (int i = 0; i < NUM_ENGINEERS; i++) {
        sem_post(&newRequest);
    }
    
  
    sleep(1);
    
    // Wait for engineer threads to finish
    for (int i = 0; i < NUM_ENGINEERS; i++) {
        pthread_join(engineerThreads[i], NULL);
    }
    
    // Cleanup
    sem_destroy(&newRequest);
    pthread_mutex_destroy(&engineerMutex);
    
    return 0;
}
