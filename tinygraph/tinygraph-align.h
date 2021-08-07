#ifndef TINYGRAPH_ALIGN_H
#define TINYGRAPH_ALIGN_H

#include <stdint.h>

#include "tinygraph-utils.h"


#define TINYGRAPH_ALIGN(x) __attribute__((aligned(x)))

TINYGRAPH_WARN_UNUSED
void* tinygraph_align_malloc(size_t alignment, size_t size);

void tinygraph_align_free(void *p);


#endif
