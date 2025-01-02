#include "VoidEngine.hpp"

#include "InputManager.hpp"
#include "Camera.hpp"
#include "RenderManager.hpp"
#include "Core/Buffer.hpp"
#include "PointLight.hpp"
#include "GameObject.hpp"

#include <chrono>
#include <vector>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#ifdef DEBUG
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "External/glm/glm.hpp"

#ifdef DEBUG
// For glm::to_string
#include "External/glm/gtx/string_cast.hpp"
#endif

#define DEBUG_PROJECTION

namespace VoidEngine
{
    Game::Game(VkExtent2D resolution)
    {
        //renderManager = std::make_unique<RenderManager>(*device, *this, resolution);
        renderManager = new RenderManager(*device, *this, resolution);
        sceneManager = std::make_unique<SceneManager>();
        lightSourceManager = std::make_unique<LightSourceManager>(*this);
        //lightSourceManager = new LightSourceManager(*this);

        globalPool = DescriptorPool::Builder(*device)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        renderer = new Renderer(*window, *device, RenderManager::FindDepthFormat(*device));

        ubo = std::make_unique<GlobalUbo>();
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
            //uboBuffers[i]->map();
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

        auto viewerObject = new GameObject(this);
        viewerObject->transform.translation.z = -2.5f;
        InputManager cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        float timer = 0;

        while (!window->shouldClose())
        {
            glfwPollEvents();

            /*
            for (auto& [id, gameObject] : sceneManager->FindGameObject())
            {
                gameObject->Update();
            }
            */

            auto newTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            //float timer = (timer + deltaTime >= 5) ? 0 : timer + deltaTime;
            timer += deltaTime;

            cameraController.moveInPlaneXZ(window->getGLFWwindow(), deltaTime, *viewerObject);
            mainCamera->setViewYXZ(viewerObject->transform.translation, viewerObject->transform.rotation);

            float aspect = renderManager->GetAspectRatio();
            mainCamera->setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

            // vkBeginCommandBuffer
            if (auto commandBuffer = renderer->beginFrame(renderManager->GetSwapChain()); commandBuffer != VK_NULL_HANDLE)
            {
                const int frameIndex = renderer->getFrameIndex();

                ubo->projection = mainCamera->getProjection();
                ubo->view = mainCamera->getView();
                ubo->inverseView = mainCamera->getInverseView();

#ifdef DEBUG_PROJECTION
                if (timer >= 1)
                {
                    glm::mat4 projection = glm::perspective(glm::radians(45.0f), renderManager->GetAspectRatio(), 0.1f, 100.0f);
                    std::cerr << "Projection Matrix (before upload):\n" << glm::to_string(projection) << std::endl;
                }
#endif

                uboBuffers[frameIndex]->map(sizeof(ubo));
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

#ifdef DEBUG_PROJECTION
                if (timer >= 1)
                {
                    std::cerr << "UBO Projection Matrix (after upload):\n";
                    std::cerr << glm::to_string(ubo->projection) << std::endl;

                    timer = 0;
                }
#endif

                //for (int i = 0; i < static_cast<int>(RenderQueueType::COUNT); i++)
                {
                    auto queueToRender = RenderQueueType::OPAQUE; //static_cast<RenderQueueType>(i);
                    auto& queue = renderManager->GetRenderQueue(queueToRender);

                    if ((queueToRender == RenderQueueType::LIGHT) && (queue.GetNumObjects() > 0))
                    {
                        for (int j = 0; j < renderManager->GetRenderQueue(queueToRender).gameObjectIDs.size(); j++)
                        {
                            auto id = renderManager->GetRenderQueue(RenderQueueType::LIGHT).gameObjectIDs[j];
                            auto light = sceneManager->FindGameObject(id)->GetAs<PointLight>();
                            light->UpdateLight(*ubo, j);
                        }
                    }

                    if (queue.GetNumObjects() > 0)
                    {
                        // vkCmdBeginRenderPass
                        renderer->beginSwapChainRenderPass(commandBuffer,
                            queue.pipeline->configInfo.renderPass,
                            renderManager->GetSwapChain(),
                            renderManager->GetFramebuffers());

                        renderManager->UpdateDescriptorSet(queue.descriptorSet, uboBuffers, renderManager->GetSwapChain().GetCurrentFrame());
                        queue.pipeline->bind(commandBuffer);
                        renderManager->RenderObjectsInQueue(queue, commandBuffer);

                        //vkCmdEndRenderPass
                        renderer->endSwapChainRenderPass(commandBuffer);
                    }
                }
                // vkEndCommandBuffer
                renderer->endFrame(renderManager->GetSwapChain(), commandBuffer);
            }
        }

        vkDeviceWaitIdle(device->device());
    }

    //template <typename T, typename... Args>
    //T* Game::AddGameObject(RenderQueueType renderQueue, Args&&... args)
    void Game::AddGameObject(GameObject* gameObject, RenderQueueType renderQueue) const
    {/*
        static_assert(std::is_base_of<GameObject, T>::value, "T must derive from GameObject");

        auto gameObject = std::make_unique<T>(std::forward<Args>(args)...);
        T* rawPtr = gameObject.get();
*/
        std::unique_ptr<GameObject>ugo(gameObject);
        renderManager->AddToRenderQueue(*gameObject, renderQueue);
        sceneManager->AddGameObject(std::move(ugo));

//        return rawPtr;
    }
}
