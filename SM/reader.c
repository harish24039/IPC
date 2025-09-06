// shmem_receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#define SHM_NAME "/myshm"
#define SHM_SIZE (1024*1024*110)

struct shm_region {
    size_t size;
    int ready;
    char buffer[1];
};

int main() {
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd < 0) { perror("shm_open"); exit(1); }

    struct shm_region *region = mmap(NULL, SHM_SIZE,
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (region == MAP_FAILED) { perror("mmap"); exit(1); }

    printf("Shared memory receiver ready.\n");

    while (1) {
        if (region->ready) {
            // read data
            size_t n = region->size;
            // For testing: just peek at first byte
            printf("Received %zu bytes. First char: %c\n",
       n, region->buffer[0]);

            __sync_synchronize();
            region->ready = 0; // clear flag
        }
    }

    munmap(region, SHM_SIZE);
    close(fd);
    return 0;
}
