#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

/* Data structures */

struct suite {
    char *path;
    void *handle;
    char **tests;
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

    register_assertions(suite->handle);

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
    const int suites = 2;

    struct suite **suite = malloc(suites * sizeof(struct suite *));

    suite[0] = open_suite("spec/example.so");
    suite[1] = open_suite("spec/example2.so");

    for (int i = 0; i < suites; i++) {
        run_tests(suite[i]);
        close_suite(suite[i]);
    }

    return EXIT_SUCCESS;
}
