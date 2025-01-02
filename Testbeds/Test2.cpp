#include <iostream>
#include <VoidEngine.hpp>

class Vase : public VoidEngine::GameObject
{
public:
    Vase(VoidEngine::Game* game) : GameObject(game) {}

    void Update() override
    {
        std::cerr << "Test\n";
    }
};

class NotVase : public VoidEngine::GameObject
{
public:
    NotVase(VoidEngine::Game* game) : GameObject(game) {}

    void Update() override
    {
        std::cerr << "Floor Test\n";
    }
};

int main()
{
    VoidEngine::Game game{VkExtent2D(800, 600)};

    Vase* flatVase = new Vase(&game);
    //Vase*  smoothVase = new Vase(&game);
    //NotVase* floor = new NotVase(&game);

    flatVase->model->LoadModelFromFile("models/flat_vase.obj");
    //smoothVase->model->LoadModelFromFile("models/smooth_vase.obj");
    //floor->model->LoadModelFromFile("models/quad.obj");

    flatVase->transform.translation = {0.5f, 0.5f, 1.0f};
    //flatVase->transform.rotation = {1.0f, 0.5f, 0.3f};
    //flatVase->transform.scale = {3.0f, 1.5f, 3.0f};

    //smoothVase->transform.translation = {0.5f, 0.5f, 1.0f};
    //smoothVase->transform.scale = {3.0f, 1.5f, 3.0f};

    //floor->transform.translation = {0.0f, 0.5f, 0.0f};
    //floor->transform.scale = {3.0f, 1.0f, 3.0f};


    game.AddGameObject(flatVase);
    //game.AddGameObject(smoothVase);
    //game.AddGameObject(floor);

    const std::vector<glm::vec3> lightColors{
                {1.f, .1f, .1f},
                {.1f, .1f, 1.f},
                {.1f, 1.f, .1f},
                {1.f, 1.f, .1f},
                {.1f, 1.f, 1.f},
                {1.f, 1.f, 1.f}  //
    };
/*
    for (int i = 0; i < lightColors.size(); i++)
    {
        game.lightSourceManager->AddPointLight(
            {lightColors[i], {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
            lightColors[i]);
    }
*/
    VoidEngine::Camera camera{&game};
    game.mainCamera = &camera;

    game.run();

    return 0;
}