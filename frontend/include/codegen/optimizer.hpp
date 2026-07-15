#ifndef INCLUDE_CODEGEN_OPTIMIZER
#define INCLUDE_CODEGEN_OPTIMIZER

#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/PassBuilder.h>

namespace language {

class Optimizer final {
  public:
    static void
    optimize(llvm::Module &module,
             llvm::OptimizationLevel level = llvm::OptimizationLevel::O2) {
        llvm::LoopAnalysisManager lam;
        llvm::FunctionAnalysisManager fam;
        llvm::CGSCCAnalysisManager cgam;
        llvm::ModuleAnalysisManager mam;

        llvm::PassBuilder pb;

        pb.registerModuleAnalyses(mam);
        pb.registerCGSCCAnalyses(cgam);
        pb.registerFunctionAnalyses(fam);
        pb.registerLoopAnalyses(lam);
        pb.crossRegisterProxies(lam, fam, cgam, mam);

        llvm::ModulePassManager mpm = pb.buildPerModuleDefaultPipeline(level);
        mpm.run(module, mam);
    }
};

} // namespace language

#endif // INCLUDE_CODEGEN_OPTIMIZER