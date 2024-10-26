#include "ModelManager.hpp"

#include <iostream>

namespace VoidEngine {
    ModelManager::ModelManager()
    {
        std::cerr << "ModelManager created.";
    }

    ModelManager::~ModelManager()
    {
        std::cerr << "ModelManager destroyed.";
    }

    bool ModelManager::LoadModel(const std::string& path)
    {
        return true;
    }
} // VoidEngine