{
  description = "cstd";

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

    devShells = forEachSystem ({pkgs}: let
      commands = import ./tooling/nix/scripts {inherit pkgs;};
    in {
      default = pkgs.mkShell {
        packages = [pkgs.gcc] ++ builtins.attrValues commands;
      };
    });
  };
}
