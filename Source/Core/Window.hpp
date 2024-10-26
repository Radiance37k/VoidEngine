#pragma once

#define GLFW_INCLUDE_VULKAN
#include <../External/glfw/include/GLFW/glfw3.h>

#include <string>

namespace VoidEngine
{

    class Window
    {
    public:
        Window(int w, int h, std::string name);
        ~Window();

        //Window(const Window &) = delete;
        //Window &operator=(const Window &) = delete;

        bool shouldClose() { return glfwWindowShouldClose(window); }
        VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
        bool wasWindowResized() { return frameBufferResized; }
        void resetWindowResizedFlag() { frameBufferResized = false; }

        GLFWwindow* getGLFWwindow() const { return window; }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

    private:
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
        void initWindow();

        int width = 800;
        int height = 600;
        bool frameBufferResized = false;

        std::string windowName;
        GLFWwindow* window;
    };
} // lve