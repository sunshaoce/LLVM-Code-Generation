In this directory, you can find examples of `mir` (Machine IR) files.

To produce a `mir` file you can use the -stop-before/after=pass-name.

In this directory, we have one input LLVM IR file that we used to produce:
- A pure `mir` file `full-dump.mir`, and
- A simplified one `simplified-dump.mir`, and
- A shrunk one `shrunk-dump.mir`

The pure one has all the fields set, whereas the simplified one only contains the fields that have non-default values or cannot be recomputed.
The shrunk one shows you the kind of manual editing you can make to reduce a file even more, in particular how the LLVM IR section can be removed.

These examples target RV64. To regenerate the full and simplified files, run:

```bash
llc -stop-after=finalize-isel input.ll -o full-dump.mir
llc -stop-after=finalize-isel -simplify-mir input.ll -o simplified-dump.mir
```

The shrunk file needs an explicit target because it has no LLVM IR section:

```bash
llc -mtriple=riscv64-unknown-linux-gnu -start-after=finalize-isel \
  -verify-machineinstrs shrunk-dump.mir -o shrunk-dump.s
```
