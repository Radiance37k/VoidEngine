#pragma once

#include "Common.hpp"
#include "Core/Window.hpp"
#include "Core/Device.hpp"
#include "Components/GameObject.hpp"
#include "Core/Renderer.hpp"
#include "Core/Descriptors.hpp"

#include <iostream>

namespace VoidEngine
{
    class Game
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        VOIDENGINE_API Game();
        VOIDENGINE_API ~Game();

        VOIDENGINE_API void run();

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
