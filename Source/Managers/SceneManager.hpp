#pragma once
#include "GameObject.hpp"

namespace VoidEngine
{
    class SceneManager
    {
    public:
        VOIDENGINE_API SceneManager();
        VOIDENGINE_API ~SceneManager();

        VOIDENGINE_API void AddGameObject(GameObject&& gameObject);
        VOIDENGINE_API void AddGameObject(unsigned int id, GameObject&& gameObject);
        VOIDENGINE_API static GameObject* FindGameObject(unsigned int id);

        // Singleton access
        static SceneManager& getInstance()
        {
            static SceneManager instance;
            return instance;
        }

    private:
        //GameObject root_; // The root gameobject
        std::unordered_map<unsigned int, GameObject> gameObjects_;
    };
} // VoidEngine