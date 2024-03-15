# Simple Object Viewer

![Window Example](/assets/example_window.png)

## Features

- Load polygons from Wavefront obj file and STL file
- Has pre-defined primitives (e.g. box, sphere, point cloud)

## Dependency
All these libraries are registered as submodules.

- GLFW
- GLAD
- Dear ImGui
- assimp
- CMake

## Tested Environments

- Windows 10 with Microsoft Visual Studio Community 2019 (16.11.21)
- Ubuntu 22.04 with gcc 11.4.0 compiler

## Clone and Build

Run following commands to build source files. The executable binaries will be copied to the `bin` directory.

```bash
# Clone
git clone --recursive https://github.com/ShineiArakawa/Simple-Object-Viewer.git 
cd Simple-Object-Viewer

# Build
mkdir build && cd build
cmake ..
make
```
