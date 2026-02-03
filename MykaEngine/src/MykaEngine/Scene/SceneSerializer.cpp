#include "mykapch.hpp"
#include "SceneSerializer.hpp"
#include "Components.hpp"
#include "MykaEngine/Renderer/IBLBaker.hpp"

#include <nlohmann/json.hpp>

namespace glm
{
    // Arbitrary types in nlohmann/json library: https://json.nlohmann.me/features/arbitrary_types

    // Vec2
    inline void to_json(nlohmann::json &j, const vec2 &v)
    {
        j = nlohmann::json::array({v.x, v.y});
    }

    inline void from_json(const nlohmann::json &j, vec2 &v)
    {
        if (!j.is_array() || j.size() != 2)
        {
            MYKA_CORE_ERROR("Invalid JSON for glm::vec2");
        }

        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
    }

    // Vec3
    inline void to_json(nlohmann::json &j, const vec3 &v)
    {
        j = nlohmann::json::array({v.x, v.y, v.z});
    }

    inline void from_json(const nlohmann::json &j, vec3 &v)
    {
        if (!j.is_array() || j.size() != 3)
        {
            MYKA_CORE_ERROR("Invalid JSON for glm::vec3");
        }

        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
    }

    // Vec4
    inline void to_json(nlohmann::json &j, const vec4 &v)
    {
        j = nlohmann::json::array({v.x, v.y, v.z, v.w});
    }

    inline void from_json(const nlohmann::json &j, vec4 &v)
    {
        if (!j.is_array() || j.size() != 4)
        {
            MYKA_CORE_ERROR("Invalid JSON for glm::vec4");
        }

        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
        v.w = j.at(3).get<float>();
    }
} // namespace glm

namespace Myka
{
    namespace Utils
    {
        static std::string Rigidbody3DBodyTypeToString(Rigidbody3DComponent::BodyType type)
        {
            switch (type)
            {
            case Rigidbody3DComponent::BodyType::Static:
                return "Static";
            case Rigidbody3DComponent::BodyType::Kinematic:
                return "Kinematic";
            case Rigidbody3DComponent::BodyType::Dynamic:
                return "Dynamic";

            default:
            {
                MYKA_CORE_ASSERT(false, "Unknown body type");
                return "";
            }
            }
        }

        static Rigidbody3DComponent::BodyType Rigidbody3DBodyTypeFromString(std::string type)
        {
            if (type == "Static")
                return Rigidbody3DComponent::BodyType::Static;
            if (type == "Kinematic")
                return Rigidbody3DComponent::BodyType::Kinematic;
            if (type == "Dynamic")
                return Rigidbody3DComponent::BodyType::Dynamic;

            return Rigidbody3DComponent::BodyType::Static;
        }

        static std::string Rigidbody2DBodyTypeToString(Rigidbody2DComponent::BodyType type)
        {
            switch (type)
            {
            case Rigidbody2DComponent::BodyType::Static:
                return "Static";
            case Rigidbody2DComponent::BodyType::Kinematic:
                return "Kinematic";
            case Rigidbody2DComponent::BodyType::Dynamic:
                return "Dynamic";

            default:
            {
                MYKA_CORE_ASSERT(false, "Unknown body type");
                return "";
            }
            }
        }

        static Rigidbody2DComponent::BodyType Rigidbody2DBodyTypeFromString(std::string type)
        {
            if (type == "Static")
                return Rigidbody2DComponent::BodyType::Static;
            if (type == "Kinematic")
                return Rigidbody2DComponent::BodyType::Kinematic;
            if (type == "Dynamic")
                return Rigidbody2DComponent::BodyType::Dynamic;

            return Rigidbody2DComponent::BodyType::Static;
        }
    } // namespace Utils

    using json = nlohmann::json;

    SceneSerializer::SceneSerializer(const Ref<Scene> &scene) : m_Scene(scene)
    {
    }

    static void SerializeEntity(json &out, Entity entity)
    {
        MYKA_CORE_ASSERT(entity.HasComponent<IDComponent>());

        json e;
        e["Entity"] = static_cast<uint64_t>(entity.GetComponent<IDComponent>().ID);

        if (entity.HasComponent<TagComponent>())
        {
            const auto &tag = entity.GetComponent<TagComponent>().Tag;

            e["TagComponent"] = {
                {"Tag", tag},
            };
        }

        if (entity.HasComponent<TransformComponent>())
        {
            const auto &tc = entity.GetComponent<TransformComponent>();

            e["TransformComponent"] = {
                {"Position", tc.Position},
                {"Rotation", tc.Rotation},
                {"Scale", tc.Scale},
            };
        }

        if (entity.HasComponent<CameraComponent>())
        {
            const auto &cameraComponent = entity.GetComponent<CameraComponent>();
            const auto &camera = cameraComponent.Camera;

            e["CameraComponent"] = {
                {"Camera", {
                               {"ProjectionType", static_cast<int>(camera.GetProjectionType())},
                               {"PerspectiveFOV", camera.GetPerspectiveFOV()},
                               {"PerspectiveNear", camera.GetPerspectiveNear()},
                               {"PerspectiveFar", camera.GetPerspectiveFar()},
                               {"OrthographicSize", camera.GetOrthographicSize()},
                               {"OrthographicNear", camera.GetOrthographicNear()},
                               {"OrthographicFar", camera.GetOrthographicFar()},
                           }},
                {"Primary", cameraComponent.Primary},
                {"FixedAspectRatio", cameraComponent.FixedAspectRatio},
            };
        }

        if (entity.HasComponent<PointLightComponent>())
        {
            const auto &plc = entity.GetComponent<PointLightComponent>();

            json componentData;
            componentData["Color"] = plc.Color;
            componentData["Intensity"] = plc.Intensity;
            componentData["Radius"] = plc.Radius;
            componentData["Falloff"] = plc.Falloff;

            e["PointLightComponent"] = componentData;
        }

        if (entity.HasComponent<DirectionalLightComponent>())
        {
            const auto &dlc = entity.GetComponent<DirectionalLightComponent>();

            json componentData;
            componentData["Color"] = dlc.Color;
            componentData["Intensity"] = dlc.Intensity;
            componentData["CastShadows"] = dlc.CastShadows;

            e["DirectionalLightComponent"] = componentData;
        }

        if (entity.HasComponent<SpotLightComponent>())
        {
            const auto &slc = entity.GetComponent<SpotLightComponent>();

            json componentData;
            componentData["Color"] = slc.Color;
            componentData["Intensity"] = slc.Intensity;
            componentData["Range"] = slc.Range;
            componentData["InnerCutoff"] = slc.InnerCutoff;
            componentData["OuterCutoff"] = slc.OuterCutoff;

            e["SpotLightComponent"] = componentData;
        }

        if (entity.HasComponent<SkyLightComponent>())
        {
            const auto &slc = entity.GetComponent<SkyLightComponent>();

            json componentData;
            componentData["Intensity"] = slc.Intensity;
            componentData["SourcePath"] = slc.SourcePath;

            e["SkyLightComponent"] = componentData;
        }

        if (entity.HasComponent<ModelComponent>())
        {
            const auto &mc = entity.GetComponent<ModelComponent>();

            json componentData;
            if (mc._Model)
                componentData["ModelPath"] = mc._Model->GetPath();
            else
                componentData["ModelPath"] = "";

            e["ModelComponent"] = componentData;
        }

        if (entity.HasComponent<SpriteRendererComponent>())
        {
            const auto &src = entity.GetComponent<SpriteRendererComponent>();
            json componentData;
            if (src.Texture)
                componentData["TexturePath"] = src.Texture->GetPath();
            else
                componentData["TexturePath"] = "";

            componentData["Color"] = src.Color;
            componentData["TilingFactor"] = src.TilingFactor;

            e["SpriteRendererComponent"] = componentData;
        }

        if (entity.HasComponent<CircleRendererComponent>())
        {
            const auto &src = entity.GetComponent<CircleRendererComponent>();
            json componentData;

            componentData["Color"] = src.Color;
            componentData["Thickness"] = src.Thickness;
            componentData["Fade"] = src.Fade;

            e["CircleRendererComponent"] = componentData;
        }

        // 3D
        if (entity.HasComponent<Rigidbody3DComponent>())
        {
            const auto &rb3d = entity.GetComponent<Rigidbody3DComponent>();

            json componentData;
            componentData["Temp"] = 0;
            
            e["Rigidbody3DComponent"] = componentData;
        }

        if (entity.HasComponent<BoxCollider3DComponent>())
        {
            const auto &bc3d = entity.GetComponent<BoxCollider3DComponent>();

            json componentData;
            componentData["Temp"] = 0;

            e["BoxCollider3DComponent"] = componentData;
        }

        if (entity.HasComponent<SphereCollider3DComponent>())
        {
            const auto &sc3d = entity.GetComponent<SphereCollider3DComponent>();

            json componentData;
            componentData["Temp"] = 0;
            
            e["SphereCollider3DComponent"] = componentData;
        }

        // 2D
        if (entity.HasComponent<Rigidbody2DComponent>())
        {
            const auto &rb2d = entity.GetComponent<Rigidbody2DComponent>();

            json componentData;
            componentData["BodyType"] = Utils::Rigidbody2DBodyTypeToString(rb2d.Type);
            componentData["FixedRotation"] = rb2d.FixedRotation;

            e["Rigidbody2DComponent"] = componentData;
        }

        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            const auto &bc2d = entity.GetComponent<BoxCollider2DComponent>();

            json componentData;
            componentData["Offset"] = bc2d.Offset;
            componentData["Size"] = bc2d.Size;

            componentData["Density"] = bc2d.Density;
            componentData["Friction"] = bc2d.Friction;
            componentData["Restitution"] = bc2d.Restitution;

            e["BoxCollider2DComponent"] = componentData;
        }

        if (entity.HasComponent<CircleCollider2DComponent>())
        {
            const auto &cc2d = entity.GetComponent<CircleCollider2DComponent>();

            json componentData;
            componentData["Offset"] = cc2d.Offset;
            componentData["Radius"] = cc2d.Radius;

            componentData["Density"] = cc2d.Density;
            componentData["Friction"] = cc2d.Friction;
            componentData["Restitution"] = cc2d.Restitution;

            e["CircleCollider2DComponent"] = componentData;
        }

        out.push_back(e);
    }

    void SceneSerializer::SerializeJSON(const std::filesystem::path &filepath)
    {
        json data;

        // Scene
        std::string sceneName = "Untilted";
        data["Scene"] = sceneName;

        // Entites
        json entities = json::array();
        for (auto entity : m_Scene->m_Registry.view<entt::entity>())
        {
            Entity e = {entity, m_Scene.get()};
            if (!e)
            {
                MYKA_CORE_WARN("Invalid Entity in Scene: {0}", sceneName);
                continue;
            }

            SerializeEntity(entities, e);
        }
        data["Entities"] = entities;

        // output
        std::ofstream out(filepath);
        out << data.dump(4);
        out.close();
    }

    void SceneSerializer::SerializeBinary(const std::filesystem::path &filepath)
    {
        // TODO:
        MYKA_CORE_ASSERT(false, "Not implemented yet");
    }

    bool SceneSerializer::DeserializeJSON(const std::filesystem::path &filepath)
    {
        std::ifstream in(filepath);
        json data = json::parse(in);

        if (!data.contains("Scene") || data["Scene"].is_null())
        {
            MYKA_CORE_ERROR("Cannot DeserializeJSON: {0}", filepath.generic_string());
            return false;
        }

        std::string sceneName = data["Scene"];
        MYKA_CORE_TRACE("Deserializing scene: {0}", sceneName);

        for (auto entity : data["Entities"])
        {
            uint64_t uuid = entity["Entity"];

            std::string name;
            auto tagComponent = entity["TagComponent"];
            if (!tagComponent.is_null())
                name = tagComponent["Tag"];

            Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

            auto transformComponentJson = entity["TransformComponent"];
            if (!transformComponentJson.is_null())
            {
                auto &tc = deserializedEntity.GetComponent<TransformComponent>();
                tc.Position = transformComponentJson["Position"];
                tc.Rotation = transformComponentJson["Rotation"];
                tc.Scale = transformComponentJson["Scale"];
            }

            auto cameraComponentJson = entity["CameraComponent"];
            if (!cameraComponentJson.is_null())
            {
                auto &cc = deserializedEntity.AddComponent<CameraComponent>();

                auto cameraJson = cameraComponentJson["Camera"];
                cc.Camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(cameraJson["ProjectionType"]));

                cc.Camera.SetPerspectiveFOV(cameraJson["PerspectiveFOV"]);
                cc.Camera.SetPerspectiveNear(cameraJson["PerspectiveNear"]);
                cc.Camera.SetPerspectiveFar(cameraJson["PerspectiveFar"]);

                cc.Camera.SetOrthographicSize(cameraJson["OrthographicSize"]);
                cc.Camera.SetOrthographicNear(cameraJson["OrthographicNear"]);
                cc.Camera.SetOrthographicFar(cameraJson["OrthographicFar"]);

                cc.Primary = cameraComponentJson["Primary"];
                cc.FixedAspectRatio = cameraComponentJson["FixedAspectRatio"];
            }

            auto pointLightComponentJson = entity["PointLightComponent"];
            if (!pointLightComponentJson.is_null())
            {
                auto &plc = deserializedEntity.AddComponent<PointLightComponent>();

                plc.Color = pointLightComponentJson["Color"];
                plc.Intensity = pointLightComponentJson["Intensity"];
                plc.Radius = pointLightComponentJson["Radius"];
                plc.Falloff = pointLightComponentJson["Falloff"];
            }

            auto directionalLightComponentJson = entity["DirectionalLightComponent"];
            if (!directionalLightComponentJson.is_null())
            {
                auto &dlc = deserializedEntity.AddComponent<DirectionalLightComponent>();

                dlc.Color = directionalLightComponentJson["Color"];
                dlc.Intensity = directionalLightComponentJson["Intensity"];
                dlc.CastShadows = directionalLightComponentJson["CastShadows"];
            }

            auto spotLightComponentJson = entity["SpotLightComponent"];
            if (!spotLightComponentJson.is_null())
            {
                auto &slc = deserializedEntity.AddComponent<SpotLightComponent>();

                slc.Color = spotLightComponentJson["Color"];
                slc.Intensity = spotLightComponentJson["Intensity"];
                slc.Range = spotLightComponentJson["Range"];
                slc.InnerCutoff = spotLightComponentJson["InnerCutoff"];
                slc.OuterCutoff = spotLightComponentJson["OuterCutoff"];
            }

            auto skyLightComponentJson = entity["SkyLightComponent"];
            if (!skyLightComponentJson.is_null())
            {
                auto &slc = deserializedEntity.AddComponent<SkyLightComponent>();
                slc.Intensity = skyLightComponentJson.value("Intensity", 1.0f);
                slc.SourcePath = skyLightComponentJson.value("SourcePath", "");

                if (!slc.SourcePath.empty())
                {
                    auto panorama = Texture2D::Create(slc.SourcePath);

                    TextureSpecification spec;
                    spec.Width = 1024;
                    spec.Height = 1024;
                    spec.Format = ImageFormat::RGBA32F;
                    spec.GenerateMips = true;
                    slc.EnvironmentMap = CubeTexture::Create(spec);

                    IBLBaker::ConvertPanoramaToCubemap(panorama, slc.EnvironmentMap);
                    // IBLBaker::CreateIrradianceMap(slc.EnvironmentMap, slc.IrradianceMap);
                }
            }

            auto modelComponentJson = entity["ModelComponent"];
            if (!modelComponentJson.is_null())
            {
                auto &mc = deserializedEntity.AddComponent<ModelComponent>();

                std::filesystem::path modelPath = modelComponentJson.value("ModelPath", "");
                if (!modelPath.empty())
                {
                    mc._Model = Model::Create(modelPath);
                }
            }

            auto spriteRendererComponentJson = entity["SpriteRendererComponent"];
            if (!spriteRendererComponentJson.is_null())
            {
                auto &src = deserializedEntity.AddComponent<SpriteRendererComponent>();

                std::filesystem::path texturePath = spriteRendererComponentJson.value("TexturePath", "");
                if (!texturePath.empty())
                {
                    src.Texture = Texture2D::Create(texturePath);
                }
                src.Color = spriteRendererComponentJson["Color"];
                src.TilingFactor = spriteRendererComponentJson["TilingFactor"];
            }

            auto circleRendererComponentJson = entity["CircleRendererComponent"];
            if (!circleRendererComponentJson.is_null())
            {
                auto &crc = deserializedEntity.AddComponent<CircleRendererComponent>();

                crc.Color = circleRendererComponentJson["Color"];
                crc.Thickness = circleRendererComponentJson["Thickness"];
                crc.Fade = circleRendererComponentJson["Fade"];
            }

            auto rigidbody3DComponentComponentJson = entity["Rigidbody3DComponent"];
            if (!rigidbody3DComponentComponentJson.is_null())
            {
                auto &rd3d = deserializedEntity.AddComponent<Rigidbody3DComponent>();
            }

            auto boxCollider3DComponentComponentJson = entity["BoxCollider3DComponent"];
            if (!boxCollider3DComponentComponentJson.is_null())
            {
                auto &bc3d = deserializedEntity.AddComponent<BoxCollider3DComponent>();
            }

            auto sphereCollider3DComponentJson = entity["SphereCollider3DComponent"];
            if (!sphereCollider3DComponentJson.is_null())
            {
                auto &sc3d = deserializedEntity.AddComponent<SphereCollider3DComponent>();
            }

            auto rigidbody2DComponentComponentJson = entity["Rigidbody2DComponent"];
            if (!rigidbody2DComponentComponentJson.is_null())
            {
                auto &rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
                rb2d.Type = Utils::Rigidbody2DBodyTypeFromString(rigidbody2DComponentComponentJson["BodyType"]);
                rb2d.FixedRotation = rigidbody2DComponentComponentJson["FixedRotation"];
            }

            auto boxCollider2DComponentComponentJson = entity["BoxCollider2DComponent"];
            if (!boxCollider2DComponentComponentJson.is_null())
            {
                auto &bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
                bc2d.Offset = boxCollider2DComponentComponentJson["Offset"];
                bc2d.Size = boxCollider2DComponentComponentJson["Size"];
                bc2d.Density = boxCollider2DComponentComponentJson["Density"];
                bc2d.Friction = boxCollider2DComponentComponentJson["Friction"];
                bc2d.Restitution = boxCollider2DComponentComponentJson["Restitution"];
            }

            auto circleCollider2DComponentJson = entity["CircleCollider2DComponent"];
            if (!circleCollider2DComponentJson.is_null())
            {
                auto &cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
                cc2d.Offset = circleCollider2DComponentJson["Offset"];
                cc2d.Radius = circleCollider2DComponentJson["Radius"];
                cc2d.Density = circleCollider2DComponentJson["Density"];
                cc2d.Friction = circleCollider2DComponentJson["Friction"];
                cc2d.Restitution = circleCollider2DComponentJson["Restitution"];
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeBinary(const std::filesystem::path &filepath)
    {
        // TODO:
        MYKA_CORE_ASSERT(false, "Not implemented yet");
        return false;
    }

} // namespace Myka
