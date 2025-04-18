CFLAGS+=-std=c99 -O3 -march=x86-64-v3 -Wall -Wextra -pedantic -fvisibility=hidden -ffunction-sections -fPIC -flto -pipe -MMD
LDFLAGS+=-Wl,--gc-sections -flto
LDLIBS+=-lm
PREFIX?=/usr/local

SRC:=$(wildcard *.c)
OBJ:=$(SRC:.c=.o)
DEP:=$(SRC:.c=.d)

.PHONY: all
all: libtinygraph.so tinygraph-example

.PHONY: install
install: libtinygraph.so
	mkdir -p $(PREFIX)/include/tinygraph
	install -m 0644 tinygraph.h $(PREFIX)/include/tinygraph
	mkdir -p $(PREFIX)/lib
	install -m 0644 libtinygraph.so $(PREFIX)/lib
	ln -sf $(PREFIX)/lib/libtinygraph.so $(PREFIX)/lib/libtinygraph.so.0
	ln -sf $(PREFIX)/lib/libtinygraph.so $(PREFIX)/lib/libtinygraph.so.0.0

-include ${DEP}

libtinygraph.so: CFLAGS+=-DNDEBUG
libtinygraph.so: LDFLAGS+=-shared -Wl,-soname,libtinygraph.so.0
libtinygraph.so: $(filter-out tinygraph-tests.o tinygraph-example.o, $(OBJ))
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@ln -sf libtinygraph.so libtinygraph.so.0

tinygraph-tests: LDFLAGS+=-Wl,-rpath=.:tinygraph
tinygraph-tests: $(filter-out tinygraph-example.o, $(OBJ))  # access to internals

tinygraph-example: LDFLAGS+=-Wl,-rpath=.:tinygraph
tinygraph-example: libtinygraph.so  # example only has access to public interface

.PHONY: info
info:
	$(info CFLAGS $(CFLAGS))
	$(info LDFLAGS $(LDFLAGS))
	$(info LDLIBS $(LDLIBS))
	$(info PREFIX $(PREFIX))

.PHONY: watch
watch:
	@while true; do \
		inotifywait -q -e modify -e create -e delete -e move *.c *.h Makefile; \
		make --no-print-directory; \
	done

.PHONY: sanitize
sanitize: CFLAGS+=-O0 -g -fsanitize=undefined,address -fno-omit-frame-pointer
sanitize: LDLIBS:=-lasan -lubsan -lm
sanitize: tinygraph-tests

.PHONY: perf
perf: CFLAGS:=$(filter-out -flto,$(CFLAGS)) -fno-omit-frame-pointer
perf: clean tinygraph-tests
	@perf record --freq 1000 -g ./tinygraph-tests
	@perf script report flamegraph --allow-download
	@xdg-open flamegraph.html

.PHONY: pgo1
pgo1: CFLAGS+=-fprofile-generate
pgo1: LDFLAGS+=-fprofile-generate
pgo1: all tinygraph-tests

.PHONY: pgo2
pgo2: CFLAGS+=-fprofile-use
pgo2: LDFLAGS+=-fprofile-use
pgo2: all tinygraph-tests

.PHONY: sh
sh: clean
	@docker build -q -t tinygraph/tinygraph .
	@docker run -it --rm --pull never --network=none -v $(CURDIR):/app:z tinygraph/tinygraph

.PHONY: clean
clean:
	@rm -f tinygraph*.o tinygraph*.d libtinygraph.so libtinygraph.so.0 tinygraph-example tinygraph-tests perf.data perf.data.old flamegraph.html
