#ifndef SPECK_H
#define SPECK_H

#include <stdlib.h>

struct suite *suite_handle = NULL;
void set_suite(struct suite *suite)
{
    suite_handle = suite;
}

void (*sp_assert)(int) = NULL;
void register_sp_assert(void (*assign_sp_assert)(int))
{
    sp_assert = assign_sp_assert;
}

void (*sp_assert_equal_i)(int, int) = NULL;
void register_sp_assert_equal_i(void (*assign_assert_equal_i)(int, int))
{
    sp_assert_equal_i = assign_assert_equal_i;
}

#endif
