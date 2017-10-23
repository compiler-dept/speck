#include <speck.h>

void spec_sample_one(void)
{
    sp_assert(42);
    sp_assert_equal_i(23, 23);
}

void spec_sample_two(void)
{
    sp_assert(23);
}

void spec_sample_three(void) {
    sp_assert(42);
}
