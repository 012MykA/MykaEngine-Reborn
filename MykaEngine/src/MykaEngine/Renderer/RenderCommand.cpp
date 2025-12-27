#include "mykapch.hpp"
#include "RenderCommand.hpp"

#include "Platform/OpenGL/OpenGLRendererAPI.hpp"

namespace Myka
{
    RendererAPI *RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
} // namespace Myka
