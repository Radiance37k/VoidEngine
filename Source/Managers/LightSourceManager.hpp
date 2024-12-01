#pragma once
#include "Common.hpp"
#include "PointLight.hpp"
#include "Transform.hpp"

namespace VoidEngine
{
    // Forward declerations
    class Game;

    class LightSourceManager
    {
    public:
        VOIDENGINE_API explicit LightSourceManager(Game& game_);
        ~LightSourceManager() = default;

        VOIDENGINE_API void AddPointLight(const Transform &transform, glm::vec3 color, float intensity = 1.0f, float radius = 0.1f);
        VOIDENGINE_API void UpdateLights() const;

        float intensity = 1.0f;
        float radius = 0.1f;
        glm::vec3 color = {1.0f, 1.0f, 1.0f};

    private:
        Game& game;

        std::vector<PointLight> pointLights;
    };
} // VoidEngine