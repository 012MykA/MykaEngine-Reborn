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

    struct ModelComponent
    {
        Ref<Model> _Model;

        ModelComponent() = default;
        ModelComponent(const ModelComponent &) = default;
        ModelComponent(const Ref<Model> &model) : _Model(model) {}
    };

    struct MeshComponent
    {
        enum class MeshType
        {
            Custom = 0,
            Cube,
            Sphere,
            Plane,
            Cylinder,
            Cone,
            Torus
        };

        Ref<Mesh> _Mesh;
        MeshType Type;

        MeshComponent() = default;
        MeshComponent(const MeshComponent &) = default;
        MeshComponent(const Ref<Mesh> &mesh, MeshType type = MeshType::Custom) : _Mesh(mesh), Type(type) {}
        MeshComponent(MeshType type) : Type(type) {}
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
        glm::vec2 Offset = {0.0f, 0.0f};
        glm::vec2 Size = {0.5f, 0.5f};

        // TODO: move into Physics Matrial;
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;

        b2ShapeId RuntimeShape = b2_nullShapeId;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent &) = default;
    };

    struct CircleCollider2DComponent
    {
        glm::vec2 Offset = {0.0f, 0.0f};
        float Radius = 0.5f;

        // TODO: move into Physics Matrial;
        float Density = 1.0f;
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
                                         MeshComponent, MaterialComponent, ModelComponent,
                                         SpriteRendererComponent, CircleRendererComponent,
                                         CameraComponent,
                                         NativeScriptComponent,
                                         Rigidbody2DComponent,
                                         BoxCollider2DComponent, CircleCollider2DComponent>;

} // namespace Myka
