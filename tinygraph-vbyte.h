#ifndef TINYGRAPH_VBYTE_H
#define TINYGRAPH_VBYTE_H

#include <stdint.h>


uint32_t tinygraph_vbyte_encode(const uint32_t *data, uint32_t *out, uint32_t n);
uint32_t tinygraph_vbyte_decode(const uint32_t *data, uint32_t *out, uint32_t n);


#endif
