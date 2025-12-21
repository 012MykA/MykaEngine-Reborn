#pragma once

#include "MykaEngine/Renderer/GraphicsContext.hpp"

struct GLFWwindow;

namespace Myka
{
    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(GLFWwindow* windowHandle);

        virtual void Init() override;
        virtual void SwapBuffers() override;

    private:
        GLFWwindow* m_WindowHandle;
    };
} // namespace Myka
