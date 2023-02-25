#ifndef TINYGRAPH_IMPL_H
#define TINYGRAPH_IMPL_H

#include <stdint.h>
#include <stdbool.h>

#include "tinygraph/utils/utils.h"


typedef struct tinygraph {
  uint32_t *offsets;
  uint32_t *targets;
  uint32_t offsets_len;
  uint32_t targets_len;
} tinygraph;

TINYGRAPH_WARN_UNUSED
uint8_t tinygraph_saturated_add_u8(uint8_t a, uint8_t b);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_max_u32(uint32_t x, uint32_t y);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_min_u32(uint32_t x, uint32_t y);

void tinygraph_minmax_u32(
    const uint32_t *data,
    uint32_t n,
    uint32_t *min,
    uint32_t *max);

TINYGRAPH_WARN_UNUSED
const uint32_t* tinygraph_find_if_u32(
    const uint32_t* first,
    const uint32_t* last,
    uint32_t value);

TINYGRAPH_WARN_UNUSED
const uint32_t* tinygraph_find_if_not_u32(
    const uint32_t* first,
    const uint32_t* last,
    uint32_t value);

TINYGRAPH_WARN_UNUSED
const uint32_t* tinygraph_binary_search_u32(
    const uint32_t* first,
    const uint32_t* last,
    uint32_t value);

TINYGRAPH_WARN_UNUSED
const uint32_t* tinygraph_exponential_search_u32(
    const uint32_t* first,
    const uint32_t* last,
    uint32_t value);

// TODO: tinygraph_interpolating_search for our sorted dense offset vectors

void tinygraph_iota_u32(uint32_t* data, uint32_t n, uint32_t value);

TINYGRAPH_WARN_UNUSED
bool tinygraph_is_sorted_u32(const uint32_t *data, uint32_t n);

TINYGRAPH_WARN_UNUSED
bool tinygraph_is_sorted_sources_targets(
    const uint32_t *sources,
    const uint32_t *targets,
    uint32_t n);

bool tinygraph_sort_sources_targets(
    uint32_t *sources,
    uint32_t *targets,
    uint32_t n);

/* TODO
void tinygraph_renumber
*/

TINYGRAPH_WARN_UNUSED
tinygraph* tinygraph_construct_empty();

TINYGRAPH_WARN_UNUSED
bool tinygraph_reserve(
    tinygraph *graph,
    uint32_t num_nodes,
    uint32_t num_edges);

void tinygraph_print_internal(tinygraph *graph);

TINYGRAPH_WARN_UNUSED
uint8_t tinygraph_requires_num_bytes_u32(uint32_t value);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bytes_wasted(tinygraph *graph);

#endif
