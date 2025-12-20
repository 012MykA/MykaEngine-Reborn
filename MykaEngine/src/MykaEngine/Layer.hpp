#pragma once

#include "MykaEngine/Core.hpp"
#include "MykaEngine/Events/Event.hpp"

namespace Myka
{
    class MYKA_API Layer
    {
    public:
        Layer(const std::string &name = "Layer");
        virtual ~Layer();

        virtual void OnAttache() {}
        virtual void OnDetach() {}
        virtual void OnUpdate() {}
        virtual void OnEvent(Event &event) {}

        inline const std::string &GetName() const { return m_DebugName; }

    private:
        std::string m_DebugName;
    };
} // namespace Myka
