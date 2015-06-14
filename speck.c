#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdarg.h>

/* Data structures */

struct suite {
    char *name;
    char *c_file;
    char *so_file;
    void *handle;
    char **tests;
    int stat_loc;
};

/* Helper functions */

char *string_dup(const char *str)
{
    size_t len = strlen(str) + 1;
    char *new_str = malloc(len * sizeof(char));
    if (new_str) {
        memcpy(new_str, str, len);
    }

    return new_str;
}

int alloc_sprintf(char **str, const char *format, ...)
{
    const int start_size = 8;
    *str = malloc(start_size * sizeof(char));
    va_list ap, ap_copy;
    va_start(ap, format);
    va_copy(ap_copy, ap);
    int size = vsnprintf(*str, start_size, format, ap);
    va_end(ap);
    if (size > start_size - 1) {
        *str = realloc(*str, (size + 1) * sizeof(char));
        vsnprintf(*str, size + 1, format, ap_copy);
        va_end(ap_copy);
    }

    return size;
}

char *str_match(const char text[])
{
    char str[] = "void spec_";
    int len = 10;
    size_t textlen = strlen(text);

    if (textlen >= len) {
        for (int i = 0; i < len; i++) {
            if (str[i] != text[i]) {
                return NULL;
            }
        }

        int pre_offset = 5; /* "void " */
        int post_offset = 7; /* "(void)\n" */

        char *match = malloc((textlen - pre_offset - post_offset + 1) * sizeof(char));

        memcpy(match, text + pre_offset, textlen - pre_offset - post_offset);
        match[textlen - pre_offset - post_offset] = '\0';

        return match;
    }

    return NULL;
}

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
    FILE *fp = fopen(suite->c_file, "r");
    char *line = NULL;
    size_t linelen = 0;
    ssize_t len;
    int test_count = 0;
    while((len = getline(&line, &linelen, fp)) > 0) {
        char *temp = str_match(line);
        if (temp) {
            suite->tests = realloc(suite->tests, (test_count + 1) * sizeof(char *));
            suite->tests[test_count++] = temp;
        }
        free(line);
        line = NULL;
    }

    suite->tests = realloc(suite->tests, (test_count + 1) * sizeof(char *));
    suite->tests[test_count] = NULL;

    free(line);
    fclose(fp);
}

void load_suite(struct suite *suite)
{
    suite->handle = dlopen(suite->so_file, RTLD_LAZY);

    get_tests(suite);

    register_assertions(suite->handle);
}

void run_tests(struct suite *suite)
{
    void (*test)(void) = NULL;

    for (int i = 0; suite->tests[i] != NULL; i++) {
        test = dlsym(suite->handle, suite->tests[i]);
        test();
    }
}

struct suite **get_suites(void)
{
    char spec_dir[] = "spec";
    struct suite **suites = NULL;

    DIR *directory = opendir(spec_dir);
    struct dirent *entry = NULL;

    int count = 0;
    while ((entry = readdir(directory)) != NULL) {
        if (strstr(entry->d_name, ".c") != NULL) {
            count++;
            suites = realloc(suites, count * sizeof(struct suite *));
            suites[count - 1] = malloc(sizeof(struct suite));

            size_t len = strstr(entry->d_name, ".c") - entry->d_name;
            char *base_name = malloc((len + 1) * sizeof(char));

            memcpy(base_name, entry->d_name, len);
            base_name[len] = '\0';

            alloc_sprintf(&(suites[count - 1]->c_file), "%s/%s.c", spec_dir, base_name);
            alloc_sprintf(&(suites[count - 1]->so_file), "%s/%s.so", spec_dir, base_name);
            suites[count - 1]->name = base_name;

            suites[count - 1]->tests = NULL;
        }
    }

    closedir(directory);

    count++;
    suites = realloc(suites, count * sizeof(struct suite *));

    suites[count - 1] = NULL;

    return suites;
}

void free_suites(struct suite **suites)
{
    for (int i = 0; suites[i] != NULL; i++) {
        free(suites[i]->name);
        free(suites[i]->c_file);
        free(suites[i]->so_file);

        if (suites[i]->stat_loc == 0) {
            dlclose(suites[i]->handle);

            for (int j = 0; suites[i]->tests[j] != NULL; j++) {
                free(suites[i]->tests[j]);
            }
            free(suites[i]->tests);
        }

        free(suites[i]);
    }

    free(suites);
}

/* Compilation */

char *compiler = "clang";

int compile_suite(struct suite *suite)
{
    printf("Compiling %s ...", suite->name);

    char *args[10];
    int idx = 0;

    args[idx++] = compiler;
    args[idx++] = "-Wall";
    args[idx++] = "-g";
    args[idx++] = "-std=c11";
    args[idx++] = "-fpic";
    args[idx++] = "-shared";
    args[idx++] = "-o";
    args[idx++] = suite->so_file;
    args[idx++] = suite->c_file;
    args[idx] = NULL;

    pid_t pid = fork();

    if (pid == 0) {
        execvp(compiler, args);
        exit(0);
    } else {
        waitpid(pid, &(suite->stat_loc), 0);
        if (suite->stat_loc == 0) {
            printf(" DONE\n");
            return 1;
        } else {
            printf(" ERROR\n");
            return 0;
        }
    }
}

/* Main */

int main(int argc, char **argv)
{
    struct suite **suites = get_suites();

    for (int i = 0; suites[i] != NULL; i++) {
        printf("Suite: %s\n", suites[i]->name);
        if (!compile_suite(suites[i])) {
            continue;
        }

        load_suite(suites[i]);
        run_tests(suites[i]);
    }

    free_suites(suites);

    return EXIT_SUCCESS;
}
