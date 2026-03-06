fn main() {
    println!("cargo:rerun-if-changed=libsystem/system.cc");

    cc::Build::new()
        .cpp(true)
        .file("libsystem/system.cc")
        .flag_if_supported("-std=c++17")
        .compile("syslite_system");

    let manifest_dir = std::env::var("CARGO_MANIFEST_DIR").expect("CARGO_MANIFEST_DIR");
    let profile = std::env::var("PROFILE").expect("PROFILE");
    let target_dir = std::env::var("CARGO_TARGET_DIR")
        .map(std::path::PathBuf::from)
        .unwrap_or_else(|_| std::path::Path::new(&manifest_dir).join("target"));

    let out_path = target_dir.join(&profile).join("libSystem.dylib");
    let obj_path = target_dir.join(&profile).join("libSystem.obj");

    let parent = out_path
        .parent()
        .expect("libSystem.dylib output path must have parent");
    std::fs::create_dir_all(parent).expect("failed to create output directory for libSystem.dylib");

    let iphoneoscc = std::env::var("CLANG_IPHON").unwrap_or_else(|_| "clang".to_string());
    let iphonld = std::env::var("IPHONE_LD").unwrap();
    let crosscc = std::process::Command::new(iphoneoscc)
        .args([
            "-target $(TARGET_TRIPLE)",
            "-isysroot $(SDKROOT)",
            "-march=$(ARCH)",
            "-miphoneos-version-min=$(IPHONEOS_MIN)",
            "-fno-stack-protector",
            "-mthumb",
            "-std=c++17",
            "-c",
            "libsystem/system.cc",
            "-o",
        ])
        .arg(&obj_path)
        .status()
        .expect("Clang got me an nah, i can't do tuff.");

    if !crosscc.success() {
        panic!("failed to build {}", obj_path.display());
    }

    let cxx = std::env::var("CXX").unwrap_or_else(|_| "c++".to_string());
    let status = std::process::Command::new(cxx)
        .args([
            "-std=c++17",
            "-fPIC",
            "-shared",
            "libsystem/system.cc",
            "-o",
        ])
        .arg(&out_path)
        .status()
        .expect("failed to invoke C++ compiler for libSystem.dylib");

    if !status.success() {
        panic!("failed to build {}", out_path.display());
    }
}
