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
- Assimp
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
  
   ### Linux
   ```bash
   # Build
   mkdir build && cd build
   cmake ..
   make
   ```

   ### Mac
   Sorry, I don't know the build system on Mac. Maybe, you can use GCC or Clang compiler.

## Known Issues

- Attempting to build Assimp with the GCC 12 compiler will likely fail. This failure is believed to be due to references to features of the std library that were deprecated in version 12. When building with the Clang compiler, there may be instances where it inadvertently references the GCC 12 toolchain. Therefore, if the build with the Clang compiler fails, re-running the CMake configuration with the following commands might resolve the issue.
  ```bash
  # Replace '/path/to/your/gcc' with the path to your GCC install dir older than 12,
  # for example '/usr/lib/gcc/x86_64-linux-gnu/11'.
  
  cmake -DCMAKE_CXX_FLAGS="--gcc-install-dir=/path/to/your/gcc" ..
  ```
