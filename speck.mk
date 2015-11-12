SPECK_PATH?=speck
SPECK=$(SPECK_PATH)/speck
SUITES=$(patsubst %.c, %.so, $(wildcard spec/*.c))

$(SPECK): $(SPECK).c
	@$(CC) -std=c11 -Wall -g -o $@ $< -ldl

spec/%.so: spec/%.c
	@$(CC) -Wall -g -I$(SPECK_PATH) $(SPECK_CFLAGS) $(SPECK_LDFLAGS) -fPIC -shared -o $@ $< $(SPECK_LIBS)
