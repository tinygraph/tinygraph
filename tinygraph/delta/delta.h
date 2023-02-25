#ifndef TINYGRAPH_DELTA_H
#define TINYGRAPH_DELTA_H

#include <stdint.h>

/*
 * Delta coding to store consecutive differences in
 * an array. The differences are (usually) smaller
 * and therefore work well in combination with a
 * vbyte coding scheme.
 *
 * Todo: expose functions for scalars not requiring
 * us writing into an out array, for efficiency.
 */

void tinygraph_delta_encode(const uint32_t *data, uint32_t *out, uint32_t n, uint32_t prev);
void tinygraph_delta_decode(const uint32_t *data, uint32_t *out, uint32_t n, uint32_t prev);


#endif
