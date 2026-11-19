



# Caliber 

![C++20](https://img.shields.io/badge/C++-20-blue.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-4.6-green.svg)


Caliber is a lightweight, performance-focused 3D rendering engine written in modern C++20. It serves as a testbed for Physically Based Rendering (PBR), procedural animation, and custom game engine architecture.

<img width="800" height="450" alt="caliber-ezgif com-video-to-gif-converter" src="https://github.com/user-attachments/assets/4a13e55d-2859-4be9-a04c-5f762048cc18" />

<img width="800" height="450" alt="caliber-ezgif com-video-to-gif-converter (222" src="https://github.com/user-attachments/assets/ceb23191-b4ca-43f6-b0bd-12d09da4a056" />


## Stack
* **Core Language:** C++20 (Utilizing modern features, move semantics, and aggregate initialization)
* **Graphics API:** OpenGL 4.6 (Core Profile)
* **Extension Loader:** GLAD (Dynamically loads modern OpenGL functions)
* **Window & Input:** GLFW 3 (Handles cross-platform window creation, mouse delta capture, and keyboard state)
* **Mathematics:** GLM (OpenGL Mathematics for matrix transformations, quaternions, and vector calculus)
* **Asset Pipeline:** Assimp (Open Asset Import Library for parsing complex `.gltf` hierarchies and materials)
* **GUI / Tooling:** Dear ImGui (Immediate-mode GUI for real-time debugging of PBR materials and lighting states)
* **Build System:** CMake (Standardized, cross-platform build generation)



## Core Features
* **Physically Based Rendering (PBR):** Full metallic/roughness workflow with HDR lighting support.
* **Complex Model Loading:** Integrates `Assimp` to parse `glTF` nodes, meshes, and hierarchical transformations.
* **Procedural Animation System:** Mathematical, frame-independent animation controller handling weapon recoil, slide mechanics, and magazine drops.
* **FPS Viewmodel Physics:** Features a spring-based viewmodel sway system that reacts to mouse delta for heavy, realistic weapon handling.
* **Dynamic Lighting:** Real-time muzzle flashes tied directly to the weapon's local coordinate space.
* **Sun and Sky:** Procedural sky with an antialiased sun disc, a soft halo, and matching directional PBR sunlight. The background follows camera rotation and zoom, while remaining independent of camera translation.
* **ImGui Integration:** Real-time debugging and parameter tuning for materials, transforms, and lights.

## Build Instructions (CMake)
Caliber uses a standard CMake build system.

```bash
# Clone the repository
git clone --recurse-submodules https://github.com/Virus2466/Caliber.git
cd Caliber

mkdir build
cd build
cmake ..
cmake --build .
```

For an existing checkout, run `git submodule update --init --recursive` first.
Run the executable from its output directory so it can find `shaders/` and `assets/`.

## Sun and Sky Controls

Press **Tab** to release the mouse and use **Sun & Sky** in the debug panel.
Adjust azimuth, elevation, angular radius, color, intensity, and exposure.
The default sun is above and to the right of the initial camera view.
Its direction is measured **toward** the sun; azimuth zero faces -Z.
The same direction and radiance drive the sky and material lighting, and
sunlight does not use the distance attenuation of the existing point lights.
**Enable Sun** toggles both the visible sun and its direct lighting.

This is a procedural daylight background with a simple lower hemisphere;
it does not yet include terrain, cast shadows, atmospheric scattering,
image-based reflections, or a day/night cycle. The existing studio lights
and ambient fill remain available. The weapon is currently a world-space
model; camera-attached arms and weapon rendering are a subsequent step.

## Gallery




<img width="1269" height="719" alt="Screenshot 2026-05-26 124846" src="https://github.com/user-attachments/assets/dae7863e-7941-4ea4-b64d-720cf4ceecf6" />

<img width="1274" height="716" alt="Screenshot 2026-05-26 124959" src="https://github.com/user-attachments/assets/ab84fb5b-3945-49bf-b041-cda47381b129" />

<video width="100%" autoplay loop muted controls>
  <source src="https://github.com/user-attachments/assets/ce6eb0fa-446e-46b0-b4d9-486d8b9b78d3" type="video/mp4">
</video>
