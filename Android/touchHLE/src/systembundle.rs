/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
//! Support for PocketDarwin/FreeDarwin `.System` bundles.
//!
//! Expected on-disk layout:
//! - `<Name>.System/Info.plist` or `<Name>.System/System.plist`
//! - `<Name>.System/System/Applications`
//! - `<Name>.System/System/Frameworks`
//! - `<Name>.System/System/usr`
//! - `<Name>.System/Data/user`
//! - `<Name>.System/Data/system`

use plist::dictionary::Dictionary;
use plist::Value;
use std::path::{Path, PathBuf};

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum PlistKind {
    Info,
    System,
}

#[derive(Debug)]
pub struct SystemBundle {
    root: PathBuf,
    plist_path: PathBuf,
    plist_kind: PlistKind,
    plist: Dictionary,
}

impl SystemBundle {
    pub fn open(path: &Path) -> Result<Self, String> {
        if !path.is_dir() {
            return Err(format!("{} is not a directory", path.display()));
        }

        if path.extension().and_then(|ext| ext.to_str()) != Some("System") {
            return Err(format!(
                "{} is not a .System directory",
                path.display()
            ));
        }

        let (plist_path, plist_kind) = Self::find_plist(path)?;
        let plist = Self::read_plist_dict(&plist_path)?;
        Self::validate_layout(path)?;

        Ok(Self {
            root: path.to_path_buf(),
            plist_path,
            plist_kind,
            plist,
        })
    }

    fn find_plist(path: &Path) -> Result<(PathBuf, PlistKind), String> {
        let info_plist = path.join("Info.plist");
        if info_plist.is_file() {
            return Ok((info_plist, PlistKind::Info));
        }

        let system_plist = path.join("System.plist");
        if system_plist.is_file() {
            return Ok((system_plist, PlistKind::System));
        }

        Err(format!(
            "Could not find Info.plist or System.plist in {}",
            path.display()
        ))
    }

    fn read_plist_dict(path: &Path) -> Result<Dictionary, String> {
        let plist = Value::from_file(path)
            .map_err(|e| format!("Could not deserialize plist at {}: {e}", path.display()))?;
        plist.into_dictionary().ok_or_else(|| {
            format!(
                "plist root in {} is not a dictionary",
                path.display()
            )
        })
    }

    fn validate_layout(root: &Path) -> Result<(), String> {
        let required = [
            "System/Applications",
            "System/Frameworks",
            "System/usr",
            "System/usr/lib",
            "System/usr/include",
            "Data/user",
            "Data/system",
            "Resources"
        ];

        let missing: Vec<String> = required
            .iter()
            .filter_map(|relative| {
                let full = root.join(relative);
                if full.is_dir() {
                    None
                } else {
                    Some(relative.to_string())
                }
            })
            .collect();

        if missing.is_empty() {
            Ok(())
        } else {
            Err(format!(
                "Invalid .System layout in {}. Missing directories: {}",
                root.display(),
                missing.join(", ")
            ))
        }
    }

    pub fn root_path(&self) -> &Path {
        &self.root
    }

    pub fn plist_path(&self) -> &Path {
        &self.plist_path
    }

    pub fn plist_kind(&self) -> PlistKind {
        self.plist_kind
    }

    pub fn plist(&self) -> &Dictionary {
        &self.plist
    }

    pub fn system_root(&self) -> PathBuf {
        self.root.join("System")
    }

    pub fn data_root(&self) -> PathBuf {
        self.root.join("Data")
    }

    pub fn system_applications_dir(&self) -> PathBuf {
        self.system_root().join("Applications")
    }

    pub fn system_frameworks_dir(&self) -> PathBuf {
        self.system_root().join("Frameworks")
    }

    pub fn system_usr_dir(&self) -> PathBuf {
        self.system_root().join("usr")
    }

    pub fn data_user_dir(&self) -> PathBuf {
        self.data_root().join("user")
    }

    pub fn data_system_dir(&self) -> PathBuf {
        self.data_root().join("system")
    }

    pub fn identifier(&self) -> Option<&str> {
        self.plist
            .get("CFBundleIdentifier")
            .and_then(|v| v.as_string())
    }
}
