

#ifndef BUFFER_H
#define BUFFER_H

#include <pthread.h>

typedef struct {
    char **data;           // Array of string lines
    int size;              // Maximum buffer size
    int count;             // Current number of items
    int in;                // Index for next write
    int out;               // Index for next read
    int done;              // Flag to indicate producer is done
    pthread_mutex_t mutex; // Mutex for protecting buffer
    pthread_cond_t full;   // Condition variable for buffer full
    pthread_cond_t empty;  // Condition variable for buffer empty
} Buffer;

/**
 * Initialize the buffer with the given size
 * @param buffer Pointer to the buffer
 * @param size Size of the buffer
 * @return 0 on success, -1 on failure
 */
int buffer_init(Buffer *buffer, int size);

/**
 * Add a line to the buffer
 * @param buffer Pointer to the buffer
 * @param line Line to add (will be copied)
 * @return 0 on success, -1 on failure
 */
int buffer_add(Buffer *buffer, char *line);

/**
 * Get a line from the buffer
 * @param buffer Pointer to the buffer
 * @return Line from buffer (caller must free) or NULL if buffer is empty and producer is done
 */
char *buffer_get(Buffer *buffer);

/**
 * Set the done flag to indicate producer is done
 * @param buffer Pointer to the buffer
 */
void buffer_set_done(Buffer *buffer);

/**
 * Check if buffer is empty and producer is done
 * @param buffer Pointer to the buffer
 * @return 1 if empty and done, 0 otherwise
 */
int buffer_is_empty_and_done(Buffer *buffer);

/**
 * Destroy the buffer and free all resources
 * @param buffer Pointer to the buffer
 */
void buffer_destroy(Buffer *buffer);

#endif /* BUFFER_H */
