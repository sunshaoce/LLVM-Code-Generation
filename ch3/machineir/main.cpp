#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h" // For InitializeAllTargets
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

extern MachineFunction *solutionPopulateMachineIR(MachineModuleInfo &,
                                                  llvm::Function &, Register,
                                                  Register);
extern MachineFunction *populateMachineIR(MachineModuleInfo &,
                                                  llvm::Function &, Register,
                                                  Register);

bool checkFunctionCorrectness(MachineFunction *Res, Register A0, Register A1) {
  // Take care of the liveness since we did not explain how to do that.
  MachineBasicBlock *EntryBB = Res->empty() ? nullptr : &*Res->begin();
  if (EntryBB) {
    EntryBB->addLiveIn(A0);
    EntryBB->addLiveIn(A1);
  }
  Res->print(errs());
  if (!Res->verify()) {
    errs() << Res->getName() << " does not verify\n";
    return false;
  }
  return true;
}

int main() {
  // We have to initialize all the targets to get the registry initialized.
  InitializeAllTargets();
  // We need the MC layer as well to query the register information.
  InitializeAllTargetMCs();

  Triple TT(Triple::normalize("riscv64--"));
  std::string Error;
  const Target *TheTarget = TargetRegistry::lookupTarget(TT, Error);
  if (!TheTarget) {
    errs() << TT.getTriple() << " is not available with this build of LLVM\n";
    return -1;
  }
  auto *LLVMTM = static_cast<CodeGenTargetMachineImpl *>(
      TheTarget->createTargetMachine(TT, "", "", TargetOptions(), std::nullopt,
                                     std::nullopt, CodeGenOptLevel::Default));
  MachineModuleInfoWrapperPass MMIWP(LLVMTM);
  LLVMContext Context;
  Module MyModule("MyModule", Context);
  MyModule.setDataLayout(LLVMTM->createDataLayout());

  Function *SolutionFoo = Function::Create(
      FunctionType::get(Type::getVoidTy(Context), /*IsVarArg=*/false),
      Function::ExternalLinkage, "solution_foo", MyModule);
  const TargetSubtargetInfo *STI = LLVMTM->getSubtargetImpl(*SolutionFoo);
  const TargetRegisterInfo *TRI = STI->getRegisterInfo();

  // Find the indices for a0 (X10) and a1 (X11).
  // Since we are not in the RISCV library we don't have access to the RISCV::X10
  // enums.
  StringRef A0Str = "X10";
  StringRef A1Str = "X11";
  Register A0 = 0;
  Register A1 = 0;
  for (unsigned i = 1, e = TRI->getNumRegs(); i != e && (!A0 || !A1); ++i) {
    if (!A0 && A0Str == TRI->getName(i)) {
      A0 = i;
      continue;
    }
    if (!A1 && A1Str == TRI->getName(i)) {
      A1 = i;
      continue;
    }
  }

  if (!A0 || !A1) {
    errs() << "Failed to find physical registers a0 and a1\n";
    return -1;
  }

  MachineFunction *Res =
      solutionPopulateMachineIR(MMIWP.getMMI(), *SolutionFoo, A0, A1);
  bool solutionIsCorrect = checkFunctionCorrectness(Res, A0, A1);

  Function *Foo = Function::Create(
      FunctionType::get(Type::getVoidTy(Context), /*IsVarArg=*/false),
      Function::ExternalLinkage, "foo", MyModule);

  MachineFunction *YourTurnRes =
      populateMachineIR(MMIWP.getMMI(), *Foo, A0, A1);
  bool yourTurnIsCorrect = checkFunctionCorrectness(YourTurnRes, A0, A1);


  return !(solutionIsCorrect && yourTurnIsCorrect);
}
