.POSIX:

CFLAGS=-std=c99 -O3 -Wall -Wextra -pedantic -fvisibility=hidden -ffunction-sections -fPIC -flto
LDFLAGS=-Wl,--gc-sections
LDLIBS=-lm

all: libtinygraph.so tinygraph-example tinygraph-tests

tinygraph.o: tinygraph.h tinygraph-utils.h
tinygraph-impl.o: tinygraph-impl.h tinygraph-utils.h
tinygraph-array.o: tinygraph-array.h tinygraph-utils.h
tinygraph-bitset.o: tinygraph-bitset.h tinygraph-utils.h

libtinygraph.so: LDFLAGS=-shared -Wl,-soname,libtinygraph.so.0
libtinygraph.so: tinygraph.o tinygraph-impl.o tinygraph-array.o tinygraph-bitset.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@ln -sf libtinygraph.so libtinygraph.so.0

tinygraph-tests: LDFLAGS+=-Wl,-rpath=.
tinygraph-tests: tinygraph.o tinygraph-impl.o tinygraph-array.o tinygraph-bitset.o

tinygraph-example: LDFLAGS+=-Wl,-rpath=.
tinygraph-example: libtinygraph.so

watch: all
	@while ! inotifywait -e modify -qq *.c *.h Makefile; do clear; make --no-print-directory; done

dev:
	@docker build -t daniel-j-h/tinygraph .
	@docker run -it --rm -v $(CURDIR):/app daniel-j-h/tinygraph

clean:
	@rm -f tinygraph*.o libtinygraph.so libtinygraph.so.0 tinygraph-example tinygraph-tests

.PHONY: all clean watch dev
