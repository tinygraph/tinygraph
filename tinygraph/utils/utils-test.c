#include <stdbool.h>

#include "tinygraph/utils/utils.h"


void test1() {
  TINYGRAPH_STATIC_ASSERT((1 + 1) == 2);

  TINYGRAPH_ASSERT(true);
}


int main() {
  test1();
}
