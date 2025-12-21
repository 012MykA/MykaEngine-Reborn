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

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;

        void Begin();
        void End();

    private:
        float m_Time = 0.0f;
    };
} // namespace Myka
