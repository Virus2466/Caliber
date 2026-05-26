# Caliber
Real-time gun animation engine built from scratch in C++20 and OpenGL 4.6.

---

##Gallery

<img width="1269" height="719" alt="Screenshot 2026-05-26 124846" src="https://github.com/user-attachments/assets/dae7863e-7941-4ea4-b64d-720cf4ceecf6" />

<img width="1274" height="716" alt="Screenshot 2026-05-26 124959" src="https://github.com/user-attachments/assets/ab84fb5b-3945-49bf-b041-cda47381b129" />


## Features
- PBR rendering — Cook-Torrance BRDF
- GLTF model loading via Assimp
- Normal mapping + multiple light types
- Skeletal animation with blend trees *(in progress)*
- First person viewmodel with spring sway *(in progress)*
- Dear ImGui debug panel

## Stack
C++· OpenGL 4.6 · GLFW · GLM · Assimp · Dear ImGui

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

