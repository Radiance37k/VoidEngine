#pragma once
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <External/glfw/include/GLFW/glfw3.h>

#include "Common.hpp"
#include "Window.hpp"

namespace VoidEngine
{
    class WindowManager
    {
    public:
        WindowManager(int w, int h, std::string name);
        ~WindowManager();

        bool ShouldClose();

        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

        Window* window;
    private:

        void init();
    };
} // VoidEngine