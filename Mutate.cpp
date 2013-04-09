// Copyright (C) 2013 Eric Schulte
#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

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
    bool cutOp(GlobalValue *G);
    bool insertOp(GlobalValue *G);
    bool swapOp(GlobalValue *G);
  };
}

bool Mutate::runOnModule(Module &M){
  changed_p = false;

  if(! strcmp(Operation.c_str(), "count")){
    // Count up all operations in the module
    for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I)
      countOp(I);
    errs() << count << "\n";
  } else if(! strcmp(Operation.c_str(), "cut")){
    // Cut Stmt1 from the module
    for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I)
      if(cutOp(I)) break;
    if(changed_p) errs() << "cut " << Stmt1 << "\n";
    else          errs() << "cut failed\n";
  } else if(! strcmp(Operation.c_str(), "insert")){
    // Insert Stmt1 before Stmt2 in the module
    for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I) insertOp(I);
    if(changed_p) errs() << "inserted " << Stmt2 << " before " << Stmt1 << "\n";
    else          errs() << "insertion failed\n";
  } else if(! strcmp(Operation.c_str(), "swap")){
    // Swap Stmt1 with Stmt2 in the module
    for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I)
      if(swapOp(I)) break;
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

bool Mutate::cutOp(GlobalValue *G){
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
          return true;
        }
      }
  }
  return false;
}

bool Mutate::insertOp(GlobalValue *G){
  if (dyn_cast<GlobalVariable>(G)){
    // ignore global variables
  } else if (dyn_cast<GlobalAlias>(G)){
    // ignore global alias
  } else {
    // descend into function objects
    Function *F = cast<Function>(G);

    // collect the instruction
    BasicBlock::iterator temp1;
    for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
      for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I) {
        count += 1;
        if(count == Stmt2){
          temp1 = I->clone();
        }
      }

    // set the instructions
    count = 0;
    for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
      for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I) {
        count += 1;
        if(count == Stmt1){
          temp1->insertBefore(I);
          changed_p = true;
          return true;
        }
      }
  }
  return false;
}

bool Mutate::swapOp(GlobalValue *G){
  if (dyn_cast<GlobalVariable>(G)){
    // ignore global variables
  } else if (dyn_cast<GlobalAlias>(G)){
    // ignore global alias
  } else {
    // descend into function objects
    Function *F = cast<Function>(G);

    // collect the instructions
    BasicBlock::iterator temp1, temp2;
    for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
      for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I) {
        count += 1;
        if(count == Stmt1){
          temp1 = I->clone();
        }
        if (count == Stmt2){
          temp2 = I->clone();
        }
      }

    // set the instructions
    count = 0;
    for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
      for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I) {
        count += 1;
        if(count == Stmt2){
          ReplaceInstWithInst(I->getParent()->getInstList(), I, temp1);
          if(changed_p) return true;
          changed_p = true;
        }
        if(count == Stmt1){
          ReplaceInstWithInst(I->getParent()->getInstList(), I, temp2);
          if(changed_p) return true;
          changed_p = true;
        }
      }
  }
  return false;
}

char Mutate::ID = 0;
static RegisterPass<Mutate> X("mutate", "Mutate llvm assembly");
