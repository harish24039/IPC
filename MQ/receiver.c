#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define QUEUE_NAME "/myqueue"
#define MAX_MSG_SIZE 8192   // maximum allowed size on this system (8 KB)
#define MAX_USER_MSG_SIZE 1024*1024*100  // Allow up to 10 MB messages
#define CHUNK_HEADER_SIZE 100  // Size for chunk metadata

// Structure to hold reassembly information
typedef struct {
    size_t msg_id;
    size_t total_chunks;
    size_t received_chunks;
    char *data;
    size_t total_size;
    size_t *chunk_received;  // Track which chunks have been received
} MessageAssembly;

// Free message assembly structure
void free_message_assembly(MessageAssembly *assembly) {
    if (assembly) {
        free(assembly->data);
        free(assembly->chunk_received);
        free(assembly);
    }
}

int main() {
    mqd_t mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    char *buffer = malloc(MAX_MSG_SIZE);
    MessageAssembly **assemblies = calloc(100, sizeof(MessageAssembly*));  // Support up to 100 concurrent messages
    size_t max_assemblies = 100;

    printf("Message Queue Receiver ready.\n");

    while (1) {
        ssize_t bytes = mq_receive(mq, buffer, MAX_MSG_SIZE, NULL);
        if (bytes >= 0) {
            // Parse header
            if (bytes < CHUNK_HEADER_SIZE) {
                printf("Received malformed message (too small for header)\n");
                continue;
            }

            char header[CHUNK_HEADER_SIZE + 1];
            memcpy(header, buffer, CHUNK_HEADER_SIZE);
            header[CHUNK_HEADER_SIZE] = '\0';
            
            size_t msg_id, total_chunks, chunk_idx, chunk_size;
            sscanf(header, "%8zu%8zu%8zu%8zu", &msg_id, &total_chunks, &chunk_idx, &chunk_size);
            
            if (chunk_idx >= total_chunks) {
                printf("Invalid chunk index %zu >= %zu\n", chunk_idx, total_chunks);
                continue;
            }

            // Make sure we have room in our tracking array
            if (msg_id >= max_assemblies) {
                size_t new_size = msg_id * 2;
                assemblies = realloc(assemblies, new_size * sizeof(MessageAssembly*));
                memset(assemblies + max_assemblies, 0, (new_size - max_assemblies) * sizeof(MessageAssembly*));
                max_assemblies = new_size;
            }
            
            // Create assembly entry if this is the first chunk we've seen
            if (!assemblies[msg_id]) {
                assemblies[msg_id] = malloc(sizeof(MessageAssembly));
                assemblies[msg_id]->msg_id = msg_id;
                assemblies[msg_id]->total_chunks = total_chunks;
                assemblies[msg_id]->received_chunks = 0;
                assemblies[msg_id]->total_size = 0;
                assemblies[msg_id]->data = malloc(MAX_USER_MSG_SIZE);
                assemblies[msg_id]->chunk_received = calloc(total_chunks, sizeof(size_t));
            }
            
            MessageAssembly *assembly = assemblies[msg_id];
            
            // Copy this chunk's data into the right place in the assembly buffer
            if (!assembly->chunk_received[chunk_idx]) {
                size_t offset = chunk_idx * (MAX_MSG_SIZE - CHUNK_HEADER_SIZE);
                memcpy(assembly->data + offset, buffer + CHUNK_HEADER_SIZE, chunk_size);
                assembly->chunk_received[chunk_idx] = 1;
                assembly->received_chunks++;
                assembly->total_size += chunk_size;
                
                // If this was the last chunk, the message is complete
                if (assembly->received_chunks == assembly->total_chunks) {
                    printf("Complete message %zu received: %zu bytes, first char: %c\n", 
                           msg_id, assembly->total_size, assembly->data[0]);
                    
                    // Free this assembly
                    free_message_assembly(assembly);
                    assemblies[msg_id] = NULL;
                } else {
                    printf("Received chunk %zu/%zu for message %zu (%zu bytes)\n", 
                           chunk_idx + 1, total_chunks, msg_id, chunk_size);
                }
            }
        } else {
            perror("mq_receive");
            break;
        }
    }

    // Clean up
    for (size_t i = 0; i < max_assemblies; i++) {
        free_message_assembly(assemblies[i]);
    }
    free(assemblies);
    free(buffer);
    mq_close(mq);
    return 0;
}
