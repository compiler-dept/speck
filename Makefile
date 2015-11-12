BIN=speck
CFLAGS=-Wall -g
LDLIBS=-ldl

.PHONY: all test valgrind clean

all: $(BIN)

SPECK_PATH=.
include speck.mk

test: $(SPECK) $(SUITES)
	@$(SPECK)

valgrind: $(SPECK) $(SUITES)
	@valgrind --leak-check=full --error-exitcode=1 $(SPECK)

style:
	astyle -A3s4SpHk3jn "*.c" "*.h" "spec/*.c"

clean:
	rm -f speck
	rm -rf speck.dSYM
	rm -f spec/*.o
	rm -f spec/*.so
	rm -rf spec/*.dSYM
