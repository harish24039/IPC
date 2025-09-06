#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/ipc_socket"

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char *buffer = NULL;
    ssize_t bytes_read;

    // Create UNIX domain socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    unlink(SOCKET_PATH); // remove existing socket

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 1) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Receiver waiting for connection...\n");
    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
        perror("accept");
        exit(1);
    }
    printf("Receiver connected.\n");

    while (1) {
        size_t data_size;
        ssize_t n = read(client_fd, &data_size, sizeof(data_size));
        if (n == 0) {
            printf("Sender closed connection. Exiting.\n");
            break;
        } else if (n != sizeof(data_size)) {
            perror("read size");
            break;
        }

        buffer = (char *)malloc(data_size);
        if (!buffer) {
            perror("malloc");
            break;
        }

        size_t total_received = 0;
        while (total_received < data_size) {
            bytes_read = read(client_fd, buffer + total_received, data_size - total_received);
            if (bytes_read <= 0) {
                perror("read data");
                free(buffer);
                goto cleanup;
            }
            total_received += bytes_read;
        }

        // Send acknowledgment
        if (write(client_fd, "ACK", 3) != 3) {
            perror("write ack");
            free(buffer);
            break;
        }

        free(buffer);
    }

cleanup:
    if (buffer) free(buffer);
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
