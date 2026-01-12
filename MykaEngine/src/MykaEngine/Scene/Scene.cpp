#include "mykapch.hpp"
#include "Scene.hpp"
#include "Components.hpp"
#include "MykaEngine/Renderer/Renderer2D.hpp"
#include "Entity.hpp"

#include <glm/glm.hpp>

#include "box2d/box2d.h"
#include "box2d/id.h"
#include "box2d/collision.h"

namespace Myka
{
    Scene::Scene() {}

    Entity Scene::CreateEntity(const std::string &name)
    {
        return CreateEntityWithUUID(UUID(), name);
    }

    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string &name)
    {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent<TransformComponent>();
        auto &tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity);
    }

    void Scene::OnRuntimeStart()
    {
        // Creating World
        b2WorldDef physicsWorldDef = b2DefaultWorldDef();
        physicsWorldDef.gravity = {0.0f, -9.81f};
        physicsWorldDef.restitutionThreshold = 0.5f;

        m_PhysicsWorldID = b2CreateWorld(&physicsWorldDef);

        auto view = m_Registry.view<Rigidbody2DComponent>();
        for (auto e : view)
        {
            Entity entity = {e, this};
            auto &transform = entity.GetComponent<TransformComponent>();
            auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();

            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = (b2BodyType)rb2d.Type;
            bodyDef.position = {transform.Position.x, transform.Position.y};
            bodyDef.rotation = b2MakeRot(transform.Rotation.z);
            bodyDef.motionLocks.angularZ = rb2d.FixedRotation;

            b2BodyId bodyID = b2CreateBody(m_PhysicsWorldID, &bodyDef);
            rb2d.RuntimeBody = bodyID;

            if (entity.HasComponent<BoxColider2DComponent>())
            {
                auto &bc2d = entity.GetComponent<BoxColider2DComponent>();

                b2Polygon box = b2MakeBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.x * transform.Scale.y);

                b2ShapeDef shapeDef = b2DefaultShapeDef();
                shapeDef.density = bc2d.Density;
                shapeDef.material.friction = bc2d.Friction;
                shapeDef.material.restitution = bc2d.Restitution;

                b2ShapeId shapeID = b2CreatePolygonShape(bodyID, &shapeDef, &box);
                bc2d.RuntimeShape = shapeID;
            }
        }
    }

    void Scene::OnRuntimeStop()
    {
        if (b2World_IsValid(m_PhysicsWorldID))
        {
            b2DestroyWorld(m_PhysicsWorldID);
            m_PhysicsWorldID = b2_nullWorldId;
        }
    }

    void Scene::OnUpdateEditor(Timestep ts, EditorCamera &camera)
    {
        Renderer2D::BeginScene(camera);

        auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
        for (auto entity : view)
        {
            auto &transform = view.get<TransformComponent>(entity);
            auto &sprite = view.get<SpriteRendererComponent>(entity);

            Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
        }

        Renderer2D::EndScene();
    }

    void Scene::OnUpdateRuntime(Timestep ts)
    {
        // Scripts
        {
            m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto &nsc)
                                                          {
                if (!nsc.Instance)
                {
                    nsc.Instance = nsc.InstantiateScript();
                    nsc.Instance->m_Entity = Entity{ entity, this };
                    nsc.Instance->OnCreate();
                }

                nsc.Instance->OnUpdate(ts); });
        }

        // Physics
        {
            b2World_Step(m_PhysicsWorldID, ts, 4);

            auto view = m_Registry.view<Rigidbody2DComponent, TransformComponent>();
            for (auto e : view)
            {
                Entity entity = {e, this};
                auto &transform = entity.GetComponent<TransformComponent>();
                auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();

                b2BodyId bodyID = rb2d.RuntimeBody;
                const b2Vec2 &position = b2Body_GetPosition(bodyID);
                const b2Rot &rotation = b2Body_GetRotation(bodyID);

                transform.Position.x = position.x;
                transform.Position.y = position.y;
                transform.Rotation.z = b2Rot_GetAngle(rotation);
            }
        }

        // Render 2D
        Camera *mainCamera = nullptr;
        glm::mat4 cameraTransform;

        auto view = m_Registry.view<TransformComponent, CameraComponent>();
        for (auto entity : view)
        {
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

            if (camera.Primary)
            {
                mainCamera = &camera.Camera;
                cameraTransform = transform.GetTransform();
                break;
            }
        }

        if (mainCamera)
        {
            Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

            auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
            for (auto entity : view)
            {
                auto &transform = view.get<TransformComponent>(entity);
                auto &sprite = view.get<SpriteRendererComponent>(entity);

                Renderer2D::DrawSprite(transform.GetTransform(), sprite);
            }

            Renderer2D::EndScene();
        }
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto &cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio)
                cameraComponent.Camera.SetViewportSize(width, height);
        }
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            const auto &camera = view.get<CameraComponent>(entity);
            if (camera.Primary)
                return Entity{entity, this};

            return {};
        }
    }

    template <typename T>
    void Scene::OnComponentAdded(Entity entity, T &component)
    {
        static_assert(false);
    }

    template <>
    void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent &component)
    {
    }

    template <>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent &component)
    {
    }

    template <>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent &component)
    {
        if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
            component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }

    template <>
    void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent &component)
    {
    }

    template <>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent &component)
    {
    }

    template <>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent &component)
    {
    }

    template <>
    void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent &component)
    {
    }

    template <>
    void Scene::OnComponentAdded<BoxColider2DComponent>(Entity entity, BoxColider2DComponent &component)
    {
    }

} // namespace Myka
