#pragma once

#include "Texture.hpp"

#include <glm/glm.hpp>

namespace Myka
{
    struct Material
    {
        glm::vec4 AlbedoColor = {1.0f, 1.0f, 1.0f, 1.0f};
        float Metallic = 0.0f;
        float Roughness = 0.5f;

        static Ref<Material> Create() { return CreateRef<Material>(); }
    };
} // namespace Myka