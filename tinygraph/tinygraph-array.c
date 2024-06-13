#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinygraph-utils.h"
#include "tinygraph-array.h"


typedef struct tinygraph_array {
  uint32_t *items;
  uint32_t items_len;
  uint32_t size;
} tinygraph_array;


tinygraph_array* tinygraph_array_construct(uint32_t size) {
  tinygraph_array *out = malloc(sizeof(tinygraph_array));

  if (!out) {
    return NULL;
  }

  *out = (tinygraph_array) {
    .items = NULL,
    .items_len = size,
    .size = size,
  };

  if (size == 0) {
    return out;
  }

  uint32_t *items = calloc(size, sizeof(uint32_t));

  if (!items) {
    free(out);

    return NULL;
  }

  out->items = items;

  TINYGRAPH_ASSERT(out->size <= out->items_len);

  return out;
}


tinygraph_array* tinygraph_array_copy(tinygraph_array *array) {
  if (!array) {
    return array;
  }

  TINYGRAPH_ASSERT(array->size <= array->items_len);

  tinygraph_array *copy = tinygraph_array_construct(array->size);

  if (!copy) {
    return NULL;
  }

  TINYGRAPH_ASSERT(copy->items_len >= array->size);
  TINYGRAPH_ASSERT(copy->size == array->size);

  if (array->size > 0) {
    TINYGRAPH_ASSERT(copy->items);
    TINYGRAPH_ASSERT(array->items);

    memcpy(copy->items, array->items, array->size * sizeof(uint32_t));
  }

  TINYGRAPH_ASSERT(array->size <= array->items_len);

  return copy;
}


void tinygraph_array_destruct(tinygraph_array *array) {
  if (!array) {
    return;
  }

  TINYGRAPH_ASSERT(array->size <= array->items_len);

  TINYGRAPH_ASSERT(array->items || array->items_len == 0);

  free(array->items);

  array->items = NULL;
  array->items_len = 0;
  array->size = 0;

  free(array);
}


bool tinygraph_array_reserve(tinygraph_array *array, uint32_t capacity) {
  TINYGRAPH_ASSERT(array);
  TINYGRAPH_ASSERT(array->size <= array->items_len);

  if (capacity <= array->items_len) {
    return true;
  }

  TINYGRAPH_ASSERT(capacity > 0);

  uint64_t growth = ceil((uint64_t)array->items_len * 1.5);

  if (growth >= UINT32_MAX) {
    growth = UINT32_MAX;
  }

  if (capacity < (uint32_t)growth) {
    capacity = (uint32_t)growth;
  }

  // initially reserve at least a cacheline
  if (capacity < 16) {
    capacity = 16;
  }

  TINYGRAPH_ASSERT(capacity >= 16);

  uint32_t *items = calloc(capacity, sizeof(uint32_t));

  if (!items) {
    return false;
  }

  TINYGRAPH_ASSERT(array->items_len >= array->size);

  if (array->size > 0) {
    TINYGRAPH_ASSERT(items);
    TINYGRAPH_ASSERT(array->items);

    memcpy(items, array->items, array->size * sizeof(uint32_t));
  }

  free(array->items);

  array->items = items;
  array->items_len = capacity;

  TINYGRAPH_ASSERT(array->size <= array->items_len);

  return true;
}


bool tinygraph_array_resize(tinygraph_array *array, uint32_t size) {
  TINYGRAPH_ASSERT(array);
  TINYGRAPH_ASSERT(array->size <= array->items_len);

  if (size == array->size) {
    return true;
  } else if (size < array->size) {
    TINYGRAPH_ASSERT(size < array->items_len);

    memset(array->items + size, 0, (array->size - size) * sizeof(uint32_t));

    array->size = size;

    return true;
  } else if (size > array->size) {
    TINYGRAPH_ASSERT(size > array->size);

    bool ok = tinygraph_array_reserve(array, size);

    if (!ok) {
      return false;
    }

    TINYGRAPH_ASSERT(size <= array->items_len);

    array->size = size;

    TINYGRAPH_ASSERT(array->size <= array->items_len);

    return true;
  }

  TINYGRAPH_UNREACHABLE();

  TINYGRAPH_ASSERT(false);

  return false;
}


uint32_t tinygraph_array_get_at(tinygraph_array *array, uint32_t i) {
  TINYGRAPH_ASSERT(array);
  TINYGRAPH_ASSERT(i < array->size);
  TINYGRAPH_ASSERT(array->size <= array->items_len);

  return array->items[i];
}


void tinygraph_array_set_at(tinygraph_array *array, uint32_t i, uint32_t value) {
  TINYGRAPH_ASSERT(array);
  TINYGRAPH_ASSERT(i < array->size);
  TINYGRAPH_ASSERT(array->size <= array->items_len);

  array->items[i] = value;
}


uint32_t tinygraph_array_get_size(tinygraph_array *array) {
  TINYGRAPH_ASSERT(array);
  TINYGRAPH_ASSERT(array->size <= array->items_len);

  return array->size;
}


uint32_t tinygraph_array_get_capacity(tinygraph_array *array) {
  TINYGRAPH_ASSERT(array);
  TINYGRAPH_ASSERT(array->size <= array->items_len);

  return array->items_len;
}


bool tinygraph_array_is_empty(tinygraph_array *array) {
  TINYGRAPH_ASSERT(array);
  TINYGRAPH_ASSERT(array->size <= array->items_len);

  return array->size == 0;
}


void tinygraph_array_clear(tinygraph_array *array) {
  TINYGRAPH_ASSERT(array);
  TINYGRAPH_ASSERT(array->size <= array->items_len);

  bool ok = tinygraph_array_resize(array, 0);

  (void)ok; // downsizing never fails
}


bool tinygraph_array_push(tinygraph_array *array, uint32_t value) {
  TINYGRAPH_ASSERT(array);
  TINYGRAPH_ASSERT(array->size <= array->items_len);

  if (array->size == UINT32_MAX) {
    TINYGRAPH_ASSERT(array->items_len == UINT32_MAX);

    return false;
  }

  const uint32_t i = array->size + 1;

  bool ok = tinygraph_array_resize(array, i);

  if (!ok) {
    return false;
  }

  tinygraph_array_set_at(array, i - 1, value);

  return true;
}


uint32_t tinygraph_array_pop(tinygraph_array *array) {
  TINYGRAPH_ASSERT(array);
  TINYGRAPH_ASSERT(array->size > 0);
  TINYGRAPH_ASSERT(array->size <= array->items_len);

  const uint32_t i = array->size - 1;
  const uint32_t value = array->items[i];

  bool ok = tinygraph_array_resize(array, i);

  (void)ok; // downsizing never fails

  return value;
}


void tinygraph_array_print_internal(tinygraph_array *array) {
  TINYGRAPH_ASSERT(array);
  TINYGRAPH_ASSERT(array->size <= array->items_len);

  fprintf(stderr, "array internals\n");

  fprintf(stderr, "size: %ju, capacity: %ju\n",
      (uintmax_t)array->size, (uintmax_t)array->items_len);

  fprintf(stderr, "items:");

  for (uint32_t i = 0; i < array->size; ++i) {
    fprintf(stderr, " %ju", (uintmax_t)array->items[i]);
  }

  fprintf(stderr, "\n");
}
