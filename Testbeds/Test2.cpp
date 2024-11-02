#include <VoidEngine.hpp>

#include "ModelManager.hpp"
#include "RenderManager.hpp"
#include "SceneManager.hpp"

int main()
{
    VoidEngine::Game game{VkExtent2D(800, 600)};
    VoidEngine::ModelManager modelManager;
    VoidEngine::SceneManager sceneManager;

    //auto* renderManager = new VoidEngine::RenderManager(*game.GetDevice(), nullptr, nullptr, game.GetResolution());

    auto flatVase = modelManager.LoadModel(*game.GetDevice(), "fne");
    sceneManager.AddGameObject(std::move(flatVase));

    game.run();

    return 0;
}
