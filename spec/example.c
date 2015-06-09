#include <stdio.h>
#include "../speck.h"

void spec_example__sample_one(void)
{
    puts("Hello, I'm a shared library.");
    sp_assert(42);
}
