//
// Created by Hrefna on 8.10.2024.
//

#include "SceneManager.hpp"

#include <iostream>

namespace VoidEngine {
    SceneManager::SceneManager()
    {

    }

    SceneManager::~SceneManager() = default;

    void SceneManager::AddGameObject(GameObject &&gameObject)
    {
        AddGameObject(gameObject.getId(), std::move(gameObject));
    }

    void SceneManager::AddGameObject(unsigned int id, GameObject &&gameObject)
    {
        if (gameObjects_.find(id) != gameObjects_.end()) {
            throw std::runtime_error("GameObject with the same ID already exists.");
        }
        //assert(gameObject != nullptr && "Invalid GameObject passed to AddGameObject.");

        try
        {
            gameObjects_.emplace(id, std::move(gameObject));
        } catch (const std::exception &e)
        {
            std::cerr << "Exception in SceneManager::AddGameObject: " << e.what() << "\n";
            throw;
        }
    }

    GameObject* SceneManager::FindGameObject(unsigned int id)
    {
        //auto& gameObjects = getInstance().gameObjects_;

        auto it = gameObjects_.find(id);
        return (it != gameObjects_.end()) ? &it->second : nullptr;
    }
} // VoidEngine