#pragma once

#include "MykaEngine/Layer.hpp"

namespace Myka
{
    class MYKA_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        void OnAttache();
        void OnDetach();
        void OnUpdate();
        void OnEvent(Event& event);

    private:
        float m_Time = 0.0f;
    };
} // namespace Myka
