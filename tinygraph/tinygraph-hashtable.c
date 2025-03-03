#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinygraph-utils.h"
#include "tinygraph-hashtable.h"

/*
 * Hash table implemented as a dense array
 * with open addressing (linear probing).
 *
 * We map fixed uint32 keys to uint32 values
 * for our particular use-cases. We could make
 * it generic via macros or void* but we don't
 * have the need for it at the moment and it
 * would make it considerably more complicated.
 *
 * The core idea is to find the index to store
 * a key-value pair at by hashing the key. On
 * hash collisions we simply do a linear scan
 * for the next free spot. This works really
 * well in practice on modern hardware with
 * cache hierarchies as long as we keep the
 * hash table load somewhere below ~0.75 and
 * the hash function is of high quality to
 * not cluster items close to each other.
 *
 * See
 *
 * - 
 *
 * - 
 */

typedef struct tinygraph_hashtable_item {
  uint32_t hash;
  uint32_t key;
  uint32_t value;
} tinygraph_hashtable_item;

TINYGRAPH_WARN_UNUSED
static inline bool tinygraph_hashtable_item_is_tombstone(
    const tinygraph_hashtable_item item)
{
  // Note: this makes use of the fact that our hash function
  // maps zero to something else than zero. If we change the
  // hash function to have h(0)=0 this will break and we will
  // have to find another way to mark an item as a tombstone.
  // It's a trade-off between tight coupling vs being able to
  // re-use the existing hash and key members without having
  // to store additional flags in the hashtable item.
  return item.key == 0 && item.hash == 0;
}


typedef struct tinygraph_hashtable {
  tinygraph_hashtable_item *items;
  uint32_t items_len;
  uint32_t size;
} tinygraph_hashtable;


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
