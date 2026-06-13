{
  description = "ckit";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    ctools.url = "github:voidptrr/ctools";
  };

  outputs = {
    ctools,
    nixpkgs,
    ...
  }: let
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

    packages = forEachSystem ({pkgs}: {
      default = pkgs.callPackage ./tools/nix/package.nix {};
      ckit = pkgs.callPackage ./tools/nix/package.nix {};
    });

    devShells = forEachSystem ({pkgs}: {
      default = ctools.lib.mkCShell {
        inherit pkgs;
        extraPackages = [
          (pkgs.python3.withPackages (ps: [
            ps.mkdocs
            ps.mkdocs-material
          ]))
        ];
      };
    });

    checks = forEachSystem ({pkgs}:
      ctools.lib.mkCChecks {
        inherit pkgs;
        src = ./.;
        formatDirs = ["src" "tests" "include"];
        nixDirs = ["flake.nix" "tools/nix"];
        sourceDirs = ["src" "tests"];
        headerDirs = ["include"];
      });
  };
}
