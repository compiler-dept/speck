#include <stdio.h>
#include "../speck.h"

void spec_example__sample_one(void)
{
    sp_assert_equal_i(42, 42);
    sp_assert(1);
}
