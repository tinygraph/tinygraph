#include <assert.h>
#include <stdint.h>

#include "tinygraph/eliasfano/eliasfano.h"


void test1() {
  uint32_t data[] = {1, 2, 10, 19, 147};

  tinygraph_bitset_s bits = tinygraph_eliasfano_encode(data, 5);

  //tinygraph_bitset_print_internal(bits);

  tinygraph_bitset_destruct(bits);
}


int main() {
  test1();
}
