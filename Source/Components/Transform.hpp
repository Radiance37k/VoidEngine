#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <External/glm/glm.hpp>

namespace VoidEngine
{
    class Transform
    {
    public:
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4() const;
        glm::mat3 normalMatrix() const;

        // TODO: Move, rotate, lookat, moveto, getrotation(euler and quaternions)
    };
} // VoidEngine