#pragma once

#include "Model.hpp"

#include <gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace VoidEngine {

    struct TransformComponent
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    struct PointLightComponent
    {
        float lightIntensity = 1.0f;
    };

    class GameObject {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, GameObject>;

        static GameObject createGameObject()
        {
            static id_t currentId = 0;
            return GameObject{currentId++};
        }

        static GameObject makePointLight(
            float intensity = 10.0f,
            float radius = 0.1f,
            glm::vec3 color = glm::vec3(1.f));

        GameObject() = default;
        ~GameObject() = default;

        GameObject(const GameObject &) = delete;
        GameObject& operator=(const GameObject &) = delete;
        GameObject(GameObject&&) noexcept = default;
        GameObject& operator=(GameObject &&) noexcept = default;

        [[nodiscard("id should not be discarded")]] id_t getId() const { return id; }

        void AddChild(GameObject* child);

        TransformComponent transform;

        std::shared_ptr<Model> model;
        std::unique_ptr<PointLightComponent> pointLight = nullptr;
        glm::vec3 color;

    private:
        GameObject(id_t objId) : id{objId} {}
        id_t id;

        GameObject* parent_ = nullptr;
        std::vector<GameObject*> children_; // id's of children, managed by SceneManager.hpp
    };
}