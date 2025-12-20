#include <MykaEngine.hpp>

class ExampleLayer : public Myka::Layer
{
private:
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override
	{
		// MYKA_INFO("ExampleLayer::OnUpdate"); // TODO: remove
	}

	void OnEvent(Myka::Event& event) override
	{
		// MYKA_TRACE("ExampleLayer::OnEvent"); // TODO: remove
	}
};

class Sandbox : public Myka::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Myka::ImGuiLayer());
	}
	
	~Sandbox() {}
};

Myka::Application* Myka::CreateApplication()
{
	return new Sandbox();
}
