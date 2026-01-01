#pragma once

#include "MykaEngine/Core/Base.hpp"
#include "MykaEngine/Core/Layer.hpp"
#include "MykaEngine/Events/Event.hpp"
#include "MykaEngine/Events/ApplicationEvent.hpp"
#include "MykaEngine/Events/MouseEvent.hpp"
#include "MykaEngine/Events/KeyEvent.hpp"

namespace Myka
{
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer() : Layer("ImGuiLayer") {}
        ~ImGuiLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        void Begin();
        void End();

    private:
        float m_Time = 0.0f;
    };
} // namespace Myka
