BIN=speck
CFLAGS=-Wall -g -std=c11
LDLIBS=-ldl

.PHONY: all test valgrind clean

all: $(BIN)

test: $(BIN)
	./speck

valgrind: $(BIN)
	valgrind --leak-check=full --error-exitcode=1 ./speck

style:
	astyle -A3s4SpHk3jn "*.c" "*.h" "spec/*.c"

clean:
	rm -f speck
	rm -rf speck.dSYM
	rm -f spec/*.o
	rm -f spec/*.so
	rm -rf spec/*.dSYM
