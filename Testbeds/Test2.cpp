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

int main()
{
    VoidEngine::Game game{VkExtent2D(800, 600)};

    //Vase* flatVase = game.AddGameObject<Vase>(VoidEngine::RenderQueueType::OPAQUE, game.GetDevice());
    Vase* flatVase = new Vase(&game);

    //auto flatVase = game.modelManager->LoadModel(*game.GetDevice(), "models/flat_vase.obj");
    //flatVase.model = flatVase.model->createModelFromFile(*game.GetDevice(), "models/flat_vase.obj");
    //auto smoothVase = game.modelManager->LoadModel(*game.GetDevice(), "models/smooth_vase.obj");
    //auto floor = game.modelManager->LoadModel(*game.GetDevice(), "models/quad.obj");

    //game.modelManager->LoadModelFromFile("models/flat_vase.obj", *flatVase);
    flatVase->model->LoadModelFromFile("models/flat_vase.obj");
    flatVase->transform.translation = {-0.5f, 0.5f, 0.0f};
    flatVase->transform.scale = {3.0f, 1.5f, 3.0f};
    //smoothVase->transform.translation = {0.5f, 0.5f, 0.0f};
    //smoothVase->transform.scale = {3.0f, 1.5f, 3.0f};
    //floor->transform.translation = {0.0f, 0.5f, 0.0f};
    //floor->transform.scale = {3.0f, 1.0f, 3.0f};


    game.AddGameObject(flatVase);
    //game.AddGameObject(std::make_unique<VoidEngine::GameObject>(*smoothVase));
    //game.AddGameObject(std::make_unique<VoidEngine::GameObject>(*floor));

    const std::vector<glm::vec3> lightColors{
                {1.f, .1f, .1f},
                {.1f, .1f, 1.f},
                {.1f, 1.f, .1f},
                {1.f, 1.f, .1f},
                {.1f, 1.f, 1.f},
                {1.f, 1.f, 1.f}  //
    };

    for (int i = 0; i < lightColors.size(); i++)
    {
        //VoidEngine::PointLight* pointLight = new VoidEngine::PointLight(game.GetDevice());
        //VoidEngine::PointLight* pointLight = game.AddGameObject<VoidEngine::PointLight>(VoidEngine::RenderQueueType::LIGHT, game.GetDevice());
        //pointLight->SetPointLight(lightColors[i]);
        //auto rotateLight = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColors.size(),{0.f, -1.f, 0.f});
        //pointLight->transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
        //game.AddGameObject(pointLight, VoidEngine::RenderQueueType::LIGHT);

        game.lightSourceManager->AddPointLight(
            {lightColors[i], {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
            lightColors[i]);
    }

    game.run();

    return 0;
}