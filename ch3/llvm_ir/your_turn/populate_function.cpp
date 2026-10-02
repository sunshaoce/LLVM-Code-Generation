#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"    // For ConstantInt.
#include "llvm/IR/DerivedTypes.h" // For PointerType, FunctionType.
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h" // For errs().

#include <memory> // For unique_ptr

using namespace llvm;

// The goal of this function is to build a Module that
// represents the lowering of the following foo, a C function:
// extern int baz();
// extern void bar(int);
// void foo(int a, int b) {
//   int var = a + b;
//   if (var == 0xFF) {
//     bar(var);
//     var = baz();
//   }
//   bar(var);
// }
//
// The IR for this snippet (at O0) is:
// define void @foo(i32 %arg, i32 %arg1) {
// bb:
//   %i = alloca i32
//   %i2 = alloca i32
//   %i3 = alloca i32
//   store i32 %arg, ptr %i
//   store i32 %arg1, ptr %i2
//   %i4 = load i32, ptr %i
//   %i5 = load i32, ptr %i2
//   %i6 = add i32 %i4, %i5
//   store i32 %i6, ptr %i3
//   %i7 = load i32, ptr %i3
//   %i8 = icmp eq i32 %i7, 255
//   br i1 %i8, label %bb9, label %bb12
//
// bb9:
//   %i10 = load i32, ptr %i3
//   call void @bar(i32 %i10)
//   %i11 = call i32 @baz()
//   store i32 %i11, ptr %i3
//   br label %bb12
//
// bb12:
//   %i13 = load i32, ptr %i3
//   call void @bar(i32 %i13)
//   ret void
// }
//
// declare void @bar(i32)
// declare i32 @baz(...)
std::unique_ptr<Module> myBuildModule(LLVMContext &Ctxt) {
  IntegerType *I32Ty = Type::getInt32Ty(Ctxt);
  Type *VoidTy = Type::getVoidTy(Ctxt);
  PointerType *PtrTy = PointerType::get(Ctxt, 0);

  std::unique_ptr<Module> MyModule =
      std::make_unique<Module>("Solution Module", Ctxt);

  FunctionType *BazTy = FunctionType::get(I32Ty, false);
  Function *BazFunc =
      Function::Create(BazTy, Function::ExternalLinkage, "baz", MyModule.get());

  FunctionType *BarTy = FunctionType::get(VoidTy, {I32Ty}, false);
  Function *BarFunc =
      Function::Create(BarTy, Function::ExternalLinkage, "bar", MyModule.get());

  FunctionType *FooTy = FunctionType::get(VoidTy, {I32Ty, I32Ty}, false);
  Function *FooFunc =
      Function::Create(FooTy, Function::ExternalLinkage, "foo", MyModule.get());

  BasicBlock *BB = BasicBlock::Create(Ctxt, "bb", FooFunc);
  BasicBlock *BB9 = BasicBlock::Create(Ctxt, "bb9", FooFunc);
  BasicBlock *BB12 = BasicBlock::Create(Ctxt, "bb12", FooFunc);

  IRBuilder Builder(BB);
  AllocaInst *I = Builder.CreateAlloca(I32Ty);
  AllocaInst *I2 = Builder.CreateAlloca(I32Ty);
  AllocaInst *I3 = Builder.CreateAlloca(I32Ty);
  Argument *Arg = FooFunc->getArg(0);
  Argument *Arg1 = FooFunc->getArg(1);
  Builder.CreateStore(Arg, I);
  Builder.CreateStore(Arg1, I2);

  LoadInst *I4 = Builder.CreateLoad(I32Ty, I);
  LoadInst *I5 = Builder.CreateLoad(I32Ty, I2);
  Value *I6 = Builder.CreateAdd(I4, I5);
  Builder.CreateStore(I6, I3);
  LoadInst *I7 = Builder.CreateLoad(I32Ty, I3);
  ConstantInt *Cst255 = ConstantInt::get(I32Ty, 255);
  Value *I8 = Builder.CreateICmpEQ(I7, Cst255);
  Builder.CreateCondBr(I8, BB9, BB12);

  Builder.SetInsertPoint(BB9);
  LoadInst *I10 = Builder.CreateLoad(I32Ty, I3);
  Builder.CreateCall(BarFunc, {I10});
  CallInst *I11 = Builder.CreateCall(BazFunc);
  Builder.CreateStore(I11, I3);
  Builder.CreateBr(BB12);

  Builder.SetInsertPoint(BB12);
  LoadInst *I13 = Builder.CreateLoad(I32Ty, I3);
  Builder.CreateCall(BarFunc, {I13});
  Builder.CreateRetVoid();

  return MyModule;
}
