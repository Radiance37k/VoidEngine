#pragma once

#include "Device.hpp"
#include "SwapChain.hpp"
#include "Window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

#include "RenderManager.hpp"

namespace VoidEngine
{
    class Renderer
    {
    public:
        Renderer(Window &window, Device &device, VkFormat depthFormat);//, VkRenderPass renderPass);
        ~Renderer();

        Renderer(const Renderer &) = delete;
        //Renderer &operator=(const Renderer &) = delete;

        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const
        {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const
        {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame(SwapChain &swapChain);
        void endFrame(SwapChain &swapChain, VkCommandBuffer &commandBuffer);
        void beginSwapChainRenderPass(VkCommandBuffer &commandBuffer, VkRenderPass renderPass, SwapChain &swapChain, std::vector<VkFramebuffer> framebuffers);
        void endSwapChainRenderPass(VkCommandBuffer& commandBuffer);

        uint32_t GetCurrentImageIndex() const { return currentImageIndex; }

    private:
        void createCommandBuffers();
        void freeCommandBuffers();

        Window &window;
        Device &device;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex{0};
        bool isFrameStarted{false};
    };
}
