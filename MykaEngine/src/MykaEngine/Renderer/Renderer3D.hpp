#pragma once

#include "EditorCamera.hpp"
#include "MykaEngine/Scene/Components.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Model.hpp"

namespace Myka
{
    class Renderer3D
    {
    public:
        struct PointLight
        {
            glm::vec3 Position;
            glm::vec3 Color;
            float Intensity;
            float Radius;
            float Falloff;
        };

        struct DirectionalLight
        {
            glm::vec3 Direction;
            glm::vec3 Color;
            float Intensity;
            int CastShadows;
        };

        struct SpotLight
        {
            glm::vec3 Position;
            glm::vec3 Direction;
            glm::vec3 Color;
            float Intensity;
            float Range;
            float InnerCutoff;
            float OuterCutoff;
        };

        struct SceneLightData
        {
            std::vector<PointLight> PointLights;
            std::vector<DirectionalLight> DirectionalLights;
            std::vector<SpotLight> SpotLights;
        };

    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const EditorCamera &camera, const SceneLightData &lightData);
        static void EndScene();

        static void DrawMesh(const Ref<Mesh> &mesh, const Ref<Material> &material, const glm::mat4 &transform, int entityID = -1);

        static void DrawModel(const Ref<Model> &model, const glm::mat4 &transform, int entityID = -1);

        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t Triangles = 0;
            uint32_t ModelsCount = 0;
        };

        static void ResetStats();
        static Statistics GetStats();
    };
} // namespace Myka
