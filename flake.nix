{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };
  outputs = { self, nixpkgs }:
    let
      # Define the system architecture
      system = "x86_64-linux"; # Replace with your system if different
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        # List of packages to be available in the shell
        packages = with pkgs;
          [
          # Example packages: git and htop
            rustc
            cargo
            gcc
            androidndkPkgs_25.sdk
            adb
            android-studio
            clang
            pkg-config
            gnustep.base
            gnustep.gui
            clangd
          ]
          ++ lib.optionals stdenv.isLinux [ libobjc2 ]
          ++ lib.optionals stdenv.isDarwin [ darwin.apple_sdk.frameworks.Foundation ];
        # Optional: Add a shell hook
#        shellHook = ''
#          export OBJCFLAGS="-I${pkgs.libobjc2}/include"
#          export LDFLAGS="-L${pkgs.libobjc2}/lib -lobjc" 
#        '';
    };
  };
}
