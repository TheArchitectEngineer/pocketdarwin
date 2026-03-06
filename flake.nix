{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { nixpkgs, ... }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f system);

      mkDevShell = system:
        let
          pkgs = import nixpkgs {
            inherit system;
            config.allowUnfree = true;
          };
          maybe = path: pkgs.lib.attrByPath path null pkgs;
          isLinux = pkgs.stdenv.isLinux;
          isDarwin = pkgs.stdenv.isDarwin;
        in
        pkgs.mkShell {
          packages =
            builtins.filter (p: p != null) (
              [
                # Core toolchain
                pkgs.git
                pkgs.rustc
                pkgs.cargo
                pkgs.gcc
                pkgs.clang
                pkgs.cmake
                pkgs.ninja
                pkgs.pkg-config

                # touchHLE dependencies (dynamic/non-bundled builds)
                pkgs.boost
                pkgs.SDL2
                pkgs.openal
                pkgs.zlib
              ]
              ++ pkgs.lib.optionals isLinux [
                (maybe [ "android-tools" ])
                (maybe [ "android-studio" ])
                (maybe [ "gnustepPackages" "base" ])
                (maybe [ "gnustepPackages" "gui" ])
                (maybe [ "libobjc2" ])
              ]
              ++ pkgs.lib.optionals isDarwin [
                # Keep this minimal to avoid legacy apple_sdk compatibility
                # attr issues on some nixpkgs revisions.
              ]
            );

          shellHook = ''
            export CC=${pkgs.clang}/bin/clang
            export CXX=${pkgs.clang}/bin/clang++
          '';
        };
    in
    {
      devShells = forAllSystems (system: {
        default = mkDevShell system;
      });
    };
}
