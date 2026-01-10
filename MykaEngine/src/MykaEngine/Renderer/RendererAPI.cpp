#include "mykapch.hpp"
#include "RendererAPI.hpp"

#include "Platform/OpenGL/OpenGLRendererAPI.hpp"

namespace Myka
{
    RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

    Scope<RendererAPI> RendererAPI::Create()
    {
        switch (s_API)
        {
        case RendererAPI::API::None:     MYKA_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
        case RendererAPI::API::OpenGL:   return CreateScope<OpenGLRendererAPI>();

        default:                         MYKA_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
        }
    }
} // namespace Myka
