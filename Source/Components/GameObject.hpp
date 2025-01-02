#pragma once

#include "common.hpp"
#include "Model.hpp"
#include "Transform.hpp"

#include <gtc/matrix_transform.hpp>

#include <memory>

namespace VoidEngine
{
    class Game;

    class GameObject
    {
    public:
        VOIDENGINE_API explicit GameObject(unsigned int objId, Game* game);
        VOIDENGINE_API explicit GameObject(Game* game);
        virtual ~GameObject() = default;

        GameObject(GameObject&&) noexcept;                          // Move constructor func(std::move());
        GameObject& operator=(GameObject &&) noexcept;              // Move assignment  var = std::move();
        GameObject(const GameObject& other)                         // Copy constructor var1 = var2;
            : transform(other.transform), model(other.model), game_(other.game_), device_(other.device_), id(nextId++) {}

        GameObject& operator=(const GameObject& other)              // Copy assignment
        {
            if (this == &other) return *this; // Handle self-assignment
            id = nextId++;
            transform = other.transform;
            model = other.model;
            //device_ = other.device_;
            return *this;
        }

        [[nodiscard("id should not be discarded")]] unsigned int getId() const { return id; }
        template <typename T> T* GetAs() { return dynamic_cast<T*>(this); }

        Transform transform;

        //std::string model;
        Model* model;

        VOIDENGINE_API virtual void Update();

    protected:
        Game& game_;
        Device& device_;

    private:
        unsigned int id;
        static std::atomic<unsigned int> nextId;

        void init();
    };
}