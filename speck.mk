SPECK_PATH?=speck
SPECK=$(SPECK_PATH)/speck
TESTS=$(patsubst %.c, %.so, $(wildcard spec/*.c))

$(SPECK): $(SPECK).c
	@$(CC) -Wall -g -o $@ $< -ldl

spec/%.so: spec/%.c
	@$(CC) -Wall -g -Ispeck $(SPECK_CFLAGS) $(SPECK_LDFLAGS) -fPIC -shared -o $@ $< $(SPECK_LIBS)
