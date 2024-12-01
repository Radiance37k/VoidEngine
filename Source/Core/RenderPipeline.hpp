#pragma once
#include <string>
#include <vector>
#include "Device.hpp"

namespace VoidEngine
{
    struct RenderQueue;

    struct PipelineConfigInfo {
        //PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        //PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkViewport viewport{};
        VkRect2D scissor{};

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        std::vector<VkDynamicState> dynamicStates;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        uint32_t subpass = 0;
    };

    class RenderPipeline
    {
    public:
        PipelineConfigInfo configInfo;

        RenderPipeline(Device& device_);
        ~RenderPipeline();

        RenderPipeline(const RenderPipeline&) = delete;
        RenderPipeline &operator=(const RenderPipeline&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void CreateGraphicsPipeline(
            const std::string& vertFilepath,
            const std::string& fragFilepath);

    private:
        static std::vector<char> readFile(const std::string& filepath);

        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
        void SetDefaultPipelineConfigInfo();

        Device& device;
        VkPipeline graphicsPipeline{};
        VkShaderModule vertShaderModule{};
        VkShaderModule fragShaderModule{};
    };
}