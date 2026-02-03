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

        enum class ShapeType
        {
            Box = 0,
            Sphere
        };

        struct Shape
        {
            GeometryType Geometry;
            ShapeType Type;

            float Density = 1.0f;
            float Friction = 0.5f;
            float Restitution = 0.0f;
            glm::vec3 Offset{0.0f};

            static Shape CreateBox(const glm::vec3 &halfSize)
            {
                Shape s;
                s.Type = ShapeType::Box;
                s.Geometry = BoxGeometry{halfSize};
                return s;
            }

            static Shape CreateSphere(float radius)
            {
                Shape s;
                s.Type;
                s.Geometry = SphereGeometry{radius};
                return s;
            }
        };

    } // namespace Physics3D

} // namespace Myka
