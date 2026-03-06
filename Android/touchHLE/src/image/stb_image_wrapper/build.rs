/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
use std::path::Path;

fn rerun_if_changed(path: &Path) {
    println!("cargo:rerun-if-changed={}", path.to_str().unwrap());
}

fn main() {
    let package_root = Path::new(env!("CARGO_MANIFEST_DIR"));
    let workspace_root = package_root.join("../../..");
    let stb_header = workspace_root.join("vendor/stb/stb_image.h");

    let mut build = cc::Build::new();
    if stb_header.is_file() {
        build.file(package_root.join("lib.c"));
    } else {
        println!(
            "cargo:warning=Missing stb source at {}. Building fallback stub (image decode disabled).",
            stb_header.display()
        );
        build.file(package_root.join("fallback.c"));
    }
    build.compile("stb_image_wrapper");

    rerun_if_changed(&package_root.join("lib.c"));
    rerun_if_changed(&package_root.join("fallback.c"));
    rerun_if_changed(&stb_header);
}
