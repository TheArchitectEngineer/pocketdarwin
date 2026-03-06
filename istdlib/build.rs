fn main() {
    // Tell cargo to rerun build.rs if any source file changes
    println!("cargo:rerun-if-changed=src/istdlib.c");
    println!("cargo:rerun-if-changed=include/istdlib.h");

    // Build the C library
    cc::Build::new()
        .file("src/istdlib.c")
        .include("include")
        .compile("istdlib"); // produces libistdlib.a
}