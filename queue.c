#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

struct queue* queue_init(int size, int belt_id) {
    struct queue* q = malloc(sizeof(struct queue));
    if (!q) return NULL;

    q->elements = malloc(sizeof(struct element) * size);
    if (!q->elements) {
        free(q);
        return NULL;
    }

    q->size = size;
    q->head = q->tail = q->count = 0;
    q->belt_id = belt_id;

    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);

    return q;
}

int queue_put(struct queue* q, struct element* elem) {
    pthread_mutex_lock(&q->lock);
    while (q->count == q->size) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }

    q->elements[q->tail] = *elem;
    printf("[OK][queue] Introduced element with id %d in belt %d.\n", elem->num_edition, elem->id_belt);
    q->tail = (q->tail + 1) % q->size;
    q->count++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
    return 0;
}

struct element* queue_get(struct queue* q) {
    pthread_mutex_lock(&q->lock);
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }

    struct element* elem = malloc(sizeof(struct element));
    *elem = q->elements[q->head];
    printf("[OK][queue] Obtained element with id %d in belt %d.\n", elem->num_edition, elem->id_belt);
    q->head = (q->head + 1) % q->size;
    q->count--;

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return elem;
}

int queue_destroy(struct queue* q) {
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
    free(q->elements);
    free(q);
    return 0;
}
