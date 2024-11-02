#pragma once
#include <string>
#include <vector>
#include "Device.hpp"

namespace VoidEngine
{
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
        RenderPipeline(
            Device& device,
            const std::string& vertFilepath,
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);
        ~RenderPipeline();

        RenderPipeline(const RenderPipeline&) = delete;
        RenderPipeline &operator=(const RenderPipeline&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        static PipelineConfigInfo DefaultPipelineConfigInfo();

    private:
        static std::vector<char> readFile(const std::string& filepath);

        void createGraphicsPipeline(
            const std::string& vertFilepath,
            const std::string& fragFilepath,
            PipelineConfigInfo configInfo);

        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        Device& device;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}
