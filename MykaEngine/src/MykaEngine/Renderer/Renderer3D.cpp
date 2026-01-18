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
            glm::vec4 Color;
            int EntityID;
        };
        EntityData EntityBuffer;
        Ref<UniformBuffer> EntityUniformBuffer;
        
        Ref<Shader> DefaultShader;

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
        s_Data.DefaultShader = Shader::Create("assets/shaders/Renderer3D_Default.glsl");

        s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::CameraData), 0);
        s_Data.EntityUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::EntityData), 1);
    }

    void Renderer3D::Shutdown()
    {
    }

    void Renderer3D::BeginScene(const EditorCamera &camera)
    {
        s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData));
    }

    void Renderer3D::EndScene()
    {
    }

    void Renderer3D::DrawMesh(const Ref<Mesh> &mesh, const glm::mat4 &transform, const glm::vec4& color, int entityID)
    {
        s_Data.EntityBuffer.Transform = transform;
        s_Data.EntityBuffer.Color = color;
        s_Data.EntityBuffer.EntityID = entityID;
        s_Data.EntityUniformBuffer->SetData(&s_Data.EntityBuffer, sizeof(Renderer3DData::EntityData));

        s_Data.DefaultShader->Bind();
        RenderCommand::DrawIndexed(mesh->GetVertexArray(), mesh->GetIndexCount());

        s_Data.Stats.DrawCalls++;
        s_Data.Stats.IndicesCount += mesh->GetIndexCount();
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
