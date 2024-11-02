//
// Created by Hrefna on 8.10.2024.
//

#include "SceneManager.hpp"

namespace VoidEngine {
    SceneManager::SceneManager()
    {

    }

    SceneManager::~SceneManager() = default;

    void SceneManager::AddGameObject(GameObject&& gameObject)
    {
        AddGameObject(gameObject.getId(), std::move(gameObject));
    }

    void SceneManager::AddGameObject(unsigned int id, GameObject&& gameObject)
    {
        gameObjects_.emplace(gameObject.getId(), std::move(gameObject));

    }

    GameObject* SceneManager::FindGameObject(unsigned int id)
    {
        auto& gameObjects = getInstance().gameObjects_;
        auto it = gameObjects.find(id);
        return (it != gameObjects.end()) ? &it->second : nullptr;
    }
} // VoidEngine