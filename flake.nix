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
        services.udev.packages = [];
        packages = with pkgs; [
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
          if pkgs.stdenv.isLinux then [ libobjc2 ] else [ ]
          if pkgs.stdenv.isDarwin then [ darwin.apple_sdk.frameworks.Foundation ] else [ ]
        ]
        # Optional: Add a shell hook
        shellHook = ''
          export OBJCFLAGS="-I${pkgs.libobjc2}/include"
          export LDFLAGS="-L${pkgs.libobjc2}/lib -lobjc" 

          export PS1=""
        ''
      };
    };

    config.android_sdk.accept_license = true;
  overlays = [
    (self: super: {
      androidPkgs_8_0 = super.androidenv.composeAndroidPackages {
        platformVersions = [ "26" ];
        abiVersions = [ "x86" "x86_64" "aarch64"];
      };
    })
  ];

  (pkgs.buildFHSUserEnv {
  name = "android-sdk-env";
  targetPkgs = pkgs: (with pkgs;
    [
      androidPkgs_8_0.androidsdk
      glibc
    ]);
  runScript = "bash";
}).env

   nixConfig = {
    bash-prompt = "[pocketdarwin] \u@\h:\w$ ";
    # Optional prefix/suffix
    bash-prompt-prefix = "❄️  ";
  };
}

