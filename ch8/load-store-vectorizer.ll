target triple = "riscv64-unknown-linux-gnu"

define void @bar(ptr %src, ptr %dst) #0 {
  %v0 = load i64, ptr %src
  %src1 = getelementptr i64, ptr %src, i64 1
  %v1 = load i64, ptr %src1
  store i64 %v0, ptr %dst
  %dst1 = getelementptr i64, ptr %dst, i64 1
  store i64 %v1, ptr %dst1
  ret void
}

attributes #0 = { "target-features"="+v,+unaligned-scalar-mem" }
