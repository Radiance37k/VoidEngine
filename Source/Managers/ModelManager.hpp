#pragma once
#include "Common.hpp"
#include "Model.hpp"

namespace VoidEngine {
    class Game;

    // Forward decleration
    class GameObject;

    class ModelManager {
    public:
        VOIDENGINE_API explicit ModelManager(Game* game);
        VOIDENGINE_API ~ModelManager();

        //void CreateBuffers();

        //VOIDENGINE_API void LoadModelFromFile(const std::string &filepathfilepath, GameObject& object);
        VOIDENGINE_API Model& GetModel(const std::string &modelPathName) { return *models[modelPathName]; }

    private:
        Game* game;

        std::unordered_map<std::string, std::unique_ptr<Model>> models;
    };

} // VoidEngine