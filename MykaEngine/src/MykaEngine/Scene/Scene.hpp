#pragma once

#include <entt.hpp>

#include "MykaEngine/Core/Timestep.hpp"
#include "MykaEngine/Renderer/EditorCamera.hpp"
#include "MykaEngine/Core/UUID.hpp"
#include "MykaEngine/Physics3D/World.hpp"

#include "box2d/id.h"

namespace Myka
{
    class Scene
    {
        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;

    public:
        Scene() = default;
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

        Entity DuplicateEntity(Entity entity);

        Entity GetPrimaryCameraEntity();

        const glm::vec3 &GetGravity() const { return m_Gravity; }
        glm::vec3& GetGravity() { return m_Gravity; }
        void SetGravity(const glm::vec3 &gravity) { m_Gravity = gravity; }

        template <typename... Components>
        auto GetAllEntitiesWith()
        {
            return m_Registry.view<Components...>();
        }

    private:
        template <typename T>
        void OnComponentAdded(Entity entity, T &component);

        void OnPhysics3DStart();
        void OnPhysics3DUpdate(Timestep ts);
        void OnPhysics3DStop();

        void OnPhysics2DStart();
        void OnPhysics2DUpdate(Timestep ts);
        void OnPhysics2DStop();

        void RenderScene(EditorCamera &camera);

    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

        Physics3D::World *m_Physics3DWorld;
        glm::vec3 m_Gravity = {0.0f, -9.81f, 0.0f};
        b2WorldId m_Physics2DWorldID = b2_nullWorldId;
    };
} // namespace Myka
