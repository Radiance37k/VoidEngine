#include "WindowManager.hpp"

#include <stdexcept>

#include <External/GLFW/include/GLFW/glfw3.h>

namespace VoidEngine {
    WindowManager::WindowManager(int w, int h, std::string name)
    {
        init();
    }

    WindowManager::~WindowManager()
    {
    }

    void WindowManager::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(instance, window->getGLFWwindow(), nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface");
        }
    }


    bool WindowManager::ShouldClose()
    {
        return glfwWindowShouldClose(window->getGLFWwindow());
    }

    void WindowManager::init()
    {
    }
} // VoidEngine