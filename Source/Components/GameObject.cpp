#include "GameObject.hpp"
#include "VoidEngine.hpp"

namespace VoidEngine
{
    std::atomic<unsigned int> GameObject::nextId{0};


    GameObject::GameObject(unsigned int objId, Game* game) : game_(*game), id(objId), device_(*game->GetDevice())
    //, model(device_)
    {
        init();
    }

    GameObject::GameObject(Game* game) : game_(*game),  id(nextId++), device_(*game->GetDevice())//, model(device_)
    {
        init();
    }

    GameObject::GameObject(GameObject&& other) noexcept
        : device_(other.device_), id(other.id), transform(other.transform), game_(other.game_)
    {
        // Ensure the moved object is in a valid state
        other.id = 0;
    }

    GameObject& GameObject::operator=(GameObject&& other) noexcept {
        if (this == &other)
            return *this; // Handle self-assignment

        //device_ = other.device_; // Reassign device reference
        id = other.id;
        //model = std::move(other.model); // Transfer ownership of model
        transform = other.transform; // Move transform

        // Invalidate the moved object
        other.id = 0;

        return *this;
    }

    void GameObject::Update()
    {
        // Derived classes should call:
        // GameObject::Update();
    }

    void GameObject::init()
    {
        //model = new Model(device_);
        //model = std::make_unique<Model>(device_);
    }
}