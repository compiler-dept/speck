#ifndef SPECK_H
#define SPECK_H

#include <stdlib.h>
#include <stdio.h>

/* State */

struct state {
    int index;
    int *codes;
};

struct state state = { .index = 0, .codes = NULL };

/* Assertions */

void sp_assert(int exp)
{
    state = 1;
    puts("ASSERT");
}

void sp_assert_equal_i(int x, int y)
{
    state = 2;
    puts("ASSERT_EQUAL_I");
}

#endif
