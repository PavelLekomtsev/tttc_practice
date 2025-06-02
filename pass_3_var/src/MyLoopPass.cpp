#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {
  struct MyLoopPass : public FunctionPass {
    static char ID;
    MyLoopPass() : FunctionPass(ID) {}

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.addRequired<LoopInfoWrapperPass>();
      AU.setPreservesAll();
    }

    bool runOnFunction(Function &F) override {
      LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
      Module *M = F.getParent();
      
      // Получаем или создаем декларации функций loop_start и loop_end
      FunctionCallee loopStartFunc = getOrCreateLoopStartFunction(*M);
      FunctionCallee loopEndFunc = getOrCreateLoopEndFunction(*M);
      
      bool modified = false;
      
      // Обрабатываем все циклы в функции
      for (Loop *L : LI) {
        modified |= instrumentLoop(L, loopStartFunc, loopEndFunc);
      }
      
      return modified;
    }

  private:
    // Создаем или получаем декларацию функции loop_start
    FunctionCallee getOrCreateLoopStartFunction(Module &M) {
      LLVMContext &Ctx = M.getContext();
      FunctionType *FT = FunctionType::get(Type::getVoidTy(Ctx), false);
      return M.getOrInsertFunction("loop_start", FT);
    }
    
    // Создаем или получаем декларацию функции loop_end
    FunctionCallee getOrCreateLoopEndFunction(Module &M) {
      LLVMContext &Ctx = M.getContext();
      FunctionType *FT = FunctionType::get(Type::getVoidTy(Ctx), false);
      return M.getOrInsertFunction("loop_end", FT);
    }
    
    
    bool instrumentLoop(Loop *L, FunctionCallee &loopStartFunc, FunctionCallee &loopEndFunc) {
      BasicBlock *Header = L->getHeader();
      SmallVector<BasicBlock*, 8> ExitBlocks;
      L->getExitBlocks(ExitBlocks);
      
      // Вставляем вызов loop_start в начало заголовка цикла
      IRBuilder<> Builder(&Header->front());
      Builder.CreateCall(loopStartFunc);
      
      // Вставляем вызовы loop_end во все выходящие блоки
      for (BasicBlock *ExitBB : ExitBlocks) {
        IRBuilder<> ExitBuilder(&ExitBB->front());
        ExitBuilder.CreateCall(loopEndFunc);
      }
      
      // Рекурсивно обрабатываем вложенные циклы
      bool modified = true;
      for (Loop *SubLoop : L->getSubLoops()) {
        modified |= instrumentLoop(SubLoop, loopStartFunc, loopEndFunc);
      }
      
      return modified;
    }
  };
}

char MyLoopPass::ID = 0;

// Регистрируем пасс
static RegisterPass<MyLoopPass> X("my_loop_pass", "My Loop Pass for Loop Instrumentation", false, false);

// Штука для автоматической регистрации в opt
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new MyLoopPass()); });