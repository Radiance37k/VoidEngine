#include "Renderer.hpp"

// std
#include <array>
#include <cassert>
#include <stdexcept>

#include "RenderManager.hpp"

namespace VoidEngine
{
    Renderer::Renderer(Window& window, Device& device, VkFormat depthFormat, VkRenderPass renderPass)
    : window{window}, device{device}
    {
        //recreateSwapChain(depthFormat, renderPass);
        createCommandBuffers();
    }

    Renderer::~Renderer() { freeCommandBuffers(); }

    void Renderer::createCommandBuffers()
    {
        commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void Renderer::freeCommandBuffers()
    {
        vkFreeCommandBuffers(
        device.device(),
        device.getCommandPool(),
        static_cast<uint32_t>(commandBuffers.size()),
        commandBuffers.data());
        commandBuffers.clear();
    }

    bool Renderer::beginFrame(VkCommandBuffer& commandBuffer, VkRenderPass renderPass, SwapChain& swapChain)
    {
        assert(!isFrameStarted && "Can't call beginFrame while already in progress");

        auto result = swapChain.acquireNextImage(&currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            //recreateSwapChain(RenderManager::FindDepthFormat(device), renderPass);
            return false;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        isFrameStarted = true;

        //auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        //vkWaitForFences(device.device(), 1, &swapChain.inFlightFences[currentImageIndex], VK_TRUE, UINT64_MAX);
        //vkWaitForFences(device.device(), 1, &swapChain.imagesInFlight[currentImageIndex], VK_TRUE, UINT64_MAX);
        //vkResetFences(device.device(), 1, &swapChain.inFlightFences[currentImageIndex]);

        // Todo: Do a more elegant wait here
        vkDeviceWaitIdle(device.device());

        //vkResetCommandBuffer(commandBuffer, 0);

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        return true;
    }

    void Renderer::endFrame(VkRenderPass renderPass, SwapChain& swapChain, VkCommandBuffer& commandBuffer)
    {
        assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
        //auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }

        auto result = swapChain.submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        window.wasWindowResized())
        {
            window.resetWindowResizedFlag();
            //recreateSwapChain(RenderManager::FindDepthFormat(device), renderPass);
        } else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::beginSwapChainRenderPass(VkCommandBuffer& commandBuffer, VkRenderPass renderPass, SwapChain& swapChain)
    {
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
        /*
        assert(
            commandBuffer == getCurrentCommandBuffer() &&
            "Can't begin render pass on command buffer from a different frame");
        */

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;//swapChain->getRenderPass();
        renderPassInfo.framebuffer = swapChain.getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain.getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain.getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, swapChain.getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::endSwapChainRenderPass(VkCommandBuffer& commandBuffer)
    {
        assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        /*
        assert(
            commandBuffer == getCurrentCommandBuffer() &&
            "Can't end render pass on command buffer from a different frame");
        */
        vkCmdEndRenderPass(commandBuffer);
    }
}
