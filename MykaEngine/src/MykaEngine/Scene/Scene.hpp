#pragma once

#include <entt.hpp>

#include "MykaEngine/Core/Timestep.hpp"

namespace Myka
{
    class Scene
    {
        friend class Entity;

    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string &name = std::string());

        // Temp
        entt::registry &Reg() { return m_Registry; }

        void OnUpdate(Timestep ts);

    private:
        entt::registry m_Registry;
    };
} // namespace Myka
