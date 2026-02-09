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
               
                glm::vec3 gravity = body->GravityEffect ? m_WorldGravity : glm::vec3(0.0f);
                body->Velocity += (gravity + body->Force * body->InverseMass) * (float)ts;
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
                        float rA = std::get<SphereGeometry>(shapeA.Geometry).Radius;
                        float rB = std::get<SphereGeometry>(shapeB.Geometry).Radius;

                        glm::vec3 worldCenterA = a->Position + shapeA.Offset;
                        glm::vec3 worldCenterB = b->Position + shapeB.Offset;

                        glm::vec3 relativePos = worldCenterB - worldCenterA;
                        float distance = glm::length(relativePos);
                        float minDistance = rA + rB;

                        if (distance < minDistance)
                        {
                            CollisionInfo contact;
                            contact.A = a;
                            contact.B = b;
                            if (distance > 0.0f)
                            {
                                contact.Normal = relativePos / distance;
                                contact.Depth = minDistance - distance;
                            }
                            else
                            {
                                contact.Normal = glm::vec3(0, 1, 0);
                                contact.Depth = minDistance;
                            }

                            SolveCollision(contact);
                        }
                    }
                }
            }
        }

        void World::SolveCollision(CollisionInfo contact)
        {
            Body *a = contact.A;
            Body *b = contact.B;

            glm::vec3 relVelocity = b->Velocity - a->Velocity;
            float velAlongNormal = glm::dot(relVelocity, contact.Normal);

            if (velAlongNormal > 0.0f)
                return;

            float invMassSum = a->InverseMass + b->InverseMass;

            // (1 + e), where e = 1
            float j = -(2.0f * velAlongNormal) / invMassSum;

            glm::vec3 impulse = j * contact.Normal;

            a->Velocity -= a->InverseMass * impulse;
            b->Velocity += b->InverseMass * impulse;

            const float percent = 0.5f;
            glm::vec3 correction = (contact.Depth / invMassSum) * percent * contact.Normal;
            a->Position -= a->InverseMass * correction;
            b->Position += b->InverseMass * correction;
        }

    } // namespace Physics3D

} // namespace Myka
