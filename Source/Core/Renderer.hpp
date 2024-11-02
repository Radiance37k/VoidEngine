#pragma once

#include "../Core/Device.hpp"
#include "SwapChain.hpp"
#include "Window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

#include "RenderManager.hpp"

namespace VoidEngine {
    class Renderer {
    public:
        Renderer(Window &window, Device &device, VkFormat depthFormat, VkRenderPass renderPass);
        ~Renderer();

        Renderer(const Renderer &) = delete;
        //Renderer &operator=(const Renderer &) = delete;

        //VkRenderPass getSwapChainRenderPass() const { return RenderManager::GetRenderPass(); }
        float getAspectRatio() const { return swapChain->extentAspectRatio(); }
        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame(VkRenderPass renderPass);
        void endFrame(VkRenderPass renderPass);
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain(VkFormat depthFormat, VkRenderPass renderPass);

        Window &window;
        Device &device;
        std::unique_ptr<SwapChain> swapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted{false};
    };
}
