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
        nativeBuildInputs = with pkgs; [
          pkg-config
        ];
        # imgui = 
        #   pkgs.imgui.overrideAttrs (old: {
        #     IMGUI_BUILD_VULKAN_BINDING = true;
        #     IMGUI_BUILD_SDL3_BINDING = true;
        #     IMGUI_BUILD_SDL3_RENDERER_BINDING = true;
        #     IMGUI_BUILD_SDLGPU3_BINDING = true;
        #   });
        buildInputs = with pkgs; [
          gcc
          clang-tools
          cmake
          ninja
          sdl3
          shaderc
          sdl3-shadercross
          pkg-config
          lazygit
          cloc
          abseil-cpp
          glm
          spdlog

          # documentation
          doxygen
          python314
          python314Packages.breathe
          python314Packages.sphinx
          python314Packages.sphinx-rtd-theme
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
