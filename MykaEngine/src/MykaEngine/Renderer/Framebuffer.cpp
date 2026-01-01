#include "mykapch.hpp"
#include "Framebuffer.hpp"

#include "Renderer.hpp"

#include "Platform/OpenGL/OpenGLFramebuffer.hpp"

namespace Myka
{
    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:     MYKA_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
        case RendererAPI::API::OpenGL:   return CreateRef<OpenGLFramebuffer>(spec);

        default:                         MYKA_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
        }
    }

} // namespace Myka
