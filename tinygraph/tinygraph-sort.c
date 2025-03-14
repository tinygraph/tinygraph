#include "tinygraph-sort.h"


// This implementation is inspired by
// https://justine.lol/sorting/
// and adapted to make it reentrant by
// adding a context pointer and an arg
// parameter to the comparator much like
// the non-standard qsort_r function.

static inline void tinygraph_insertion_sort_u32(
    uint32_t* a,
    uint32_t n,
    int32_t (*cmp)(const void* lhs, const void* rhs, void* arg),
    void *arg)
{
  uint32_t j;
  uint32_t t;

  for (uint32_t i = 1; i < n; ++i) {
    t = a[i];
    j = i - 1;

    while (j > 0 && cmp(&a[j], &t, arg) > 0) {
      a[j + 1] = a[j];
      j = j - 1;
    }

    a[j + 1] = t;
  }
}


void tinygraph_sort_u32(
    uint32_t* a,
    uint32_t n,
    int32_t (*cmp)(const void* lhs, const void* rhs, void* arg),
    void* arg)
{
  if (n < 2) {
    return;
  }

  if (n <= 32) {
    tinygraph_insertion_sort_u32(a, n, cmp, arg);
  }

  uint32_t p = a[n >> 1];
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

    uint32_t t = a[i];
    a[i] = a[j];
    a[j] = t;
  }

  tinygraph_sort_u32(a, i, cmp, arg);
  tinygraph_sort_u32(a + i, n - i, cmp, arg);
}
