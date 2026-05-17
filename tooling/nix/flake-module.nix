{...}: {
  perSystem = {pkgs, ...}: let
    commands = import ./packages {inherit pkgs;};
  in {
    formatter = pkgs.alejandra;

    devShells.default = pkgs.mkShell {
      packages =
        [
          pkgs.gcc
        ]
        ++ builtins.attrValues commands;
    };
  };
}
