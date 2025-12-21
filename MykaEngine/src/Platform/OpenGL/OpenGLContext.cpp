#include "OpenGLContext.hpp"

#include "MykaEngine/Log.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Myka
{
    OpenGLContext::OpenGLContext(GLFWwindow *windowHandle) : m_WindowHandle(windowHandle)
    {
        MYKA_CORE_ASSERT(windowHandle, "Window handle is null!");
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        MYKA_CORE_ASSERT(status, "Failed to initialize Glad!");

        MYKA_CORE_INFO("OpenGL Info:");
        MYKA_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
        MYKA_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
        MYKA_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
} // namespace Myka
