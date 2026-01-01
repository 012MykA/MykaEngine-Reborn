#pragma once

#include "MykaEngine/Core/Base.hpp"
#include "MykaEngine/Events/Event.hpp"
#include "MykaEngine/Core/Timestep.hpp"

namespace Myka
{
    class Layer
    {
    public:
        Layer(const std::string &name = "Layer");
        virtual ~Layer();

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnImGuiRender() {}
        virtual void OnEvent(Event &e) {}

        const std::string &GetName() const { return m_DebugName; }

    private:
        std::string m_DebugName;
    };
} // namespace Myka
