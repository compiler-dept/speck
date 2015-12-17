SPECK_PATH?=speck
SPECK=$(SPECK_PATH)/speck
SUITES=$(patsubst %.c, %.so, $(wildcard spec/*.c))
SPECK_VERSION=$(shell git describe --tags --dirty=+ || echo "UNKNOWN")

$(SPECK): $(SPECK).c
	@$(CC) -std=c1x -Wall -g -o $@ -DSPECK_VERSION=\"$(SPECK_VERSION)\" $< -ldl

spec/%.so: spec/%.c
	@$(CC) -Wall -g -I$(SPECK_PATH) $(SPECK_CFLAGS) $(SPECK_LDFLAGS) -fPIC -shared -o $@ $< $(SPECK_LIBS)
