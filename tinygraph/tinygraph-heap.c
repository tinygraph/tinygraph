#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinygraph-utils.h"
#include "tinygraph-heap.h"

/*
 * Binary min-heap implemented as a dense array
 * of heap items. Traversal happens via array
 * indices of: parent, left child, right child.
 *
 * Insertion (push) happens by adding a new item
 * at the very end (bottom right of tree) and
 * then bubbling up the item to its position
 * where it is smaller than its children.
 *
 * Removal (pop) happens by taking the very
 * first item from the array, then switching in
 * a new item from the very end, and bubbling
 * it down the tree to its position where
 * it is bigger than its parent.
 *
 * Both operations are in O(log n)
 *
 * Note that we will use this heap for graph
 * search algorithms such as Dijkstra's. In
 * these algorithms there's a need for a
 * DecreaseKey operation, changing the value
 * of an inserted item. Our simple binary
 * min-heap here can not support this operation
 * efficiently, but instead the work around is
 * to e.g. insert the item with the new value
 * again and keeping track of visited items.
 *
 * The benefit is that this simple binary
 * min-heap can be implemented efficiently
 * as a single flat array which works really
 * well in practice on modern hardware with
 * its cache hierarchies.
 *
 * In addition the combination of Dijkstra's
 * algorithm running on rather sparse graphs
 * such as road networks means the heap will
 * often fit into L1, L2 caches anyway.
 *
 * See
 *
 * - https://en.wikipedia.org/wiki/Binary_heap
 *
 * - https://www3.cs.stonybrook.edu/~rezaul/papers/TR-07-54.pdf
 *   Priority Queues and Dijkstra’s Algorithm
 */

// A single heap item and utility functions
// for comparision, swapping, and traversing.
typedef struct tinygraph_heap_item {
  uint32_t value;
  uint32_t priority;
} tinygraph_heap_item;

static inline bool tinygraph_heap_item_comp(
    const tinygraph_heap_item lhs,
    const tinygraph_heap_item rhs)
{
  return lhs.priority <= rhs.priority;
}

static inline void tinygraph_heap_item_swap(
    tinygraph_heap_item * const lhs,
    tinygraph_heap_item * const rhs)
{
  TINYGRAPH_ASSERT(lhs);
  TINYGRAPH_ASSERT(rhs);

  const tinygraph_heap_item tmp = *lhs;

  *lhs = *rhs;
  *rhs = tmp;
}

static inline uint32_t tinygraph_heap_item_parent(uint32_t i) {
  TINYGRAPH_ASSERT(i > 0); // root has no parent

  const uint32_t rv = (i - 1) / 2;

  TINYGRAPH_ASSERT(rv < i);

  return rv;
}

static inline uint32_t tinygraph_heap_item_left(uint32_t i) {
  const uint32_t rv = 2 * i + 1;
  TINYGRAPH_ASSERT(rv > i);

  return rv;
}

static inline uint32_t tinygraph_heap_item_right(uint32_t i) {
  const uint32_t rv = 2 * i + 2;
  TINYGRAPH_ASSERT(rv > i);

  return rv;
}

// The actual binary min-heap implemented as
// a flat array of consecutive heap items
typedef struct tinygraph_heap {
  tinygraph_heap_item *items;
  uint32_t items_len;
  uint32_t size;
} tinygraph_heap;


static inline void tinygraph_heap_bubble_up(
    tinygraph_heap * const heap, uint32_t i)
{
  /*
   *      p    parent
   *    /  \
   *   i    o  children (we are at i)
   *
   *   To bubble up i, we walk upwards comparing
   *   i with p wrt. their priority and swap them
   *   if needed to have p <= i in a min-heap.
   */
  while (i > 0) {
    const uint32_t p = tinygraph_heap_item_parent(i);

    tinygraph_heap_item * const item = &heap->items[i];
    tinygraph_heap_item * const parent = &heap->items[p];

    if (tinygraph_heap_item_comp(*parent, *item)) {
      break;
    } else {
      tinygraph_heap_item_swap(parent, item);

      i = p;
    }
  }
  // TODO: benchmark if prefetching the parent items
  // upfront on the way up makes a difference since
  // there is only a single path up the tree
  // TINYGRAPH_PREFETCH();
}

static inline void tinygraph_heap_bubble_down(
    tinygraph_heap * const heap, uint32_t i)
{
  /*
   *      p    parent (we are at p=i)
   *    /  \
   *   l    r  children
   *
   * To bubble down i, we walk downwards, checking
   * if the left or right child exist, and which
   * way to go down.
   */
  while (tinygraph_heap_item_left(i) < heap->size) {
    const uint32_t l = tinygraph_heap_item_left(i);
    const uint32_t r = tinygraph_heap_item_right(i);

    uint32_t s = i;

    // go down left?
    if (l < heap->size) {
      tinygraph_heap_item * const left = &heap->items[l];
      tinygraph_heap_item * const item = &heap->items[s];

      if (tinygraph_heap_item_comp(*left, *item)) {
        s = l;
      }
    }

    // go down right?
    if (r < heap->size) {
      tinygraph_heap_item * const right = &heap->items[r];
      tinygraph_heap_item * const item = &heap->items[s];

      if (tinygraph_heap_item_comp(*right, *item)) {
        s = r;
      }
    }

    // if neither left nor right is smaller we're done here,
    // otherwise we go down the path of the smallest child
    if (s == i) {
      break;
    } else {
      tinygraph_heap_item_swap(&heap->items[i], &heap->items[s]);

      i = s;
    }
  }
  // TODO: benchmark if prefetching the child items
  // upfront on the way down makes a difference but
  // because there is not a single path down the
  // tree we'll have to be careful how we do it.
  // TINYGRAPH_PREFETCH();
}

tinygraph_heap* tinygraph_heap_construct(void) {
  tinygraph_heap *out = malloc(sizeof(tinygraph_heap));

  if (!out) {
    return NULL;
  }

  // initially reserve a cacheline (64 bytes / 8 bytes struct = 8)
  const uint32_t capacity = 8;

  // TODO: we should make an initial capacity the standard
  // in all our other datastructures, too, for consistency
  // (e.g. in the array impl)

  tinygraph_heap_item *items = calloc(capacity, sizeof(tinygraph_heap_item));

  if (!items) {
    return NULL;
  }

  *out = (tinygraph_heap) {
    .items = items,
    .items_len = capacity,
    .size = 0,
  };

  return out;
}


tinygraph_heap* tinygraph_heap_copy(const tinygraph_heap * const heap) {
  TINYGRAPH_ASSERT(heap);

  tinygraph_heap *copy = tinygraph_heap_construct();

  if (!copy) {
    return NULL;
  }

  if (heap->size == 0) {
    return copy;
  }

  const bool ok = tinygraph_heap_reserve(copy, heap->size);

  if (!ok) {
    tinygraph_heap_destruct(copy);
    return NULL;
  }

  memcpy(copy->items, heap->items, heap->size * sizeof(tinygraph_heap_item));

  copy->size = heap->size;

  return copy;
}


void tinygraph_heap_destruct(tinygraph_heap * const heap) {
  if (!heap) {
    return;
  }

  free(heap->items);
  free(heap);
}


bool tinygraph_heap_reserve(tinygraph_heap * const heap, uint32_t capacity) {
  TINYGRAPH_ASSERT(heap);

  // reserve can't resize down
  if (capacity < heap->size) {
    return false;
  }

  // already enough capacity
  if (capacity <= heap->items_len) {
    return true;
  }

  // TODO: realloc
  tinygraph_heap_item *items = calloc(capacity, sizeof(tinygraph_heap_item));

  if (!items) {
    return false;
  }

  if (heap->size > 0) {
    memcpy(items, heap->items, heap->size * sizeof(tinygraph_heap_item));
  }

  free(heap->items);

  heap->items = items;
  heap->items_len = capacity;

  return true;
}


uint32_t tinygraph_heap_get_size(const tinygraph_heap * const heap) {
  TINYGRAPH_ASSERT(heap);

  return heap->size;
}


uint32_t tinygraph_heap_get_capacity(const tinygraph_heap * const heap) {
  TINYGRAPH_ASSERT(heap);

  return heap->items_len;
}


bool tinygraph_heap_is_empty(const tinygraph_heap * const heap) {
  TINYGRAPH_ASSERT(heap);

  return heap->size == 0;
}

void tinygraph_heap_clear(tinygraph_heap * const heap) {
  TINYGRAPH_ASSERT(heap);

  memset(heap->items, 0, heap->size * sizeof(tinygraph_heap_item));

  heap->size = 0;
}


bool tinygraph_heap_push(tinygraph_heap * const heap, uint32_t value, uint32_t priority) {
  TINYGRAPH_ASSERT(heap);

  if (heap->size == UINT32_MAX) {
    return false;
  }

  if (heap->size == heap->items_len) {
    uint64_t growth = ceil((uint64_t)heap->items_len * 1.5);

    if (growth >= UINT32_MAX) {
      growth = UINT32_MAX;
    }

    const bool ok = tinygraph_heap_reserve(heap, (uint32_t)growth);

    if (!ok) {
      return false;
    }
  }

  // Add a new item to the very end and then bubble it up
  // the binary tree until it's at the correct position.

  const tinygraph_heap_item item = (tinygraph_heap_item){
    .value = value,
    .priority = priority,
  };

  heap->items[heap->size] = item;
  heap->size = heap->size + 1;

  tinygraph_heap_bubble_up(heap, heap->size - 1);

  return true;
}


uint32_t tinygraph_heap_pop(tinygraph_heap * const heap) {
  TINYGRAPH_ASSERT(heap);
  TINYGRAPH_ASSERT(heap->size > 0);

  // Remove the item from the very top, swap it with the last
  // item at the very end, and then bubble it down the binary
  // tree until it's at the correct position.

  const tinygraph_heap_item item = heap->items[0];

  heap->items[0] = heap->items[heap->size - 1];
  heap->size = heap->size - 1;

  tinygraph_heap_bubble_down(heap, 0);

  return item.value;
}


void tinygraph_heap_print_internal(const tinygraph_heap * const heap) {
  TINYGRAPH_ASSERT(heap);

  fprintf(stderr, "heap internals\n");

  fprintf(stderr, "size: %ju, capacity: %ju\n",
      (uintmax_t)heap->size, (uintmax_t)heap->items_len);

  fprintf(stderr, "items:");

  for (uint32_t i = 0; i < heap->size; ++i) {
    const uint32_t value = heap->items[i].value;
    const uint32_t priority = heap->items[i].priority;

    fprintf(stderr, " (%ju, %ju)", (uintmax_t)value, (uintmax_t)priority);
  }

  fprintf(stderr, "\n");
}
