#pragma once

#include "EditorCamera.hpp"
#include "MykaEngine/Scene/Components.hpp"
#include "UniformBuffer.hpp"
#include "Shader.hpp"
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
            float Intensity;
            glm::vec3 Color;
            float Radius;
            float Falloff;
            float _padding[3];
        };

        struct DirectionalLight
        {
            glm::vec3 Direction;
            float Intensity;
            glm::vec3 Color;
            int CastShadows;
        };

        struct SpotLight
        {
            glm::vec3 Position;
            float Intensity;
            glm::vec3 Direction;
            float Range;
            glm::vec3 Color;
            float InnerCutoff;
            float OuterCutoff;
            float _padding[3];
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
        static void BeginScene(const Camera &camera, const glm::mat4 &transform, const SceneLightData &lightData);
        static void EndScene();

        static void DrawMesh(const Ref<Mesh> &mesh, const Ref<Material> &material, const glm::mat4 &transform, int entityID = -1);
        static void DrawSkybox(const Ref<CubeTexture> &skybox, float intensity);
        static void DrawModel(const Ref<Model> &model, const glm::mat4 &transform, int entityID = -1);

        static void SetCameraMatrices(const glm::mat4 &proj, const glm::mat4 &view);
        static Ref<Shader> GetEquirectToCubeShader();
        static void DrawCubeMesh();

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
