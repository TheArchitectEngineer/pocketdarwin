extern crate clap;
use clap::{Arg, App, SubCommand};

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

fn main() {
    let matches = App::new("PocketDarwin System Bundle Manager & creator.")
                                                .version("0.1.2-rs-r1")
                                                .author("OakyMacintosh/Miguel V. Mesquita")
                                                .about("This program is able to create & modify System bundles.")
                                                .arg(Arg::with_name("rx")

                                            )
}