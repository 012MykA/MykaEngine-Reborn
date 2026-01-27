#pragma once

#include "Texture.hpp"
#include "Shader.hpp"

namespace Myka
{
    class IBLBaker
    {
    public:
        // Конвертирует 2D панораму в CubeTexture
        static void ConvertPanoramaToCubemap(const Ref<Texture2D> &pano, const Ref<CubeTexture> &cube);

        // Будущие методы для IBL
        // static void CreateIrradianceMap(...);
        // static void CreatePrefilterMap(...);
    };

}