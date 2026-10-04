# Implicit change of scope #

This example illustrates the implicit change of scope, meaning that when you follow the def-use chains of certain values you may jump around different functions.

This is true unsurprisingly for instance with global variables, but it happens as well with constants.

To see this in action, configure, build and run this example by following the next steps:
```bash
cmake -GNinja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DLLVM_DIR="$HOME/dev/llvm-project/build/lib/cmake/llvm" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_CXX_FLAGS_DEBUG="-g -stdlib=libc++" \
  -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=lld" \
  -DCMAKE_BUILD_RPATH="$HOME/usr/llvm-23.1.2/lib/x86_64-unknown-linux-gnu" \
  -Bbuild .
ninja -Cbuild
./build/implicit_change_of_scope
```

Running `implicit_change_of_scope` will print some messages when jumping from one function to another.

Look at the `InputIR` string in `main.cpp` to see if you can rebuild in your head the def-use chains and explain the change of scope from `foo` to`bar`.
