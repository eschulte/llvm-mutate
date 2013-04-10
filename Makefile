# Makefile for mutation pass

# Path to top level of LLVM hierarchy
LEVEL = ../../..

# Name of the library to build
LIBRARYNAME = Mutate

# Make the shared library become a loadable module so the tools can
# dlopen/dlsym on the resulting library.
LOADABLE_MODULE = 1

# Include the makefile implementation stuff
include $(LEVEL)/Makefile.common

# Testing support
OPT_FLAGS=-load ${LEVEL}/Debug+Asserts/lib/Mutate.so
LLVMC=clang -x c - -S -emit-llvm

greet.ll:
	echo 'main(){ puts("hello"); puts("goodbye"); }'|$(LLVMC) -o $@

arith.ll:
	echo 'main(){ int x=2; x+=10; x=x*x; return x;}'|$(LLVMC) -o $@

real-clean:
	rm -f a.out *.ll *.bl
