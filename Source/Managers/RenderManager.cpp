#include "RenderManager.hpp"
#include "RenderPipeline.hpp"
#include "SceneManager.hpp"
#include "VoidEngine.hpp"
#include "WindowManager.hpp"
#include "FrameInfo.hpp"
#include "GameObject.hpp"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

namespace VoidEngine
{
    struct SimplePushConstantData
    {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 normalMatrix{1.f};
    };

    void RenderQueue::AddToQueue(const GameObject &gameObject)
    {
        gameObjectIDs.push_back(gameObject.getId());
    }

    void RenderManager::createFrameBuffers(Device& device, SwapChain& swapChain, VkRenderPass pass)
    {
        auto imageCount = swapChain.GetSwapChainImages().size();
        framebuffers.resize(imageCount);
        for (size_t i = 0; i < imageCount; i++)
        {
            std::array<VkImageView, 2> attachments = {swapChain.GetImageView(i), swapChain.GetDepthImageView(i)};

            VkExtent2D swapChainExtent = swapChain.GetSwapChainExtent();
            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = pass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(
                device.device(),
                &framebufferInfo,
                nullptr,
                &framebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    RenderManager::RenderManager(Device& device_, Game& gameInstance, VkExtent2D resolution) : device(device_), game_(gameInstance)
    {
        /* Creation order:
         *
         * Render pass
         * Descriptor set layout
         * Pipeline layout
         * Pipeline
         * Framebuffer
         * Descriptor set allocation and update
         * Command buffer recording
         */

        renderQueue[RenderQueueType::OPAQUE] = std::make_unique<RenderQueue>();
        renderQueue[RenderQueueType::OPAQUE]->pipeline = std::make_unique<RenderPipeline>(device);

        renderQueue[RenderQueueType::LIGHT] = std::make_unique<RenderQueue>();
        renderQueue[RenderQueueType::LIGHT]->pipeline = std::make_unique<RenderPipeline>(device);

        createRenderPass(renderQueue[RenderQueueType::OPAQUE]->pipeline->configInfo.renderPass);
        createRenderPass(renderQueue[RenderQueueType::LIGHT]->pipeline->configInfo.renderPass);

        createDescriptorSetPool();
        std::unique_ptr<DescriptorSetLayout> globalSetLayout = DescriptorSetLayout::Builder(device)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                .build();

        createPipelineLayout(*renderQueue[RenderQueueType::OPAQUE], globalSetLayout->getDescriptorSetLayout());
        createPipelineLayout(*renderQueue[RenderQueueType::LIGHT], globalSetLayout->getDescriptorSetLayout());

        renderQueue[RenderQueueType::OPAQUE]->pipeline->CreateGraphicsPipeline("Shaders/Simple_shader.vert.spv", "Shaders/Simple_shader.frag.spv");
        renderQueue[RenderQueueType::LIGHT]->pipeline->CreateGraphicsPipeline("Shaders/Point_Light.vert.spv", "Shaders/Point_Light.frag.spv");

        swapChain_ = std::make_unique<SwapChain>(device_, resolution, FindDepthFormat(device_));

        createFrameBuffers(device, *swapChain_, renderQueue[RenderQueueType::OPAQUE]->pipeline->configInfo.renderPass);
        createFrameBuffers(device, *swapChain_, renderQueue[RenderQueueType::LIGHT]->pipeline->configInfo.renderPass);

        allocateDescriptorSet(globalSetLayout->getDescriptorSetLayout(), renderQueue[RenderQueueType::OPAQUE]->descriptorSet);
        allocateDescriptorSet(globalSetLayout->getDescriptorSetLayout(), renderQueue[RenderQueueType::LIGHT]->descriptorSet);

        allocateCommandBuffers(commandBuffer);
    }

    RenderManager::~RenderManager()
    {
        //vkDestroyRenderPass(device.device(), renderQueue[RenderQueueType::SKYBOX]->renderPass, nullptr);
        //vkDestroyRenderPass(device.device(), renderQueue[RenderQueueType::OPAQUE]->renderPass, nullptr);
        //vkDestroyRenderPass(device.device(), renderQueue[RenderQueueType::LIGHT]->renderPass, nullptr);
        vkDestroyRenderPass(device.device(), renderQueue[RenderQueueType::OPAQUE]->pipeline->configInfo.renderPass, nullptr);
        vkDestroyRenderPass(device.device(), renderQueue[RenderQueueType::LIGHT]->pipeline->configInfo.renderPass, nullptr);
        /*
        vkDestroyRenderPass(device.device(), renderQueue[RenderQueueType::SHADOW]->renderPass, nullptr);
        vkDestroyRenderPass(device.device(), renderQueue[RenderQueueType::TRANSPARENT]->renderPass, nullptr);
        vkDestroyRenderPass(device.device(), renderQueue[RenderQueueType::REFLECTION]->renderPass, nullptr);
        vkDestroyRenderPass(device.device(), renderQueue[RenderQueueType::UI]->renderPass, nullptr);
        vkDestroyRenderPass(device.device(), renderQueue[RenderQueueType::POSTPROCESSING]->renderPass, nullptr);
        vkDestroyRenderPass(device.device(), renderQueue[RenderQueueType::DEBUG]->renderPass, nullptr);
        */

        //vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void RenderManager::createPipelineLayout(RenderQueue& renderQueue, VkDescriptorSetLayout layout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{layout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &renderQueue.pipeline->configInfo.pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout.");
        }
    }

    void RenderManager::createSwapChain(VkFormat depthFormat, VkRenderPass renderPass, VkExtent2D extent)
    {
        while (extent.width == 0 || extent.height == 0)
        {
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(device.device());

        if (swapChain_ == nullptr)
        {
            swapChain_ = std::make_unique<SwapChain>(device, extent, depthFormat);
        } else
        {
            std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain_);
            swapChain_ = std::make_unique<SwapChain>(device, extent, oldSwapChain, depthFormat);

            if (!oldSwapChain->compareSwapFormats(*swapChain_))
            {
                throw std::runtime_error("Swap chain image(or depth) format has changed!");
            }
        }
    }

    void RenderManager::createDescriptorSetPool()
    {
        // Set up pool sizes for types of descriptors (e.g., uniform buffers)
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<int>(RenderQueueType::COUNT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<int>(RenderQueueType::COUNT);

        vkCreateDescriptorPool(device.device(), &poolInfo, nullptr, &descriptorPool);
    }

    void RenderManager::allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet& descriptorSet)
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        vkAllocateDescriptorSets(device.device(), &allocInfo, &descriptorSet);
    }

    void RenderManager::UpdateDescriptorSet(VkDescriptorSet destSet, const std::vector<std::unique_ptr<Buffer>>& uboBuffers, uint32_t currentFrame) const
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uboBuffers[currentFrame]->getBuffer();  // Your uniform buffer handle
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(GlobalUbo);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = destSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device.device(), 1, &descriptorWrite, 0, nullptr);
    }

    void RenderManager::RenderObjectsInQueue(const RenderQueue& queue, VkCommandBuffer cmdBuffer)
    {
        if (queue.pipeline == nullptr) return;

        //queue.pipeline->bind(commandBuffer);

        vkCmdBindDescriptorSets(
            cmdBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            queue.pipeline->configInfo.pipelineLayout,
            0,
            1,
            &queue.descriptorSet,
            0,
            nullptr);

        for (auto& id : queue.gameObjectIDs)
        {
            auto obj = game_.sceneManager->FindGameObject(id);
            if (obj->model == nullptr) continue;
            SimplePushConstantData push{};
            push.modelMatrix = obj->transform.mat4();
            push.normalMatrix = obj->transform.normalMatrix();

            vkCmdPushConstants(
                cmdBuffer,
                queue.pipeline->configInfo.pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);

            // LIGHT does not use any , that causes an exception, make it use 1 vertex at 0, 0, 0 for starters
            VkBuffer buffers[] = {obj->model->vertexBuffer->getBuffer()};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(cmdBuffer, 0, 1, buffers, offsets);

            if (obj->model->hasIndexBuffer)
            {
                vkCmdBindIndexBuffer(cmdBuffer, obj->model->indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
                vkCmdDrawIndexed(cmdBuffer, obj->model->indexCount, 1, 0, 0, 0);
            } else
            {
                vkCmdDraw(cmdBuffer, obj->model->vertexCount, 1, 0, 0);
            }
        }
    }

    void RenderManager::AddToRenderQueue(const GameObject& gameObject, RenderQueueType queueType)
    {
        renderQueue[queueType]->AddToQueue(gameObject);
    }

    VkFormat RenderManager::FindDepthFormat(Device& device)
    {
        return device.findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    void RenderManager::allocateCommandBuffers(VkCommandBuffer& commandBuffer)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;  // Or secondary, depending on your use case
        allocInfo.commandPool = device.getCommandPool();                // Replace with your actual command pool
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(device.device(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffer");
        }
    }

    void RenderManager::createRenderPass(VkRenderPass& renderPass, VkFormat imageFormat)
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

        dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

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