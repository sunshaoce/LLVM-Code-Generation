#include "MyTargetInstrInfo.h"
#include "MyTargetRegisterInfo.h"

#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Support/Debug.h" // For dbgs().
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

#if LLVM_VERSION_MAJOR >= 23
// This example only inspects instruction descriptions, so no CPU features or
// scheduling model are needed for its subtarget.
class MyTargetSubtargetInfo : public TargetSubtargetInfo {
  const TargetRegisterInfo &TRI;
  FeatureBitset NoFeatures;

public:
  explicit MyTargetSubtargetInfo(const TargetRegisterInfo &TRI)
      : TargetSubtargetInfo(Triple(), "", "", "", "", {}, {}, nullptr, nullptr,
                            nullptr, nullptr, nullptr, nullptr, nullptr),
        TRI(TRI) {}

  const TargetRegisterInfo *getRegisterInfo() const override { return &TRI; }
  const FeatureBitset &getInlineIgnoreFeatures() const override {
    return NoFeatures;
  }
  const FeatureBitset &getInlineInverseFeatures() const override {
    return NoFeatures;
  }
  const FeatureBitset &getInlineMustMatchFeatures() const override {
    return NoFeatures;
  }
};
#endif

int main() {
  MyTargetRegisterInfo MyTRI;
#if LLVM_VERSION_MAJOR >= 23
  MyTargetSubtargetInfo MySTI(MyTRI);
  MyTargetInstrInfo MyTII(MySTI, MyTRI);
#else
  MyTargetInstrInfo MyTII;
#endif
  TargetRegisterInfo *RegInfos[] = {&MyTRI};
  unsigned NbInstrs = MyTII.getNumOpcodes();
  dbgs() << "Found " << NbInstrs << " instructions for MyTarget.\n";
  dbgs() << "Print the non-generic ones:\n";
  for (unsigned i = 0; i != NbInstrs; ++i) {
    const MCInstrDesc &InstrDesc = MyTII.get(i);
    // Skip the generic opcode to focus on the target specific ones.
    if (InstrDesc.isPseudo())
      continue;

    dbgs() << MyTII.getName(i) << ":isAsCheapAsMove("
           << InstrDesc.isAsCheapAsAMove() << ")\t";
    for (auto [index, MCOI] : enumerate(InstrDesc.operands())) {
      if (MCOI.OperandType == MCOI::OperandType::OPERAND_REGISTER) {
        if (index < InstrDesc.getNumDefs())
          dbgs() << "(def)";
        dbgs() << MyTRI.getRegClassName(MyTRI.getRegClass(MCOI.RegClass));
      } else if (MCOI.OperandType == MCOI::OperandType::OPERAND_IMMEDIATE) {
        dbgs() << "imm";
      } else
        dbgs() << "other";
      dbgs() << ", ";
    }
    dbgs() << '\n';
  }
  return 0;
}
