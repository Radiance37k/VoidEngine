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
        pl->UpdateLight(*game.ubo, pointLights.size());

        const std::vector<Model::Vertex> v =
            {
                {{-1.0f, 1.0f, 0.0f}, color, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                {{-1.0f, -1.0f, 0.0f}, color, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                {{1.0f, -1.0f, 0.0f}, color, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                {{1.0f, 1.0f, 0.0f}, color, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            };

        pl->model->AddVertex(v[0]);
        pl->model->AddVertex(v[1]);
        pl->model->AddVertex(v[2]);
        pl->model->CreateBuffers();

        game.AddGameObject(pl, RenderQueueType::LIGHT);

        pointLights.push_back(*pl);
        game.ubo->numLights++;
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