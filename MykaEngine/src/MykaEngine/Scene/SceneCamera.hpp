#pragma once

#include "MykaEngine/Renderer/Camera.hpp"

namespace Myka
{

    class SceneCamera : public Camera
    {
    public:
        enum class ProjectionType
        {
            Perspective = 0,
            Orthographic = 1
        };

    public:
        SceneCamera();
        virtual ~SceneCamera() = default;

        void SetOrthographic(float size, float nearClip, float farClip);
        void SetPerspective(float FOV, float nearClip, float farClip);

        void SetViewportSize(uint32_t width, uint32_t height);

    public:
        float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
        void SetPerspectiveFOV(float FOV);
        float GetPerspectiveNear() const { return m_PerspectiveNear; }
        void SetPerspectiveNear(float nearClip);
        float GetPerspectiveFar() const { return m_PerspectiveFar; }
        void SetPerspectiveFar(float farClip);

        float GetOrthographicSize() const { return m_OrthographicSize; }
        void SetOrthographicSize(float size);
        float GetOrthographicNear() const { return m_OrthographicNear; }
        void SetOrthographicNear(float nearClip);
        float GetOrthographicFar() const { return m_OrthographicFar; }
        void SetOrthographicFar(float farClip);

    public:
        ProjectionType GetProjectionType() const { return m_ProjectionType; }
        void SetProjectionType(ProjectionType type);

    private:
        void RecalculateProjection();

    private:
        ProjectionType m_ProjectionType = ProjectionType::Orthographic;

        float m_PerspectiveFOV = glm::radians(45.0f);
        float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;

        float m_OrthographicSize = 10.0f;
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

        float m_AspectRatio = 0.0f;
    };
} // namespace Myka
