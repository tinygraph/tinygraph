[![](tinygraph.png)](https://tinygraph.org)

Tiny and efficient graph abstractions.


# Usage

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

Use the self-contained and reproducible docker environment

```bash
make dev
```

watching file modification events, re-compiling automatically.


## License

Copyright © 2021 Daniel J. Hofmann

Distributed under the MIT License (MIT).
