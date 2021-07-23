#include "tinygraph-zigzag.h"


uint32_t tinygraph_zigzag_encode(int32_t value) {
  return ((uint32_t)value << 1UL) ^ (uint32_t)(-(int32_t)((uint32_t)value >> 31UL));
}


int32_t tinygraph_zigzag_decode(uint32_t value) {
  return (int32_t)((value >> 1UL) ^ (uint32_t)(-(int32_t)(value & 1UL)));
}
