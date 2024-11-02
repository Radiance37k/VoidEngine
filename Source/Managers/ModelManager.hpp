#pragma once
#include <unordered_map>

#include "Model.hpp"

namespace VoidEngine {

    // Forward decleration
    class GameObject;

    class ModelManager {
    public:
        VOIDENGINE_API ModelManager();
        VOIDENGINE_API ~ModelManager();

        //Model GetModel(std::string path);
        VOIDENGINE_API GameObject LoadModel(Device& device_, const std::string &path);

//    private:
//        std::unordered_map<std::string, Model> m_Models;
    };

} // VoidEngine