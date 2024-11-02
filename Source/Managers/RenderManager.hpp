/// -------------------------------------------------------------------------------------------------------
/// The RenderManager class oversees frame rendering, manages render queues, and coordinates with pipelines for different passes.
/// Variables
///
/// RenderQueue renderQueue: Holds categorized game objects for different rendering needs.
/// std::vector<RenderPipeline> renderPipelines: Stores various render pipelines(e.g., forward pass, shadow pass).
/// VkDevice device: Vulkan device handle.
/// std::vector<VkSemaphore> imageAvailableSemaphores: Synchronization for when an image is ready for rendering.
/// std::vector<VkSemaphore> renderFinishedSemaphores: Synchronization for when rendering is complete.
/// std::vector<VkFence> inFlightFences: Frame synchronization.
/// Camera *camera: Reference to the active camera for view and projection matrices.
///
/// Functions
///
/// void Initialize(): Sets up Vulkan resources, render pipelines, and synchronization objects.
/// void BeginFrame(): Prepares for a new frame by acquiring the next image, setting up the command buffer, etc.
/// void SubmitFrame(): Submits the frame’s command buffer to the GPU, handles synchronization, and presents the image.
/// void RenderScene(): Main function to sort the render queue, set up each render pipeline, and execute the render commands.
/// void Cleanup(): Releases Vulkan resources and clears synchronization objects.
/// -------------------------------------------------------------------------------------------------------

#pragma once
#include <memory>
#include <optional>
#include <stdexcept>

#include "Camera.hpp"
#include "SwapChain.hpp"
#include "../Core/Device.hpp"
#include "../Components/GameObject.hpp"
#include "../Core/RenderPipeline.hpp"
#include "../Core/FrameInfo.hpp"

namespace VoidEngine
{
    enum class RenderQueueType
    {
        SKYBOX,
        OPAQUE,
        SHADOW,
        TRANSPARENT,
        REFLECTION,
        UI,
        POSTPROCESSING,
        DEBUG
    };

    struct RenderQueue
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        std::optional<Camera*> camera;
        VkDescriptorSet globalDescriptorSet;
        std::vector<unsigned int> gameObjectIDs;

        RenderQueue() = default;

        void AddToQueue(const GameObject& gameObject)
        {
            gameObjectIDs.push_back(gameObject.getId());
        }

        void begin(VkPipelineLayout layout);
    /*
        {
            {
                // Start command buffer recording
                VkCommandBufferBeginInfo beginInfo{};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // Use as appropriate

                if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
                    throw std::runtime_error("Failed to begin recording command buffer");
                }

                // Bind the camera's view/projection matrices if the camera is available
                if (camera) {
                    // Set up necessary camera parameters here, e.g., view/projection matrices
                }

                // Bind other resources (like global descriptor set)
                vkCmdBindDescriptorSets(
                    commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    layout, 0, 1,
                    &globalDescriptorSet, 0, nullptr
                );

                // Further setup before actual rendering
            }
        }*/

        void end();
    /*
        {
            // Finish recording the command buffer
            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer");
            }
        }*/
    };

    class RenderManager
    {
    public:
        VOIDENGINE_API RenderManager(Device& device_, VkDescriptorSetLayout globalSetLayout, VkExtent2D resolution);
        VOIDENGINE_API ~RenderManager();

        RenderManager(const RenderManager &) = delete;
        RenderManager &operator=(const RenderManager &) = delete;

        void renderGameObjects(FrameInfo& frameInfo);
        VOIDENGINE_API void RenderObjectsInQueue(const RenderQueueType& queueType);

        VOIDENGINE_API void AddToRenderQueue(const GameObject& gameObject, RenderQueueType queueType);

        VkRenderPass GetRenderPass() const;

        static VkFormat FindDepthFormat(Device& device);

        static  RenderManager& getInstance(Device& device_, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, VkExtent2D resolution)
        {
            static RenderManager instance(device_, globalSetLayout, resolution);
            return instance;
        }

    private:
        void createRenderPass();
        void createRenderPass(VkFormat imageFormat);
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        Device& device;

        std::unique_ptr<RenderPipeline> pipeline;
        VkPipelineLayout pipelineLayout{};

        std::unordered_map<RenderQueueType, RenderQueue> renderQueue;

        VkRenderPass renderPass{};
        std::unique_ptr<SwapChain> swapChain_{};
    };
}
