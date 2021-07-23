#ifndef TINYGRAPH_ZIGZAG_H
#define TINYGRAPH_ZIGZAG_H

#include <stdint.h>


uint32_t tinygraph_zigzag_encode(int32_t value);
int32_t tinygraph_zigzag_decode(uint32_t value);


#endif
