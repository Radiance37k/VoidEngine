#pragma once
#include <memory>
#include <optional>

#include "Camera.hpp"
#include "SwapChain.hpp"
#include "../Core/Device.hpp"
#include "../Components/GameObject.hpp"
#include "../Core/RenderPipeline.hpp"

namespace VoidEngine
{
    // Forward declerations
    class Game;

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
        ~RenderQueue() = default;

        void AddToQueue(const GameObject& gameObject)
        {
            gameObjectIDs.push_back(gameObject.getId());
        }

        void SetDescriptor(VkDescriptorSet ds) { globalDescriptorSet = ds; }

        void renderQueueBegin(VkPipelineLayout layout);
        void renderQueueEnd();
    };

    class RenderManager
    {
    public:
        VOIDENGINE_API RenderManager(Device& device_, Game& gameInstance, VkExtent2D resolution);
        VOIDENGINE_API ~RenderManager();

        RenderManager(const RenderManager &) = delete;
        RenderManager &operator=(const RenderManager &) = delete;
        //RenderManager(RenderManager&&) noexcept = default;
        //RenderManager& operator=(RenderManager&&) noexcept = default;

        VOIDENGINE_API void RenderObjectsInQueue(const RenderQueueType& queueType, VkBuffer ubo);

        VOIDENGINE_API void AddToRenderQueue(const GameObject& gameObject, RenderQueueType queueType);

        VkRenderPass GetRenderPass() const;

        static VkFormat FindDepthFormat(Device& device);

        float GetAspectRatio() { return swapChain_->extentAspectRatio(); }
        SwapChain& GetSwapChain() { return *swapChain_; }
        VkCommandBuffer& GetQueueCommandBuffer(RenderQueueType queue) { return renderQueue[queue].commandBuffer; }
        void UpdateDescriptorSet(VkDescriptorSet destSet, VkBuffer uboBuffer);
        VkDescriptorSet GetGlobalDescriptorSet(RenderQueueType queue) { return renderQueue[queue].globalDescriptorSet; }
        RenderQueue& GetRenderQueue(RenderQueueType queue) { return renderQueue[queue]; }

    private:
        void allocateCommandBuffers(VkCommandBuffer& commandBuffer);
        void createRenderPass();
        void createRenderPass(VkFormat imageFormat);
        void createPipelineLayout(VkDescriptorSetLayout layout);
        void createPipeline(VkRenderPass renderPass);

        void createSwapChain(VkFormat depthFormat, VkRenderPass renderPass, VkExtent2D extent);

        void createDescriptorSetLayout();
        void createDescriptorSetPool();
        void allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet& decriptorSet);

        Game& game_;
        Device& device;

        std::unique_ptr<RenderPipeline> pipeline;
        VkPipelineLayout pipelineLayout{};

        VkDescriptorPool descriptorPool{};

        std::unordered_map<RenderQueueType, RenderQueue> renderQueue;

        VkRenderPass renderPass{};
        std::unique_ptr<SwapChain> swapChain_{};
    };
}
