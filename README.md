# [tinygraph](https://tinygraph.org)

Tiny and efficient graph abstractions


# Overview

The tinygraph library is a modern approach for working with compact graphs.

See our website at [tinygraph.org](https://tinygraph.org)


# Usage

```c
const uint32_t sources[5] = {0, 0, 1, 1, 2};
const uint32_t targets[5] = {1, 2, 0, 2, 1};

tinygraph_s graph = tinygraph_construct_from_sorted_edges(sources, targets, 5);

if (!graph) {
  fprintf(stderr, "error: unable to construct graph\n");
  return EXIT_FAILURE;
}

tinygraph_print(graph);

tinygraph_destruct(graph);
```

See the example in [tinygraph/tinygraph-example.c](./tinygraph/tinygraph-example.c)


# API

See the documentation in [tinygraph/tinygraph.h](./tinygraph/tinygraph.h) for the interface the library provides.


# Building

The tinygraph library requires 64bit Linux (gcc or clang).

To compile the library, its tests, and the example, run in the `tinygraph` directory

```bash
make
```

Then
1. compile your own program against `tinygraph.h`
2. link your own program against `libtinygraph.so`


# Development

Use the self-contained and reproducible docker environment in the `tinygraph` directory

```bash
make dev
```

watching file modification events, re-compiling automatically.


## License

Copyright © 2021-2023 Daniel J. Hofmann

Distributed under the MIT License (MIT).
