#ifndef SPECK_H
#define SPECK_H

#include <stdlib.h>

void (*sp_assert)(int) = NULL;

void register_sp_assert(void (*assign_sp_assert)(int))
{
    sp_assert = assign_sp_assert;
}

#endif
