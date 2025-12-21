#include <MykaEngine.hpp>
#include <imgui.h>

class ExampleLayer : public Myka::Layer
{
private:
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override {}

	void OnImGuiRender() override
	{
		ImGui::Begin("Hello");
		ImGui::Text("MykaEngine LOL");
		ImGui::End();
	}

	void OnEvent(Myka::Event& event) override {}
};

class Sandbox : public Myka::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
	
	~Sandbox() {}
};

Myka::Application* Myka::CreateApplication()
{
	return new Sandbox();
}
