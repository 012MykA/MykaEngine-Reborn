#pragma once

#include "MykaEngine/Core/Timestep.hpp"
#include "Body.hpp"

#include <vector>

#include <glm/glm.hpp>

namespace Myka
{
    namespace Physics3D
    {
        class World
        {
            struct CollisionContact
            {
                Body *A;
                Body *B;
                glm::vec3 Normal;
                float Depth;
            };

        public:
            World() = default;
            ~World() = default;

            void SetGravity(const glm::vec3 &gravity) { m_Gravity = gravity; }

            void AddBody(Body *body) { m_Bodies.push_back(body); }

            void Step(Timestep ts);

        private:
            void HandleCollisions();
            void SolveCollision(CollisionContact contact);

            glm::vec3 m_Gravity = {0.0f, -9.81f, 0.0f};
            std::vector<Body *> m_Bodies;
        };

    } // namespace Physics3D

} // namespace Myka