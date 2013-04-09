#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static cl::opt<std::string>
Operation("mut-op", cl::desc("Mutation operation to perform"));

namespace {
  struct Mutate : public ModulePass {
    static char ID; // pass identification
    Mutate() : ModulePass(ID) {}
    // consists of functions, global variables, and symbol table entries
    bool runOnModule(Module &M);

  private:
    int count;
    void doFunction(GlobalValue *G);
  };
}

bool Mutate::runOnModule(Module &M){
  if(! strcmp(Operation.c_str(), "count")){
    errs() << "counting pass: ";

    // Module Iterator
    for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I)
      doFunction(I);

    errs() << count << '\n';
  } else {
    errs() << "other pass\n";
  }
  return false; // true if modified by pass
}

void Mutate::doFunction(GlobalValue *G){
  if (dyn_cast<GlobalVariable>(G)){
  } else if (dyn_cast<GlobalAlias>(G)){
  } else {
    // this must be a function object.
    Function *F = cast<Function>(G);
    
    for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
      for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I)
        for (User::op_iterator U = I->op_begin(), E = I->op_end(); U != E; ++U)
          count += 1;
  }
}

char Mutate::ID = 0;
static RegisterPass<Mutate> X("mutate", "Mutate llvm assembly");
