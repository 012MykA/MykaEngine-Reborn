#include "mykapch.hpp"
#include "Renderer3D.hpp"

#include "Shader.hpp"
#include "UniformBuffer.hpp"
#include "RenderCommand.hpp"

namespace Myka
{
    struct Renderer3DData
    {
        struct EntityData
        {
            glm::mat4 Transform;
            glm::vec4 AlbedoColor;
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
        s_Data.EntityBuffer.Transform = transform;
        s_Data.EntityBuffer.AlbedoColor = material->AlbedoColor;
        s_Data.EntityBuffer.EntityID = entityID;
        s_Data.EntityUniformBuffer->SetData(&s_Data.EntityBuffer, sizeof(Renderer3DData::EntityData));

        s_Data.MaterialShader->Bind();
        RenderCommand::DrawIndexed(mesh->GetVertexArray(), mesh->GetIndexCount());

        s_Data.Stats.DrawCalls++;
        s_Data.Stats.MeshesCount++;
        s_Data.Stats.IndicesCount += mesh->GetIndexCount();
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
