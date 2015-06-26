# Speck

[![Build Status](https://travis-ci.org/compiler-dept/speck.svg?branch=master)](https://travis-ci.org/compiler-dept/speck)

`Speck`, pronounced as /ˈbeɪ.kən/.

`Speck` is a small and fast unit testing framework for the C programming
language, that helps you to keep track of your programs integrity throughout
your development. It is provided as a set of just two source files that are
copied into your project. There is also no need for some extra scripts that
generate your test code from a specification. Your specification is the test
itself and its written also in pure C.

## Quick Start

To get started using `Speck` for your tests, you just have to copy the files
`speck.c` and `speck.h` into the root folder of your C project. Create a folder
called `spec` that will hold your test suites:

    $ mkdir spec

You will need to add some rules to your `Makefile` in order to compile `speck`
and your tests.

The first rule you need to add, is the list of test suites:

    SUITES=$(patsubst %.c, %.so, $(wildcard spec/*.c))

This is a wildcard matcher that matches all `.c` files in your `spec` folder as
a single test suite.

Every `Speck` test suite is compiled into a shared object file (`.so`) and then
loaded and executed by `Speck`. Every piece of code that needs to be tested
has to be compiled additionally into the test suites. For that you need to add
another rule to your `Makefile`:

    spec/%.so: spec/%.c
        @$(CC) -fPIC -shared -o $@ $<

At the end of the second line you can attach other files (`.c`, `.o`, `.s`) or
libraries (`-lx`) to pull in all needed code for execution. You can also use all
kinds of `CFLAGS` and other options you need to compile the test suites.

Of cause we need a rule to compile `Speck` itself:

    speck: speck.c
    	$(CC) -std=c11 -o $@ $< -ldl

We're almost done, but wouldn't it be nice to be able to call your test with
`make test`? Therefore we need a last rule:

    test: speck $(SUITES)
    	@./speck

The rule tells `make`, that if the target `test` is executed, `Speck` and all
test suites have to be compiled and then execute `Speck`.

Again, all additions to your `Makefile` at once:

    SUITES=$(patsubst %.c, %.so, $(wildcard spec/*.c))

    spec/%.so: spec/%.c
	    @$(CC) -g -fPIC -shared -o $@ $<

    speck: speck.c
   	    $(CC) -g -std=c11 -o $@ $< -ldl

    test: speck $(SUITES)
       	@./speck

## Writing Tests

To start writing tests, you have to know, that you can collect your tests in
test suites. Every suite is represented as a `.c` file in the `spec` directory.

Let's create a first test suite to test some arithmetics:

    $ touch spec/arithmetics.c

The first thing you need to insert in the test suite is an `include` to the
header file `speck.h`

    #include "../speck.h"

Because the test suites are plain C, you can include any header you want, to
build your tests.

`Speck` searches for specific test function that are later executed. They must
match the following form:

    void spec_<name>(void)
    {
        // test code
    }

The only thing you can change is the `<name>` tag of the function. `Speck` will
run all functions defined like these. Let's write a first test for our suite:

    void spec_addition(void)
    {
        int number = 4 + 3;

        sp_assert_equal_i(number, 7);
    }

As you can see, we wrote some code we want to test (`int number = 4 + 3;`). We
assume we are testing the `+` operator for its correctness. To give `Speck` a
hint about if everything went okay or wrong, you have to use assertions. There
are different types of assertions implemented. Scroll down to find out what
assertions are possible. In the **addition** example above, we let `Speck` check
that `number` is indeed `7` at the end of the test.

## Assertions

There are currently the following assertions implemented:

- `sp_assert(expression)`: Evaluate the expression and fail if the result is
false.
- `sp_assert_equal_i(number_a, number_b)`: Evaluate the equality of both numbers
and fail if they're different.

## Running Speck

If you have finished writing your test suites, you can run `Speck`:

    $ make test

## A Real-World Example

If you want to see `Speck` in real-life you can checkout our
[LibCollect](https://github.com/compiler-dept/libcollect) project.
