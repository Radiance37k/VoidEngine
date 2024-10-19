#pragma once

#include "Common.hpp"
#include "Window.hpp"
#include "Device.hpp"
#include "GameObject.hpp"
#include "Renderer.hpp"
#include "Descriptors.hpp"

#include <iostream>

namespace VoidEngine
{
    class VOIDENGINE_API Game
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        Game();
        ~Game();

        void run();

    private:
        void loadGameObjects();

        Window* window = new Window(WIDTH, HEIGHT, "Hello Vulkan");
        Device* device = new Device(*window);
        Renderer renderer{*window, *device};

        // Note: Order of declerations matter
        std::unique_ptr<DescriptorPool> globalPool;
        GameObject::Map gameObjects;
    };
}
