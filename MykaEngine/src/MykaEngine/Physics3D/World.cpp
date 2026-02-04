#include "mykapch.hpp"
#include "World.hpp"

#include "MykaEngine/Core/Base.hpp"

namespace Myka
{
    namespace Physics3D
    {
        void World::Step(Timestep ts)
        {
            for (auto body : m_Bodies)
            {
                if (body->Type != BodyType::Dynamic)
                    continue;

                /*
                    Velocity update: v = v0 + at
                    Position update: x = x0 + vt
                */

                body->Velocity += (m_Gravity + body->Force * body->InverseMass) * (float)ts;
                body->Position += body->Velocity * (float)ts;

                body->Force = glm::vec3(0.0f);
            }

            HandleCollisions();
        }

        void World::HandleCollisions()
        {
            for (size_t i = 0; i < m_Bodies.size(); i++)
            {
                for (size_t j = i + 1; j < m_Bodies.size(); j++)
                {
                    Body *a = m_Bodies[i];
                    Body *b = m_Bodies[j];

                    if (a->InverseMass == 0 && b->InverseMass == 0)
                        continue;

                    Shape &shapeA = a->_Shape;
                    Shape &shapeB = b->_Shape;
                    if (shapeA.IsSphere() && shapeB.IsSphere())
                    {
                        // Radius
                        float rA = std::get<SphereGeometry>(shapeA.Geometry).Radius;
                        float rB = std::get<SphereGeometry>(shapeB.Geometry).Radius;

                        // Center Position with offset
                        glm::vec3 posA = a->Position + shapeA.Offset;
                        glm::vec3 posB = b->Position + shapeB.Offset;

                        // Vector from center a to center b
                        glm::vec3 relativePos = posB - posA;

                        // Distance between a and b;
                        float distance = glm::length(relativePos) - (rA + rB);

                        // Distance between radiusA and radiusB
                        float collisionDistance = rA + rB;

                        // Checking for collision
                        if (distance < collisionDistance)
                        {
                            CollisionContact contact;
                            contact.A = a;
                            contact.B = b;
                            contact.Normal = glm::normalize(relativePos); // Normalizing to get only direction
                            contact.Depth = distance - collisionDistance; // Depth

                            SolveCollision(contact); // Solving the collision
                        }
                    }
                }
            }
        }

        void World::SolveCollision(CollisionContact contact)
        {
            Body *a = contact.A;
            Body *b = contact.B;

            /*
                a = g + F / m
                v = v0 + at
                x = x0 + vt

                m1v1 + m2v2 = m1v1' + m2v2'
                Ek = (mv * v) / 2
                Ep = mgh
                Em = Ek + Ep
            */

            a->Velocity *= glm::vec3(-0.9f);
            b->Velocity *= glm::vec3(-0.9f);

            // a->Velocity *= glm::vec3(0.0f);
            // b->Velocity *= glm::vec3(0.0f);
        }

    } // namespace Physics3D

} // namespace Myka
