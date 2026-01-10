#include <MykaEngine.hpp>
#include <MykaEngine/Core/EntryPoint.hpp>

#include "Sandbox2D.hpp"

class Sandbox : public Myka::Application
{
public:
	Sandbox(Myka::ApplicationCommandLineArgs args) : Myka::Application("Sandbox", args)
	{
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {}
};

Myka::Application *Myka::CreateApplication(Myka::ApplicationCommandLineArgs args)
{
	return new Sandbox(args);
}
