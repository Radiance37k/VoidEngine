#include "RenderManager.hpp"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <fstream>
#include <glm.hpp>
#include <iostream>
#include <gtc/constants.hpp>
#include "RenderPipeline.hpp"
#include "SceneManager.hpp"
#include "VoidEngine.hpp"
#include "WindowManager.hpp"

namespace VoidEngine
{
    struct SimplePushConstantData
    {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 normalMatrix{1.f};
    };

    void RenderQueue::begin(VkPipelineLayout layout)
    {
        std::cerr << "Muahahahaha\n";
    }

    void RenderQueue::end()
    {
        std::cerr << "Oh noes\n";
    }

    RenderManager::RenderManager(Device& device_, VkExtent2D resolution) : device(device_)
    {
        renderQueue[RenderQueueType::SKYBOX] = RenderQueue();
        renderQueue[RenderQueueType::OPAQUE] = RenderQueue();
        renderQueue[RenderQueueType::SHADOW] = RenderQueue();
        renderQueue[RenderQueueType::TRANSPARENT] = RenderQueue();
        renderQueue[RenderQueueType::REFLECTION] = RenderQueue();
        renderQueue[RenderQueueType::UI] = RenderQueue();
        renderQueue[RenderQueueType::POSTPROCESSING] = RenderQueue();
        renderQueue[RenderQueueType::DEBUG] = RenderQueue();

        createRenderPass();
        createPipelineLayout();
        createPipeline(renderPass);

        //swapChain_ = new SwapChain(device_, resolution, FindDepthFormat(device_), renderPass);
        swapChain_ = std::make_unique<SwapChain>(device_, resolution, FindDepthFormat(device_), renderPass);
    }

    RenderManager::~RenderManager()
    {
        vkDestroyRenderPass(device.device(), renderPass, nullptr);
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void RenderManager::createPipelineLayout()
    {
        if (globalSetLayout == nullptr)
        {
            globalSetLayout = DescriptorSetLayout::Builder(device)
                        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                        .build()->getDescriptorSetLayout();
        }

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

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

    void RenderManager::createPipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Can't create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        pipelineConfig = RenderPipeline::DefaultPipelineConfigInfo();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;

        pipeline = std::make_unique<RenderPipeline>(
            device,
            "Shaders/Simple_shader.vert.spv",
            "Shaders/Simple_shader.frag.spv",
            pipelineConfig);
    }

    void RenderManager::renderGameObjects(FrameInfo& frameInfo)
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
            if (obj.model == nullptr) continue;
            SimplePushConstantData push{};
            push.modelMatrix = obj.transform.mat4();
            push.normalMatrix = obj.transform.normalMatrix();

            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);

            obj.model->bind(frameInfo.commandBuffer);
            obj.model->draw(frameInfo.commandBuffer);
        }
    }

    void RenderManager::RenderObjectsInQueue(const RenderQueueType& queueType)
    {
        if (pipeline == nullptr) return;

        pipeline->bind(renderQueue[queueType].commandBuffer);

        vkCmdBindDescriptorSets(
            renderQueue[queueType].commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &renderQueue[queueType].globalDescriptorSet,
            0,
            nullptr);

        auto q = renderQueue.find(queueType);
        //for (auto& kv : renderQueue[queueType])
        if (q != renderQueue.end())
        {
            //for (auto& kv : q->second)
            for (auto& id : q->second.gameObjectIDs)
            {
                auto obj = SceneManager::FindGameObject(id);
                if (obj->model == nullptr) continue;
                SimplePushConstantData push{};
                push.modelMatrix = obj->transform.mat4();
                push.normalMatrix = obj->transform.normalMatrix();

                vkCmdPushConstants(
                    renderQueue[queueType].commandBuffer,
                    pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(SimplePushConstantData),
                    &push);

                obj->model->bind(renderQueue[queueType].commandBuffer);
                obj->model->draw(renderQueue[queueType].commandBuffer);
            }
        }
    }

    void RenderManager::AddToRenderQueue(const GameObject& gameObject, RenderQueueType queueType)
    {
        renderQueue[queueType].AddToQueue(gameObject);
    }

    VkRenderPass RenderManager::GetRenderPass() const
    {
        return renderPass;
    }

    VkFormat RenderManager::FindDepthFormat(Device& device)
    {
        return device.findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    void RenderManager::createRenderPass()
    {
        createRenderPass(VK_FORMAT_B8G8R8A8_UNORM);
    }

    void RenderManager::createRenderPass(VkFormat imageFormat)
    {
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = FindDepthFormat(device);
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = imageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstSubpass = 0;
        dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(device.device(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }
}
