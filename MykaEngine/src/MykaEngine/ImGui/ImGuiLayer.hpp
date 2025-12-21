#pragma once

#include "MykaEngine/Core.hpp"
#include "MykaEngine/Layer.hpp"
#include "MykaEngine/Events/Event.hpp"
#include "MykaEngine/Events/ApplicationEvent.hpp"
#include "MykaEngine/Events/MouseEvent.hpp"
#include "MykaEngine/Events/KeyEvent.hpp"

namespace Myka
{
    class MYKA_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        void OnAttach();
        void OnDetach();
        void OnUpdate();
        void OnEvent(Event& event);

    private:
        bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
        bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
        bool OnMouseMovedEvent(MouseMovedEvent& e);
        bool OnMouseScrolledEvent(MouseScrolledEvent& e);
        bool OnKeyPressedEvent(KeyPressedEvent& e);
        bool OnKeyReleasedEvent(KeyReleasedEvent& e);
        bool OnKeyTypedEvent(KeyTypedEvent& e);
        bool OnWindowResizeEvent(WindowResizeEvent& e);

    private:
        float m_Time = 0.0f;
    };
} // namespace Myka
