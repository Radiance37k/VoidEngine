#include "RenderManager.hpp"
#include "RenderPipeline.hpp"
#include "SceneManager.hpp"
#include "VoidEngine.hpp"
#include "WindowManager.hpp"

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

    void RenderQueue::renderQueueBegin(VkPipelineLayout layout)
    {
        {
            // Start command buffer recording
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // Use as appropriate

            if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording command buffer");
            }

            // Bind the camera's view/projection matrices if the camera is available
            if (camera) {
                // Set up necessary camera parameters here, e.g., view/projection matrices
            }

            // Bind other resources (like global descriptor set)
            vkCmdBindDescriptorSets(
                commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                layout, 0, 1,
                &globalDescriptorSet, 0, nullptr
            );

            // Further setup before actual rendering
        }
    }

    void RenderQueue::renderQueueEnd()
    {
        // Finish recording the command buffer
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer");
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
        renderQueue[RenderQueueType::SKYBOX] = RenderQueue();
        renderQueue[RenderQueueType::OPAQUE] = RenderQueue();
        renderQueue[RenderQueueType::SHADOW] = RenderQueue();
        renderQueue[RenderQueueType::TRANSPARENT] = RenderQueue();
        renderQueue[RenderQueueType::REFLECTION] = RenderQueue();
        renderQueue[RenderQueueType::UI] = RenderQueue();
        renderQueue[RenderQueueType::POSTPROCESSING] = RenderQueue();
        renderQueue[RenderQueueType::DEBUG] = RenderQueue();

        createRenderPass();

        std::unique_ptr<DescriptorSetLayout> globalSetLayout = DescriptorSetLayout::Builder(device)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                .build();

        createPipelineLayout(globalSetLayout->getDescriptorSetLayout());
        createPipeline(renderPass);
        // Todo: Create framebuffer
        createDescriptorSetPool();
        allocateDescriptorSet(globalSetLayout->getDescriptorSetLayout(), renderQueue[RenderQueueType::OPAQUE].globalDescriptorSet);

        allocateCommandBuffers(renderQueue[RenderQueueType::SKYBOX].commandBuffer);
        allocateCommandBuffers(renderQueue[RenderQueueType::OPAQUE].commandBuffer);
        allocateCommandBuffers(renderQueue[RenderQueueType::SHADOW].commandBuffer);
        allocateCommandBuffers(renderQueue[RenderQueueType::TRANSPARENT].commandBuffer);
        allocateCommandBuffers(renderQueue[RenderQueueType::REFLECTION].commandBuffer);
        allocateCommandBuffers(renderQueue[RenderQueueType::UI].commandBuffer);
        allocateCommandBuffers(renderQueue[RenderQueueType::POSTPROCESSING].commandBuffer);
        allocateCommandBuffers(renderQueue[RenderQueueType::DEBUG].commandBuffer);

        swapChain_ = std::make_unique<SwapChain>(device_, resolution, FindDepthFormat(device_), renderPass);
    }

    RenderManager::~RenderManager()
    {
        vkDestroyRenderPass(device.device(), renderPass, nullptr);
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void RenderManager::createPipelineLayout(VkDescriptorSetLayout layout)
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

    void RenderManager::createSwapChain(VkFormat depthFormat, VkRenderPass renderPass, VkExtent2D extent)
    {
        while (extent.width == 0 || extent.height == 0)
        {
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(device.device());

        if (swapChain_ == nullptr)
        {
            swapChain_ = std::make_unique<SwapChain>(device, extent, depthFormat, renderPass);
        } else
        {
            std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain_);
            swapChain_ = std::make_unique<SwapChain>(device, extent, oldSwapChain, depthFormat, renderPass);

            if (!oldSwapChain->compareSwapFormats(*swapChain_.get()))
            {
                throw std::runtime_error("Swap chain image(or depth) format has changed!");
            }
        }
    }

    void RenderManager::createDescriptorSetLayout()
    {
    }

    void RenderManager::createDescriptorSetPool()
    {
        // Set up pool sizes for types of descriptors (e.g., uniform buffers)
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = 1;

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

    void RenderManager::UpdateDescriptorSet(VkDescriptorSet destSet, VkBuffer uboBuffer)
    {
        /*
        void* data;
        vkMapMemory(device.device(), uboMemory, 0, sizeof(GlobalUbo), 0, &data);
        memcpy(data, &game_., sizeof(GlobalUbo));
        vkUnmapMemory(device.device(), uboMemory);
        */

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uboBuffer;  // Your uniform buffer handle
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

    void RenderManager::RenderObjectsInQueue(const RenderQueueType& queueType, VkBuffer ubo)
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
                auto obj = game_.sceneManager->FindGameObject(id);
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

        //renderQueue[queueType].renderQueueEnd();
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
