#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/ipc_socket"

double time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6; // ms
}

int main() {
    int sockfd;
    struct sockaddr_un addr;
    char *buffer;
    size_t data_size;
    int choice;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect");
        exit(1);
    }

    while (1) {
        printf("\nSelect mode:\n");
        printf("1. Latency (round-trip)\n");
        printf("2. Throughput (one-way)\n");
        printf("3. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        if (choice == 3) {
            printf("Exiting...\n");
            break;
        }

        printf("Enter data size in bytes: ");
        scanf("%zu", &data_size);

        buffer = (char *)malloc(data_size);
        if (!buffer) {
            perror("malloc");
            exit(1);
        }
        memset(buffer, 'A', data_size);

        struct timespec start, end;

        if (choice == 1) { // Latency test
            int iterations = 1000;
            char ack[4];
            clock_gettime(CLOCK_MONOTONIC, &start);
            for (int i = 0; i < iterations; i++) {
                if (write(sockfd, &data_size, sizeof(data_size)) != sizeof(data_size)) {
                    perror("write size");
                    exit(1);
                }
                if (write(sockfd, buffer, data_size) != (ssize_t)data_size) {
                    perror("write data");
                    exit(1);
                }
                if (read(sockfd, ack, 3) != 3) {
                    perror("read ack");
                    exit(1);
                }
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            double total_ms = time_diff(start, end);
            printf("Average one-way latency: %.3f ms\n", (total_ms / iterations) / 2.0);
        } else if (choice == 2) { // Throughput test
            char ack[4];
            clock_gettime(CLOCK_MONOTONIC, &start);
            if (write(sockfd, &data_size, sizeof(data_size)) != sizeof(data_size)) {
                perror("write size");
                exit(1);
            }
            if (write(sockfd, buffer, data_size) != (ssize_t)data_size) {
                perror("write data");
                exit(1);
            }
            if (read(sockfd, ack, 3) != 3) { // ensure receiver got it
                perror("read ack");
                exit(1);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);

            double total_ms = time_diff(start, end);
            double mbps = (data_size / (1024.0 * 1024.0)) / (total_ms / 1000.0);
            printf("Throughput: %.2f MB/s (%.3f ms for %zu bytes)\n", mbps, total_ms, data_size);
        } else {
            printf("Invalid choice. Try again.\n");
        }

        free(buffer);
    }

    close(sockfd);
    return 0;
}
