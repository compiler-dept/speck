#include <speck.h>

void spec_sample_one(void)
{
    //int i = *(int*)0;
    //(void)i;
    sp_assert_equal_i(42, 42);
    sp_assert(42);
}

void spec_sample_two(void)
{
    sp_assert_equal_s("foo", "foo");
}
