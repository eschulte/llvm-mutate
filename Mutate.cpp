#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static cl::opt<std::string>
Operation("mut-op", cl::desc("Mutation operation to perform"));

static cl::opt<unsigned>
Stmt1("stmt1", cl::init(0), cl::desc("first statement to mutate"));

static cl::opt<unsigned>
Stmt2("stmt2", cl::init(0), cl::desc("second statement to mutate"));

namespace {
  struct Mutate : public ModulePass {
    static char ID; // pass identification
    Mutate() : ModulePass(ID) {}
    // consists of functions, global variables, and symbol table entries
    bool runOnModule(Module &M);

  private:
    int count;
    void countOp(GlobalValue *G);
  };
}

bool Mutate::runOnModule(Module &M){
  bool changed_p = false;

  // Count up all operations in the module
  for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I) countOp(I);

  if(! strcmp(Operation.c_str(), "count")){
    errs() << count << "\n";
  } else if(! strcmp(Operation.c_str(), "cut")){
    errs() << "cutting " << Stmt1 << "\n";
  } else if(! strcmp(Operation.c_str(), "insert")){
    errs() << "inserting " << Stmt1 << " into " << Stmt2 << "\n";
  } else if(! strcmp(Operation.c_str(), "swap")){
    errs() << "swapping " << Stmt1 << " with " << Stmt2 << "\n";
  } else {
    errs() << "unknown mutation: '" << Operation << "'\n";
  }

  return changed_p;
}

void Mutate::countOp(GlobalValue *G){
  if (dyn_cast<GlobalVariable>(G)){
    // ignore global variables
  } else if (dyn_cast<GlobalAlias>(G)){
    // ignore global alias
  } else {
    // descend into function objects
    Function *F = cast<Function>(G);
    
    for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
      for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I)
        for (User::op_iterator U = I->op_begin(), E = I->op_end(); U != E; ++U)
          count += 1;
  }
}

char Mutate::ID = 0;
static RegisterPass<Mutate> X("mutate", "Mutate llvm assembly");
