#include <stdio.h>
#include "../speck.h"

void spec_example2__sample_one(void)
{
    puts("Hello, I'm a shared library 2.");
    sp_assert_equal_i(42, 23);
}
