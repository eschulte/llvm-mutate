cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "-fPIC")
set(CMAKE_CXX_FLAGS "-fPIC -fno-rtti -std=c++11")

find_package(LLVM)

# Work around LLVM bugs
# http://lists.cs.uiuc.edu/pipermail/llvmdev/2015-January/081050.html
#
set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/bin)
set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/lib)
set(LLVM_PLUGIN_EXT ".so")
set(LLVM_ENABLE_PLUGINS ON)
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_subdirectory(Mutate)

INSTALL_PROGRAMS(/bin llvm-mutate)
