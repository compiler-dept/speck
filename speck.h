#ifndef SPECK_H
#define SPECK_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* State */

struct state {
    int index;
    char **assertions;
    int *codes;
};

struct state state = { .index = 0, .assertions = NULL, .codes = NULL };

/* Assertions */

void sp_assert(int expr)
{
    state.assertions = realloc(state.assertions, (state.index + 1) * sizeof(char *));
    state.assertions[state.index] = malloc((strlen("sp_assert") + 1) * sizeof(char));
    strcpy(state.assertions[state.index], "sp_assert");

    state.codes = realloc(state.codes, (state.index + 1) * sizeof(int));

    if (expr) {
        state.codes[state.index] = 1;
    } else {
        state.codes[state.index] = 0;
    }

    state.index++;
}

void sp_assert_equal_i(int x, int y)
{
    state.assertions = realloc(state.assertions, (state.index + 1) * sizeof(char *));
    state.assertions[state.index] = malloc((strlen("sp_assert_equal_i") + 1) * sizeof(char));
    strcpy(state.assertions[state.index], "sp_assert_equal_i");

    state.codes = realloc(state.codes, (state.index + 1) * sizeof(int));

    if (x == y) {
        state.codes[state.index] = 1;
    } else {
        state.codes[state.index] = 0;
    }

    state.index++;
}

#endif
