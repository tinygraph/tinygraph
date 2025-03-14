#ifndef TINYGRAPH_SORT_H
#define TINYGRAPH_SORT_H

#include <stdint.h>

/*
 * Simple sorting with a comparator
 * and a context pointer, much like
 * the non-standard qsort_r extension.
 */


void tinygraph_sort_u32(
    uint32_t* a,
    uint32_t n,
    int (*cmp)(const void* lhs, const void* rhs, void* arg),
    void* arg);


#endif
