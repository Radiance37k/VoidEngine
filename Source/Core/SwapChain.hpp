#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Device.hpp"

namespace VoidEngine
{
    class SwapChain
    {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        SwapChain(Device &deviceRef, VkExtent2D extent, VkFormat depthFormat, VkRenderPass renderPass);
        SwapChain(Device& deviceRef, VkExtent2D extent, std::shared_ptr<SwapChain> previous, VkFormat depthFormat, VkRenderPass renderPass);
        ~SwapChain();

        VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
        //VkRenderPass getRenderPass() { return renderPass; }
        VkImageView getImageView(int index) { return swapChainImageViews[index]; }
        size_t imageCount() { return swapChainImages.size(); }
        VkFormat getSwapChainImageFormat();
        VkExtent2D getSwapChainExtent() { return swapChainExtent; }
        uint32_t width() { return swapChainExtent.width; }
        uint32_t height() { return swapChainExtent.height; }

        float extentAspectRatio()
        {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }

        VkResult acquireNextImage(uint32_t *imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

        bool compareSwapFormats(const SwapChain swapChain) const
        {
            return swapChain.swapChainDepthFormat == swapChainDepthFormat &&
                swapChain.swapChainImageFormat == swapChainImageFormat;
        }

        void createSwapChain();

        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;

    private:
        void init(VkFormat depthFormat, VkRenderPass renderPass);
        void createImageViews();
        void createDepthResources(VkFormat depthFormat);
        void createFramebuffers(VkRenderPass renderPass);
        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        VkFormat swapChainImageFormat;
        VkFormat swapChainDepthFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkFramebuffer> swapChainFramebuffers;
        //VkRenderPass renderPass;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemorys;
        std::vector<VkImageView> depthImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        Device &device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;
        std::shared_ptr<SwapChain> oldSwapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        size_t currentFrame = 0;
    };
} // VoidEngine