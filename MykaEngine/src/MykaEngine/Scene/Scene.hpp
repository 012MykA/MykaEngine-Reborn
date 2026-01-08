#pragma once

#include <entt.hpp>

#include "MykaEngine/Core/Timestep.hpp"
#include "MykaEngine/Renderer/EditorCamera.hpp"

namespace Myka
{
    class Scene
    {
        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;

    public:
        Scene();
        ~Scene() = default;

        Entity CreateEntity(const std::string &name = std::string());
        void DestroyEntity(Entity entity);

        void OnUpdateEditor(Timestep ts, EditorCamera& camera);
        void OnUpdateRuntime(Timestep ts);
        void OnViewportResize(uint32_t width, uint32_t height);

        Entity GetPrimaryCameraEntity();

    private:
        template <typename T>
        void OnComponentAdded(Entity entity, T &component);

    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
    };
} // namespace Myka
