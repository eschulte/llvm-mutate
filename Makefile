# Makefile for mutation pass

# Path to top level of LLVM hierarchy
LEVEL = ../../..

# Name of the library to build
LIBRARYNAME = Mutate

# Make the shared library become a loadable module so the tools can
# dlopen/dlsym on the resulting library.
LOADABLE_MODULE = 1

# Include the makefile implementation stuff
ifneq ($(wildcard $(LEVEL)/Makefile.common),)
	include $(LEVEL)/Makefile.common
endif

# Testing support
OPT_FLAGS=-load ${LEVEL}/Debug+Asserts/lib/Mutate.so
LLVMC=clang -x c - -S -emit-llvm

all:: llvm_mutate_trace.o

greet.ll:
	echo 'main(){ puts("hello"); puts("goodbye"); }'|$(LLVMC) -o $@

arith.ll:
	echo 'main(){ int x=2; x+=3; x=x*x; printf("%d\n", x);}'|$(LLVMC) -o $@

branch.ll:
	echo 'main(int c,char**v){if(atoi(v[1])==1){puts("1");}}'|$(LLVMC) -o $@

clean::
	rm -f a.out *.ll *.bl *.o
