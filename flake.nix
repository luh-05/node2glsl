{
  description = "node2glsl flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        buildInputs = with pkgs; [
          gcc
          clang-tools
          cmake
          ninja
          sdl3
          pkg-config
          lazygit
          cloc
        ];
      in
      {
        devShell = pkgs.mkShell {
          inherit buildInputs;
          shellHook = ''
            alias la="eza -lA"
          '';
        };
      }
    );
}
