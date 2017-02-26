# Speck

[![Build
Status](https://travis-ci.org/compiler-dept/speck.svg?branch=master)](https://travis-ci.org/compiler-dept/speck)

`Speck`, pronounced as /ˈbeɪ.kən/.

`Speck` is a small and fast unit testing framework for the C programming
language, that helps you to keep track of your programs integrity throughout
your development. It is provided as a set of just two source files that can be
copied into your project. There is also no need for extra scripts that generate
your test code from a specification. Your specification is the test itself and
it too is written in pure C.

## Setup

Speck offers three ways to integrate it into your C project. You can set it up
by importing it as a Git submodule or by manual install. The preferred method
presently is using a Git submodule. It makes updating `Speck` a lot easier in
the long run.

### Git Submodule

Importing `Speck` as a Git submodule works like importing every other submodule:

    $ git submodule add https://github.com/compiler-dept/speck.git

This creates a folder called `speck` containing the test framework. `Speck` is
distributed with a `Makefile` include `speck.mk`, that makes it a lot easier to
set it up. `speck.mk` contains all necessary targets and variables to build
`Speck` and your test suites.

Create a folder called `spec` that will hold your test suites:

    $ mkdir spec

Everything else you have to do, is including the file `speck.mk` into your
`Makefile`:

    -include speck/speck.mk

Now, all variables and targets needed for `Speck` are available in your
`Makefile`. The only thing that is left to do, is creating a target to run your
test suites. Let's call it `test`:

    test: $(SPECK) $(SUITES)
        @$(SPECK)

`speck.mk` also provides you with some variables you can set in your `Makefile`
to control what is compiled and linked into your test suites:

- `SPECK_CFLAGS`: Additional `CFLAGS` used for compilation of every test suite
(e.g. `-Werror` or `-Iinclude`).
- `SPECK_LDFLAGS`: Additional `LDFLAGS` used for compilation of every test suite
(e.g. `-Llibs`).
- `SPECK_LIBS`: Additional files and libraries for linking with every test
suite. This is where you put the code you want to test (e.g. `-lpthread` or just
`src/file.c`).

If you are only using `*.c` files inside the the `SPECK_LIBS` variable, you can
create an additional `Makefile` target:

    $(SUITES): $(SPECK_LIBS)

This allows you to recompile all suites automatically if some library source has
changed.

If you want to put `Speck` into a folder other than `speck`, you can do so. But
you have to set the `SPECK_PATH` variable in your `Makefile` to the location,
to ensure that `Speck` is working as expected.

### Manual Setup

If you aren't using Git or don't want to use a Git submodule you can follow this
manual setup. In order to update your copy of `Speck` you have to take care of
that yourself, instead of doing a simple `git submodule update`.

To get started, you just have to copy the files `speck.c` and `speck.h` into the
root folder of your C project. Create a folder called `spec` that will hold your
test suites:

    $ mkdir spec

You will need to add some rules to your `Makefile` in order to compile `speck`
and your tests.

The first rule you need to add, is the list of test suites:

    SUITES=$(patsubst %.c, %.so, $(wildcard spec/*.c))

This wildcard matches all `.c` files in your `spec` folder as a single test
suite.

Every `Speck` test suite is compiled into a shared object file (`.so`) and then
loaded and executed by `Speck`. Every piece of code that needs to be tested has
to be compiled additionally into the test suites. For this to work you need to
add another rule to your `Makefile`:

    spec/%.so: spec/%.c
        @$(CC) -g -I. -fPIC -shared -o $@ $<

At the end of the second line you can attach further files (`.c`, `.o`, `.s`) or
libraries (`-lx`) to pull in all code needed for execution. You can also use all
kinds of `CFLAGS` and other options you need to compile the test suites.

Of course we need a rule to compile `Speck` itself:

    speck: speck.c
    	$(CC) -std=c11 -o $@ $< -ldl

Don't worry, we're almost done. However, wouldn't it be nice to be able to call
your tests with `make test`? Therefore we need a last rule:

    test: speck $(SUITES)
    	@./speck

The rule tells `make`, that if the target `test` is executed, `Speck` and all
test suites have to be compiled and then executes `Speck`.

For your convenience, all the `Makefile` additions in one place:

    SUITES=$(patsubst %.c, %.so, $(wildcard spec/*.c))

    spec/%.so: spec/%.c
        @$(CC) -g -I. -fPIC -shared -o $@ $<

    speck: speck.c
        $(CC) -g -std=c11 -o $@ $< -ldl

    test: speck $(SUITES)
        @./speck

## Writing Tests

To start writing tests, you have to know, that you can organize your tests in
test suites. Every suite is represented as a `.c` file in the `spec` directory.

Let's create a first test suite to test some arithmetics:

    $ touch spec/arithmetics.c

The first thing you need to do is insert the `include` to the header file
`speck.h` into the suite.

    #include <speck.h>

Because the test suites are plain C, you can include any header you want, to
build your tests.

`Speck` searches for specific test functions that are to be executed later. They
must match the following form:

    void spec_<name>(void)
    {
        // test code
    }

The only thing you may change is the `<name>` tag of the function. `Speck` will
run all functions defined this way. Let's write a first test for our suite:

    void spec_addition(void)
    {
        int number = 4 + 3;

        sp_assert_equal_i(number, 7);
    }

As you can see, we wrote some code we want to test (`int number = 4 + 3;`). We
assume we are testing the `+` operator for its correctness. To give `Speck` a
way of determining if everything worked out as expected , you have to use
assertions. There are different types of assertions implemented. Scroll down to
find out what assertions are possible. In the **addition** example above, we let
`Speck` check that `number` is indeed `7` at the end of the test.

## Assertions

There are currently the following assertions implemented:

- `sp_assert(expression)`: Evaluate the expression and fail if the result is
false.
- `sp_assert_equal_i(number_a, number_b)`: Evaluate the equality of both numbers
and fail if they're different.
- `sp_assert_equal_s(str_a, str_b)`: Evaluate the equality of both strings using
`strcmp` and fail if they're different.

## Running Speck

If you have finished writing your test suites, you can run `Speck`:

    $ make test

## Forking mode

Running the test suites in normal mode is nice and fast, but sometimes you want
to test something other than simple assertions. In this case it can come to
lower level errors like segmentation faults. These may be hard to detect,
because the whole test runner will crash upon a segfault. With the `Forking
mode`, `Speck` will start every test inside its own forked process. When a test
provokes a segfault, only its corresponding child process will die. The error
that caused the crash will then be reported back to the test runner and is
displayed to the user. To enable the `Forking mode` you have to run `Speck` with
the command line option `-f`:

    $ speck -f

You can also change your `Makefile` to run your tests in `Forking mode` whenever
you type `make test`:

    test: $(SPECK) $(SUITES)
        @$(SPECK) -f

## Command line options

- `-f`: Enable forking mode.
- `-v`: Show version number.

## A Real-World Example

If you want to see `Speck` in real-life you can checkout our
[LibCollect](https://github.com/compiler-dept/libcollect) project.
