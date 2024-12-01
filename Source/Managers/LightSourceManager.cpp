#include "LightSourceManager.hpp"
#include "VoidEngine.hpp"

namespace VoidEngine {
    LightSourceManager::LightSourceManager(Game& game_) : game(game_)
    {
    }

    void LightSourceManager::AddPointLight(const Transform &transform, const glm::vec3 color, const float intensity, const float radius)
    {
        auto* pl = new PointLight(&game);
        pl->SetPointLight(intensity, radius, color);
        pl->transform = transform;
        pl->UpdateLight(*game.ubo);

        pointLights.push_back(*pl);
    }

    void LightSourceManager::UpdateLights() const
    {
        /*
        for (auto light : pointLights)
        {
            light.UpdateLight(*game.ubo);
        }
        */
    }
} // VoidEngine