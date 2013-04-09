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
    unsigned int count;
    bool changed_p;
    void countOp(GlobalValue *G);
    void cutOp(GlobalValue *G);
    void insertOp(GlobalValue *G);
    void swapOp(GlobalValue *G);
  };
}

bool Mutate::runOnModule(Module &M){
  changed_p = false;

  if(! strcmp(Operation.c_str(), "count")){
    // Count up all operations in the module
    for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I) countOp(I);
    errs() << count << "\n";
  } else if(! strcmp(Operation.c_str(), "cut")){
    // Cut Stmt1 from the module
    for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I) cutOp(I);
    if(changed_p) errs() << "cut " << Stmt1 << "\n";
    else          errs() << "cut failed\n";
  } else if(! strcmp(Operation.c_str(), "insert")){
    // Insert Stmt1 before Stmt2 in the module
    for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I) insertOp(I);
    if(changed_p) errs() << "inserted " << Stmt1 << " into " << Stmt2 << "\n";
    else          errs() << "insertion failed\n";
  } else if(! strcmp(Operation.c_str(), "swap")){
    // Swap Stmt1 with Stmt2 in the module
    for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I) swapOp(I);
    if(changed_p) errs() << "swapped " << Stmt1 << " with " << Stmt2 << "\n";
    else          errs() << "swap failed\n";
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
        count += 1;
  }
}

void Mutate::cutOp(GlobalValue *G){
  if (dyn_cast<GlobalVariable>(G)){
    // ignore global variables
  } else if (dyn_cast<GlobalAlias>(G)){
    // ignore global alias
  } else {
    // descend into function objects
    Function *F = cast<Function>(G);
    
    for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
      for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I) {
        count += 1;
        if(count == Stmt1){
          I->eraseFromParent();
          changed_p = true;
          return;
        }
      }
  }
}

void Mutate::insertOp(GlobalValue *G){
  errs() << "insert not implemented";
}

void Mutate::swapOp(GlobalValue *G){
  if (dyn_cast<GlobalVariable>(G)){
    // ignore global variables
  } else if (dyn_cast<GlobalAlias>(G)){
    // ignore global alias
  } else {
    // descend into function objects
    Function *F = cast<Function>(G);

    // collect the first instruction
    BasicBlock::iterator temp;
    for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
      for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I) {
        count += 1;
        if(count == Stmt1){
          temp = I;
          break;
        }
      }

    count = 0;
    for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
      for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I) {
        count += 1;
        if(count == Stmt2){
          std::swap(temp,I);
          changed_p = true;
          return;
        }
      }    
  }
}

char Mutate::ID = 0;
static RegisterPass<Mutate> X("mutate", "Mutate llvm assembly");
