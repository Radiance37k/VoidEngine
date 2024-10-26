#pragma once
#include <memory>

#include "Camera.hpp"
#include "../Core/Device.hpp"
#include "../Components/GameObject.hpp"
#include "../Core/RenderPipeline.hpp"
#include "../Core/FrameInfo.hpp"

namespace VoidEngine
{
    class SimpleRenderSystem
    {
    public:
        SimpleRenderSystem(Device& _device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void renderGameObjects(FrameInfo& frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        Device& device;

        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
    };
}