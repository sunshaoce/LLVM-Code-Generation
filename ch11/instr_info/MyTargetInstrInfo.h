#ifndef LLVM_LIB_TARGET_MYTARGET_MYTARGETINSTRINFO_H
#define LLVM_LIB_TARGET_MYTARGET_MYTARGETINSTRINFO_H

#include "MyTargetRegisterInfo.h" // For the definition of the register class.
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Config/llvm-config.h"

#define GET_INSTRINFO_HEADER
#define GET_INSTRINFO_ENUM            // This should be in MC
#define GET_INSTRINFO_MC_HELPER_DECLS // This should be in MC
#include "MyTargetGenInstrInfo.inc"

namespace llvm {

class MyTargetInstrInfo : public MyTargetGenInstrInfo {
public:
#if LLVM_VERSION_MAJOR >= 23
  MyTargetInstrInfo(const TargetSubtargetInfo &STI,
                    const TargetRegisterInfo &TRI);
#else
  MyTargetInstrInfo();
#endif
};
} // namespace llvm

#endif
