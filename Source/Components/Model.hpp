#pragma once

#include "Common.hpp"
#include "Device.hpp"
#include "Buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <iostream>
#include <External/glm/glm.hpp>

#include <vector>
#include <memory>

namespace VoidEngine
{
    class Model
    {
    public:
        struct Vertex
        {
            glm::vec3 position{0.0f, 0.0f, 0.0f};
            glm::vec3 color{0.0f, 0.0f, 0.0f};
            glm::vec3 normal{0.0f, 0.0f, 0.0f};
            glm::vec2 uv{0.0f, 0.0f};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex &other) const
            {
                return ((position == other.position) && (color == other.color) && (normal == other.normal) && (uv == other.uv));
            }
        };

        VOIDENGINE_API explicit Model(Device& _device);
        VOIDENGINE_API ~Model();

        //void bind(VkCommandBuffer commandBuffer) const;
        void draw(VkCommandBuffer commandBuffer) const;

        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        bool hasIndexBuffer = false;
        uint32_t vertexCount;
        uint32_t indexCount;

        VOIDENGINE_API void LoadModelFromFile(const std::string &filepath);
        void AddVertex(const Vertex &v);

        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};
        void CreateBuffers();

    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);

        Device& device;
    };
}