#ifndef TINYGRAPH_DELTA_H
#define TINYGRAPH_DELTA_H

#include <stdint.h>


void tinygraph_delta_encode(const uint32_t *data, uint32_t *out, uint32_t n, uint32_t prev);
void tinygraph_delta_decode(const uint32_t *data, uint32_t *out, uint32_t n, uint32_t prev);


#endif
