BIN=speck
CFLAGS=-Wall -g -std=c11
LDLIBS=-ldl

TESTS=spec/example.so spec/example2.so

.PHONY: all test valgrind clean

all: $(BIN)

spec/%.o: spec/%.c
	$(CC) $(CFLAGS) -fpic -c -o $@ $^

spec/%.so: spec/%.o
	$(CC) $(CFLAGS) --shared -o $@ $^

test: $(BIN) $(TESTS)
	./speck

valgrind: test
	valgrind --leak-check=full --error-exitcode=1 ./speck

style:
	astyle -A3s4SpHk3jn "*.c" "*.h" "spec/*.c"

clean:
	rm -f speck
	rm -f spec/*.o
	rm -f spec/*.so
