#include "VoidEngine.hpp"
#include "ModelManager.hpp"

#include <iostream>
#include <External/tinyobjloader/tinyobjloader.hpp>

namespace VoidEngine {
    ModelManager::ModelManager(Game* game) : game(game)
    {
        std::cerr << "ModelManager created.\n";
    }

    ModelManager::~ModelManager()
    {
        std::cerr << "ModelManager destroyed.\n";
    }
} // VoidEngine