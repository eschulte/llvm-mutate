#include "llvm_mutate_trace.h"
void llvm_mutate_trace(int count){
  if(llvm_trace == NULL){ llvm_trace = fopen("llvm_mutate_trace","a"); }
  fprintf(llvm_trace, "%d\n", count);
}
