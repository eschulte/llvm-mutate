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

test/greet.ll:
	echo 'main(){ puts("hello"); puts("goodbye"); }'|clang -x c - -S -emit-llvm -o $@

test/arith.ll:
	echo 'main(){ int x=2; x+=10; x=x*x; return x;}'|clang -x c - -S -emit-llvm -o $@

%.ll: %.bl
	llvm-dis $< -o $@

%: %.ll
	cat $<|llc|clang -x assembler - -o $@

test/arith-count: test/arith.ll
	opt $(OPT_FLAGS) -count $< -o /dev/null

test/arith-cut.bl: test/arith.ll
	opt $(OPT_FLAGS) -cut -stmt1=4 $< -o $@

test/arith-ins.bl: test/arith.ll
	opt $(OPT_FLAGS) -insert -stmt1=5 -stmt2=7 $< -o $@

test/arith-swp.bl: test/arith.ll
	opt $(OPT_FLAGS) -swap -stmt1=6 -stmt2=7 $< -o $@

test/greet-count: test/greet.ll
	opt $(OPT_FLAGS) -count $< -o /dev/null

test/greet-cut.bl: test/greet.ll
	opt $(OPT_FLAGS) -cut -stmt1=1 $< -o $@

real-clean:
	rm -rf test/*
