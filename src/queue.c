#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "queue.h"

#define DEFAULT_CAPACITY 50

enum queue_status queue_init(struct queue *queue, size_t elem_size) {
    if (queue == NULL) {
        return QUEUE_ERR_NULL;
    }

    void *buffer = malloc(elem_size * DEFAULT_CAPACITY);
    if (buffer == NULL) {
        return QUEUE_ERR_OOM;
    }

    queue->size = 0;
    queue->elem_size = elem_size;
    queue->capacity = DEFAULT_CAPACITY;
    queue->head = 0;
    queue->tail = 0;
    queue->buffer = buffer;

    return QUEUE_OK;
}

enum queue_status queue_push(struct queue *queue, const void *element) {
    if (queue == NULL || element == NULL) {
        return QUEUE_ERR_NULL;
    }

    if (queue->size == queue->capacity) {
        size_t old_capacity = queue->capacity;
        size_t new_capacity = old_capacity * 2;
        uint8_t *old_buffer = (uint8_t *)queue->buffer;
        uint8_t *new_buffer = malloc(new_capacity * queue->elem_size);

        if (new_buffer == NULL) {
            return QUEUE_ERR_OOM;
        }

        if (queue->size > 0) {
            if (queue->head < queue->tail) {
                memcpy(new_buffer, old_buffer + (queue->head * queue->elem_size),
                       queue->size * queue->elem_size);
            } else {
                size_t first_count = old_capacity - queue->head;
                size_t second_count = queue->tail;

                memcpy(new_buffer, old_buffer + (queue->head * queue->elem_size),
                       first_count * queue->elem_size);

                if (second_count > 0) {
                    memcpy(new_buffer + (first_count * queue->elem_size), old_buffer,
                           second_count * queue->elem_size);
                }
            }
        }

        free(queue->buffer);
        queue->buffer = new_buffer;
        queue->capacity = new_capacity;
        queue->head = 0;
        queue->tail = queue->size;
    }

    uint8_t *base = (uint8_t *)queue->buffer;
    void *dst = base + (queue->tail * queue->elem_size);
    memcpy(dst, element, queue->elem_size);
    queue->size += 1;
    queue->tail = (queue->tail + 1) % queue->capacity;

    return QUEUE_OK;
}

enum queue_status queue_popleft(struct queue *queue, void *out) {
    if (queue == NULL || out == NULL) {
        return QUEUE_ERR_NULL;
    }

    if (queue->size == 0) {
        return QUEUE_ERR_EMPTY;
    }

    uint8_t *base = (uint8_t *)queue->buffer;
    void *src = base + (queue->head * queue->elem_size);
    memcpy(out, src, queue->elem_size);

    queue->head = (queue->head + 1) % queue->capacity;
    queue->size -= 1;

    return QUEUE_OK;
}

enum queue_status queue_popback(struct queue *queue, void *out) {
    if (queue == NULL || out == NULL) {
        return QUEUE_ERR_NULL;
    }

    if (queue->size == 0) {
        return QUEUE_ERR_EMPTY;
    }

    queue->tail = (queue->tail + queue->capacity - 1) % queue->capacity;

    uint8_t *base = (uint8_t *)queue->buffer;
    void *src = base + (queue->tail * queue->elem_size);
    memcpy(out, src, queue->elem_size);

    queue->size -= 1;

    return QUEUE_OK;
}

enum queue_status queue_free(struct queue *queue) {
    if (queue == NULL) {
        return QUEUE_ERR_NULL;
    }

    free(queue->buffer);
    queue->buffer = NULL;
    queue->size = 0;
    queue->capacity = 0;
    queue->head = 0;
    queue->tail = 0;

    return QUEUE_OK;
}

size_t queue_size(struct queue *queue) {
    if (queue == NULL) {
        return 0;
    }

    return queue->size;
}
