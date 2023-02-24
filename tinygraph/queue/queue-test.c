#include <assert.h>
#include <stdbool.h>

#include "tinygraph/queue/queue.h"


void test1() {
  tinygraph_queue_s queue1 = tinygraph_queue_construct();
  assert(queue1);

  assert(tinygraph_queue_is_empty(queue1) == true);
  assert(tinygraph_queue_get_size(queue1) == 0);
  assert(tinygraph_queue_push(queue1, 2));
  assert(tinygraph_queue_push(queue1, 3));
  assert(tinygraph_queue_is_empty(queue1) == false);
  assert(tinygraph_queue_get_size(queue1) == 2);
  assert(tinygraph_queue_pop(queue1) == 2);
  assert(tinygraph_queue_pop(queue1) == 3);
  assert(tinygraph_queue_is_empty(queue1) == true);
  assert(tinygraph_queue_get_size(queue1) == 0);
  tinygraph_queue_destruct(queue1);

  tinygraph_queue_s queue2 = tinygraph_queue_construct();
  assert(queue2);
  assert(tinygraph_queue_push(queue2, 2));
  assert(tinygraph_queue_push(queue2, 3));
  assert(tinygraph_queue_get_front(queue2) == 2);
  assert(tinygraph_queue_get_back(queue2) == 3);
  assert(tinygraph_queue_get_size(queue2) == 2);
  tinygraph_queue_destruct(queue2);

  tinygraph_queue_s queue3 = tinygraph_queue_construct();
  assert(queue3);
  assert(tinygraph_queue_push(queue3, 2));
  assert(tinygraph_queue_push(queue3, 3));
  assert(tinygraph_queue_get_front(queue3) == 2);
  assert(tinygraph_queue_get_back(queue3) == 3);
  assert(tinygraph_queue_pop(queue3) == 2);
  assert(tinygraph_queue_get_front(queue3) == 3);
  assert(tinygraph_queue_get_back(queue3) == 3);
  assert(tinygraph_queue_get_size(queue3) == 1);
  tinygraph_queue_destruct(queue3);
}


int main() {
  test1();
}
