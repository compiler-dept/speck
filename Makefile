BIN=cspeck
CFLAGS=-Wall -g

.PHONY: all test clean

all: $(BIN)	

clean:
	rm -f cspeck
