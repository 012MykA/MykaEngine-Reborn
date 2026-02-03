#pragma once

#include <vector>
#include <cmath>

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

            std::vector<Shape> Shapes;

            void AddShape(const Shape &shape) { Shapes.push_back(shape); }

            void CalculateMassFromShapes()
            {
                if (Type != BodyType::Dynamic)
                {
                    Mass = 0.0f;
                    InverseMass = 0.0f;
                    return;
                }

                float totalMass = 0.0f;
                for (const auto& shape : Shapes)
                {
                    float volume;
                    if (shape.Type == ShapeType::Box)
                    {
                        /*
                            As we have half size
                            V(box) = 2a * 2b * 2c
                        */
                        glm::vec3 hSize = std::get<BoxGeometry>(shape.Geometry).HalfSize;
                        volume = (2.0f * hSize.x) * (2.0f * hSize.y) * (2.0f * hSize.z);
                    }
                    else if (shape.Type == ShapeType::Box)
                    {
                        /*
                            V(sphere) = (4 / 3) * PI * R^3
                        */
                        float radius = std::get<SphereGeometry>(shape.Geometry).Radius;
                        volume = (4.0f / 3.0f) * std::pow(radius, 3.0f);
                    }
                    totalMass += volume * shape.Density;
                }

                SetMass(totalMass > 0 ? totalMass : 1.0f);
            }

            void SetType(BodyType type)
            {
                Type = type;
                InverseMass = (type == BodyType::Dynamic) ? (1.0f / Mass) : 0.0f;
            }

            void SetMass(float mass)
            {
                Mass = (mass <= 0.0f) ? 0.0001f : mass;
                if (Type == BodyType::Dynamic)
                    InverseMass = 1.0f / Mass;
                else
                    InverseMass = 0.0f;
            }

            void ApplyForce(const glm::vec3 &force)
            {
                if (Type != BodyType::Dynamic)
                    return;
                Force += force;
            }
        };

    } // namespace Physics3D

} // namespace Myka
