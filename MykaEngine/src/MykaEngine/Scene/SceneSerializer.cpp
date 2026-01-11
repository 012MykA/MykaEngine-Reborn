#include "mykapch.hpp"
#include "SceneSerializer.hpp"
#include "Components.hpp"

#include <nlohmann/json.hpp>

namespace glm
{
    // Arbitrary types in nlohmann/json library: https://json.nlohmann.me/features/arbitrary_types

    inline void to_json(nlohmann::json &j, const glm::vec3 &v)
    {
        j = nlohmann::json::array({v.x, v.y, v.z});
    }

    inline void to_json(nlohmann::json &j, const glm::vec4 &v)
    {
        j = nlohmann::json::array({v.x, v.y, v.z, v.w});
    }

    inline void from_json(const nlohmann::json &j, glm::vec3 &v)
    {
        if (!j.is_array() || j.size() != 3)
            throw std::runtime_error("Invalid JSON for glm::vec3");

        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
    }

    inline void from_json(const nlohmann::json &j, glm::vec4 &v)
    {
        if (!j.is_array() || j.size() != 4)
            throw std::runtime_error("Invalid JSON for glm::vec4");

        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
        v.w = j.at(3).get<float>();
    }
} // namespace glm

namespace Myka
{
    using json = nlohmann::json;

    SceneSerializer::SceneSerializer(const Ref<Scene> &scene) : m_Scene(scene)
    {
    }

    static void SerializeEntity(json &out, Entity entity)
    {
        json e;
        e["Entity"] = static_cast<uint32_t>(entity); // TODO: replace with UUID

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

        out.push_back(e);
    }

    void SceneSerializer::SerializeJSON(const std::filesystem::path &filepath)
    {
        json data;

        // Scene
        std::string sceneName = "Untilted"; // TODO: remove
        data["Scene"] = sceneName;

        // Entites
        json entities = json::array();
        for (auto entity : m_Scene->m_Registry.storage<entt::entity>())
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
            MYKA_CORE_ERROR("Cannot DeserializeJSON: {0}", filepath.string());
            return false;
        }

        std::string sceneName = data["Scene"];
        MYKA_CORE_TRACE("Deserializing scene: {0}", sceneName);

        for (auto entity : data["Entities"])
        {
            uint64_t uuid = entity["Entity"]; // TODO: UUID

            std::string name;
            auto tagComponent = entity["TagComponent"];
            if (!tagComponent.is_null())
                name = tagComponent["Tag"];

            Entity deserializedEntity = m_Scene->CreateEntity(name);

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
                src.TilingFactor = spriteRendererComponentJson.value("TilingFactor", 1.0f);
            }

            MYKA_CORE_TRACE("Deserialized entity: ID={0}, name={1}", uuid, name);
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
