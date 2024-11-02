#include "PointLight.hpp"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

namespace VoidEngine
{
    struct SPointLightPushConstants
    {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };

    PointLight::PointLight(Device& _device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(_device)
    {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    PointLight::~PointLight()
    {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void PointLight::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SPointLightPushConstants);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout.");
        }
    }

    void PointLight::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Can't create point light before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        RenderPipeline::DefaultPipelineConfigInfo();

        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<RenderPipeline>(
            device,
            "Shaders/point_light.vert.spv",
            "Shaders/point_light.frag.spv",
            pipelineConfig);
    }

    void PointLight::update(FrameInfo& frameInfo, GlobalUbo& ubo)
    {
        auto rotateLight = glm::rotate(
            glm::mat4(1.f),
            frameInfo.frameTime,
            {0.f, -1.f, 0.f});
        int lightIndex = 0;
        for (auto& kv : frameInfo.gameObjects)
        {
            auto& obj = kv.second;
            if (obj.pointLight == nullptr) continue;

            assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum specified");

            obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.f));

            ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.f);
            ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
            lightIndex++;
        }
        ubo.numLights = lightIndex;
    }

    void PointLight::render(FrameInfo& frameInfo)
    {
        pipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);

        for (auto& kv : frameInfo.gameObjects)
        {
            auto& obj = kv.second;
            if (obj.pointLight == nullptr) continue;

            SPointLightPushConstants push{};
            push.position = glm::vec4(obj.transform.translation, 1.f);
            push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
            push.radius = obj.transform.scale.x;

            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SPointLightPushConstants),
                &push);

            vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
        }
    }
}