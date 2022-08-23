CFLAGS+=-std=c99 -O3 -Wall -Wextra -pedantic -fvisibility=hidden -ffunction-sections -fPIC -flto -pipe -MMD
CFLAGS+=-march=native -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -DTINYGRAPH_HAS_BMI2
LDFLAGS+=-Wl,--gc-sections
LDLIBS+=-lm

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: libtinygraph.so tinygraph-example tinygraph-tests

-include ${DEP}

libtinygraph.so: LDFLAGS=-shared -Wl,-soname,libtinygraph.so.0
libtinygraph.so: $(filter-out tinygraph-tests.o tinygraph-example.o, $(OBJ))
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@ln -sf libtinygraph.so libtinygraph.so.0

tinygraph-tests: LDFLAGS+=-Wl,-rpath=.
tinygraph-tests: $(filter-out tinygraph-example.o, $(OBJ))  # access to internals

tinygraph-example: LDFLAGS+=-Wl,-rpath=.
tinygraph-example: libtinygraph.so  # example only has access to public interface

watch:
	@while true; do \
		inotifywait -q -e modify -e create -e delete -e move *.c *.h Makefile; \
		clear; make --no-print-directory; \
	done

dev: clean
	@docker build -t tinygraph/tinygraph .
	@docker run -it --rm --pull never --read-only -v $(CURDIR):/app tinygraph/tinygraph make watch

clean:
	@rm -f tinygraph*.o tinygraph*.d libtinygraph.so libtinygraph.so.0 tinygraph-example tinygraph-tests

.PHONY: all clean watch dev
