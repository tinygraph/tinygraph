#include <string.h>
#include <stdlib.h>

#include "tinygraph-sort.h"


// This implementation is inspired by
// https://justine.lol/sorting/
// and adapted to make it reentrant by
// adding a context pointer and an arg
// parameter to the comparator much like
// the non-standard qsort_r function.
//
// There are two ideas for improvements
// 1. AVX2 bitonic sort for small ranges
// 2. trade-off space and use radix sort
//
// Note that sometimes we don't need a
// full sorting but e.g. one use case
// is knowing the first p elements are
// the smallest ones. In that case look
// into nth-element like re-ordering.


static inline void tinygraph_insertion_sort_u32(
    uint32_t * restrict a,
    uint32_t n,
    int32_t (*cmp)(const uint32_t * restrict lhs, const uint32_t * restrict rhs, void * restrict arg),
    void * restrict arg)
{
  for (uint32_t i = 1; i < n; ++i) {
    uint32_t j = i - 1;

    const uint32_t tmp = a[i];

    while (j > 0 && cmp(&a[j], &tmp, arg) > 0) {
      a[j + 1] = a[j];
      j = j - 1;
    }

    a[j + 1] = tmp;
  }
}


void tinygraph_sort_u32(
    uint32_t * restrict a,
    uint32_t n,
    int32_t (*cmp)(const uint32_t * restrict lhs, const uint32_t * restrict rhs, void * restrict arg),
    void * restrict arg)
{
  if (n < 2) {
    return;
  }

  if (n <= 32) {
    tinygraph_insertion_sort_u32(a, n, cmp, arg);
  }

  const uint32_t p = a[n >> 1];

  uint32_t i = 0;

  for (uint32_t j = n - 1; /**/; ++i, --j) {
    while (cmp(&a[i], &p, arg) < 0) {
      i = i + 1;
    }

    while (cmp(&a[j], &p, arg) > 0) {
      j = j - 1;
    }

    if (i >= j) {
      break;
    }

    const uint32_t tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
  }

  tinygraph_sort_u32(a, i, cmp, arg);
  tinygraph_sort_u32(a + i, n - i, cmp, arg);
}


static inline void tinygraph_radix_sort_u32_with_mem(
    uint32_t * restrict a,
    uint32_t * restrict copy,
    uint32_t n,
    uint32_t (*op)(const uint32_t * restrict item, void * restrict arg),
    void * restrict arg)
{
  uint32_t level0[256] = {0};
  uint32_t level1[256] = {0};
  uint32_t level2[256] = {0};
  uint32_t level3[256] = {0};

  for (uint32_t i = 0; i < n; ++i) {
    const uint32_t key = op(&a[i], arg);

    level0[(key >> 0UL) & 0xff]++;
    level1[(key >> 8UL) & 0xff]++;
    level2[(key >> 16UL) & 0xff]++;
    level3[(key >> 24UL) & 0xff]++;
  }

  uint32_t sum0 = 0;
  uint32_t sum1 = 0;
  uint32_t sum2 = 0;
  uint32_t sum3 = 0;

  for (uint32_t i = 0; i < 256; ++i) {
    uint32_t tmp0 = level0[i];
    uint32_t tmp1 = level1[i];
    uint32_t tmp2 = level2[i];
    uint32_t tmp3 = level3[i];

    level0[i] = sum0; sum0 += tmp0;
    level1[i] = sum1; sum1 += tmp1;
    level2[i] = sum2; sum2 += tmp2;
    level3[i] = sum3; sum3 += tmp3;
  }

  for (uint32_t i = 0; i < n; ++i) {
    const uint32_t key = op(&a[i], arg);
    copy[level0[(key >> 0UL) & 0xff]++] = a[i];
  }

  for (uint32_t i = 0; i < n; ++i) {
    const uint32_t key = op(&copy[i], arg);
    a[level1[(key >> 8UL) & 0xff]++] = copy[i];
  }

  for (uint32_t i = 0; i < n; ++i) {
    const uint32_t key = op(&a[i], arg);
    copy[level2[(key >> 16UL) & 0xff]++] = a[i];
  }

  for (uint32_t i = 0; i < n; ++i) {
    const uint32_t key = op(&copy[i], arg);
    a[level3[(key >> 24UL) & 0xff]++] = copy[i];
  }
}

bool tinygraph_radix_sort_u32(
  uint32_t * restrict a,
  uint32_t n,
  uint32_t (*op)(const uint32_t * restrict item, void * restrict arg),
  void * restrict arg)
{
  if (n < 2) {
    return true;
  }

  uint32_t * const copy = malloc(n * sizeof(uint32_t));

  if (!copy) {
    return false;
  }

  tinygraph_radix_sort_u32_with_mem(a, copy, n, op, arg);

  free(copy);

  return true;
}
