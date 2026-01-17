#include "mykapch.hpp"
#include "Renderer.hpp"

#include "Platform/OpenGL/OpenGLShader.hpp"
#include "Renderer2D.hpp"
#include "Renderer3D.hpp"

namespace Myka
{
    void Renderer::Init()
    {
        MYKA_PROFILE_FUNCTION();

        RenderCommand::Init();
        Renderer2D::Init();
        Renderer3D::Init();
    }

    void Renderer::Shutdown()
    {
        Renderer2D::Shutdown();
        Renderer3D::Shutdown();
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
    }

} // namespace Myka
