#include <stdio.h>
#include <stdlib.h>

#include "tinygraph-utils.h"
#include "tinygraph-heap.h"


typedef struct tinygraph_heap {
  uint32_t dummy;
} tinygraph_heap;


tinygraph_heap* tinygraph_heap_construct(void) {
  tinygraph_heap *out = malloc(sizeof(tinygraph_heap));

  if (!out) {
    return NULL;
  }

  *out = (tinygraph_heap) {
    .dummy = -1,
  };

  return out;
}
