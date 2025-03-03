#ifndef TINYGRAPH_HASHTABLE_H
#define TINYGRAPH_HASHTABLE_H

#include <stdint.h>
#include <stdbool.h>

#include "tinygraph-utils.h"

/*
 * Simple hash table to store a mapping
 * between unique keys and their values
 * efficiently. If you know about its
 * size, make sure to reserve().
 */

typedef struct tinygraph_hashtable* tinygraph_hashtable_s;
typedef const struct tinygraph_hashtable* tinygraph_hashtable_const_s;


TINYGRAPH_WARN_UNUSED
tinygraph_hashtable_s tinygraph_hashtable_construct(void);

TINYGRAPH_WARN_UNUSED
tinygraph_hashtable_s tinygraph_hashtable_copy(tinygraph_hashtable_const_s hashtable);

void tinygraph_hashtable_destruct(tinygraph_hashtable_s hashtable);

TINYGRAPH_WARN_UNUSED
bool tinygraph_hashtable_reserve(tinygraph_hashtable_s hashtable, uint32_t capacity);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_hashtable_get_size(tinygraph_hashtable_const_s hashtable);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_hashtable_get_capacity(tinygraph_hashtable_const_s hashtable);

TINYGRAPH_WARN_UNUSED
bool tinygraph_hashtable_is_empty(tinygraph_hashtable_const_s hashtable);

void tinygraph_hashtable_clear(tinygraph_hashtable_s hashtable);

TINYGRAPH_WARN_UNUSED
bool tinygraph_hashtable_set(tinygraph_hashtable_s hashtable, uint32_t key, uint32_t value);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_hashtable_get(tinygraph_hashtable_s hashtable);

void tinygraph_hashtable_print_internal(tinygraph_hashtable_const_s hashtable);

#endif
