#ifndef TINYGRAPH_VBYTE_H
#define TINYGRAPH_VBYTE_H

#include <stdint.h>

#include "tinygraph-utils.h"

/*
 * VByte coding to store small 32 bit integers
 * in 1-5 bytes, depending on their value. Use
 * in combination with delta coding, to get to
 * smaller values.
 *
 * Todo: expose functions for scalars not requiring
 * us writing into an out array, for efficiency. Also
 * expose function to decode an array of n bytes and
 * don't require the user to tell us about num ints.
 */


TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_vbyte_encode(const uint32_t *data, uint32_t n, uint8_t *out);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_vbyte_decode(const uint8_t *data, uint32_t n, uint32_t *out);


#endif
