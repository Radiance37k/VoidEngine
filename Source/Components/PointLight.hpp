#pragma once
#include "GameObject.hpp"
#include "FrameInfo.hpp"

namespace VoidEngine
{
    class GameObject;

    class PointLight : public GameObject
    {
    public:
        VOIDENGINE_API void SetPointLight(glm::vec3 c = glm::vec3(1.f)) { SetPointLight(10.0f, 0.1f, c); }
        VOIDENGINE_API void SetPointLight(
            float i = 10.0f,
            float r = 0.1f,
            glm::vec3 c = glm::vec3(1.f));

        VOIDENGINE_API explicit PointLight(Game* game);
        VOIDENGINE_API ~PointLight() override = default;

        //PointLight(PointLight&&) noexcept = default;
        //PointLight& operator=(PointLight&&) noexcept = default;

        //PointLight(const PointLight &) = delete;
        //PointLight& operator=(const PointLight &) = delete;

        VOIDENGINE_API void Update() override;

        void UpdateLight(GlobalUbo &ubo);
        void render(FrameInfo& frameInfo);

    private:
        float intensity = 1.0f;
        float radius = 0.1f;
        glm::vec3 color = glm::vec3(1.f);
    };
}