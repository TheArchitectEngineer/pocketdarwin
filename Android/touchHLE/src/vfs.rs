// 
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
//! Virtual filesystem facade.
//!
//! touchHLE's guest filesystem implementation currently lives in [crate::fs].
//! This module provides VFS naming and constructors so new code can depend on
//! `vfs` terminology while reusing the existing, production filesystem core.

use crate::fs;

pub use crate::fs::{
    resolve_path, BundleData, FsError, GuestFile, GuestOpenOptions, GuestPath, GuestPathBuf,
    APPLICATIONS,
};

/// Main virtual filesystem type used by touchHLE.
pub type VirtualFileSystem = fs::Fs;

/// File handle returned by VFS operations.
pub type VirtualFile = GuestFile;

/// Builds a virtual filesystem for an app bundle and returns it together with
/// the guest path of the mounted app bundle directory.
pub fn new(
    app_bundle: BundleData,
    bundle_dir_name: String,
    bundle_id: &str,
    read_only_mode: bool,
) -> (VirtualFileSystem, GuestPathBuf) {
    fs::Fs::new(app_bundle, bundle_dir_name, bundle_id, read_only_mode)
}

/// Creates an empty/fake virtual filesystem used by the app picker path.
pub fn new_fake() -> VirtualFileSystem {
    fs::Fs::new_fake_fs()
}
