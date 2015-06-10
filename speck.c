#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

void (*register_sp_assert)(void(*)(int));
void sp_assert(int exp)
{
    puts("ASSERT");
}

int main(int argc, char **argv)
{
    void *lib = dlopen("spec/example.so", RTLD_LAZY);
    void *lib2 = dlopen("spec/example2.so", RTLD_LAZY);

    register_sp_assert = dlsym(lib, "register_sp_assert");
    register_sp_assert(sp_assert);
    register_sp_assert = dlsym(lib2, "register_sp_assert");
    register_sp_assert(sp_assert);

    void (*fun)(void) = dlsym(lib, "spec_example__sample_one");
    fun();
    fun = dlsym(lib2, "spec_example2__sample_one");
    fun();

    dlclose(lib);

    return EXIT_SUCCESS;
}
