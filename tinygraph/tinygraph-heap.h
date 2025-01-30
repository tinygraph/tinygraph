#ifndef TINYGRAPH_HEAP_H
#define TINYGRAPH_HEAP_H

#include <stdint.h>
#include <stdbool.h>

#include "tinygraph-utils.h"

/*
 * Simple min-heap to push items into
 * and then pop the smallest item from
 * efficiently. If you know about its
 * size, make sure to reserve().
 *
 * Both push and pop are in O(log n)
 */

typedef struct tinygraph_heap* tinygraph_heap_s;
typedef const struct tinygraph_heap* tinygraph_heap_const_s;


TINYGRAPH_WARN_UNUSED
tinygraph_heap_s tinygraph_heap_construct(void);

TINYGRAPH_WARN_UNUSED
tinygraph_heap_s tinygraph_heap_copy(tinygraph_heap_const_s heap);

void tinygraph_heap_destruct(tinygraph_heap_s heap);

TINYGRAPH_WARN_UNUSED
bool tinygraph_heap_reserve(tinygraph_heap_s heap, uint32_t capacity);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_heap_get_size(tinygraph_heap_const_s heap);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_heap_get_capacity(tinygraph_heap_const_s heap);

TINYGRAPH_WARN_UNUSED
bool tinygraph_heap_is_empty(tinygraph_heap_const_s heap);

void tinygraph_heap_clear(tinygraph_heap_s heap);

TINYGRAPH_WARN_UNUSED
bool tinygraph_heap_push(tinygraph_heap_s heap, uint32_t value, uint32_t priority);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_heap_pop(tinygraph_heap_s heap);

void tinygraph_heap_print_internal(tinygraph_heap_const_s heap);

#endif
