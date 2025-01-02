#pragma once
#include <complex.h>
#include <memory>
#include <optional>

#include "Buffer.hpp"
#include "Camera.hpp"
#include "SwapChain.hpp"
#include "../Core/Device.hpp"
#include "../Core/RenderPipeline.hpp"
#include "Common.hpp"

namespace VoidEngine
{
    // Forward declerations
    class Game;
    class GameObject;

    /*
    enum class RenderQueueType
    {
        SKYBOX,
        OPAQUE,
        LIGHT,
        SHADOW,
        TRANSPARENT,
        REFLECTION,
        UI,
        POSTPROCESSING,
        DEBUG,
        COUNT
    };
    */

    enum class RenderQueueType
    {
        OPAQUE,
        LIGHT,
        COUNT
    };

    struct RenderQueue
    {
        RenderQueue() = default;
        ~RenderQueue() = default;

        Camera* camera = nullptr;
        std::vector<unsigned int> gameObjectIDs;

        VkDescriptorSet descriptorSet;

        std::unique_ptr<RenderPipeline> pipeline;

        void AddToQueue(const GameObject& gameObject);

        unsigned int GetNumObjects() const { return gameObjectIDs.size(); }
        void SetDescriptor(VkDescriptorSet ds) { descriptorSet = ds; }
    };

    class RenderManager
    {
    public:
        void createFrameBuffers(Device &device, SwapChain &swapChain, VkRenderPass pass);

        VOIDENGINE_API RenderManager(Device& device_, Game& gameInstance, VkExtent2D resolution);
        VOIDENGINE_API ~RenderManager();

        RenderManager(const RenderManager &) = delete;
        RenderManager &operator=(const RenderManager &) = delete;
        //RenderManager(RenderManager&&) noexcept = default;
        //RenderManager& operator=(RenderManager&&) noexcept = default;

        VOIDENGINE_API void RenderObjectsInQueue(const RenderQueue& queue, VkCommandBuffer cmdBuffer);
        VOIDENGINE_API void AddToRenderQueue(const GameObject& gameObject, RenderQueueType queueType);

        static VkFormat FindDepthFormat(Device& device);

        float GetAspectRatio() const { return swapChain_->extentAspectRatio(); }
        SwapChain& GetSwapChain() const { return *swapChain_; }
        VkCommandBuffer& GetQueueCommandBuffer(RenderQueueType queue) { return commandBuffer; }
        void UpdateDescriptorSet(VkDescriptorSet destSet, const std::vector<std::unique_ptr<Buffer>> &uboBuffers, uint32_t currentFrame) const;
        VkDescriptorSet GetDescriptorSet(RenderQueueType queue) { return renderQueue[queue]->descriptorSet; }
        RenderQueue& GetRenderQueue(const RenderQueueType queue) const { return *renderQueue.at(queue); }
        std::vector<VkFramebuffer>& GetFramebuffers() { return framebuffers; }

    private:
        void allocateCommandBuffers(VkCommandBuffer& commandBuffer);
        void createRenderPass(VkRenderPass& renderPass, VkFormat imageFormat = VK_FORMAT_B8G8R8A8_UNORM);
        void createPipelineLayout(RenderQueue& renderQueue, VkDescriptorSetLayout layout);
        void createSwapChain(VkFormat depthFormat, VkRenderPass renderPass, VkExtent2D extent);
        void createDescriptorSetPool();
        void allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet& decriptorSet);

        Game& game_;
        Device& device;

        std::vector<VkFramebuffer> framebuffers;

        VkDescriptorPool descriptorPool{};

        std::unordered_map<RenderQueueType, std::unique_ptr<RenderQueue>> renderQueue{};

        std::unique_ptr<SwapChain> swapChain_{};
        VkCommandBuffer commandBuffer;
    };
}
