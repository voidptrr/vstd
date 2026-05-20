{
  description = "ckit";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = {nixpkgs, ...}: let
    systems = [
      "x86_64-linux"
      "aarch64-linux"
    ];
    forEachSystem = f:
      nixpkgs.lib.genAttrs systems (system:
        f {
          pkgs = import nixpkgs {inherit system;};
        });
  in {
    formatter = forEachSystem ({pkgs}: pkgs.alejandra);

    devShells = forEachSystem ({pkgs}: {
      default = import ./tools/nix/shell.nix {inherit pkgs;};
    });
  };
}
