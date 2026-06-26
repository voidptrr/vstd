# MIT License
#
# Copyright (c) 2026 Tommaso Bruno
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
{
  description = "vstd";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    vtools.url = "github:voidptrr/vtools/3f68414dcb8ecb40655b111b943f244376b69afb";
  };

  outputs = {
    nixpkgs,
    vtools,
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
      default = pkgs.callPackage ./nix/package.nix {};
      vstd = pkgs.callPackage ./nix/package.nix {};
    });

    devShells = forEachSystem ({pkgs}: {
      default = vtools.lib.mkShell {
        inherit pkgs;
        enableC = true;
        extraPackages = [pkgs.zensical];
      };
    });

    checks = forEachSystem ({pkgs}: let
      checks = vtools.lib.mkChecks {
        inherit pkgs;
        src = ./.;
        enableNixFormat = true;
        enableCFormat = true;
        enableCLint = true;
        enableCTest = true;
      };
    in {
      inherit
        (checks)
        build-check
        code-check
        format-check
        lint-check
        ;
      test-check = pkgs.callPackage ./nix/package.nix {};
    });
  };
}
