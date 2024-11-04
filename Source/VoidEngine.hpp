#pragma once

#include "Common.hpp"
#include "Core/Window.hpp"
#include "Core/Device.hpp"
#include "Components/GameObject.hpp"
#include "Core/Renderer.hpp"
#include "Core/Descriptors.hpp"

#include <iostream>

#include "CameraManager.hpp"
#include "InputManager.hpp"
#include "LightSourceManager.hpp"
#include "ModelManager.hpp"
#include "RenderManager.hpp"
#include "SceneManager.hpp"
#include "UIManager.hpp"
#include "WindowManager.hpp"

namespace VoidEngine
{
    class Game
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        VOIDENGINE_API explicit Game(VkExtent2D resolution = {800, 600});
        VOIDENGINE_API ~Game();

        VOIDENGINE_API void run();

        VOIDENGINE_API inline Device* GetDevice() const { return device; };
        VOIDENGINE_API inline VkExtent2D GetResolution() const { return window->getExtent(); };

        VOIDENGINE_API inline SceneManager* GetSceneManager() const { return sceneManager; };

        CameraManager* cameraManager;
        InputManager* inputManager;
        LightSourceManager* lightSourceManager;
        ModelManager* modelManager;
        RenderManager* renderManager;
        SceneManager* sceneManager;
        UIManager* uiManager;
        WindowManager* windowManager;

    private:
        void loadGameObjects();

        Window* window = new Window(WIDTH, HEIGHT, "Hello Vulkan");
        Device* device = new Device(*window);
        Renderer* renderer;

        // Note: Order of declerations matter
        std::unique_ptr<DescriptorPool> globalPool;
        GameObject::Map gameObjects;
    };
}
