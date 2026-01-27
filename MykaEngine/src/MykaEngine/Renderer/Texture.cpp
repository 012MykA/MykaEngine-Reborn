#include "mykapch.hpp"
#include "Texture.hpp"

#include "Renderer.hpp"
#include "Platform/OpenGL/OpenGLTexture.hpp"
#include "Platform/OpenGL/OpenGLCubeTexture.hpp"

namespace Myka
{
    Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:     MYKA_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
        case RendererAPI::API::OpenGL:   return CreateRef<OpenGLTexture2D>(specification);

        default:                         MYKA_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
        }
    }

    Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:     MYKA_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
        case RendererAPI::API::OpenGL:   return CreateRef<OpenGLTexture2D>(path);

        default:                         MYKA_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
        }
    }

    Ref<CubeTexture> CubeTexture::Create(const TextureSpecification &specification)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:     MYKA_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
        case RendererAPI::API::OpenGL:   return CreateRef<OpenGLCubeTexture>(specification);

        default:                         MYKA_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
        }
    }

    Ref<CubeTexture> CubeTexture::Create(const std::vector<std::filesystem::path> &paths)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:     MYKA_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
        case RendererAPI::API::OpenGL:   return CreateRef<OpenGLCubeTexture>(paths);

        default:                         MYKA_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
        }
    }

} // namespace Myka
