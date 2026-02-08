This is a placeholder for toolchain binaries or wrapper scripts.

Recommended layout:
- clang / clang++ wrappers that set --target and --sysroot
- ld.lld wrapper for darwin flavor
- ar, ranlib, strip

On macOS, Xcode will look here when using the PocketDarwin toolchain.
On Linux/Windows/BSD, use these wrappers directly or point your build system to them.
