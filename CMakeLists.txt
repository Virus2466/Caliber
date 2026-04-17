cmake_minimum_required(VERSION 3.20)
project(Caliber VERSION 0.1.0 LANGUAGES CXX)

# ── C++ Standard ──────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # needed for clangd IntelliSense

# ── Source Files ──────────────────────────────────────────────
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "external/glad/src/glad.c"          # GLAD source compiled with project
)

# ── Executable ────────────────────────────────────────────────
add_executable(Caliber ${SOURCES})

# ── Include Directories ───────────────────────────────────────
target_include_directories(Caliber PRIVATE
    src
    external/glad/include
    external/glfw/include
    external/glm
    external/stb
)

# ── GLFW ──────────────────────────────────────────────────────
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# ── OpenGL ────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)

# ── Link Libraries ────────────────────────────────────────────
target_link_libraries(Caliber PRIVATE
    OpenGL::GL
    glfw
)

# ── Compiler Warnings (good practice) ─────────────────────────
target_compile_options(Caliber PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# ── Copy Assets to build directory ────────────────────────────
add_custom_command(TARGET Caliber POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:Caliber>/shaders
)
add_custom_command(TARGET Caliber POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:Caliber>/assets
)