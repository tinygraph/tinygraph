#ifndef TINYGRAPH_INDEX_H
#define TINYGRAPH_INDEX_H

#include <stdint.h>
#include <stdbool.h>

#include "tinygraph-utils.h"

/*
 * Spatial index for nearest neighbor queries.
 */

typedef struct tinygraph_index* tinygraph_index_s;
typedef const struct tinygraph_index* tinygraph_index_const_s;


TINYGRAPH_WARN_UNUSED
tinygraph_index_s tinygraph_index_construct(
    const uint32_t* nodes,
    const uint32_t* lngs,
    const uint32_t* lats,
    uint32_t n);

TINYGRAPH_WARN_UNUSED
tinygraph_index_s tinygraph_index_copy(tinygraph_index_const_s index);

void tinygraph_index_destruct(tinygraph_index_s index);

typedef struct tinygraph_index_search_opts {
  uint32_t lngmin;
  uint32_t latmin;
  uint32_t lngmax;
  uint32_t latmax;
  uint32_t n;
} tinygraph_index_search_opts;

TINYGRAPH_WARN_UNUSED
bool tinygraph_index_search(
    tinygraph_index_const_s index,
    tinygraph_index_search_opts opts,
    uint32_t* out,
    uint32_t* len);

void tinygraph_index_print_internal(tinygraph_index_const_s index);

#endif
