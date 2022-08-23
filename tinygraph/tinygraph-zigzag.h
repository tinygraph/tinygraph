#ifndef TINYGRAPH_ZIGZAG_H
#define TINYGRAPH_ZIGZAG_H

#include <stdint.h>

#include "tinygraph-utils.h"

/*
 * Zig-Zag coding to interleave negative
 * and positive integers, such that small
 * positive or small negative values will
 * have a small encoded value, too. Use in
 * combination with delta and vbyte coding.
 */

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_zigzag_encode(int32_t value);

TINYGRAPH_WARN_UNUSED
int32_t tinygraph_zigzag_decode(uint32_t value);


#endif
