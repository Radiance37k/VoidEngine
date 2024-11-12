#include "ModelManager.hpp"
#include "Device.hpp"
#include "GameObject.hpp"

#include <iostream>

#include "VoidEngine.hpp"

namespace VoidEngine {
    ModelManager::ModelManager()
    {
        std::cerr << "ModelManager created.\n";
    }

    ModelManager::~ModelManager()
    {
        std::cerr << "ModelManager destroyed.\n";
    }

    GameObject ModelManager::LoadModel(Device& device_, const std::string &path)
    {
        std::shared_ptr<Model> model;
        model = Model::createModelFromFile(device_, path);
        auto gameObject = GameObject::createGameObject();
        gameObject.model = model;
        gameObject.transform.translation = {-0.5f, 0.5f, 0.0f};
        gameObject.transform.scale = {3.0f, 1.5f, 3.0f};

        return gameObject;
    }
} // VoidEngine