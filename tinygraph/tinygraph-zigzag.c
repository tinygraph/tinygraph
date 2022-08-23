#include "tinygraph-zigzag.h"


uint32_t tinygraph_zigzag_encode(int32_t value) {
  return ((uint32_t)value << 1) ^ (uint32_t)(value >> 31);
}


int32_t tinygraph_zigzag_decode(uint32_t value) {
  return (int32_t)((value >> 1) ^ (~(value & 1) + 1));
}
