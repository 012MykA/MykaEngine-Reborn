#include <MykaEngine.hpp>
#include <MykaEngine/Core/EntryPoint.hpp>

#include "Sandbox2D.hpp"

class Sandbox : public Myka::Application
{
public:
	Sandbox(const Myka::ApplicationSpecification& specification) : Myka::Application(specification)
	{
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {}
};

Myka::Application *Myka::CreateApplication(Myka::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../../Sandbox";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}
