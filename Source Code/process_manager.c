#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "queue.h"

typedef struct {
    int id;
    int belt_size;
    int items;
    struct queue* q;
} process_manager_args;

void* producer(void* arg) {
    process_manager_args* args = (process_manager_args*)arg;
    for (int i = 0; i < args->items; i++) {
        struct element elem;
        elem.num_edition = i;
        elem.id_belt = args->id;
        elem.last = (i == args->items - 1) ? 1 : 0;
        queue_put(args->q, &elem);
    }
    pthread_exit(NULL);
}

void* consumer(void* arg) {
    process_manager_args* args = (process_manager_args*)arg;
    while (1) {
        struct element* elem = queue_get(args->q);
        if (elem == NULL) continue;
        if (elem->last) {
            free(elem);
            break;
        }
        free(elem);
    }
    pthread_exit(NULL);
}

int process_manager(int id, int belt_size, int items) {
    printf("[OK][process_manager] Process_manager with id %d waiting to produce %d elements.\n",
           id, items);

    struct queue* q = queue_init(belt_size, id);
    if (q == NULL) {
        printf("[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
        return -1;
    }

    printf("[OK][process_manager] Belt with id %d has been created with a maximum of %d elements.\n",
           id, belt_size);

    process_manager_args args = {id, belt_size, items, q};

    pthread_t producer_thread, consumer_thread;

    if (pthread_create(&producer_thread, NULL, producer, &args) != 0 ||
        pthread_create(&consumer_thread, NULL, consumer, &args) != 0) {
        printf("[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
        queue_destroy(q);
        return -1;
    }

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    printf("[OK][process_manager] Process_manager with id %d has produced %d elements.\n",
           id, items);

    return 0;
}
