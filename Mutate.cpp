#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
  struct Mutate : public ModulePass {
    static char ID; // pass identification
    Mutate() : ModulePass(ID) {}
    virtual bool runOnModule(Module &M) {
      errs() << "ran module pass\n";
      
      return false; // true if modified by pass
    }
  };
}

char Mutate::ID = 0;
static RegisterPass<Mutate> X("mutate", "Mutate llvm assembly");
