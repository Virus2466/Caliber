# Caliber
Real-time gun animation engine built from scratch in C++20 and OpenGL 4.6.


---

## Features
- PBR rendering — Cook-Torrance BRDF
- GLTF model loading via Assimp
- Normal mapping + multiple light types
- Skeletal animation with blend trees *(in progress)*
- First person viewmodel with spring sway *(in progress)*
- Dear ImGui debug panel

## Stack
C++20 · OpenGL 4.6 · GLFW · GLM · Assimp · Dear ImGui

## Build
```bash
git clone https://github.com/Virus2466/Caliber.git
cd Caliber
git submodule update --init --recursive
mkdir build && cd build
cmake .. -G Ninja
cmake --build .
```

## Roadmap
- [x] PBR rendering pipeline
- [x] GLTF model loading
- [x] Normal mapping + lighting
- [ ] Skeletal animation
- [ ] Gun animations (fire, reload, inspect)
- [ ] First person viewmodel

---

*Built by Dev Patel under FrostWorks · MIT License*
