#include <MykaEngine.hpp>
#include <MykaEngine/Core/EntryPoint.hpp>

#include "EditorLayer.hpp"

namespace Myka
{
	class MykaEditor : public Myka::Application
	{
	public:
		MykaEditor(const Myka::ApplicationSpecification& specification) : Application(specification)
		{
			PushLayer(new EditorLayer());
		}

		~MykaEditor() {}
	};

	Myka::Application *Myka::CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "MykaEditor";
		spec.WorkingDirectory = "../../MykaEditor";
		spec.CommandLineArgs = args;
		
		return new MykaEditor(spec);
	}
} // namespace Myka
