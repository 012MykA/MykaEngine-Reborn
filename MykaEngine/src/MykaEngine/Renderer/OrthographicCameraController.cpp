#include "OrthographicCameraController.hpp"

#include "MykaEngine/Core/Input.hpp"
#include "MykaEngine/Core/KeyCodes.hpp"

namespace Myka
{
    OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
        : m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
    {
    }

    void OrthographicCameraController::OnUpdate(Timestep ts)
    {
        if (Input::IsKeyPressed(MYKA_KEY_A))
            m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
        if (Input::IsKeyPressed(MYKA_KEY_D))
            m_CameraPosition.x += m_CameraTranslationSpeed * ts;

        if (Input::IsKeyPressed(MYKA_KEY_S))
            m_CameraPosition.y -= m_CameraTranslationSpeed * ts;
        if (Input::IsKeyPressed(MYKA_KEY_W))
            m_CameraPosition.y += m_CameraTranslationSpeed * ts;

        if (m_Rotation)
        {
            if (Input::IsKeyPressed(MYKA_KEY_Q))
                m_CameraRotation += m_CameraRotationSpeed * ts;
            if (Input::IsKeyPressed(MYKA_KEY_E))
                m_CameraRotation -= m_CameraRotationSpeed * ts;

            m_Camera.SetRotation(m_CameraRotation);
        }

        m_Camera.SetPosition(m_CameraPosition);

        m_CameraTranslationSpeed = m_ZoomLevel;
    }

    void OrthographicCameraController::OnEvent(Event &e)
    {
        EventDispatcher dp(e);
        dp.Dispatch<MouseScrolledEvent>(MYKA_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
        dp.Dispatch<WindowResizeEvent>(MYKA_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
    }

    bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent &e)
    {
        m_ZoomLevel -= e.GetYOffset() * 0.25f;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);     // Min Zoom
        m_ZoomLevel = std::min(m_ZoomLevel, 10.0f);      // Max Zoom
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

        return false;
    }

    bool OrthographicCameraController::OnWindowResized(WindowResizeEvent &e)
    {
        m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

        return false;
    }
} // namespace Myka
