#pragma once

#include <glm/glm.hpp>

namespace Myka
{
    namespace Physics3D
    {
        enum class BodyType
        {
            Static = 0,
            Kinematic,
            Dynamic
        };
        
        struct Body
        {        
            BodyType Type = BodyType::Static;

            glm::vec3 Position{0.0f};
            glm::vec3 Velocity{0.0f};
            glm::vec3 Force{0.0f};
            float Mass = 1.0;
            float InverseMass = 1.0f;

            float Restitution = 0.5f;
            float Friction = 0.2f;

            void ApplyForce(const glm::vec3 &force)
            {
                if (Type != BodyType::Dynamic)
                    return;
                Force += force;
            }

            void SetType(BodyType type)
            {
                Type = type;
                InverseMass = (type == BodyType::Dynamic) ? (1.0f / Mass) : 0.0f;
            }

            void SetMass(float mass)
            {
                if (mass > 0.0f)
                {
                    Mass = mass;
                    InverseMass = (Type == BodyType::Dynamic) ? (1.0f / Mass) : 0.0f;
                }
            }
        };
        
    } // namespace Physics3D

} // namespace Myka
