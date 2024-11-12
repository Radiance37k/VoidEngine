#include <VoidEngine.hpp>

#include "ModelManager.hpp"
#include "RenderManager.hpp"
#include "SceneManager.hpp"

int main()
{
    VoidEngine::Game game{VkExtent2D(800, 600)};

    auto flatVase = game.modelManager->LoadModel(*game.GetDevice(), "models/flat_vase.obj");
    auto smoothVase = game.modelManager->LoadModel(*game.GetDevice(), "models/smooth_vase.obj");
    auto floor = game.modelManager->LoadModel(*game.GetDevice(), "models/quad.obj");

    flatVase.transform.translation = {-0.5f, 0.5f, 0.0f};
    flatVase.transform.scale = {3.0f, 1.5f, 3.0f};
    smoothVase.transform.translation = {0.5f, 0.5f, 0.0f};
    smoothVase.transform.scale = {3.0f, 1.5f, 3.0f};
    floor.transform.translation = {0.0f, 0.5f, 0.0f};
    floor.transform.scale = {3.0f, 1.0f, 3.0f};


    game.AddGameObject(flatVase);
    game.AddGameObject(smoothVase);
    game.AddGameObject(floor);

    game.run();

    return 0;
}
