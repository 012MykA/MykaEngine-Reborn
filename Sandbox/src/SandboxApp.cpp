#include <MykaEngine.hpp>
#include <MykaEngine/Core/EntryPoint.hpp>

#include "Platform/OpenGL/OpenGLShader.hpp"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.hpp"

class ExampleLayer : public Myka::Layer
{
public:
	ExampleLayer() : Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
		// triangle
		m_VertexArray = Myka::VertexArray::Create();

		float vertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
			0.5, -0.5, 0.0f, 1.0f, 0.0f};

		Myka::Ref<Myka::VertexBuffer> m_VertexBuffer(Myka::VertexBuffer::Create(vertices, sizeof(vertices)));
		Myka::BufferLayout layout = {
			{Myka::ShaderDataType::Float3, "a_Position"},
			{Myka::ShaderDataType::Float2, "a_TexCoord"},
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[6] = {0, 1, 2, 2, 3, 0};
		Myka::Ref<Myka::IndexBuffer> m_IndexBuffer(Myka::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		std::string flatShaderVertexSrc = R"(
			#version 460 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main()
			{
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string flatShaderFragmentSrc = R"(
			#version 460 core

			layout(location = 0) out vec4 FragColor;
			
			uniform vec3 u_Color;

			void main()
			{
				FragColor = vec4(u_Color, 1.0);
			}
		)";

		m_FlatColorShader = Myka::Shader::Create("FlatColor", flatShaderVertexSrc, flatShaderFragmentSrc);

		auto textureSHader = m_ShaderLibrary.Load("C:/dev/MykaEngine-Reborn/Sandbox/assets/shaders/Texture.glsl");

		m_Texture = Myka::Texture2D::Create("C:/dev/MykaEngine-Reborn/Sandbox/assets/textures/opengl-logo.png");

		std::dynamic_pointer_cast<Myka::OpenGLShader>(textureSHader)->Bind();
		std::dynamic_pointer_cast<Myka::OpenGLShader>(textureSHader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Myka::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		// Render
		Myka::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		Myka::RenderCommand::Clear();

		Myka::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Myka::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Myka::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 10; ++y)
		{
			for (int x = 0; x < 10; ++x)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Myka::Renderer::Submit(m_FlatColorShader, m_VertexArray, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		Myka::Renderer::Submit(textureShader, m_VertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		Myka::Renderer::EndScene();
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Myka::Event &e) override
	{
		m_CameraController.OnEvent(e);
	}

private:
	Myka::ShaderLibrary m_ShaderLibrary;
	Myka::Ref<Myka::Shader> m_FlatColorShader;
	Myka::Ref<Myka::VertexArray> m_VertexArray;

	Myka::Ref<Myka::Texture2D> m_Texture;

	Myka::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
};

class Sandbox : public Myka::Application
{
public:
	Sandbox() : Application("Sandbox")
	{
		// PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {}
};

Myka::Application *Myka::CreateApplication()
{
	return new Sandbox();
}
