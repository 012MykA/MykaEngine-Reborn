#include "mykapch.hpp"
#include "Renderer2D.hpp"

#include "VertexArray.hpp"
#include "Shader.hpp"
#include "RenderCommand.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Myka
{
    struct Rendeerer2DStorage
    {
        Ref<VertexArray> QuadVertexArray;
        Ref<Shader> FlatColorShader;
        Ref<Shader> TextureShader;
    };

    static Rendeerer2DStorage *s_Data;

    void Renderer2D::Init()
    {
        s_Data = new Rendeerer2DStorage();

        s_Data->QuadVertexArray = VertexArray::Create();

        float vertices[4 * 5] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
            0.5, -0.5, 0.0f, 1.0f, 0.0f
        };

        Ref<VertexBuffer> m_VertexBuffer(VertexBuffer::Create(vertices, sizeof(vertices)));
        m_VertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float2, "a_TexCoord"}
        });

        s_Data->QuadVertexArray->AddVertexBuffer(m_VertexBuffer);

        uint32_t indices[6] = {0, 1, 2, 2, 3, 0};
        Ref<IndexBuffer> m_IndexBuffer(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        s_Data->QuadVertexArray->SetIndexBuffer(m_IndexBuffer);

        s_Data->FlatColorShader = Shader::Create("C:/dev/MykaEngine-Reborn/Sandbox/assets/shaders/FlatColor.glsl");
        s_Data->TextureShader = Shader::Create("C:/dev/MykaEngine-Reborn/Sandbox/assets/shaders/Texture.glsl");
        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetInt("u_Texture", 0);
    }

    void Renderer2D::Shutdown()
    {
        delete s_Data;
    }

    void Renderer2D::BeginScene(const OrthographicCamera camera)
    {
        s_Data->FlatColorShader->Bind();
        s_Data->FlatColorShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
    }

    void Renderer2D::EndScene()
    {
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        s_Data->FlatColorShader->Bind();
        s_Data->FlatColorShader->SetFloat4("u_Color", color);

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        s_Data->FlatColorShader->SetMat4("u_Transform", transform);

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, texture);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture)
    {
        s_Data->TextureShader->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        s_Data->TextureShader->SetMat4("u_Transform", transform);

        texture->Bind();

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
    }

} // namespace Myka
