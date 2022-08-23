#ifndef TINYGRAPH_ZORDER_H
#define TINYGRAPH_ZORDER_H

#include <stdint.h>

#include "tinygraph-utils.h"

/*
 * Z-order / Morton Code space filling curve.
 */


TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_zorder_encode32(uint16_t x, uint16_t y);

TINYGRAPH_WARN_UNUSED
uint64_t tinygraph_zorder_encode64(uint32_t x, uint32_t y);


void tinygraph_zorder_decode32(uint32_t z, uint16_t *x, uint16_t *y);

void tinygraph_zorder_decode64(uint64_t z, uint32_t *x, uint32_t *y);


#endif
