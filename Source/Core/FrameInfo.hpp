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
        glm::mat4 projection{1.0f};
        glm::mat4 view{1.0f};
        glm::mat4 inverseView{1.f};
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, 0.02f};
        SPointLight pointLights[MAX_LIGHTS];
        int numLights;
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
