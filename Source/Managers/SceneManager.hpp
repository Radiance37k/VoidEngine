#pragma once
#include "RenderManager.hpp"

namespace VoidEngine
{
    class SceneManager
    {
    public:
        VOIDENGINE_API SceneManager() = default;
        VOIDENGINE_API ~SceneManager() = default;

        VOIDENGINE_API void AddGameObject(std::unique_ptr<GameObject> gameObject);

        VOIDENGINE_API GameObject* FindGameObject(unsigned int id);

        //std::unordered_map<unsigned int, std::unique_ptr<GameObject>> GetGameObjects() { return gameObjects_; }

        // Singleton access
        static SceneManager& getInstance()
        {
            static SceneManager instance;
            return instance;
        }

    private:
        std::unordered_map<unsigned int, std::unique_ptr<GameObject>> gameObjects_{};
    };
} // VoidEngine