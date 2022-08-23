# [tinygraph](https://tinygraph.org)

Tiny and efficient graph abstractions


# Overview

## Graph Format

We use a compressed sparse row format to represent graphs in compact and efficient ways: ideal for large scale and high-performance use-cases.
The trade-off is limited support for modification. The use-cases we support: build a graph, work on the graph, and potentially build new graphs from there.

## Graph Compression

For compression we use techniques like: succinct data structures, storing deltas in vbyte format, and permuting graphs based on space filling curves.
Compressed graphs not only have a much smaller footprint, but we also keep an eye on viability and practicality: this is not a research project.

## Modern Hardware

Vector instruction sets are widely deployed with AVX2 available for 7+ years by now: we target modern hardware for efficiency gains at no cost.
We provide scalar fallbacks and base line implementations in case vector instructions are not supported: at the cost of runtime efficiency.

## Stable Library

We provide a stable API and ABI using semantic versioning: a stable ABI allows for simple shared library upgrades across minor versions.
On top of the stable API and ABI we provide bindings to other languages: they come with their own stability and support guarantees.


# Usage

See the example in [tinygraph/tinygraph-example.c](./tinygraph/tinygraph-example.c) for how to use the library.


# Interface

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

Copyright © 2021-2022 Daniel J. Hofmann

Distributed under the MIT License (MIT).
