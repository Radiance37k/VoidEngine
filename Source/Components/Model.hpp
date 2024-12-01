#pragma once
#include "Common.hpp"
#include "Device.hpp"
#include "Buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <External/glm/glm.hpp>

#include <vector>
#include <memory>

namespace VoidEngine {

    class Model {
    public:
        struct Vertex
        {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex &other) const
            {
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
            }
        };

        //VOIDENGINE_API Model(Device& _device, const Model::Builder &builder);
        VOIDENGINE_API explicit Model(Device& _device);
        VOIDENGINE_API virtual ~Model();

        //Model(const Model &) = delete;
        //Model &operator=(const Model &) = delete;

        //void CreateBuffers();

        //VOIDENGINE_API void LoadModelFromFile(const std::string &filepath);
        //VOIDENGINE_API static std::unique_ptr<Model> createModelFromFile(Device& device, const std::string& filepath);

        void bind(VkCommandBuffer commandBuffer) const;
        void draw(VkCommandBuffer commandBuffer) const;

        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        bool hasIndexBuffer = false;
        uint32_t vertexCount;
        uint32_t indexCount;

        //void SetVertices(const std::vector<Vertex> &v) { vertices = v; }
        void Clear();
        uint32_t NumVertices() const { return vertices.size(); }
        void AddVertex(Vertex vertex) { vertices.push_back(vertex); }
        void AddIndex(uint32_t index) { indices.push_back(index); }

        VOIDENGINE_API void LoadModelFromFile(const std::string &filepath);

        void CreateBuffers();

    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);

        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};

        Device& device;
    };

}