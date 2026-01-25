#include "mykapch.hpp"
#include "Scene.hpp"
#include "Components.hpp"
#include "MykaEngine/Renderer/Renderer2D.hpp"
#include "MykaEngine/Renderer/Renderer3D.hpp"
#include "Entity.hpp"

#include <glm/glm.hpp>

#include "box2d/box2d.h"
#include "box2d/id.h"
#include "box2d/collision.h"

namespace Myka
{
    template <typename... Component>
    static void CopyComponent(entt::registry &dst, const entt::registry &src, const std::unordered_map<UUID, entt::entity> &enttMap)
    {
        // Expand the parameter pack using a fold expression with a lambda.
        // This executes the following block for each type in Component...
        ([&]()
         {
            auto view = src.view<Component>();
            for (auto srcEntity : view)
            {
                entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

                auto& srcComponent = src.get<Component>(srcEntity);
                dst.emplace_or_replace<Component>(dstEntity, srcComponent);
            } }(), ...);
    }

    template <typename... Component>
    static void CopyComponent(ComponentGroup<Component...>, entt::registry &dst, const entt::registry &src, const std::unordered_map<UUID, entt::entity> &enttMap)
    {
        CopyComponent<Component...>(dst, src, enttMap);
    }

    template <typename... Component>
    static void CopyComponentIfExists(Entity dst, Entity src)
    {
        ([&]()
         {
            if (src.HasComponent<Component>())
                dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>()); }(), ...);
    }

    template <typename... Component>
    static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
    {
        CopyComponentIfExists<Component...>(dst, src);
    }

    Ref<Scene> Scene::Copy(Ref<Scene> other)
    {
        Ref<Scene> newScene = CreateRef<Scene>();

        newScene->m_ViewportWidth = other->m_ViewportWidth;
        newScene->m_ViewportHeight = other->m_ViewportHeight;

        std::unordered_map<UUID, entt::entity> enttMap;

        auto &srcSceneRegistry = other->m_Registry;
        auto &dstSceneRegistry = newScene->m_Registry;

        // Create entity in new Scene
        auto idView = srcSceneRegistry.view<IDComponent>();
        for (auto e : idView)
        {
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
            const auto &name = srcSceneRegistry.get<TagComponent>(e).Tag;
            Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
            enttMap[uuid] = (entt::entity)newEntity;
        }

        // Copy  Components (except IDComponent nad TagComponent)
        CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

        return newScene;
    }

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
        OnPhysics2DStart();
    }

    void Scene::OnRuntimeStop()
    {
        OnPhysics2DStop();
    }

    void Scene::OnUpdateEditor(Timestep ts, EditorCamera &camera)
    {
        // Render
        RenderScene(camera);
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

            // Draw sprites
            {
                auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
                for (auto entity : view)
                {
                    auto &transform = view.get<TransformComponent>(entity);
                    auto &sprite = view.get<SpriteRendererComponent>(entity);

                    Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
                }
            }

            // Draw circles
            {
                auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
                for (auto entity : view)
                {
                    auto &transform = view.get<TransformComponent>(entity);
                    auto &circle = view.get<CircleRendererComponent>(entity);

                    Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, static_cast<int>(entity));
                }
            }

            Renderer2D::EndScene();
        }
    }

    void Scene::OnSimulationStart()
    {
        OnPhysics2DStart();
    }

    void Scene::OnSimulationStop()
    {
        OnPhysics2DStop();
    }

    void Scene::OnUpdateSimulation(Timestep ts, EditorCamera &camera)
    {
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

        // Render
        RenderScene(camera);
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

    Entity Scene::DuplicateEntity(Entity entity)
    {
        Entity newEntity = CreateEntity(entity.GetComponent<TagComponent>().Tag);
        CopyComponentIfExists(AllComponents{}, newEntity, entity);
        return newEntity;
    }

    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            const auto &camera = view.get<CameraComponent>(entity);
            if (camera.Primary)
                return Entity{entity, this};
        }

        return {};
    }

    void Scene::OnPhysics2DStart()
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

            if (entity.HasComponent<BoxCollider2DComponent>())
            {
                auto &bc2d = entity.GetComponent<BoxCollider2DComponent>();

                float hx = (bc2d.Size.x * transform.Scale.x) * 0.5f;
                float hy = (bc2d.Size.y * transform.Scale.y) * 0.5f;

                b2Vec2 center = {bc2d.Offset.x * transform.Scale.x, bc2d.Offset.y * transform.Scale.y};
                b2Polygon box = b2MakeOffsetBox(hx, hy, center, b2Rot_identity);

                b2ShapeDef shapeDef = b2DefaultShapeDef();
                shapeDef.density = bc2d.Density;
                shapeDef.material.friction = bc2d.Friction;
                shapeDef.material.restitution = bc2d.Restitution;

                bc2d.RuntimeShape = b2CreatePolygonShape(bodyID, &shapeDef, &box);
            }

            if (entity.HasComponent<CircleCollider2DComponent>())
            {
                auto &cc2d = entity.GetComponent<CircleCollider2DComponent>();

                b2Circle circle;
                circle.center = {cc2d.Offset.x * transform.Scale.x, cc2d.Offset.y * transform.Scale.y};
                circle.radius = cc2d.Radius * std::max(transform.Scale.x, transform.Scale.y);

                b2ShapeDef shapeDef = b2DefaultShapeDef();
                shapeDef.density = cc2d.Density;
                shapeDef.material.friction = cc2d.Friction;
                shapeDef.material.restitution = cc2d.Restitution;

                cc2d.RuntimeShape = b2CreateCircleShape(bodyID, &shapeDef, &circle);
            }
        }
    }

    void Scene::OnPhysics2DStop()
    {
        // Cleanup
        auto view = m_Registry.view<Rigidbody2DComponent>();
        for (auto e : view)
        {
            auto &rb2d = view.get<Rigidbody2DComponent>(e);
            rb2d.RuntimeBody = b2_nullBodyId;

            if (Entity entity = {e, this}; entity.HasComponent<BoxCollider2DComponent>())
            {
                auto &bc2d = entity.GetComponent<BoxCollider2DComponent>();
                bc2d.RuntimeShape = b2_nullShapeId;
            }

            if (Entity entity = {e, this}; entity.HasComponent<CircleCollider2DComponent>())
            {
                auto &cc2d = entity.GetComponent<CircleCollider2DComponent>();
                cc2d.RuntimeShape = b2_nullShapeId;
            }
        }

        if (b2World_IsValid(m_PhysicsWorldID))
        {
            b2DestroyWorld(m_PhysicsWorldID);
            m_PhysicsWorldID = b2_nullWorldId;
        }
    }

    void Scene::RenderScene(EditorCamera &camera)
    {
        Renderer3D::SceneLightData lightData;

        auto pointView = m_Registry.view<TransformComponent, PointLightComponent>();
        for (auto entity : pointView)
        {
            auto [tc, plc] = pointView.get<TransformComponent, PointLightComponent>(entity);
            lightData.PointLights.push_back({tc.Position, plc.Intensity, plc.Color, plc.Radius, plc.Falloff});
        }

        auto dirView = m_Registry.view<TransformComponent, DirectionalLightComponent>();
        for (auto entity : dirView)
        {
            auto [tc, dlc] = dirView.get<TransformComponent, DirectionalLightComponent>(entity);
            glm::vec3 rotationInRadians = glm::vec3(
                glm::radians(tc.Rotation.x),
                glm::radians(tc.Rotation.y),
                glm::radians(tc.Rotation.z));
            glm::quat rotationQuat = glm::quat(rotationInRadians);
            glm::vec3 direction = rotationQuat * glm::vec3(0.0f, 0.0f, -1.0f);
            lightData.DirectionalLights.push_back({direction, dlc.Intensity, dlc.Color, dlc.CastShadows ? 1 : 0});
        }

        auto spotView = m_Registry.view<TransformComponent, SpotLightComponent>();
        for (auto entity : spotView)
        {
            auto [tc, slc] = spotView.get<TransformComponent, SpotLightComponent>(entity);
            glm::vec3 rotationInRadians = glm::vec3(
                glm::radians(tc.Rotation.x),
                glm::radians(tc.Rotation.y),
                glm::radians(tc.Rotation.z));
            glm::quat rotationQuat = glm::quat(rotationInRadians);
            glm::vec3 direction = rotationQuat * glm::vec3(0.0f, 0.0f, -1.0f);
            lightData.SpotLights.push_back({
                tc.Position,
                slc.Intensity,
                direction,
                slc.Range,
                slc.Color,
                glm::cos(glm::radians(slc.InnerCutoff)),
                glm::cos(glm::radians(slc.OuterCutoff)),
            });
        }

        Renderer3D::BeginScene(camera, lightData);

        // Draw models
        {
            auto view = m_Registry.view<TransformComponent, ModelComponent>();
            for (auto entity : view)
            {
                auto &tc = view.get<TransformComponent>(entity);
                auto &model = view.get<ModelComponent>(entity);

                if (model._Model)
                    Renderer3D::DrawModel(model._Model, tc.GetTransform(), static_cast<int>(entity));
            }
        }

        // Draw meshes
        {
            auto view = m_Registry.view<TransformComponent, MeshComponent, MaterialComponent>();
            for (auto entity : view)
            {
                auto &tc = view.get<TransformComponent>(entity);
                auto &mesh = view.get<MeshComponent>(entity)._Mesh;
                auto &material = view.get<MaterialComponent>(entity)._Material;

                if (mesh && material)
                    Renderer3D::DrawMesh(mesh, material, tc.GetTransform(), static_cast<int>(entity));
            }
        }

        Renderer3D::EndScene();

        Renderer2D::BeginScene(camera);

        // Draw sprites
        {
            auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
            for (auto entity : view)
            {
                auto &transform = view.get<TransformComponent>(entity);
                auto &sprite = view.get<SpriteRendererComponent>(entity);

                Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
            }
        }

        // Draw circles
        {
            auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
            for (auto entity : view)
            {
                auto &transform = view.get<TransformComponent>(entity);
                auto &circle = view.get<CircleRendererComponent>(entity);

                Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, static_cast<int>(entity));
            }
        }

        Renderer2D::EndScene();
    }

    template <typename T>
    void Scene::OnComponentAdded(Entity entity, T &component)
    {
        if constexpr (std::is_same_v<T, CameraComponent>)
        {
            if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
                component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
        }
    }
} // namespace Myka
