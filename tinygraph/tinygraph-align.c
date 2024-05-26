#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>


void* tinygraph_align_malloc(size_t alignment, size_t size) {
  void *p;

  if (posix_memalign(&p, alignment, size) != 0) {
    return NULL;
  }

  return p;
}


void tinygraph_align_free(void* p) {
  free(p);
}
