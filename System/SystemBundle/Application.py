#!/usr/bin/env python3

import plistlib
import shutil
import subprocess
from pathlib import Path

import typer
from rich import print

app = typer.Typer(
    name="pdsys",
    help="PocketDarwin System bundle tool. Sharp edges included.",
)

DEFAULT_DIRS = [
    "Kernel",
    "Extensions",
    "Libraries",
    "Resources",
]


def run(cmd: list[str]):
    try:
        subprocess.run(cmd, check=True)
    except FileNotFoundError:
        print(f"[red]Error:[/red] Command not found: {cmd[0]}")
        raise typer.Exit(1)
    except subprocess.CalledProcessError:
        print(f"[red]Error:[/red] Command failed: {' '.join(cmd)}")
        raise typer.Exit(1)


@app.command()
def create(
    name: str = typer.Argument(..., help="System name (e.g. BaseOS)"),
    version: str = typer.Option("0.1", help="System version"),
    arch: str = typer.Option("arm64", help="Target architecture"),
    out: Path = typer.Option(Path("."), help="Output directory"),
    kernel: Path = typer.Option(
        None,
        "--kernel",
        help="Path to kernel binary to copy into the bundle",
    ),
    image: Path = typer.Option(
        None,
        "--image",
        help="Create a raw disk image at this path",
    ),
    image_size: str = typer.Option(
        "512M",
        help="Raw disk image size (qemu-img syntax, e.g. 512M, 1G)",
    ),
    identifier: str = typer.Option(
        None,
        help="Reverse-DNS identifier (default: org.pocketdarwin.<name>)",
    ),
):
    """
    Create a PocketDarwin .System bundle.
    """

    bundle_name = f"{name}.System"
    bundle_path = out / bundle_name

    if bundle_path.exists():
        print(f"[red]Error:[/red] {bundle_name} already exists.")
        raise typer.Exit(1)

    print(f"[cyan]Creating system bundle:[/cyan] {bundle_path}")
    bundle_path.mkdir(parents=True)

    for d in DEFAULT_DIRS:
        (bundle_path / d).mkdir()
        print(f"  [green]✓[/green] {d}/")

    if kernel:
        if not kernel.exists():
            print(f"[red]Error:[/red] Kernel not found: {kernel}")
            raise typer.Exit(1)

        kernel_dst = bundle_path / "Kernel" / kernel.name
        shutil.copy2(kernel, kernel_dst)
        print(f"  [green]✓[/green] Kernel copied → Kernel/{kernel.name}")

    system_plist = {
        "CFBundleName": name,
        "CFBundleIdentifier": identifier or f"org.pocketdarwin.{name.lower()}",
        "CFBundleVersion": version,
        "PDSystemArchitecture": arch,
        "PDSystemFormatVersion": 1,
        "PDSystemType": "Base",
    }

    plist_path = bundle_path / "System.plist"
    with plist_path.open("wb") as f:
        plistlib.dump(system_plist, f)

    print(f"  [green]✓[/green] System.plist")

    if image:
        print(f"[cyan]Creating raw disk image:[/cyan] {image}")
        run([
            "qemu-img",
            "create",
            "-f",
            "raw",
            str(image),
            image_size,
        ])
        print(f"  [green]✓[/green] Raw image ({image_size})")

    print()
    print("[bold green]System bundle creation complete.[/bold green]")
    print("You now possess:")
    print(f"  • A .System bundle with a kernel inside")
    print(f"  • A raw disk image (if requested)")
    print("The rest is bootloader politics.")


def main():
    app()


if __name__ == "__main__":
    main()

