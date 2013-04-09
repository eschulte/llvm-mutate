// Copyright (C) 2013 Eric Schulte
#include "llvm/Pass.h"
#include "llvm/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

static cl::opt<unsigned>
Stmt1("stmt1", cl::init(0), cl::desc("first statement to mutate"));

static cl::opt<unsigned>
Stmt2("stmt2", cl::init(0), cl::desc("second statement to mutate"));

namespace {
  struct Count : public ModulePass {
    static char ID;
    Count() : ModulePass(ID) {}

    bool runOnModule(Module &M){
      count = 0;
      for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I)
        walkFunction(I);

      errs() << count << "\n";

      return false;
    }

    // We don't modify the program, so we preserve all analyses
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll(); }

  private:
    int unsigned count;
    void walkFunction(Function *F){
      for (Function::iterator B = F->begin(), E = F->end(); B != E; ++B) {
        for (BasicBlock::iterator I = B->begin(), E = B->end(); I != E; ++I) {
          count += 1; } } }
  };
}

namespace {
  struct Cut : public ModulePass {
    static char ID;
    Cut() : ModulePass(ID) {}

    bool runOnModule(Module &M){
      count = 0;
      changed_p = false;
      for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I)
        if(walkFunction(I)) break;

      if(changed_p) errs() << "cut " << Stmt1 << "\n";
      else          errs() << "cut failed\n";

      return changed_p; }

  private:
    int unsigned count;
    bool changed_p;
    bool walkFunction(Function *F){
      for (Function::iterator B = F->begin(), E = F->end(); B != E; ++B) {
        for (BasicBlock::iterator I = B->begin(), E = B->end(); I != E; ++I) {
          count += 1;
          if(count == Stmt1){
            I->eraseFromParent();
            changed_p = true;
            return true; } } }
      return false; }
  };
}

namespace {
  struct Insert : public ModulePass {
    static char ID;
    Insert() : ModulePass(ID) {}

    bool runOnModule(Module &M){
      count = 0;
      changed_p = false;
      for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I)
        if(walkCollect(I)) break;
      count = 0;
      for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I)
        if(walkPlace(I)) break;

      if(changed_p) errs()<<"inserted "<<Stmt2<<" before "<<Stmt1<<"\n";
      else          errs()<<"insertion failed\n";

      return changed_p; }

  private:
    int unsigned count;
    bool changed_p;
    BasicBlock::iterator temp;

    bool walkCollect(Function *F){
      for (Function::iterator B = F->begin(), E = F->end(); B != E; ++B) {
        for (BasicBlock::iterator I = B->begin(), E = B->end(); I != E; ++I) {
          count += 1;
          if(count == Stmt2) {
            temp = I->clone();
            return true; } } }
      return false; }

    bool walkPlace(Function *F){
      for (Function::iterator B = F->begin(), E = F->end(); B != E; ++B) {
        for (BasicBlock::iterator I = B->begin(), E = B->end(); I != E; ++I) {
          count += 1;
          if(count == Stmt1){
            temp->insertBefore(I);
            changed_p = true;
            return true; } } }
      return false; }
  };
}

namespace {
  struct Swap : public ModulePass {
    static char ID; // pass identification
    Swap() : ModulePass(ID) {}

    // Count up all operations in the module
    bool runOnModule(Module &M){
      count = 0;
      changed_p = false;
      for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I)
        walkCollect(I);
      count = 0;
      for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I)
        if(walkPlace(I)) break;

      if(changed_p) errs() << "swapped " << Stmt1 << " with " << Stmt2 << "\n";
      else          errs() << "swap failed\n";

      return changed_p; }

  private:
    int unsigned count;
    bool changed_p;
    BasicBlock::iterator temp1, temp2;

    void walkCollect(Function *F){
      for (Function::iterator B = F->begin(), E = F->end(); B != E; ++B) {
        for (BasicBlock::iterator I = B->begin(), E = B->end(); I != E; ++I) {
          count += 1;
          if (count == Stmt1) temp1 = I->clone();
          if (count == Stmt2) temp2 = I->clone(); } } }

    bool walkPlace(Function *F){
      for (Function::iterator B = F->begin(), E = F->end(); B != E; ++B) {
        for (BasicBlock::iterator I = B->begin(), E = B->end(); I != E; ++I) {
          count += 1;
          if(count == Stmt2){
            ReplaceInstWithInst(I->getParent()->getInstList(), I, temp1);
            if(changed_p) return true;
            changed_p = true;
          }
          if(count == Stmt1){
            ReplaceInstWithInst(I->getParent()->getInstList(), I, temp2);
            if(changed_p) return true;
            changed_p = true; } } }
      return false; }
  };
}

char Count::ID = 0;
char Cut::ID = 0;
char Insert::ID = 0;
char Swap::ID = 0;
static RegisterPass<Count>  W("count",  "count the number of instructions");
static RegisterPass<Cut>    X("cut",    "cut instruction number stmt1");
static RegisterPass<Insert> Y("insert", "insert stmt2 before stmt1");
static RegisterPass<Swap>   Z("swap",   "swap stmt1 and stmt2");
