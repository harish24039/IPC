// shmem_sender.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

#define SHM_NAME "/myshm"
#define SHM_SIZE (1024 * 1024 * 110) // ~110MB for safety

struct shm_region
{
    size_t size;
    int ready;
    char buffer[1]; // flexible array
};

double now_sec()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main()
{
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd < 0)
    {
        perror("shm_open");
        exit(1);
    }

    ftruncate(fd, SHM_SIZE);

    struct shm_region *region = mmap(NULL, SHM_SIZE,
                                     PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (region == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    printf("Shared memory sender ready.\n");

    int choice;
    size_t msg_size, iterations;
    while (1)
    {
        printf("\nMenu:\n");
        printf("1. Latency test (small messages)\n");
        printf("2. Throughput test (large message)\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 3)
            break;

        printf("Enter message size in bytes: ");
        scanf("%zu", &msg_size);
        printf("Enter number of messages: ");
        scanf("%zu", &iterations);

        char *data = malloc(msg_size);
        memset(data, 'A', msg_size);

        double start = now_sec();

        for (size_t i = 0; i < iterations; i++)
        {
            while (__sync_lock_test_and_set(&region->ready, 1))
            {
                // spin until receiver consumes
            }
            region->size = msg_size;

            // Fill buffer with a character based on message index
            memset(data, 'A' + (i % 26), msg_size);

            memcpy(region->buffer, data, msg_size);
            __sync_synchronize(); // memory barrier
        }

        double end = now_sec();
        double total_time = end - start;

        if (total_time < 1e-9)
        {
            printf("Transfer too fast to measure reliably.\n");
        }
        else if (choice == 1)
        {
            printf("Latency per message: %.6f ms\n",
                   (total_time / iterations) * 1000);
        }
        else if (choice == 2)
        {
            double throughput = (msg_size * iterations) / (total_time * 1e6);
            printf("Throughput: %.2f MB/s\n", throughput);
        }

        free(data);
    }

    munmap(region, SHM_SIZE);
    close(fd);
    shm_unlink(SHM_NAME);
    return 0;
}
