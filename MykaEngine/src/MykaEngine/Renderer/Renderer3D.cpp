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
        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture

        struct EntityData
        {
            glm::mat4 Transform;
            glm::vec4 AlbedoColor;
            int AlbedoTexIndex;
            int MetRoughTexIndex;
            int EntityID;
        } EntityBuffer;
        Ref<UniformBuffer> EntityUniformBuffer;

        Ref<Shader> MaterialShader;

        struct LightBufferData
        {
            Renderer3D::PointLight PointLights[MaxPointLights];
            Renderer3D::DirectionalLight DirectionalLights[MaxDirectionalLights];
            Renderer3D::SpotLight SpotLights[MaxSpotLights];
            uint32_t PointCount = 0;
            uint32_t DirectionalCount = 0;
            uint32_t SpotCount = 0;
        } LightBuffer;
        Ref<UniformBuffer> LightUniformBuffer;

        struct CameraData
        {
            glm::mat4 ViewProjection;
            glm::vec3 Position;
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

        s_Data.TextureSlots[0] = s_Data.WhiteTexture;

        s_Data.MaterialShader = Shader::Create("assets/shaders/Renderer3D_PBR.glsl");

        s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::CameraData), 0);
        s_Data.EntityUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::EntityData), 1);
        s_Data.LightUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::LightBuffer), 2);
    }

    void Renderer3D::Shutdown() {}

    void Renderer3D::BeginScene(const EditorCamera &camera, const SceneLightData &lightData)
    {
        s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
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

    void Renderer3D::DrawMesh(const Ref<Mesh> &mesh, const Ref<Material> &material, const glm::mat4 &transform, int entityID)
    {
        int albedoSlot = 0;
        int metRoughSlot = 0;

        if (material->AlbedoColorTexture)
        {
            albedoSlot = 1;
            material->AlbedoColorTexture->Bind(albedoSlot);
        }
        else
        {
            s_Data.WhiteTexture->Bind(0);
        }

        if (material->MetallicRoughnessTexture)
        {
            metRoughSlot = 2;
            material->MetallicRoughnessTexture->Bind(metRoughSlot);
        }
        else
        {
            s_Data.WhiteTexture->Bind(0);
        }

        s_Data.EntityBuffer.Transform = transform;
        s_Data.EntityBuffer.AlbedoColor = material->AlbedoColor;
        s_Data.EntityBuffer.AlbedoTexIndex = albedoSlot;
        s_Data.EntityBuffer.MetRoughTexIndex = metRoughSlot;
        s_Data.EntityBuffer.EntityID = entityID;

        s_Data.EntityUniformBuffer->SetData(&s_Data.EntityBuffer, sizeof(Renderer3DData::EntityData));

        s_Data.MaterialShader->Bind();
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

    void Renderer3D::ResetStats()
    {
        std::memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    Renderer3D::Statistics Renderer3D::GetStats()
    {
        return s_Data.Stats;
    }

} // namespace Myka
