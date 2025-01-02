#pragma once

#include "Common.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "Descriptors.hpp"
#include "GameObject.hpp"
#include "CameraManager.hpp"
#include "FrameInfo.hpp"
#include "InputManager.hpp"
#include "LightSourceManager.hpp"
#include "ModelManager.hpp"
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

        VOIDENGINE_API explicit Game(VkExtent2D resolution = {WIDTH, HEIGHT});
        VOIDENGINE_API ~Game();

        VOIDENGINE_API void run();

        VOIDENGINE_API inline Device* GetDevice() const { return device; }
        VOIDENGINE_API inline VkExtent2D GetResolution() const { return window->getExtent(); }

        VOIDENGINE_API inline SceneManager* GetSceneManager() const { return sceneManager.get(); }

        //template <typename T, typename... Args>
        //VOIDENGINE_API T* AddGameObject(RenderQueueType renderQueue = RenderQueueType::OPAQUE, Args&&... args);
        VOIDENGINE_API void AddGameObject(GameObject* gameObject, RenderQueueType renderQueue = RenderQueueType::OPAQUE) const;

        std::unique_ptr<CameraManager> cameraManager;
        std::unique_ptr<InputManager> inputManager;
        std::unique_ptr<LightSourceManager> lightSourceManager;
        //LightSourceManager* lightSourceManager;
        std::unique_ptr<ModelManager> modelManager;
        //std::unique_ptr<RenderManager> renderManager;
        RenderManager* renderManager;
        std::unique_ptr<SceneManager> sceneManager;
        std::unique_ptr<UIManager> uiManager;
        std::unique_ptr<WindowManager> windowManager;

        std::unique_ptr<GlobalUbo> ubo{};

        Camera* mainCamera;

    private:
        Window* window = new Window(WIDTH, HEIGHT, "Hello Vulkan");
        Device* device = new Device(*window);
        Renderer* renderer;

        VkDescriptorSet dset{};

        // Note: Order of declerations matter
        std::unique_ptr<DescriptorPool> globalPool;
        std::unordered_map<unsigned int, GameObject> gameObjects;
    };
}
