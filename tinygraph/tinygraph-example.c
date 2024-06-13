#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "tinygraph.h"


int main(void) {
  const uint32_t sources[5] = {0, 0, 1, 1, 2};
  const uint32_t targets[5] = {1, 2, 0, 2, 1};

  tinygraph_s graph = tinygraph_construct_from_sorted_edges(
    sources, targets, 5);

  if (!graph) {
    fprintf(stderr, "error: unable to construct graph\n");
    return EXIT_FAILURE;
  }

  tinygraph_print(graph);

  tinygraph_destruct(graph);

  return EXIT_SUCCESS;
}
