#include "mykapch.hpp"
#include "RenderCommand.hpp"

#include "Platform/OpenGL/OpenGLRendererAPI.hpp"

namespace Myka
{
    Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
} // namespace Myka
