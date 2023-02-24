#include <assert.h>
#include <stdint.h>

#include "tinygraph/delta/delta.h"


void test1() {
  const uint32_t original[8] = {1, 2, UINT32_MAX, 0, UINT32_MAX, 0, 1, 2};

  uint32_t deltas[8];

  tinygraph_delta_encode(original, deltas, 8, 0);

  uint32_t roundtrip[8];

  tinygraph_delta_decode(deltas, roundtrip, 8, 0);

  for (uint32_t i = 0; i < 8; ++i) {
    assert(original[i] == roundtrip[i]);
  }
}


int main() {
  test1();
}
