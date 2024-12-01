#pragma once

#include "Window.hpp"

// std lib headers
#include <string>
#include <vector>

namespace VoidEngine
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices
    {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
        bool isComplete() const { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

    class Device
    {
    public:
    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

        explicit Device(Window &window);
        ~Device();

        // Move constructor
        Device(Device&& other)
            : instance(other.instance),
            debugMessenger(other.debugMessenger),
            physicalDevice(other.physicalDevice),
            window(other.window),
            commandPool(other.getCommandPool()),
            device_(other.device_),
            surface_(other.surface_),
            graphicsQueue_(other.graphicsQueue_),
            presentQueue_(other.presentQueue_),
            properties(other.properties)
        {
            other.instance = VK_NULL_HANDLE;
            other.debugMessenger = VK_NULL_HANDLE;
            other.physicalDevice = VK_NULL_HANDLE;
            other.commandPool = VK_NULL_HANDLE;
            other.device_ = VK_NULL_HANDLE;
            other.surface_ = VK_NULL_HANDLE;
            other.graphicsQueue_ = VK_NULL_HANDLE;
            other.presentQueue_ = VK_NULL_HANDLE;
        }

        // Move assignment
        Device& operator=(Device&& other)
        {
            if (this == &other) return *this; // Handle self-assignment

            // Clean up current resources
            cleanup();

            // Move resources
            instance = other.instance;
            debugMessenger = other.debugMessenger;
            physicalDevice = other.physicalDevice;
            window = other.window;
            commandPool = other.commandPool;
            device_ = other.device_;
            surface_ = other.surface_;
            graphicsQueue_ = other.graphicsQueue_;
            presentQueue_ = other.presentQueue_;
            properties = other.properties;

            // Nullify moved-from object
            other.instance = VK_NULL_HANDLE;
            other.debugMessenger = VK_NULL_HANDLE;
            other.physicalDevice = VK_NULL_HANDLE;
            other.commandPool = VK_NULL_HANDLE;
            other.device_ = VK_NULL_HANDLE;
            other.surface_ = VK_NULL_HANDLE;
            other.graphicsQueue_ = VK_NULL_HANDLE;
            other.presentQueue_ = VK_NULL_HANDLE;

            return *this;
        }

        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;



        VkCommandPool getCommandPool() { return commandPool; }
        VkDevice &device() { return device_; }
        VkSurfaceKHR surface() { return surface_; }
        VkQueue graphicsQueue() { return graphicsQueue_; }
        VkQueue presentQueue() { return presentQueue_; }

        SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }
        VkFormat findSupportedFormat(
            const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        // Buffer Helper Functions
        void createBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer &buffer,
            VkDeviceMemory &bufferMemory);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void copyBufferToImage(
            VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        void createImageWithInfo(
            const VkImageCreateInfo &imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage &image,
            VkDeviceMemory &imageMemory);

        VkPhysicalDeviceProperties properties;

    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();

        void cleanup();

        // helper functions
        bool isDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char *> getRequiredExtensions();
        bool checkValidationLayerSupport();
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        void hasGflwRequiredInstanceExtensions();
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        Window &window;
        VkCommandPool commandPool;

        VkDevice device_;
        VkSurfaceKHR surface_;
        VkQueue graphicsQueue_;
        VkQueue presentQueue_;

        const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };
}