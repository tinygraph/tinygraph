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

See the example in [src/example/example.c](./tinygraph/tinygraph-example.c)


# API

See the documentation in [src/tinygraph/tinygraph.h](./src/tinygraph/tinygraph.h) for the interface the library provides.


# Development

We use bazel for development; to set it up and get started

```bash
curl --proto 'https' --tlsv1.3 -sSLf https://github.com/bazelbuild/bazelisk/releases/download/v1.16.0/bazelisk-linux-amd64 -o ~/.local/bin/bazel
chmod a+x ~/.local/bin/bazel
```

To build libtinygraph

```bash
bazel build //tinygraph/tinygraph
```

To build specific packages and run their tests e.g. for the bitset package

```bash
bazel test //tinygraph/bitset:all
```


## License

Copyright © 2021-2023 Daniel J. Hofmann

Distributed under the MIT License (MIT).
