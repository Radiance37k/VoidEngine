#pragma once
#include <unordered_map>

#include "Model.hpp"

namespace VoidEngine {

    class ModelManager {
    public:
        ModelManager();
        ~ModelManager();

        //Model GetModel(std::string path);
        static bool LoadModel(const std::string& path);

//    private:
//        std::unordered_map<std::string, Model> m_Models;
    };

} // VoidEngine