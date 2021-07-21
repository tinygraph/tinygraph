[![](tinygraph.png)](https://tinygraph.org)

Tinygraph provides compressed and efficient graph abstractions.


# Usage

```c
// Edges in the form of (source, target): (0, 1), (0, 2), etc.
const uint32_t sources[5] = {0, 0, 1, 1, 2};
const uint32_t targets[5] = {1, 2, 0, 2, 1};

// Creates a tiny graph with five edges in total
tinygraph_s graph = tinygraph_construct_from_sorted_edges(
  sources, targets, 5);

// Tiny graph functions might fail; check for this
if (!graph) {
  fprintf(stderr, "error: unable to construct graph\n");
  return EXIT_FAILURE;
}

// From here on we can use tinygraph functions taking a graph

// Make sure to release resources
tinygraph_destruct(graph);
```

See [tinygraph-example.c](./tinygraph-example.c)


# Interface

See documentation in [tinygraph.h](./tinygraph.h)


# Building

```bash
make
```

Then
1. compile your program against `tinygraph.h`
2. link your program against `libtinygraph.so`


# Development

Use the self-contained and reproducible docker environment we provide

```bash
make dev
```

watching file modification events, re-compiling automatically.


## License

Copyright © 2021 Daniel J. Hofmann

Distributed under the MIT License (MIT).
