#include "mykapch.hpp"
#include "OpenGLRendererAPI.hpp"

#include <glad/glad.h>

namespace Myka
{
    static void OpenGLMessageCallback(
        unsigned source,
        unsigned type,
        unsigned id,
        unsigned severity,
        int length,
        const char *message,
        const void *userParam)
    {
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            MYKA_CORE_TRACE(message);
            return;
        case GL_DEBUG_SEVERITY_LOW:
            MYKA_CORE_WARN(message);
            return;
        case GL_DEBUG_SEVERITY_MEDIUM:
            MYKA_CORE_ERROR(message);
            return;
        case GL_DEBUG_SEVERITY_HIGH:
            MYKA_CORE_CRITICAL(message);
            return;

        default:
            MYKA_CORE_ASSERT(false, "Unknown severity level!");
        }
    }

    void OpenGLRendererAPI::Init()
    {
        MYKA_PROFILE_FUNCTION();

#ifdef MYKA_DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t height, uint32_t width)
    {
        glViewport(x, y, width, height);
    }

    void OpenGLRendererAPI::SetClearColor(const glm::vec4 &color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLRendererAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t indexCount)
    {
        vertexArray->Bind();
        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
    }

    void OpenGLRendererAPI::DrawIndexedInstanced(const Ref<VertexArray> &vertexArray, uint32_t instanceCount, uint32_t indexCount)
    {
        vertexArray->Bind();
        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
        glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, instanceCount);
    }

    void OpenGLRendererAPI::DrawLines(const Ref<VertexArray> &vertexArray, uint32_t vertexCount)
    {
        vertexArray->Bind();
        glDrawArrays(GL_LINES, 0, vertexCount);
    }

    void OpenGLRendererAPI::SetLineWidth(float width)
    {
        glLineWidth(width);
    }

    void OpenGLRendererAPI::ShowTriangles(bool show)
    {
        if (show)
        {
            glEnable(GL_POLYGON_SMOOTH);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glDisable(GL_POLYGON_SMOOTH);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

} // namespace Myka
