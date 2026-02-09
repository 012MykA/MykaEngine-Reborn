#pragma once

// MykaEngine
#include "SceneCamera.hpp"
#include "ScriptableEntity.hpp"
#include "MykaEngine/Renderer/Texture.hpp"
#include "MykaEngine/Core/UUID.hpp"
#include "MykaEngine/Renderer/Model.hpp"
#include "MykaEngine/Renderer/Mesh.hpp"
#include "MykaEngine/Renderer/Material.hpp"

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

// box2d
#include "box2d/id.h"

namespace Myka
{
    struct IDComponent
    {
        UUID ID;

        IDComponent() = default;
        IDComponent(const IDComponent &) = default;
        IDComponent(const UUID &uuid) : ID(uuid) {}
    };

    struct TagComponent
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent &) = default;
        TagComponent(const std::string &tag) : Tag(tag) {}
    };

    struct TransformComponent
    {
        glm::vec3 Position = {0.0f, 0.0f, 0.0f};
        glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
        glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

        TransformComponent() = default;
        TransformComponent(const TransformComponent &) = default;
        TransformComponent(const glm::vec3 &position) : Position(position) {}

        glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }
    };

    struct PointLightComponent
    {
        glm::vec3 Color = {1.0f, 1.0f, 1.0f};
        float Intensity = 1.0f;
        float Radius = 10.0f;
        float Falloff = 1.0f;

        PointLightComponent() = default;
        PointLightComponent(const PointLightComponent &) = default;
        PointLightComponent(const glm::vec3 &color) : Color(color) {}
    };

    struct DirectionalLightComponent
    {
        glm::vec3 Color = {1.0f, 1.0f, 1.0f};
        float Intensity = 1.0f;
        bool CastShadows = true;

        DirectionalLightComponent() = default;
        DirectionalLightComponent(const DirectionalLightComponent &) = default;
        DirectionalLightComponent(const glm::vec3 &color) : Color(color) {}
    };

    struct SpotLightComponent
    {
        glm::vec3 Color = {1.0f, 1.0f, 1.0f};
        float Intensity = 1.0f;
        float Range = 10.0f;
        float InnerCutoff = 12.5f;
        float OuterCutoff = 15.0f;

        SpotLightComponent() = default;
        SpotLightComponent(const SpotLightComponent &) = default;
        SpotLightComponent(const glm::vec3 &color) : Color(color) {}
    };

    struct SkyLightComponent
    {
        float Intensity = 1.0f;
        std::filesystem::path SourcePath;

        Ref<CubeTexture> EnvironmentMap;
        Ref<CubeTexture> IrradianceMap;
        Ref<CubeTexture> PrefilterMap;

        SkyLightComponent() = default;
        SkyLightComponent(const SkyLightComponent &) = default;
    };

    struct ModelComponent
    {
        Ref<Model> _Model;

        ModelComponent() = default;
        ModelComponent(const ModelComponent &) = default;
        ModelComponent(const Ref<Model> &model) : _Model(model) {}
    };

    struct MeshComponent
    {
        Ref<Mesh> _Mesh;

        MeshComponent() = default;
        MeshComponent(const MeshComponent &) = default;
        MeshComponent(const Ref<Mesh> &mesh) : _Mesh(mesh) {}
    };

    struct MaterialComponent
    {
        Ref<Material> _Material = Material::Create();

        MaterialComponent() = default;
        MaterialComponent(const MaterialComponent &) = default;
        MaterialComponent(const Ref<Material> &material) : _Material(material) {}
    };

    struct SpriteRendererComponent
    {
        Ref<Texture2D> Texture;
        glm::vec4 Color = glm::vec4(1.0f);
        float TilingFactor = 1.0f;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent &) = default;
        SpriteRendererComponent(const glm::vec4 &color) : Color(color) {}

        operator glm::vec4 &() { return Color; }
        operator const glm::vec4 &() const { return Color; }
    };

    struct CircleRendererComponent
    {
        glm::vec4 Color = glm::vec4(1.0f);
        float Radius = 0.5f; // 1m
        float Thickness = 1.0f;
        float Fade = 0.005f;

        CircleRendererComponent() = default;
        CircleRendererComponent(const CircleRendererComponent &) = default;
    };

    struct CameraComponent
    {
        SceneCamera Camera;
        bool Primary = true;
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent &) = default;
    };

    struct NativeScriptComponent
    {
        ScriptableEntity *Instance = nullptr;

        ScriptableEntity *(*InstantiateScript)();
        void (*DestroyScript)(NativeScriptComponent *);

        template <typename T>
        void Bind()
        {
            InstantiateScript = []()
            { return static_cast<ScriptableEntity *>(new T()); };

            DestroyScript = [](NativeScriptComponent *nsc)
            { delete nsc->Instance; nsc->Instance = nullptr; };
        }
    };

    struct Rigidbody3DComponent
    {
        enum class BodyType
        {
            Static = 0,
            Kinematic,
            Dynamic
        };
        BodyType Type = BodyType::Static;

        bool AutoMass = true;
        float Mass = 1.0f;
        bool GravityEffect = false;
        glm::vec3 InitialVelocity{0.0f};

        Physics3D::Body RuntimeBody;

        Rigidbody3DComponent() = default;
        Rigidbody3DComponent(const Rigidbody3DComponent &) = default;
    };

    struct BoxCollider3DComponent
    {
        glm::vec3 Size = {1.0f, 1.0f, 1.0f};
        glm::vec3 Offset = {0.0f, 0.0f, 0.0f};

        float Density = 1000.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;

        BoxCollider3DComponent() = default;
        BoxCollider3DComponent(const BoxCollider3DComponent &) = default;
    };

    struct SphereCollider3DComponent
    {
        float Radius = 1.0f;
        glm::vec3 Offset = {0.0f, 0.0f, 0.0f};

        float Density = 1000.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;

        SphereCollider3DComponent() = default;
        SphereCollider3DComponent(const SphereCollider3DComponent &) = default;
    };

    struct Rigidbody2DComponent
    {
        enum class BodyType
        {
            Static = 0,
            Kinematic,
            Dynamic
        };
        BodyType Type = BodyType::Static;
        bool FixedRotation = false;

        b2BodyId RuntimeBody = b2_nullBodyId;

        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent &) = default;
    };

    struct BoxCollider2DComponent
    {
        glm::vec2 Size = {0.5f, 0.5f};
        glm::vec2 Offset = {0.0f, 0.0f};

        // TODO: move into Physics Matrial;
        float Density = 1000.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;

        b2ShapeId RuntimeShape = b2_nullShapeId;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent &) = default;
    };

    struct CircleCollider2DComponent
    {
        float Radius = 0.5f;
        glm::vec2 Offset = {0.0f, 0.0f};

        // TODO: move into Physics Matrial;
        float Density = 1000.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;

        b2ShapeId RuntimeShape = b2_nullShapeId;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent &) = default;
    };

    template <typename... Component>
    struct ComponentGroup
    {
    };

    using AllComponents = ComponentGroup<TransformComponent,
                                         SkyLightComponent,
                                         PointLightComponent, DirectionalLightComponent, SpotLightComponent,
                                         MeshComponent, MaterialComponent, ModelComponent,
                                         SpriteRendererComponent, CircleRendererComponent,
                                         CameraComponent,
                                         NativeScriptComponent,
                                         Rigidbody3DComponent, BoxCollider3DComponent, SphereCollider3DComponent,
                                         Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent>;

} // namespace Myka
