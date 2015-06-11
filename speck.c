#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

/* Data structures */

struct suite {
    char *path;
    void *handle;
    void **tests;
};

/* Assertions */

void sp_assert(int exp)
{
    puts("ASSERT");
}

void sp_assert_equal_i(int x, int y)
{
    puts("ASSERT_EQUAL_I");
}

/* Control functions */

void register_assertions(void *lib)
{
    void (*register_assert)(void(*)(int)) = NULL;
    register_assert = dlsym(lib, "register_sp_assert");
    register_assert(sp_assert);

    void (*register_assert_equal_i)(void(*)(int, int)) = NULL;
    register_assert_equal_i = dlsym(lib, "register_sp_assert_equal_i");
    register_assert_equal_i(sp_assert_equal_i);
}

void get_tests(struct suite *suite)
{
    if (suite) {
        if (strcmp(suite->path, "spec/example.so") == 0) {
            puts(suite->path);
            suite->tests = malloc(2 * sizeof(char *));
            suite->tests[0] = strdup("spec_example__sample_one");
            suite->tests[1] = NULL;
        } else if (strcmp(suite->path, "spec/example2.so") == 0) {
            suite->tests = malloc(2 * sizeof(char *));
            suite->tests[0] = strdup("spec_example2__sample_one");
            suite->tests[1] = NULL;
        }
    }
}

struct suite *open_suite(const char *path)
{
    struct suite *suite = malloc(sizeof(struct suite));
    suite->path = strdup(path);
    suite->handle = dlopen(path, RTLD_LAZY);
    get_tests(suite);

    return suite;
}

void close_suite(struct suite *suite)
{
    free(suite->path);
    dlclose(suite->handle);
    for (int i = 0; suite->tests[i] != NULL; i++) {
        free(suite->tests[i]);
    }
    free(suite->tests);
    free(suite);
}

void run_tests(struct suite *suite) {
    void (*test)(void) = NULL;

    for (int i = 0; suite->tests[i] != NULL; i++) {
        test = dlsym(suite->handle, suite->tests[i]);
        test();
    }
}

/* Main */

int main(int argc, char **argv)
{
    struct suite *suite1 = open_suite("spec/example.so");
    struct suite *suite2 = open_suite("spec/example2.so");

    register_assertions(suite1->handle);
    register_assertions(suite2->handle);

    run_tests(suite1);
    run_tests(suite2);

    close_suite(suite1);
    close_suite(suite2);

    return EXIT_SUCCESS;
}
