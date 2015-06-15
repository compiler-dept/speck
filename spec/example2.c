#include <stdio.h>
#include "../speck.h"

void spec_example2__sample_one(void)
{
    sp_assert(42);
    sp_assert_equal_i(23, 23);
}

void spec_example2__sample_two(void)
{
    sp_assert(23);
}
