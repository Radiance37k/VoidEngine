#pragma once

#include "Common.hpp"
#include "Window.hpp"
#include "Device.hpp"
#include "Renderer.hpp"
#include "Descriptors.hpp"
#include "GameObject.hpp"
#include "RenderManager.hpp"
#include "CameraManager.hpp"
#include "InputManager.hpp"
#include "LightSourceManager.hpp"
#include "ModelManager.hpp"
#include "SceneManager.hpp"
#include "UIManager.hpp"
#include "WindowManager.hpp"

#include <iostream>

#include "FrameInfo.hpp"

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

        VOIDENGINE_API inline Device* GetDevice() const { return device; }
        VOIDENGINE_API inline VkExtent2D GetResolution() const { return window->getExtent(); }

        VOIDENGINE_API inline SceneManager* GetSceneManager() const { return sceneManager.get(); }

        VOIDENGINE_API void AddGameObject(GameObject& gameObject);//, RenderQueueType renderQueue = RenderQueueType::OPAQUE);

        std::unique_ptr<CameraManager> cameraManager;
        std::unique_ptr<InputManager> inputManager;
        std::unique_ptr<LightSourceManager> lightSourceManager;
        std::unique_ptr<ModelManager> modelManager;
        std::unique_ptr<RenderManager> renderManager;
        std::unique_ptr<SceneManager> sceneManager;
        std::unique_ptr<UIManager> uiManager;
        std::unique_ptr<WindowManager> windowManager;

    private:
        void loadGameObjects();

        Window* window = new Window(WIDTH, HEIGHT, "Hello Vulkan");
        Device* device = new Device(*window);
        Renderer* renderer;

        VkDescriptorSet dset{};

        // Note: Order of declerations matter
        std::unique_ptr<DescriptorPool> globalPool;
        GameObject::Map gameObjects;
    };
}
