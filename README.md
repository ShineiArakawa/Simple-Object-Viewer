# Simple Object Viewer

![Window Example](/assets/example_window.png)

## Features

- Load polygons from Wavefront obj file, STL file, and other format files
- Has pre-defined primitives (e.g. box, sphere, point cloud, text box)
- Save screen shots
- Shadow mapping

## Dependency
All these libraries are registered as submodules.

- GLFW
- GLAD
- Dear ImGui
- assimp
- FreeType

## Tested Environments

- Ubuntu 22.04 with gcc 11.4.0 compiler
- Windows 10 with Microsoft Visual Studio Community 2019 (16.11.21)

## Clone and Build

1. Clone
   ```bash
   git clone --recursive https://github.com/ShineiArakawa/Simple-Object-Viewer.git 
   ```
   
2. Build
   Run following commands to build source files. The executable binaries will be copied to the `bin` directory.

   ### Windows
   Please use Visual Studio with CMake.
  
   ### Ubuntu
   ```bash
   # Clone
   git clone --recursive https://github.com/ShineiArakawa/Simple-Object-Viewer.git 
   cd Simple-Object-Viewer
   
   # Build
   mkdir build && cd build
   cmake ..
   make
   ```
