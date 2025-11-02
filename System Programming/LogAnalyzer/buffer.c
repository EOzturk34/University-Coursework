
#include "buffer.h"
#include <stdlib.h>
#include <string.h>

int buffer_init(Buffer *buffer, int size) {
    buffer->data = (char **)malloc(size * sizeof(char *));
    if (buffer->data == NULL) {
        return -1;
    }
    
    buffer->size = size;
    buffer->count = 0;
    buffer->in = 0;
    buffer->out = 0;
    buffer->done = 0;
    
    if (pthread_mutex_init(&buffer->mutex, NULL) != 0) {
        free(buffer->data);
        return -1;
    }
    
    if (pthread_cond_init(&buffer->full, NULL) != 0) {
        pthread_mutex_destroy(&buffer->mutex);
        free(buffer->data);
        return -1;
    }
    
    if (pthread_cond_init(&buffer->empty, NULL) != 0) {
        pthread_cond_destroy(&buffer->full);
        pthread_mutex_destroy(&buffer->mutex);
        free(buffer->data);
        return -1;
    }
    
    return 0;
}

int buffer_add(Buffer *buffer, char *line) {
    pthread_mutex_lock(&buffer->mutex);
    
    // Wait until buffer is not full
    while (buffer->count == buffer->size) {
        pthread_cond_wait(&buffer->full, &buffer->mutex);
    }
    
    // Copy the line into the buffer
    if (line != NULL) {
        buffer->data[buffer->in] = strdup(line);
        if (buffer->data[buffer->in] == NULL) {
            pthread_mutex_unlock(&buffer->mutex);
            return -1;
        }
    } else {
        // Special case for EOF marker (NULL)
        buffer->data[buffer->in] = NULL;
    }
    
    // Update buffer state
    buffer->in = (buffer->in + 1) % buffer->size;
    buffer->count++;
    
    // Signal that buffer is not empty
    pthread_cond_signal(&buffer->empty);
    pthread_mutex_unlock(&buffer->mutex);
    
    return 0;
}

char *buffer_get(Buffer *buffer) {
    char *line;
    
    pthread_mutex_lock(&buffer->mutex);
    
    // Wait until buffer is not empty or producer is done
    while (buffer->count == 0) {
        if (buffer->done) {
            pthread_mutex_unlock(&buffer->mutex);
            return NULL;
        }
        pthread_cond_wait(&buffer->empty, &buffer->mutex);
    }
    
    // Get the line from the buffer
    line = buffer->data[buffer->out];
    
    // Update buffer state
    buffer->out = (buffer->out + 1) % buffer->size;
    buffer->count--;
    
    // Signal that buffer is not full
    pthread_cond_signal(&buffer->full);
    pthread_mutex_unlock(&buffer->mutex);
    
    return line;
}

void buffer_set_done(Buffer *buffer) {
    pthread_mutex_lock(&buffer->mutex);
    buffer->done = 1;
    // Wake up all consumers to check the done flag
    pthread_cond_broadcast(&buffer->empty);
    pthread_mutex_unlock(&buffer->mutex);
}

int buffer_is_empty_and_done(Buffer *buffer) {
    int result;
    
    pthread_mutex_lock(&buffer->mutex);
    result = (buffer->count == 0 && buffer->done);
    pthread_mutex_unlock(&buffer->mutex);
    
    return result;
}

void buffer_destroy(Buffer *buffer) {
    pthread_mutex_lock(&buffer->mutex);
    
    // Free all remaining lines in the buffer
    while (buffer->count > 0) {
        free(buffer->data[buffer->out]);
        buffer->out = (buffer->out + 1) % buffer->size;
        buffer->count--;
    }
    
    // Free the data array
    free(buffer->data);
    
    pthread_mutex_unlock(&buffer->mutex);
    
    // Destroy synchronization primitives
    pthread_mutex_destroy(&buffer->mutex);
    pthread_cond_destroy(&buffer->full);
    pthread_cond_destroy(&buffer->empty);
}
