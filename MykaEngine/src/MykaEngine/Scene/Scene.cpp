#include "mykapch.hpp"
#include "Scene.hpp"
#include "Components.hpp"
#include "MykaEngine/Renderer/Renderer2D.hpp"
#include "Entity.hpp"

#include <glm/glm.hpp>

namespace Myka
{
    Scene::Scene()
    {
#if ENTT_EXAMPLE
        struct MeshComponent
        {
            bool Value;
            MeshComponent() = default;
        };

        struct TransformComponent
        {
            glm::mat4 Transform;

            TransformComponent() = default;
            TransformComponent(const TransformComponent &) = default;
            TransformComponent(const glm::mat4 &transform) : Transform(transform) {}

            operator glm::mat4() { return Transform; }
            operator const glm::mat4 &() const { return Transform; }
        };

        // Rendererg
        auto group = m_Registry.group<TransformComponent, MeshComponent>();
        for (auto entity : group)
        {
            auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

            Renderer::Submit(mesh, transform);
        }
#endif
    }

    Scene::~Scene()
    {
    }

    Entity Scene::CreateEntity(const std::string &name)
    {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<TransformComponent>();
        auto &tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        return entity;
    }

    void Scene::OnUpdate(Timestep ts)
    {
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group)
        {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

            Renderer2D::DrawQuad(transform, sprite);
        }
    }

} // namespace Myka
