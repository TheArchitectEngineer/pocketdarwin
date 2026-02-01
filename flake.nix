{
  description = "Dev flake for PocketDarwin: development shell with build tools and helpers";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11"; # reasonably recent
  };

  outputs = { self, nixpkgs }: let
    systems = [ "x86_64-linux" ];
    pkgsFor = system: import nixpkgs { inherit system; };
  in {
    devShells = builtins.listToAttrs (map (system: {
      name = system;
      value = let
        pkgs = pkgsFor system;
        # cross toolchains: aarch64 linux-gnu and bare-metal (none/elf)
        pkgsCrossAarch64Linux = import nixpkgs { inherit system; crossSystem = { config = "aarch64-unknown-linux-gnu"; }; };
        pkgsCrossAarch64None = import nixpkgs { inherit system; crossSystem = { config = "aarch64-unknown-none-eabi"; }; };
      in pkgs.mkShell {
        name = "pocketdarwin-dev-${system}";

        buildInputs = with pkgs; [
          # native compiler and tools
          clang
          gcc
          gnumake
          binutils
          nasm

          # device tree compiler
          device-tree-compiler

          # utilities
          python3
          unzip
          ccache
          pkgconfig
          patchelf

          # useful dev tools
          git
          file
          rsync
          # cross compilers (may be large; provided via pkgsCross)
          pkgsCrossAarch64Linux.gcc
          pkgsCrossAarch64Linux.binutils
          pkgsCrossAarch64None.gcc
          pkgsCrossAarch64None.binutils
        ];

        # environment hints and helpful wrappers
        shellHook = ''
          echo "PocketDarwin dev shell (system=${system})"
          echo "Make: use 'make' in OCMobile; for cross builds set CC/CXX to your cross toolchain."
          echo "To build Android boot images you'll need 'mkbootimg' — install it in your system or add it to this flake."

          # If user has aarch64-none-elf-* in PATH, prefer them
          if command -v aarch64-none-elf-gcc >/dev/null 2>&1; then
            export CROSS_COMPILE=aarch64-none-elf-
            export CC=${CROSS_COMPILE}gcc
            export CXX=${CROSS_COMPILE}g++
            echo "Detected aarch64-none-elf cross compiler; CC/CXX set to ${CC}/${CXX}"
          fi
        '';
      };
    }) systems);

    # A small utility output: a README explaining how to add cross-toolchain or mkbootimg
    packages = {
      README = {
        type = "text";
        text = ''
PocketDarwin flake
==================
This flake provides a development shell with native build tools (clang, gcc, make),
`device-tree-compiler` and helpers.

How to use
----------
Install and enter the shell:

  nix develop

Inside the shell run the usual build commands in OCMobile:

  cd OCMobile
  make clean
  make

Cross-compilers
---------------
You will likely want an aarch64 bare-metal cross toolchain such as
`aarch64-none-elf-gcc` or an LLVM-based `aarch64-none-elf-clang` to build
`-ffreestanding` artifacts for the device. There are two approaches:

1) Install a cross-toolchain on your host and ensure `aarch64-none-elf-gcc` is
   on your `PATH`. The shell hook will auto-detect and set `CC/CXX`.

2) Extend this flake to provide a cross toolchain by importing nixpkgs with
   `crossSystem` — see Nixpkgs manual for `pkgsCross` / `crossSystem` examples.

mkbootimg / Android tools
-------------------------
`mkbootimg` is not universally available as a single nixpkgs package. You can:
- Install Android platform tools on the host (some distros include `mkbootimg`).
- Add a custom package or use `abootimg` as a fallback.

If you want, I can update this flake to try to provide a specific cross-toolchain
by default (for example, `gcc-aarch64-elf`) — tell me which target/toolchain
you prefer and I will add it.
'';
      };
    };
  };
}
