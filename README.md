# PC3D — Interactive 3D Computer Case & Component Explorer

Graphics Fundamentals final project · C++17 · OpenGL 3.3 core · GLFW · GLAD · GLM · CMake

## Current progress (what already works)


Parts are built from procedural boxes/cylinders for now; the OBJ/glTF loader and the Blender meshes are the next step.

## Controls
| Key / mouse | Action |
|---|---|
| Left drag | rotate camera |
| Right drag | pan |
| Wheel | zoom |
| E | explode / assemble |
| T | start / stop cinematic tour |
| N / Shift+N | highlight next / previous component |
| L | toggle moving light |
| R | reset camera |
| ESC | quit |

## Build in VS Code (Windows / Linux / macOS)
1. Install: VS Code, a C++ compiler (Windows: *Visual Studio Build Tools* "Desktop development with C++", or MinGW-w64), CMake, Git.
2. VS Code extensions: *C/C++* and *CMake Tools* (VS Code will suggest them automatically).
3. `File → Open Folder…` → choose the `PC3D` folder.
4. Pick a kit when asked (e.g. *Visual Studio Community … amd64* or *GCC …*).
5. Click Build in the bottom bar, then Run (or press `F5` after choosing the PC3D target).

The first configure downloads GLFW, GLM and GLAD automatically (internet needed once) — nothing to install manually.

Command line alternative:
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/PC3D            # Windows MSVC: build\Release\PC3D.exe
```
Linux packages needed for GLFW: `sudo apt install build-essential cmake git libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev`

## Project structure
```
CMakeLists.txt
shaders/phong.vert, phong.frag     Aiym      – Blinn-Phong, glass alpha, emissive glow
src/shader.h                       Aiym      – shader compile/link + uniforms
src/camera.h                       Baizhana  – orbit camera, keyframe tour (Catmull-Rom)
src/mesh.h                         Assylzhan – box/cylinder meshes (loader comes next)
src/scene.h                        Assylzhan – scene graph, exploded-view logic
src/pc_model.h                     Baizhana & Aiym - PC model
src/main.cpp                       all       – window, input, render loop
```
