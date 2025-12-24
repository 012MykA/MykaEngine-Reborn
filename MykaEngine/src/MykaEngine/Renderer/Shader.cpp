#include "Shader.hpp"

#include "Renderer.hpp"

#include "Platform/OpenGL/OpenGLShader.hpp"

namespace Myka
{
    Shader* Shader::Create(const std::string &vertexSrc, const std::string &fragmentSrc)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:     MYKA_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
        case RendererAPI::API::OpenGL:   return new OpenGLShader(vertexSrc, fragmentSrc);

        default:                         MYKA_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
        }
    }
} // namespace Myka
