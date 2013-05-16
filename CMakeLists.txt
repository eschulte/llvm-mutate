cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "-fPIC")
set(CMAKE_CXX_FLAGS "-fPIC -fno-rtti -Wno-c++11-extensions")

find_package(LLVM)

# Define add_llvm_* macro's.
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_subdirectory(Mutate)

INSTALL_PROGRAMS(/bin llvm-mutate)
