extern crate plist;
#[macro_use]
extern crate serde_derive;

#[derive(Deserialize)]
#[serde(rename_all = "PascalCase")]
struct SystemPkg {
    name: String,
    version: u64,
    identif: String,
    kernel_entr: Path,
    iphroot: Path,
}

// TODO: finish the implementation of CFSystemBundle.