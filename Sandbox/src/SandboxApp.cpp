#include <MykaEngine.hpp>

class ExampleLayer : public Myka::Layer
{
private:
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override
	{
		if (Myka::Input::IsKeyPressed(MYKA_KEY_TAB))
			MYKA_TRACE("Tab key is pressed!");
	}

	void OnEvent(Myka::Event& event) override
	{
		if (event.GetEventType() == Myka::EventType::KeyPressed)
		{
			Myka::KeyPressedEvent& e = (Myka::KeyPressedEvent&)event;
			MYKA_TRACE("{0}", e.GetKeyCode());
		}
	}
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
