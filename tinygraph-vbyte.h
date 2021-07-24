#ifndef TINYGRAPH_VBYTE_H
#define TINYGRAPH_VBYTE_H

#include <stdint.h>

#include "tinygraph-utils.h"


TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_vbyte_encode(const uint32_t *data, uint32_t n, uint8_t *out);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_vbyte_decode(const uint8_t *data, uint32_t n, uint32_t *out);


#endif
