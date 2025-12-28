#include "mykapch.hpp"
#include "VertexArray.hpp"

#include "Renderer.hpp"
#include "Platform/OpenGL/OpenGLVertexArray.hpp"

namespace Myka
{
    Ref<VertexArray> VertexArray::Create()
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:     MYKA_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
        case RendererAPI::API::OpenGL:   return CreateRef<OpenGLVertexArray>();

        default:                         MYKA_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
        }
    }
} // namespace Myka
