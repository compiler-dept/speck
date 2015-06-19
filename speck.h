#ifndef SPECK_H
#define SPECK_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* State */

struct state {
    int index;
    char **assertions;
    int *codes;
    char *function;
};

struct state state = { .index = 0, .assertions = NULL, .codes = NULL };

/* Helper functions */

int alloc_sprintf(char **str, const char *format, ...)
{
    const int start_size = 8;
    *str = malloc(start_size * sizeof(char));
    va_list ap, ap_copy;
    va_start(ap, format);
    va_copy(ap_copy, ap);
    int size = vsnprintf(*str, start_size, format, ap);
    va_end(ap);
    if (size > start_size - 1) {
        *str = realloc(*str, (size + 1) * sizeof(char));
        vsnprintf(*str, size + 1, format, ap_copy);
        va_end(ap_copy);
    }

    return size;
}

/* Assertions */

void sp_assert(int expr)
{
    state.assertions = realloc(state.assertions, (state.index + 1) * sizeof(char *));
    alloc_sprintf(&(state.assertions[state.index]), "%s::sp_assert(%d)", state.function, expr);

    state.codes = realloc(state.codes, (state.index + 1) * sizeof(int));

    if (expr) {
        state.codes[state.index] = 0;
    } else {
        state.codes[state.index] = 1;
    }

    state.index++;
}

void sp_assert_equal_i(int x, int y)
{
    state.assertions = realloc(state.assertions, (state.index + 1) * sizeof(char *));
    alloc_sprintf(&(state.assertions[state.index]), "%s::sp_assert(%d, %d)", state.function, x, y);

    state.codes = realloc(state.codes, (state.index + 1) * sizeof(int));

    if (x == y) {
        state.codes[state.index] = 0;
    } else {
        state.codes[state.index] = 1;
    }

    state.index++;
}

#endif
