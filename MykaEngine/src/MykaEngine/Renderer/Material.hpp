#pragma once

#include "Texture.hpp"

#include <glm/glm.hpp>

namespace Myka
{
    class Material
    {
    public:
        glm::vec4 AlbedoColor{1.0f, 1.0f, 1.0f, 1.0f};
        Ref<Texture2D> AlbedoMap;
        float Metallic = 0.0f;
        float Roughness = 0.5f;

        static Ref<Material> Create() { return CreateRef<Material>(); }
    };
} // namespace Myka