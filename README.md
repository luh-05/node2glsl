# node2glsl
Utility to convert a shader node-graph to glsl

## Installation
**Prerequisites**:
 - git
 - gcc
 - sdl3 (not on windows)

> [!TIP]
> A **Nix flake** containing all dependencies is provided.
> Open the dev shell with `nix develop`

### Step 1 (Clone)
**Clone (ssh)**:
```sh
git clone git@github.com:luh-05/node2glsl.git
cd node2glsl
git submodule update --init --recurse
```

**Clone (https)**:
```sh
git clone https://github.com/luh-05/node2glsl.git
cd node2glsl
git submodule update --init --recurse
```
### Step 2 (Compile)
**On Linux**:
```sh
./build-debug.sh
```

**On Windows/Mac**:
```cmd
cmake --preset debug
cmake --build --preset debug
```

> [!IMPORTANT]
> On Unix SDL3 is assumed to be installed to the system.
> On Windows SDL3 is compiled locally.

> [!TIP]
> It's possible to compile locally on linux too using `-DCOMPILE_SDL_LOCALLY=ON`.
> This requires the dev libraries of wayland/x11 to be installed (not included in the nix flake).
