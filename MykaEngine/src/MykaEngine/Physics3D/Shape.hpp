#pragma once

#include <variant>

#include <glm/glm.hpp>

namespace Myka
{
    namespace Physics3D
    {
        struct BoxGeometry
        {
            glm::vec3 HalfSize{0.5f, 0.5f, 0.5f};
        };

        struct SphereGeometry
        {
            float Radius = 0.5f;
        };

        using GeometryType = std::variant<BoxGeometry, SphereGeometry>;

        struct Shape
        {
            GeometryType Geometry;
            float Density = 1.0f;
            float Friction = 0.5f;
            float Restitution = 0.0f;
            glm::vec3 Offset{0.0f};

            bool IsBox() const { return std::holds_alternative<BoxGeometry>(Geometry); }
            bool IsSphere() const { return std::holds_alternative<SphereGeometry>(Geometry); }

            static Shape CreateBox(const glm::vec3 &halfSize)
            {
                Shape s;
                s.Geometry = BoxGeometry{halfSize};
                return s;
            }

            static Shape CreateSphere(float radius)
            {
                Shape s;
                s.Geometry = SphereGeometry{radius};
                return s;
            }
        };

    } // namespace Physics3D

} // namespace Myka
