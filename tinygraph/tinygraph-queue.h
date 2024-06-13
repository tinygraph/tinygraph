#ifndef TINYGRAPH_QUEUE_H
#define TINYGRAPH_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

#include "tinygraph-utils.h"

/*
 * Simple queue to push to the back and pop
 * from the front efficiently. If you know
 * about its size, make sure to reserve().
 *
 * Note: we implement a queue with two stacks
 * at the moment; a ringbuffer based queue
 * might make more sense for our graph walk
 * use case.
 */

typedef struct tinygraph_queue* tinygraph_queue_s;


TINYGRAPH_WARN_UNUSED
tinygraph_queue_s tinygraph_queue_construct(void);

TINYGRAPH_WARN_UNUSED
tinygraph_queue_s tinygraph_queue_copy(tinygraph_queue_s queue);

void tinygraph_queue_destruct(tinygraph_queue_s queue);

TINYGRAPH_WARN_UNUSED
bool tinygraph_queue_reserve(tinygraph_queue_s queue, uint32_t capacity);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_queue_get_front(tinygraph_queue_s queue);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_queue_get_back(tinygraph_queue_s queue);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_queue_get_size(tinygraph_queue_s queue);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_queue_get_capacity(tinygraph_queue_s queue);

TINYGRAPH_WARN_UNUSED
bool tinygraph_queue_is_empty(tinygraph_queue_s queue);

void tinygraph_queue_clear(tinygraph_queue_s queue);

TINYGRAPH_WARN_UNUSED
bool tinygraph_queue_push(tinygraph_queue_s queue, uint32_t value);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_queue_pop(tinygraph_queue_s queue);

void tinygraph_queue_print_internal(tinygraph_queue_s queue);


#endif
