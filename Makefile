BIN=speck
CFLAGS=-Wall -g
LDLIBS=-ldl

TESTS=spec/example.so

.PHONY: all test clean

all: $(BIN)

spec/%.o: spec/%.c
	$(CC) $(CFLAGS) -fpic -c -o $@ $^

spec/%.so: spec/%.o
	$(CC) $(CFLAGS) --shared -o $@ $^

test: $(BIN) $(TESTS)
	./speck

clean:
	rm -f speck
	rm -f spec/*.o
	rm -f spec/*.so
