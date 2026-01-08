#pragma once

#include "Camera.hpp"
#include "MykaEngine/Core/Timestep.hpp"
#include "MykaEngine/Events/Event.hpp"
#include "MykaEngine/Events/MouseEvent.hpp"

#include <glm/glm.hpp>

// Using TheCherno's EditorCamera class. Thanks him a lot!
// source: https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Renderer/EditorCamera.cpp

namespace Myka
{
    class EditorCamera : public Camera
    {
    public:
        EditorCamera() = default;
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void OnUpdate(Timestep ts);
        void OnEvent(Event &e);

        float GetDistance() const { return m_Distance; }
        void SetDistance(float distance) { m_Distance = distance; }

        void SetViewportSize(float width, float height);

        const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
        glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

        glm::vec3 GetUpDirection() const;
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetForwardDirection() const;
        const glm::vec3 &GetPosition() const { return m_Position; }
        glm::quat GetOrientation() const;

        float GetPitch() const { return m_Pitch; }
        float GetYaw() const { return m_Yaw; }

    private:
        void UpdateProjection();
        void UpdateView();

        bool OnMouseScroll(MouseScrolledEvent &e);

        void MousePan(const glm::vec2 &delta);
        void MouseRotate(const glm::vec2 &delta);
        void MouseZoom(float delta);

        glm::vec3 CalculatePosition() const;

        std::pair<float, float> PanSpeed() const;
        float ZoomSpeed() const;

    private:
        float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

        glm::mat4 m_ViewMatrix;
        glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
        glm::vec3 m_FocalPoint = {0.0f, 0.0f, 0.0f};

        glm::vec2 m_InitialMousePosition;

        float m_RotationSpeed = 0.8f;

        float m_Distance = 10.0f;
        float m_Pitch = 0.0f, m_Yaw = 0.0f;

        float m_ViewportWidth = 1280, m_ViewportHeight = 720;
    };
} // namespace Myka
