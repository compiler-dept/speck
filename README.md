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
called `spec`:

    $ mkdir spec

You will need to add some rule to your `Makefile` in order to compile `speck` and
your tests.

The first rule you need to add is the the list of test suites:

    TESTS=$(patsubst %.c, %.so, $(wildcard spec/*.c))

This is a wildcard matcher that matches all `.c` files in your `spec` folder as
a single test suite.

Every `Speck` test suite is compiled into a shared object file (`.so`) and then
loaded and executed by `Speck`. So, every piece of code that needs to be tested
has to be compiled into the single test suites. For that you need to add another
rule to your `Makefile`:

    spec/%.so: spec/%.c
        @$(CC) -fPIC -shared -o $@ $<

At the end of the second line you can attach other files (`.c`, `.o`, `.s`) or
libraries (`-lx`) to pull in all needed code for execution. You can also use all
kinds of `CFLAGS` and other options you need to compile the test suites.

Of cause we need a rule to compile `Speck` itself:

    speck: speck.c
    	$(CC) -std=c11 -o $@ $< -ldl

We're almost done, but it would be nice to be able to call your test with
`make test`. Therefore we need a last rule:

    test: speck $(TESTS)
    	@./speck

The rule tells `make`, that if the target `test` is executed, `Speck` and all
test suites have to be compiled and then execute `Speck`.

## Writing Tests

TODO
