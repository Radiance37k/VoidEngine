#pragma once
#include <memory>

#include "Camera.hpp"
#include "../Core/Device.hpp"
#include "GameObject.hpp"
#include "PointLight.hpp"
#include "../Core/FrameInfo.hpp"
#include "../Core/RenderPipeline.hpp"

namespace VoidEngine
{
    class PointLight
    {
    public:
        PointLight(Device& _device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLight();

        PointLight(const PointLight &) = delete;
        PointLight &operator=(const PointLight &) = delete;

        void update(FrameInfo& frameInfo, GlobalUbo& ubo);
        void render(FrameInfo& frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        Device& device;

        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
    };

}