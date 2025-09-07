#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define QUEUE_NAME "/myqueue"
#define MAX_MSG_SIZE 8192   // maximum allowed size on this system (8 KB)
#define MAX_USER_MSG_SIZE 1024*1024*100  // Allow up to 100 MB messages
#define MAX_MESSAGES 10
#define CHUNK_HEADER_SIZE 100  // Size for chunk metadata

double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec/1e9;
}

int main() {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    printf("Message Queue Sender ready.\n");

    int choice;
    size_t msg_size, iterations;

    while (1) {
        printf("\nMenu:\n");
        printf("1. Latency test (small messages)\n");
        printf("2. Throughput test (large messages)\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 3) break;

        printf("Enter message size in bytes (<= %d): ", MAX_USER_MSG_SIZE);
        scanf("%zu", &msg_size);
        printf("Enter number of messages: ");
        scanf("%zu", &iterations);

        char *data = malloc(msg_size);
        char *chunk = malloc(MAX_MSG_SIZE);
        
        double start = now_sec();
        
        for (size_t i = 0; i < iterations; i++) {
            memset(data, 'A' + (i % 26), msg_size); // fill with pattern
            
            // Calculate number of chunks needed
            size_t num_chunks = (msg_size + (MAX_MSG_SIZE - CHUNK_HEADER_SIZE - 1)) / 
                                (MAX_MSG_SIZE - CHUNK_HEADER_SIZE);
            
            for (size_t chunk_idx = 0; chunk_idx < num_chunks; chunk_idx++) {
                // Calculate offset and size for this chunk
                size_t offset = chunk_idx * (MAX_MSG_SIZE - CHUNK_HEADER_SIZE);
                size_t chunk_data_size = (offset + (MAX_MSG_SIZE - CHUNK_HEADER_SIZE) > msg_size) ?
                                        (msg_size - offset) : (MAX_MSG_SIZE - CHUNK_HEADER_SIZE);
                
                // Format header: [message_id][total_chunks][chunk_index][chunk_size]
                snprintf(chunk, CHUNK_HEADER_SIZE, "%08zu%08zu%08zu%08zu", 
                         i, num_chunks, chunk_idx, chunk_data_size);
                
                // Copy data for this chunk
                memcpy(chunk + CHUNK_HEADER_SIZE, data + offset, chunk_data_size);
                
                // Send this chunk
                if (mq_send(mq, chunk, CHUNK_HEADER_SIZE + chunk_data_size, 0) == -1) {
                    perror("mq_send");
                    goto cleanup;
                }
            }
        }
        
        double end = now_sec();
        double total_time = end - start;

        if (total_time < 1e-9) {
            printf("Transfer too fast to measure reliably.\n");
        } else if (choice == 1) {
            printf("Latency per message: %.6f ms\n",
                   (total_time/iterations)*1000);
        } else if (choice == 2) {
            double throughput = (msg_size * iterations) / (total_time * 1e6);
            printf("Throughput: %.2f MB/s\n", throughput);
        }

cleanup:
        free(data);
        free(chunk);
    }

    mq_close(mq);
    mq_unlink(QUEUE_NAME); // remove queue when done
    return 0;
}
