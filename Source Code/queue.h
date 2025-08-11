#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

struct element {
    int num_edition;
    int id_belt;
    int last;
};

struct queue {
    struct element* elements;
    int size;
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    int belt_id;
};

struct queue* queue_init(int size, int belt_id);
int queue_put(struct queue* q, struct element* elem);
struct element* queue_get(struct queue* q);
int queue_destroy(struct queue* q);
int queue_empty(struct queue* q);
int queue_full(struct queue* q);

#endif
