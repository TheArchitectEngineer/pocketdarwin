use anyhow::{anyhow, bail, Context, Result};
use clap::{Parser, Subcommand, ValueEnum};
use colored::*;
use plist::Value;
use plist::Dictionary;
use std::fs;
use std::path::PathBuf;
use std::process::Command;

const DEFAULT_DIRS: &[&str] = &["Kernel", "Extensions", "Libraries", "Resources"];

#[derive(Parser)]
#[command(
    name = "pdsys",
    about = "PocketDarwin System bundle tool. Sharp edges included."
)]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    /// Create a PocketDarwin .System bundle.
    Create {
        name: String,

        #[arg(long, default_value = "0.1")]
        version: String,

        #[arg(long, default_value = "arm64")]
        arch: String,

        #[arg(long, default_value = ".")]
        out: PathBuf,

        #[arg(long)]
        kernel: Option<PathBuf>,

        #[arg(long)]
        iphoneosroot: Option<PathBuf>,

        #[arg(long, value_enum)]
        iphoneosreportingver: Option<IPhoneOSReportingVersion>,

        /// macOS reporting version
        #[arg(long, value_enum)]
        macosreportingver: Option<MacOSReportingVersion>,

        #[arg(long)]
        image: Option<PathBuf>,

        #[arg(long, default_value = "512M")]
        image_size: String,

        #[arg(long)]
        identifier: Option<String>,
    },
}

#[derive(Clone, ValueEnum)]
enum IPhoneOSReportingVersion {
    Iphoneos2,
    Iphoneos3,
    Iphoneos4,
}

impl IPhoneOSReportingVersion {
    fn as_str(&self) -> &'static str {
        match self {
            Self::Iphoneos2 => "iphoneos2",
            Self::Iphoneos3 => "iphoneos3",
            Self::Iphoneos4 => "iphoneos4",
        }
    }
}

#[derive(Clone, ValueEnum)]
enum MacOSReportingVersion {
//    V10_4,
    V10_5,
    V10_6,
//    V10_7,
//    V10_8,
//    V10_9,
//    V10_10,
//    V10_11,
//    V10_12,
//    V10_13,
//    V10_14,
//    V10_15,
//    V11,
//    V12,
//    V13,
//    V14,
}

impl MacOSReportingVersion {
    fn as_str(&self) -> &'static str {
        match self {
//            Self::V10_4 => "10.4",
            Self::V10_5 => "10.5",
            Self::V10_6 => "10.6",
//            Self::V10_7 => "10.7",
//            Self::V10_8 => "10.8",
//            Self::V10_9 => "10.9",
//            Self::V10_10 => "10.10",
//            Self::V10_11 => "10.11",
//            Self::V10_12 => "10.12",
//            Self::V10_13 => "10.13",
//            Self::V10_14 => "10.14",
//            Self::V10_15 => "10.15",
//            Self::V11 => "11",
//            Self::V12 => "12",
//            Self::V13 => "13",
//            Self::V14 => "14",
        }
    }
}

fn run(cmd: &mut Command) -> Result<()> {
    let status = cmd.status().context("Failed to execute command")?;
    if !status.success() {
        bail!("Command failed");
    }
    Ok(())
}

fn create(
    name: String,
    version: String,
    arch: String,
    out: PathBuf,
    kernel: Option<PathBuf>,
    iphoneosroot: Option<PathBuf>,
    iphoneosreportingver: Option<IPhoneOSReportingVersion>,
    macosreportingver: Option<MacOSReportingVersion>,
    image: Option<PathBuf>,
    image_size: String,
    identifier: Option<String>,
) -> Result<()> {
    if kernel.is_some() && iphoneosroot.is_some() {
        bail!("You cannot use --kernel and --iphoneosroot at the same time.");
    }

    if kernel.is_none() && iphoneosroot.is_none() {
        bail!("You must provide either --kernel or --iphoneosroot.");
    }

    if kernel.is_some() && iphoneosreportingver.is_some() {
        bail!("--iphoneosreportingver can only be used with --iphoneosroot.");
    }

    if iphoneosroot.is_some() && iphoneosreportingver.is_none() {
        bail!("--iphoneosreportingver is required when using --iphoneosroot.");
    }

    let bundle_name = format!("{}.System", name);
    let bundle_path = out.join(&bundle_name);

    if bundle_path.exists() {
        bail!("{} already exists.", bundle_name);
    }

    println!("{} {}", "Creating system bundle:".cyan(), bundle_path.display());
    fs::create_dir_all(&bundle_path)?;

    for dir in DEFAULT_DIRS {
        fs::create_dir(bundle_path.join(dir))?;
        println!("  {} {}/", "✓".green(), dir);
    }

    let mut system_plist = Dictionary::new();
    system_plist.insert("CFBundleName".into(), Value::String(name.clone()));
    system_plist.insert(
        "CFBundleIdentifier".into(),
        Value::String(
            identifier.unwrap_or_else(|| format!("org.pocketdarwin.{}", name.to_lowercase())),
        ),
    );
    system_plist.insert("CFBundleVersion".into(), Value::String(version));
    system_plist.insert("PDSystemArchitecture".into(), Value::String(arch));
    system_plist.insert("PDSystemFormatVersion".into(), Value::Integer(1.into()));

    if let Some(ver) = macosreportingver {
        system_plist.insert(
            "PDMacOSReportingVersion".into(),
            Value::String(ver.as_str().into()),
        );
    }

    if let Some(kernel_path) = kernel {
        let file_name = kernel_path
            .file_name()
            .ok_or_else(|| anyhow!("Invalid kernel filename"))?;
        fs::copy(&kernel_path, bundle_path.join("Kernel").join(file_name))?;
        system_plist.insert("PDSystemType".into(), Value::String("Base".into()));
    }

    if let Some(root_path) = iphoneosroot {
        let file_name = root_path
            .file_name()
            .ok_or_else(|| anyhow!("Invalid tarball filename"))?;
        fs::copy(&root_path, bundle_path.join("Resources").join(file_name))?;

        system_plist.insert("PDSystemType".into(), Value::String("iPhoneOSROOT".into()));
        system_plist.insert(
            "PDRootTarballPath".into(),
            Value::String(format!("Resources/{}", file_name.to_string_lossy())),
        );
        system_plist.insert(
            "PDRootExtractionPolicy".into(),
            Value::String("ExtractOnFirstBoot".into()),
        );
        system_plist.insert(
            "PDiPhoneOSReportingVersion".into(),
            Value::String(iphoneosreportingver.unwrap().as_str().into()),
        );
    }

    let file = fs::File::create(bundle_path.join("System.plist"))?;
    Value::Dictionary(system_plist).to_writer_xml(file)?; // expected `Dictionary`, found `BTreeMap<_, Value>`

    println!("  {} System.plist", "✓".green());

    if let Some(image_path) = image {
        println!("{} {}", "Creating raw disk image:".cyan(), image_path.display());
        run(
            Command::new("qemu-img")
                .arg("create")
                .arg("-f")
                .arg("raw")
                .arg(&image_path)
                .arg(&image_size),
        )?;
        println!("  {} Raw image ({})", "✓".green(), image_size);
    }

    println!();
    println!("{}", "System bundle creation complete.".bold().green());
    println!("Reporting layers successfully fabricated.");

    Ok(())
}

fn main() -> Result<()> {
    let cli = Cli::parse();

    match cli.command {
        Commands::Create {
            name,
            version,
            arch,
            out,
            kernel,
            iphoneosroot,
            iphoneosreportingver,
            macosreportingver,
            image,
            image_size,
            identifier,
        } => create(
            name,
            version,
            arch,
            out,
            kernel,
            iphoneosroot,
            iphoneosreportingver,
            macosreportingver,
            image,
            image_size,
            identifier,
        )?,
    }

    Ok(())
}