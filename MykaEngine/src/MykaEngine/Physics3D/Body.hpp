#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Shape.hpp"

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

            float Mass = 1.0f;
            float InverseMass = 1.0f;
            bool GravityEffect = false;
            Shape _Shape;

            void SetType(BodyType type)
            {
                if (Type == type)
                    return;

                Type = type;

                if (Type != BodyType::Dynamic)
                {
                    InverseMass = 0.0f;
                    Velocity = glm::vec3(0.0f);
                }
            }

            void SetMass(float mass)
            {
                Mass = mass;
                if (Type == BodyType::Dynamic && Mass > 0.0001f)
                    InverseMass = 1.0f / Mass;
                else
                    InverseMass = 0.0f;
            }

            void SetShape(const Shape &shape)
            {
                _Shape = shape;
            }

            float CalculateMassFromShape()
            {
                // Returns calculated mass

                if (Type != BodyType::Dynamic)
                {
                    SetMass(0.0f);
                    return 0.0f;
                }

                float volume = std::visit([](auto &&geo) -> float
                                          {
                    using T = std::decay_t<decltype(geo)>;
                    
                    if constexpr (std::is_same_v<T, BoxGeometry>)
                    {
                        // V = (2a * 2b * 2c) = 8 * a * b * c
                        glm::vec3 size = geo.HalfSize * 2.0f;
                        return 8.0f * (geo.HalfSize.x * geo.HalfSize.y * geo.HalfSize.z);
                    }
                    else if constexpr (std::is_same_v<T, SphereGeometry>)
                    {
                        // V = (4 / 3) * PI * r^3
                        const float r = geo.Radius;
                        return (4.0f / 3.0f) * std::numbers::pi_v<float> * (r * r * r);
                    }
                    return 0.0f; }, _Shape.Geometry);

                SetMass(volume * _Shape.Density);
                return volume * _Shape.Density;
            }
        };

    } // namespace Physics3D

} // namespace Myka
