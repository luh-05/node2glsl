# Mollusk
Utility to convert a shader node-graph to glsl

## Installation
**Prerequisites**:
 - git
 - gcc (and g++)
 - ninja
 - cmake
 - shaderc
 - sdl3
 - doxygen
 - sphinx
 - breathe

> [!TIP]
> A **Nix flake** containing all dependencies is provided.
> Open the dev shell with `nix develop`

### Windows Prerequisites Installation Guide (except git)
**Step 1:** Install MSYS2 https://www.msys2.org/  
**Step 2:** Add the path of the "MSYS2 UCRT64" shell (should be "C:\msys64\ucrt64\bin") to your PATH environment variable.
- **DE guide:** Windows Suche "Systemumgebungsvariablen bearbeiten" -> Umgebungsvariablen (ganz unten) -> In der Liste "Path" auswählen -> Bearbeiten -> Neu -> C:\msys64\ucrt64\bin hinzufügen
  
**Step 3:** Open "MSYS2 UCRT64" shell and install the required packages with the following command:
```sh
pacman -S mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-doxygen mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-pkgconf mingw-w64-ucrt-x86_64-shaderc mingw-w64-ucrt-x86_64-sdl3 mingw-w64-ucrt-x86_64-abseil-cpp
```
**Step 4:** Download ninja https://github.com/ninja-build/ninja/releases. Put the .exe in a place you like (for this example "C:\Program Files\ninja") **and add that path to the PATH environment variable too**  
**Step 5:** In the UCRT64 shell you can now navigate to where you want to install this project. To get to your C:/ drive use `cd /c/`  
**(optional Step6:)** You may additionally install cmake via the msi installer https://cmake.org/download/. With this you can build this project in a normal terminal via cmake commands.

### Step 1 (Clone)
**Clone (ssh)**:
```sh
git clone git@github.com:luh-05/node2glsl.git
cd node2glsl
git submodule update --init --recursive
```

**Clone (https)**:
```sh
git clone https://github.com/luh-05/node2glsl.git
cd node2glsl
git submodule update --init --recursive
```
### Step 2 (Compile)
**On Linux or Windows with MSYS2 UCRT64 shell**:
```sh
./build-debug.sh
```

**On Windows/Mac**:
```cmd
cmake --preset debug
cmake --build --preset debug
```

### Step 3 (Run)
```sh
./bin/debug/viz.exe --gpu_driver vulkan
```

> [!IMPORTANT]
> On Windows you might need to disable "Smart App Control" to run viz.exe
