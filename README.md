# FastDivide128

Clang natively supports U128, aka uint128_t, aka __uint128_t, aka unsigned __int128. However, on Windows, attempting to perform division or modulo on these large integers calls a function (__udivti3, __umodti3, or __udivmodti4) that is not available, causing "unresolved external symbol" errors. There's an LLVM bug ticket but it's been open for literally years:

https://bugs.llvm.org/show_bug.cgi?id=25305

And some related work here for interested parties:

https://reviews.llvm.org/D41813

But even if you manually bring in the sources from compiler-rt for this, or you're on a platform that isn't Windows, you may have noticed that the above functions are not very fast at all.

This library provides `Div`, `Mod`, and `DivMod` for U128s for modern x86 (Haswell+), about 2x faster than the compiler-rt stuff. Single header and CPP file.

Compilers are not very good; inline assembly is used where it was not possible to get the compiler to produce the desired codegen.

This library is intended primarily for Clang. It compiles under GCC but the codegen is not as good. A fully assembly version should probably be made (taken from the Clang-compiled output) for use under GCC.

This library is not intended for compile-time use. If arguments are known at compile-time, a simpler approach for the three possible combinations (known numerator, known denominator, known both) should be employed, but FastDivide128 focuses on maximizing runtime performance.

This library is for 128-bit unsigned integers. For fast 64-bit division, see https://github.com/komrad36/FastDivide.
