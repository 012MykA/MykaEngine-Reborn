#include "mykapch.hpp"
#include "Renderer3D.hpp"

#include "Shader.hpp"
#include "UniformBuffer.hpp"
#include "RenderCommand.hpp"

namespace Myka
{
    struct Renderer3DData
    {
        static constexpr uint32_t MaxTextureSlots = 32;

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
        };
        EntityData EntityBuffer;
        Ref<UniformBuffer> EntityUniformBuffer;

        Ref<Shader> MaterialShader;

        Renderer3D::Statistics Stats;

        struct CameraData
        {
            glm::mat4 ViewProjection;
        };
        CameraData CameraBuffer;
        Ref<UniformBuffer> CameraUniformBuffer;
    };

    static Renderer3DData s_Data;

    void Renderer3D::Init()
    {
        s_Data.WhiteTexture = Texture2D::Create(1, 1);
        const uint32_t whiteTextureData = 0xFFFFFFFF;
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        s_Data.TextureSlots[0] = s_Data.WhiteTexture;

        s_Data.MaterialShader = Shader::Create("assets/shaders/Renderer3D_Material.glsl");

        s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::CameraData), 0);
        s_Data.EntityUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::EntityData), 1);
    }

    void Renderer3D::Shutdown() {}

    void Renderer3D::BeginScene(const EditorCamera &camera)
    {
        s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData));
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
