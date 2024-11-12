#include "VoidEngine.hpp"

#include "InputManager.hpp"
#include "Camera.hpp"
#include "RenderManager.hpp"
#include "Core/Buffer.hpp"
#include "PointLight.hpp"

#include <chrono>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "External/glm/glm.hpp"
//#include <External/glm/gtc/constants.hpp>

namespace VoidEngine
{
    Game::Game(VkExtent2D resolution)
    //: renderManager(std::make_unique<RenderManager>(*device, *this, resolution))
    {
        //windowManager = new WindowManager(resolution.width, resolution.height, "Whoop");
        //cameraManager = new CameraManager();
        //inputManager = new InputManager();
        //lightSourceManager = new LightSourceManager();
        //modelManager = new ModelManager();
        //renderManager = new RenderManager(*device, *this, resolution);
        //sceneManager = new SceneManager();
        //uiManager = new UIManager();

        renderManager = std::make_unique<RenderManager>(*device, *this, resolution);
        sceneManager = std::make_unique<SceneManager>();

        globalPool = DescriptorPool::Builder(*device)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        renderer = new Renderer(*window, *device, RenderManager::FindDepthFormat(*device), renderManager->GetRenderPass());

        loadGameObjects();
    }

    Game::~Game()
    {
        std::cout << "Engine out!\n";
    }

    void Game::run()
    {
        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
            for (int i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<Buffer>(
                *device,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        auto globalSetLayout = DescriptorSetLayout::Builder(*device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            DescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        /*
        RenderManager simpleRenderSystem{
            *device,
            renderer.getSwapChainRenderPass(),
            globalSetLayout->getDescriptorSetLayout()};
            */

        /*
        PointLight pointLight{
            *device,
            renderManager->GetRenderPass(),
            globalSetLayout->getDescriptorSetLayout()};
        */

        Camera camera{};

        auto viewerObject = GameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        InputManager cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!window->shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(window->getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = renderManager->GetAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

            //if (auto commandBuffer = renderer->beginFrame(renderManager->GetRenderPass(), renderManager->GetSwapChain()))
            auto queueToRender = RenderQueueType::OPAQUE;

            auto commandBuffer = renderManager->GetQueueCommandBuffer(queueToRender);

            // vkBeginCommandBuffer
            if (renderer->beginFrame(commandBuffer, renderManager->GetRenderPass(), renderManager->GetSwapChain()))
            {
                int frameIndex = renderer->getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    gameObjects
                };

                // Update
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();
//                pointLight.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // vkCmdBeginRenderPass
                renderer->beginSwapChainRenderPass(commandBuffer, renderManager->GetRenderPass(), renderManager->GetSwapChain());
                //simpleRenderSystem.renderGameObjects(frameInfo);
                renderManager->UpdateDescriptorSet(renderManager->GetGlobalDescriptorSet(queueToRender), uboBuffers[frameIndex]->getBuffer());


                renderManager->RenderObjectsInQueue(queueToRender, nullptr);
//                pointLight.render(frameInfo);
                //vkCmdEndRenderPass
                renderer->endSwapChainRenderPass(commandBuffer);
                renderer->endFrame(renderManager->GetRenderPass(), renderManager->GetSwapChain(), commandBuffer);
            }
        }

        vkDeviceWaitIdle(device->device());
    }

    void Game::AddGameObject(GameObject& gameObject)//, RenderQueueType renderQueue)
    {
        renderManager->AddToRenderQueue(gameObject, RenderQueueType::OPAQUE);
        sceneManager->AddGameObject(std::move(gameObject));
    }

    void Game::loadGameObjects()
    {
        std::shared_ptr<Model> model;

        /*
        model = Model::createModelFromFile(*device, "models/flat_vase.obj");
        auto flatVase = GameObject::createGameObject();
        flatVase.model = model;
        flatVase.transform.translation = {-0.5f, 0.5f, 0.0f};
        flatVase.transform.scale = {3.0f, 1.5f, 3.0f};
        gameObjects.emplace(flatVase.getId(), std::move(flatVase));
        */

        model = Model::createModelFromFile(*device, "models/smooth_vase.obj");
        auto smoothVase = GameObject::createGameObject();
        smoothVase.model = model;
        smoothVase.transform.translation = {0.5f, 0.5f, 0.0f};
        smoothVase.transform.scale = {3.0f, 1.5f, 3.0f};
        gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

        model = Model::createModelFromFile(*device, "models/quad.obj");
        auto floor = GameObject::createGameObject();
        floor.model = model;
        floor.transform.translation = {0.0f, 0.5f, 0.0f};
        floor.transform.scale = {3.0f, 1.0f, 3.0f};
        gameObjects.emplace(floor.getId(), std::move(floor));

        {
            auto pointLight = GameObject::makePointLight(0.2f);
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        }

        std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f}  //
        };

        for (int i = 0; i < lightColors.size(); i++)
        {
            auto pointLight = GameObject::makePointLight(0.2f);
            pointLight.color = lightColors[i];
            auto rotateLight = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColors.size(),
                {0.f, -1.f, 0.f});
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        }
    }
}