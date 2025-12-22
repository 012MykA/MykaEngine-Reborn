#include <MykaEngine.hpp>
#include <imgui.h>

class ExampleLayer : public Myka::Layer
{
private:
public:
	ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_CameraRotation(0.0f)
	{
		// triangle
		m_VertexArray.reset(Myka::VertexArray::Create());

		float vertices[4 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			-0.5f, 0.5f, 0.0f, 0.2f, 0.8f, 0.2f, 1.0f,
			0.5f, 0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			0.5, -0.5, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		m_VertexBuffer.reset(Myka::VertexBuffer::Create(vertices, sizeof(vertices)));
		Myka::BufferLayout layout = {
			{Myka::ShaderDataType::Float3, "a_Position"},
			{Myka::ShaderDataType::Float4, "a_Color"}
		};
		
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[6] = {0, 1, 2, 2, 3, 0};
		m_IndexBuffer.reset(Myka::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		std::string vertexSrc = R"(
			#version 460 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 460 core

			layout(location = 0) out vec4 FragColor;

			in vec4 v_Color;

			void main()
			{
				FragColor = v_Color;
			}
		)";

		m_Shader.reset(new Myka::Shader(vertexSrc, fragmentSrc));
	}

	void OnUpdate() override
	{
		if (Myka::Input::IsKeyPressed(MYKA_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed;
		if (Myka::Input::IsKeyPressed(MYKA_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed;
		if (Myka::Input::IsKeyPressed(MYKA_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed;
		if (Myka::Input::IsKeyPressed(MYKA_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed;
			
		if (Myka::Input::IsKeyPressed(MYKA_KEY_A))
			m_CameraRotation -= m_CameraRotationSpeed;
		if (Myka::Input::IsKeyPressed(MYKA_KEY_D))
			m_CameraRotation += m_CameraRotationSpeed;

		Myka::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		Myka::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Myka::Renderer::BeginScene(m_Camera);

		Myka::Renderer::Submit(m_Shader, m_VertexArray);

		Myka::Renderer::EndScene();
	}

	void OnImGuiRender() override
	{
	}

	void OnEvent(Myka::Event &event) override
	{

	}

private:
	std::shared_ptr<Myka::Shader> m_Shader;
	std::shared_ptr<Myka::VertexArray> m_VertexArray;
	std::shared_ptr<Myka::VertexBuffer> m_VertexBuffer;
	std::shared_ptr<Myka::IndexBuffer> m_IndexBuffer;

	Myka::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraRotation;
	float m_CameraMoveSpeed = 0.1f;
	float m_CameraRotationSpeed = 2.0f;
};

class Sandbox : public Myka::Application
{
public:
	Sandbox() : Application()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {}
};

Myka::Application *Myka::CreateApplication()
{
	return new Sandbox();
}
