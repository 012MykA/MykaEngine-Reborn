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
        public:
            World() = default;
            ~World() = default;

            void SetGravity(const glm::vec3 &gravity) { m_Gravity = gravity; }

            void AddBody(Body *body) { m_Bodies.push_back(body); }

            void Step(Timestep ts)
            {
                for (auto body : m_Bodies)
                {
                    if (body->Type != BodyType::Dynamic)
                        continue;

                    /*
                        inverse mass = 1 / mass
                        It is done to prevent division by zero in such equasions as: a = F / m

                        Example:
                        F = m * a       }
                                        } -> a = F * invMass
                        invMass = 1 / m }
                    */

                    /*
                        Kinematic equations
                        ====================
                        Velocity update: v = v0 + at
                        Position update: x = x0 + vt

                        invMass = 1 / m
                        F = ma -> a = F * invMass
                    */

                    body->Velocity += (m_Gravity + body->Force * body->InverseMass) * (float)ts;
                    body->Position += body->Velocity * (float)ts;

                    body->Force = glm::vec3(0.0f);
                }
            }

        private:
            void CalculateCollisions() {}

            glm::vec3 m_Gravity = {0.0f, -9.81f, 0.0f};
            std::vector<Body *> m_Bodies;
        };

    } // namespace Physics3D
    
} // namespace Myka