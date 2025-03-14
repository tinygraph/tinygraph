#ifndef TINYGRAPH_SORT_H
#define TINYGRAPH_SORT_H

#include <stdint.h>
#include <stdbool.h>

#include "tinygraph-utils.h"

/*
 * Simple sorting with a comparator
 * and a context pointer, much like
 * the non-standard qsort_r extension.
 *
 * The comparator must return -1, 0, 1
 * depending on if lhs is less, equal,
 * or greater than rhs.
 */
void tinygraph_sort_u32(
    uint32_t * restrict a,
    uint32_t n,
    int32_t (*cmp)(const uint32_t * restrict lhs, const uint32_t * restrict rhs, void * restrict arg),
    void * restrict arg);

/*
 * Radix sort with a unary operation
 * as a key extractor to sort by and
 * a context pointer, much like the
 * non-standard qsort_r extension.
 *
 * Creates a copy of the array to be
 * sorted internally, trades off space
 * vs runtime.
 */
TINYGRAPH_WARN_UNUSED
bool tinygraph_radix_sort_u32(
    uint32_t * restrict a,
    uint32_t n,
    uint32_t (*op)(const uint32_t * restrict item, void * restrict arg),
    void * restrict arg);


#endif
