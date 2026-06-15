cmake_minimum_required(VERSION 3.20)
project(Caliber VERSION 0.1.0 LANGUAGES C CXX)

# ── C++ Standard ──────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Source Files ──────────────────────────────────────────────
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(Caliber
    ${SOURCES}
    external/glad/src/glad.c
    src/renderer/Shader.h
    src/core/Types.h
    src/renderer/Buffer.cpp
    src/scene/Camera.cpp
    external/stb/stb_image.cpp      # GLAD compiled directly with project
)

# ── Include Directories ───────────────────────────────────────
target_include_directories(Caliber PRIVATE
    src
    external/glad/include
    external/glfw/include
    external/glm
    external/stb
    external/assimp/include
)

# ── GLFW (built from source via submodule) ────────────────────
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# ── OpenGL ────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)

# ── Link Everything ───────────────────────────────────────────
target_link_libraries(Caliber PRIVATE
    OpenGL::GL
    glfw
    ImGui
    assimp 
)

# ── ImGui ─────────────────────────────────────────────────────
set(IMGUI_DIR external/imgui)

add_library(ImGui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(ImGui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    external/glfw/include
)



# ── Assimp ────────────────────────────────────────────────────
set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_EXAMPLES     OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL            OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT          ON  CACHE BOOL "" FORCE)

# only build GLTF and OBJ importers — speeds up compile massively
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER           ON  CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER            ON  CACHE BOOL "" FORCE)

add_subdirectory(external/assimp)





# ── Warnings ──────────────────────────────────────────────────
target_compile_options(Caliber PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# collect shader files
file(GLOB_RECURSE SHADER_FILES
    "${CMAKE_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_SOURCE_DIR}/shaders/*.frag"
)

add_custom_target(CopyShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:Caliber>/shaders
    COMMENT "Copying shaders..."
)

add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:Caliber>/assets
    COMMENT "Copying assets..."
)

add_dependencies(Caliber CopyShaders CopyAssets)