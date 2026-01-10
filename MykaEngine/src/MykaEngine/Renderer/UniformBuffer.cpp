#include "mykapch.hpp"
#include "UniformBuffer.hpp"

#include "Renderer.hpp"
#include "Platform/OpenGL/OpenGLUniformBuffer.hpp"

namespace Myka
{
    Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:     MYKA_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
        case RendererAPI::API::OpenGL:   return CreateRef<OpenGLUniformBuffer>(size, binding);

        default:                         MYKA_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
        }
    }

} // namespace Myka
