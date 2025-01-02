#pragma once
#include "Camera.hpp"

namespace VoidEngine
{
#define MAX_LIGHTS 10

    struct SPointLight
    {
        glm::vec4 position; // Ignore w
        glm::vec4 color; // w is intensity
    };

    struct GlobalUbo
    {
        alignas(16) glm::mat4 projection{1.0f};
        alignas(16) glm::mat4 view{1.0f};
        alignas(16) glm::mat4 inverseView{1.f};
        alignas(16) glm::vec4 ambientLightColor{1.f, 1.f, 1.f, 0.02f};
        alignas(16) SPointLight pointLights[MAX_LIGHTS];
        alignas(4) int numLights = 0;
    };

    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera &camera;
        VkDescriptorSet globalDescriptorSet;
        std::unordered_map<unsigned int, GameObject> &gameObjects;
    };
}
