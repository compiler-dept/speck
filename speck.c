#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdarg.h>

/* Constants */

static const struct {
    char *red;
    char *green;
    char *yellow;
    char *blue;
    char *magenta;
    char *cyan;
    char *white;
    char *normal;
} colors = {
    .red     = "\x1B[31m",
    .green   = "\x1B[32m",
    .yellow  = "\x1B[33m",
    .blue    = "\x1B[34m",
    .magenta = "\x1B[35m",
    .cyan    = "\x1B[36m",
    .white   = "\x1B[37m",
    .normal  = "\x1B[0m"
};

/* Data structures */

struct state {
    int index;
    char **assertions;
    int *codes;
    char *function;
};

struct suite {
    char *name;
    char *c_file;
    char *so_file;
    int stat_loc;
    void *handle;
    char **tests;
    struct state **states;
};

struct statistic {
    char *symbols;
    char **failures;
    int flag;
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

/* Control functions */

void get_tests(struct suite *suite)
{
    FILE *fp = fopen(suite->c_file, "r");
    char *line = NULL;
    size_t linelen = 0;
    ssize_t len;
    int test_count = 0;
    while ((len = getline(&line, &linelen, fp)) > 0) {
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
}

void run_tests(struct suite *suite)
{
    int i = 0;
    for (; suite->tests[i] != NULL; i++) {
        struct state *state = dlsym(suite->handle, "state");
        state->index = 0;
        state->codes = NULL;
        state->assertions = NULL;
        state->function = suite->tests[i];

        void (*test)(void) = dlsym(suite->handle, suite->tests[i]);
        test();

        suite->states = realloc(suite->states, (i + 1) * sizeof(struct state *));
        suite->states[i] = malloc(sizeof(struct state));
        suite->states[i]->index = state->index;
        suite->states[i]->assertions = state->assertions;
        suite->states[i]->codes = state->codes;
    }

    suite->states = realloc(suite->states, (i + 1) * sizeof(struct state *));
    suite->states[i] = NULL;
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

            suites[count - 1]->states = NULL;
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

            if (suites[i]->states) {
                for (int j = 0; suites[i]->states[j] != NULL; j++) {
                    if (suites[i]->states[j]->codes) {
                        free(suites[i]->states[j]->codes);
                    }
                    if (suites[i]->states[j]->assertions) {
                        for (int k = 0; k < suites[i]->states[j]->index; k++) {
                            free(suites[i]->states[j]->assertions[k]);
                        }
                        free(suites[i]->states[j]->assertions);
                    }
                    free(suites[i]->states[j]);
                }
                free(suites[i]->states);
            }
        }

        free(suites[i]);
    }

    free(suites);
}

/* Statistic */

struct statistic *build_statistic(struct suite **suites)
{
    struct statistic *statistic = malloc(sizeof(struct statistic));
    statistic->symbols = malloc(sizeof(char));
    statistic->symbols[0] = '\0';
    statistic->failures = NULL;
    statistic->flag = 0;

    int index = 0;

    for (int suite = 0; suites[suite] != NULL; suite++) {
        if (suites[suite]->stat_loc == 0) {
            for (int state = 0; suites[suite]->states[state] != NULL; state++) {
                for (int assertion = 0; assertion < suites[suite]->states[state]->index; assertion++) {
                    size_t length = strlen(statistic->symbols);
                    statistic->symbols = realloc(statistic->symbols, (length + 5 + 1 + 1) * sizeof(char));
                    statistic->failures = realloc(statistic->failures, (index + 2) * sizeof(char *));
                    statistic->failures[index] = NULL;
                    statistic->failures[index + 1] = NULL;

                    if (suites[suite]->states[state]->codes[assertion] == 0) {
                        sprintf(statistic->symbols + length, "%s.", colors.green);
                        alloc_sprintf(&(statistic->failures[index]), "");
                    } else {
                        sprintf(statistic->symbols + length, "%sF", colors.red);
                        alloc_sprintf(&(statistic->failures[index]), "  - %s::%s", suites[suite]->name, suites[suite]->states[state]->assertions[assertion]);
                        statistic->flag = 1;
                    }

                    index++;
                }
            }
        }
    }

    return statistic;
}

void print_statistic(struct statistic *statistic)
{
    printf("%s\n", statistic->symbols);

    if (statistic->flag) {
        printf("\n%sFailures:\n", colors.red);

        for (int failure = 0; statistic->failures[failure] != NULL; failure++) {
            if (strlen(statistic->failures[failure]) > 0) {
                printf("%s\n", statistic->failures[failure]);
            }
        }
    }

    printf("%s", colors.normal);
}

void free_statistic(struct statistic *statistic)
{
    if (statistic) {
        if (statistic->symbols) {
            free(statistic->symbols);
        }

        if (statistic->failures) {
            for (int failure = 0; statistic->failures[failure] != NULL; failure++) {
                free(statistic->failures[failure]);
            }
            free(statistic->failures);
        }

        free(statistic);
    }
}

/* Compilation */

int compile_suite(struct suite *suite)
{
    char *compiler = "clang";

    char *args[24];
    int idx = 0;

    char *env_cc = getenv("CC");
    char *env_cflags = getenv("CFLAGS");
    char *env_ldflags = getenv("LDFLAGS");
    char *env_ldlibs = getenv("LDLIBS");

    if (env_cc) {
        args[idx++] = env_cc;
    } else {
        args[idx++] = compiler;
    }

    if (env_cflags) {
        args[idx++] = env_cflags;
    }

    if (env_ldflags) {
        args[idx++] = env_ldflags;
    }

    args[idx++] = "-Wall";
    args[idx++] = "-Werror";
    args[idx++] = "-g";
    args[idx++] = "-std=c11";
    args[idx++] = "-fpic";
    args[idx++] = "-shared";
    args[idx++] = "-o";
    args[idx++] = suite->so_file;
    args[idx++] = suite->c_file;

    if (env_ldlibs) {
        args[idx++] = env_ldlibs;
    }

    args[idx] = NULL;

    pid_t pid = fork();

    if (pid == 0) {
        execvp(compiler, args);
        exit(0);
    } else {
        waitpid(pid, &(suite->stat_loc), 0);
        return suite->stat_loc;
    }
}

/* Main */

int main(int argc, char **argv)
{
    int exit_code = EXIT_SUCCESS;
    struct suite **suites = get_suites();

    for (int suite = 0; suites[suite] != NULL; suite++) {
        if (compile_suite(suites[suite]) != 0) {
            continue;
        }

        load_suite(suites[suite]);
        run_tests(suites[suite]);
    }

    struct statistic *statistic = build_statistic(suites);

    if (statistic->flag) {
        exit_code = EXIT_FAILURE;
    }

    print_statistic(statistic);
    free_statistic(statistic);

    free_suites(suites);

    return exit_code;
}
