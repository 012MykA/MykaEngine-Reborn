#pragma once

#include <entt.hpp>

#include "MykaEngine/Core/Timestep.hpp"
#include "MykaEngine/Renderer/EditorCamera.hpp"
#include "MykaEngine/Core/UUID.hpp"

#include "box2d/id.h"

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

        static Ref<Scene> Copy(Ref<Scene> other);

        Entity CreateEntity(const std::string &name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string &name = std::string());
        void DestroyEntity(Entity entity);

        void OnRuntimeStart();
        void OnRuntimeStop();
        void OnUpdateRuntime(Timestep ts);

        void OnSimulationStart();
        void OnSimulationStop();
        void OnUpdateSimulation(Timestep ts, EditorCamera &camera);

        void OnUpdateEditor(Timestep ts, EditorCamera &camera);

        void OnViewportResize(uint32_t width, uint32_t height);

        void DuplicateEntity(Entity entity);

        Entity GetPrimaryCameraEntity();

        template <typename... Components>
        auto GetAllEntitiesWith()
        {
            return m_Registry.view<Components...>();
        }

    private:
        template <typename T>
        void OnComponentAdded(Entity entity, T &component);

        void OnPhysics2DStart();
        void OnPhysics2DStop();

        void RenderScene(EditorCamera &camera);

    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

        b2WorldId m_PhysicsWorldID;
    };
} // namespace Myka
