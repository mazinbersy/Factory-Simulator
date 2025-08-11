#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include "queue.h"

int process_manager(int id, int belt_size, int items);

#define BUFFER_SIZE 1024

typedef struct {
    int id;
    int belt_size;
    int items;
    sem_t* start_all;
} thread_args;

void* process_manager_thread(void* arg) {
    thread_args* args = (thread_args*)arg;

    sem_wait(args->start_all);  // Wait for factory manager signal to start

    int result = process_manager(args->id, args->belt_size, args->items);

    if (result == 0) {
        printf("[OK][factory_manager] Process_manager with id %d has finished.\n", args->id);
    } else {
        printf("[ERROR][factory_manager] Process_manager with id %d has finished with errors.\n", args->id);
    }

    free(args);
    pthread_exit(NULL);
}

typedef struct {
    int id;
    int belt_size;
    int items;
} belt_info;

int parse_input_file(int fd, int* max_belts, belt_info** belts) {
    char buffer[BUFFER_SIZE];
    int bytes_read;
    int belt_count = 0;

    bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read <= 0) {
        printf("[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }
    buffer[bytes_read] = '\0';

    char* ptr = buffer;
    *max_belts = strtol(ptr, &ptr, 10);
    if (*max_belts <= 0) {
        printf("[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }

    *belts = (belt_info*)malloc(*max_belts * sizeof(belt_info));
    if (*belts == NULL) {
        printf("[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }

    while (belt_count < *max_belts) {
        (*belts)[belt_count].id = strtol(ptr, &ptr, 10);
        (*belts)[belt_count].belt_size = strtol(ptr, &ptr, 10);
        (*belts)[belt_count].items = strtol(ptr, &ptr, 10);

        if ((*belts)[belt_count].belt_size <= 0 || (*belts)[belt_count].items <= 0) {
            printf("[ERROR][factory_manager] Invalid file.\n");
            free(*belts);
            return -1;
        }

        belt_count++;

        while (*ptr == ' ' || *ptr == '\n' || *ptr == '\t') ptr++;
        if (*ptr == '\0') break;
    }

    if (*ptr != '\0') {
        printf("[ERROR][factory_manager] Invalid file.\n");
        free(*belts);
        return -1;
    }

    return belt_count;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        printf("[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }

    int max_belts;
    belt_info* belts = NULL;

    int belt_count = parse_input_file(fd, &max_belts, &belts);
    close(fd);
    if (belt_count == -1) return -1;

    sem_t start_all;
    if (sem_init(&start_all, 0, 0) != 0) {
        free(belts);
        printf("[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }

    pthread_t* threads = (pthread_t*)malloc(belt_count * sizeof(pthread_t));
    if (!threads) {
        free(belts);
        sem_destroy(&start_all);
        printf("[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }

    for (int i = 0; i < belt_count; i++) {
        printf("[OK][factory_manager] Process_manager with id %d has been created.\n", belts[i].id);

        thread_args* args = (thread_args*)malloc(sizeof(thread_args));
        if (!args) {
            printf("[ERROR][factory_manager] Invalid file.\n");
            for (int j = 0; j < i; j++) pthread_join(threads[j], NULL);
            free(threads);
            free(belts);
            sem_destroy(&start_all);
            return -1;
        }

        args->id = belts[i].id;
        args->belt_size = belts[i].belt_size;
        args->items = belts[i].items;
        args->start_all = &start_all;

        if (pthread_create(&threads[i], NULL, process_manager_thread, args) != 0) {
            printf("[ERROR][factory_manager] Invalid file.\n");
            free(args);
            for (int j = 0; j < i; j++) pthread_join(threads[j], NULL);
            free(threads);
            free(belts);
            sem_destroy(&start_all);
            return -1;
        }
    }

    for (int i = 0; i < belt_count; i++) {
        sem_post(&start_all);
    }

    for (int i = 0; i < belt_count; i++) {
        pthread_join(threads[i], NULL);
    }

    free(belts);
    free(threads);
    sem_destroy(&start_all);

    printf("[OK][factory_manager] Finishing.\n");
    return 0;
}
