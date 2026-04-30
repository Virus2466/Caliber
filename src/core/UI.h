// File : UI.h
// @author : Dev Patel
// Date : 30/4/2026
// Thin wrapper around Dear ImGui for Caliber's debug interface.
// Manages ImGui initialization, frame lifecycle and shutdown

#pragma once 


#include <GLFW/glfw3.h>

namespace Caliber{
    class UI{
        public:
            static void init(GLFWwindow* window);
            static void shutdown();
            static void beginFrame();
            static void endFrame();
    };

} // namespace caliber