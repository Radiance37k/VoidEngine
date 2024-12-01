#include "SceneManager.hpp"
#include "GameObject.hpp"

#include <iostream>

namespace VoidEngine {
    void SceneManager::AddGameObject(std::unique_ptr<GameObject> gameObject)
    {
        assert(gameObject != nullptr && "Invalid GameObject passed to AddGameObject.");

        auto id = gameObject->getId();

        if (gameObjects_.contains(id))
        {
            throw std::runtime_error("GameObject with the same ID already exists.");
        }

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
        auto it = gameObjects_.find(id);
        return it != gameObjects_.end() ? it->second.get() : nullptr;
    }
} // VoidEngine