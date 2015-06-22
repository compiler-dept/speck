BIN=speck
CFLAGS=-Wall -g
LDLIBS=-ldl

.PHONY: all test valgrind clean

all: $(BIN)

TESTS=$(patsubst %.c, %.so, $(wildcard spec/*.c))

spec/%.so: spec/%.c
	@$(CC) -fPIC -shared -o $@ $<

test: speck $(TESTS)
	@./speck

valgrind: speck $(TESTS)
	@valgrind --leak-check=full --error-exitcode=1 ./speck

style:
	astyle -A3s4SpHk3jn "*.c" "*.h" "spec/*.c"

clean:
	rm -f speck
	rm -rf speck.dSYM
	rm -f spec/*.o
	rm -f spec/*.so
	rm -rf spec/*.dSYM
