#include "mykapch.hpp"
#include "World.hpp"

#include "MykaEngine/Core/Base.hpp"

namespace Myka
{
    namespace Physics3D
    {
        void World::Step(Timestep ts)
        {
            float dt = static_cast<float>(ts);

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
                        float rA = std::get<SphereGeometry>(shapeA.Geometry).Radius * 2.0f;
                        float rB = std::get<SphereGeometry>(shapeB.Geometry).Radius * 2.0f;

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
                    else if ((shapeA.IsSphere() && shapeB.IsBox()) || (shapeA.IsBox() && shapeB.IsSphere()))
                    {
                        Body *sBody = shapeA.IsSphere() ? a : b;
                        Body *bBody = shapeA.IsBox() ? a : b;

                        float radius = std::get<SphereGeometry>(sBody->_Shape.Geometry).Radius * 2.0f;
                        glm::vec3 halfSize = std::get<BoxGeometry>(bBody->_Shape.Geometry).HalfSize * 4.0f;

                        glm::vec3 sCenter = sBody->Position + sBody->_Shape.Offset;
                        glm::vec3 bCenter = bBody->Position + bBody->_Shape.Offset;

                        glm::vec3 relPos = sCenter - bCenter;

                        glm::vec3 closest = glm::clamp(relPos, -halfSize, halfSize);

                        glm::vec3 difference = relPos - closest;
                        float distance = glm::length(difference);

                        if (distance < radius)
                        {
                            CollisionInfo contact;
                            contact.A = bBody;
                            contact.B = sBody;

                            if (distance > 0.0f)
                            {
                                contact.Normal = difference / distance;
                                contact.Depth = radius - distance;
                            }
                            else
                            {
                                contact.Normal = glm::vec3(0, 1, 0);
                                contact.Depth = radius;
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

            float e = std::min(a->_Shape.Restitution, b->_Shape.Restitution);
            float inverseMassSum = a->InverseMass + b->InverseMass;

            float j = (-(1.0f + e) * velAlongNormal) / inverseMassSum;

            glm::vec3 impulse = j * contact.Normal;

            a->Velocity -= a->InverseMass * impulse;
            b->Velocity += b->InverseMass * impulse;

            const float percent = 0.4f;
            const float slop = 0.01f;
            glm::vec3 correction = std::max(contact.Depth - slop, 0.0f) / inverseMassSum * percent * contact.Normal;

            a->Position -= a->InverseMass * correction;
            b->Position += b->InverseMass * correction;
        }

    } // namespace Physics3D

} // namespace Myka
