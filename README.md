# [tinygraph](https://tinygraph.org)

Tiny and efficient graph abstractions


# Overview

The tinygraph library is a modern approach for working with compact graphs.

See our website at [tinygraph.org](https://tinygraph.org)


# Usage

See the example in [tinygraph/tinygraph-example.c](./tinygraph/tinygraph-example.c)


# API

See the documentation in [tinygraph/tinygraph.h](./tinygraph/tinygraph.h) for the library interface.


# Requirements

The tinygraph library requires 64bit Linux (gcc or clang).

By default we compile for the `x86-64-v3` microarchitecture which allows us to target hardware instruction sets such as `POPCNT`, `BMI2`, and the `AVX2` vector instruction set.
If you want to compile the library for your specific hardware you can pass `-march=native` instead in the `Makefile`.

On AMD CPUs before Zen 3 (released 2020) the `PDEP` instruction we rely on is implemented in microcode and you will notice a performance penalty.


# Building

To compile the library, its tests, and the example run

```bash
make
```

To install the library run

```bash
make install
```

Then
1. compile your own program against `tinygraph.h`
2. link your own program against `libtinygraph.so`


# Development

We provide a self-contained and reproducible docker environment for development with a tried and tested compiler and make setup.

In the `tinygraph` directory run

```bash
make sh
```

In the self-contained and reproducible environment you can then compile with

```bash
make
```

or for watching file modification events, re-compiling automatically

```bash
make watch
```


## License

Copyright © 2021-2025 Daniel J. Hofmann

Distributed under the MIT License (MIT).
