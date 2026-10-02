include "llvm/Target/Target.td"

include "reginfo.td"
include "instrinfo.td"

#ifdef LLVM_23_OR_NEWER
defm : RemapAllTargetPseudoPointerOperands<GPR64>;
#endif

def MyTargetii: InstrInfo;
def MyTarget : Target {
  let InstructionSet = MyTargetii;
}

