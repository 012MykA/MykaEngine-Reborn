#include "mykapch.hpp"
#include "Renderer3D.hpp"

#include "Shader.hpp"
#include "UniformBuffer.hpp"
#include "RenderCommand.hpp"

namespace Myka
{
    struct Renderer3DData
    {
        static constexpr uint32_t MaxTextureSlots = 32ui32;
        static constexpr uint32_t MaxLights = 10ui32;
        static constexpr uint32_t MaxPointLights = MaxLights;
        static constexpr uint32_t MaxDirectionalLights = MaxLights;
        static constexpr uint32_t MaxSpotLights = MaxLights;

        Ref<Texture2D> WhiteTexture;
        Ref<Texture2D> DefaultPBRTexture;
        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture

        struct SkyboxSettings
        {
            float Intensity = 1.0f;
            float _padding[3];
        } SkyboxBuffer;
        Ref<UniformBuffer> SkyboxUniformBuffer;

        struct EntityData
        {
            glm::mat4 Transform;
            glm::vec4 AlbedoColor;
            int AlbedoTexIndex;
            int MetRoughTexIndex;
            float Metallic;
            float Roughness;
            int EntityID;
            float _padding[3];
        } EntityBuffer;
        Ref<UniformBuffer> EntityUniformBuffer;

        Ref<Shader> PBRShader;
        Ref<Shader> SkyboxShader;
        Ref<Shader> EquirectToCubeShader;

        Ref<Mesh> SkyboxMesh; // TODO: remove

        struct LightBufferData
        {
            Renderer3D::PointLight PointLights[MaxPointLights];
            Renderer3D::DirectionalLight DirectionalLights[MaxDirectionalLights];
            Renderer3D::SpotLight SpotLights[MaxSpotLights];
            uint32_t PointCount = 0;
            uint32_t DirectionalCount = 0;
            uint32_t SpotCount = 0;
            uint32_t _padding;
        } LightBuffer;
        Ref<UniformBuffer> LightUniformBuffer;

        struct CameraData
        {
            glm::mat4 View;
            glm::mat4 Projection;
            glm::vec3 Position;
            float _padding;
        } CameraBuffer;
        Ref<UniformBuffer> CameraUniformBuffer;

        Renderer3D::Statistics Stats;
    };

    static Renderer3DData s_Data;

    void Renderer3D::Init()
    {
        s_Data.WhiteTexture = Texture2D::Create(TextureSpecification());
        const uint32_t whiteTextureData = 0xFFFFFFFF;
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        s_Data.DefaultPBRTexture = Texture2D::Create(TextureSpecification());
        uint32_t pbrData = 0xFFFFFFFF;
        s_Data.DefaultPBRTexture->SetData(&pbrData, sizeof(uint32_t));

        std::vector<Vertex> cubeVertices = {
            {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},

            {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},

            {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
            {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
            {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},

            {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
            {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        };

        std::vector<uint32_t> cubeIndices = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            8, 9, 10, 10, 11, 8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20};

        s_Data.SkyboxMesh = CreateRef<Mesh>(cubeVertices, cubeIndices);

        s_Data.TextureSlots[0] = s_Data.WhiteTexture;
        s_Data.TextureSlots[1] = s_Data.DefaultPBRTexture;

        s_Data.PBRShader = Shader::Create("assets/shaders/Renderer3D_PBR.glsl");
        s_Data.SkyboxShader = Shader::Create("assets/shaders/Renderer3D_Skybox.glsl");
        s_Data.EquirectToCubeShader = Shader::Create("assets/shaders/IBLBaker_EquirectToCube.glsl");

        s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::CameraData), 0);
        s_Data.EntityUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::EntityData), 1);
        s_Data.LightUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::LightBufferData), 2);
        s_Data.SkyboxUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::SkyboxSettings), 3);
    }

    void Renderer3D::Shutdown() {}

    void Renderer3D::BeginScene(const EditorCamera &camera, const SceneLightData &lightData)
    {
        s_Data.CameraBuffer.View = camera.GetViewMatrix();
        s_Data.CameraBuffer.Projection = camera.GetProjection();
        s_Data.CameraBuffer.Position = camera.GetPosition();
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData));

        std::memset(&s_Data.LightBuffer, 0, sizeof(Renderer3DData::LightBuffer));

        s_Data.LightBuffer.PointCount = std::min(static_cast<uint32_t>(lightData.PointLights.size()), Renderer3DData::MaxPointLights);
        for (uint32_t i = 0; i < s_Data.LightBuffer.PointCount; ++i)
            s_Data.LightBuffer.PointLights[i] = lightData.PointLights[i];

        s_Data.LightBuffer.DirectionalCount = std::min(static_cast<uint32_t>(lightData.DirectionalLights.size()), Renderer3DData::MaxDirectionalLights);
        for (uint32_t i = 0; i < s_Data.LightBuffer.DirectionalCount; ++i)
            s_Data.LightBuffer.DirectionalLights[i] = lightData.DirectionalLights[i];

        s_Data.LightBuffer.SpotCount = std::min(static_cast<uint32_t>(lightData.SpotLights.size()), Renderer3DData::MaxSpotLights);
        for (uint32_t i = 0; i < s_Data.LightBuffer.SpotCount; ++i)
            s_Data.LightBuffer.SpotLights[i] = lightData.SpotLights[i];

        s_Data.LightUniformBuffer->SetData(&s_Data.LightBuffer, sizeof(Renderer3DData::LightBuffer));
    }

    void Renderer3D::EndScene()
    {
    }

#include <glad/glad.h> // TODO: remove
    void Renderer3D::DrawSkybox(const Ref<CubeTexture> &skybox, float intensity)
    {
        if (!skybox)
            return;

        s_Data.SkyboxShader->Bind();

        s_Data.SkyboxBuffer.Intensity = intensity;
        s_Data.SkyboxUniformBuffer->SetData(&s_Data.SkyboxBuffer, sizeof(Renderer3DData::SkyboxSettings));

        skybox->Bind(31);

        glDepthFunc(GL_LEQUAL);
        RenderCommand::DrawIndexed(s_Data.SkyboxMesh->GetVertexArray(), s_Data.SkyboxMesh->GetIndexCount());
        glDepthFunc(GL_LESS);
    }

    void Renderer3D::DrawMesh(const Ref<Mesh> &mesh, const Ref<Material> &material, const glm::mat4 &transform, int entityID)
    {
        int albedoSlot = 0;
        int metRoughSlot = 1;

        if (material->AlbedoColorTexture)
        {
            albedoSlot = 2;
            material->AlbedoColorTexture->Bind(albedoSlot);
        }
        else
        {
            s_Data.WhiteTexture->Bind(0);
        }

        if (material->MetallicRoughnessTexture)
        {
            metRoughSlot = 3;
            material->MetallicRoughnessTexture->Bind(metRoughSlot);
        }
        else
        {
            s_Data.DefaultPBRTexture->Bind(metRoughSlot);
        }

        s_Data.EntityBuffer.Transform = transform;
        s_Data.EntityBuffer.AlbedoColor = material->AlbedoColor;
        s_Data.EntityBuffer.AlbedoTexIndex = albedoSlot;
        s_Data.EntityBuffer.MetRoughTexIndex = metRoughSlot;
        s_Data.EntityBuffer.Metallic = material->Metallic;
        s_Data.EntityBuffer.Roughness = material->Roughness;
        s_Data.EntityBuffer.EntityID = entityID;

        s_Data.EntityUniformBuffer->SetData(&s_Data.EntityBuffer, sizeof(Renderer3DData::EntityData));

        s_Data.PBRShader->Bind();
        RenderCommand::DrawIndexed(mesh->GetVertexArray(), mesh->GetIndexCount());

        s_Data.Stats.DrawCalls++;
        s_Data.Stats.Triangles += mesh->GetIndexCount() / 3;
    }

    void Renderer3D::DrawModel(const Ref<Model> &model, const glm::mat4 &transform, int entityID)
    {
        for (const auto &node : model->GetNodes())
        {
            if (node._Mesh)
            {
                DrawMesh(node._Mesh, node._Material, transform * node.LocalTransform, entityID);
            }
        }

        s_Data.Stats.ModelsCount++;
    }

    void Renderer3D::DrawCubeMesh()
    {
        RenderCommand::DrawIndexed(s_Data.SkyboxMesh->GetVertexArray(), s_Data.SkyboxMesh->GetIndexCount());

        s_Data.Stats.DrawCalls++;
    }

    void Renderer3D::SetCameraMatrices(const glm::mat4 &proj, const glm::mat4 &view)
    {
        s_Data.CameraBuffer.Projection = proj;
        s_Data.CameraBuffer.View = view;
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, 2 * sizeof(glm::mat4));
    }

    Ref<Shader> Renderer3D::GetEquirectToCubeShader()
    {
        return s_Data.EquirectToCubeShader;
    }

    void Renderer3D::ResetStats()
    {
        std::memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    Renderer3D::Statistics Renderer3D::GetStats()
    {
        return s_Data.Stats;
    }

} // namespace Myka
