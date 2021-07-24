CFLAGS=-std=c99 -O3 -Wall -Wextra -pedantic -fvisibility=hidden -ffunction-sections -fPIC -flto -pipe
LDFLAGS=-Wl,--gc-sections
LDLIBS=-lm

all: libtinygraph.so tinygraph-example tinygraph-tests

tinygraph.o: tinygraph.h tinygraph-utils.h tinygraph-impl.h
tinygraph-impl.o: tinygraph-impl.h tinygraph-utils.h
tinygraph-array.o: tinygraph-array.h tinygraph-utils.h
tinygraph-bitset.o: tinygraph-bitset.h tinygraph-utils.h
tinygraph-delta.o: tinygraph-delta.h tinygraph-utils.h
tinygraph-zigzag.o: tinygraph-zigzag.h tinygraph-utils.h
tinygraph-vbyte.o: tinygraph-vbyte.h tinygraph-utils.h
tinygraph-stack.o: tinygraph-stack.h tinygraph-utils.h tinygraph-array.h tinygraph-array.o
tinygraph-queue.o: tinygraph-queue.h tinygraph-utils.h tinygraph-stack.h tinygraph-stack.o
tinygraph-example.o: tinygraph.h
tinygraph-tests.o: tinygraph.h tinygraph-impl.h tinygraph-array.h tinygraph-stack.h tinygraph-queue.h tinygraph-bitset.h tinygraph-delta.h tinygraph-utils.h

libtinygraph.so: LDFLAGS=-shared -Wl,-soname,libtinygraph.so.0
libtinygraph.so: tinygraph.o tinygraph-impl.o tinygraph-array.o tinygraph-bitset.o tinygraph-stack.o tinygraph-queue.o tinygraph-delta.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@ln -sf libtinygraph.so libtinygraph.so.0

tinygraph-tests: LDFLAGS+=-Wl,-rpath=.
tinygraph-tests: tinygraph.o tinygraph-impl.o tinygraph-array.o tinygraph-bitset.o tinygraph-stack.o tinygraph-queue.o tinygraph-delta.o tinygraph-zigzag.o tinygraph-vbyte.o

tinygraph-example: LDFLAGS+=-Wl,-rpath=.
tinygraph-example: libtinygraph.so

watch:
	@while true; do \
		inotifywait -q -e modify -e create -e delete -e move *.c *.h Makefile; \
		clear; make --no-print-directory; \
	done

dev: clean
	@docker build -t tinygraph/tinygraph .
	@docker run -it --rm --pull never --read-only -v $(CURDIR):/app tinygraph/tinygraph make watch

clean:
	@rm -f tinygraph*.o libtinygraph.so libtinygraph.so.0 tinygraph-example tinygraph-tests

.PHONY: all clean watch dev
