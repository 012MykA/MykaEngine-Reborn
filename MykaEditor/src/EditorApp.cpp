#include <MykaEngine.hpp>
#include <MykaEngine/Core/EntryPoint.hpp>

#include "EditorLayer.hpp"

namespace Myka
{
	class MykaEditor : public Myka::Application
	{
	public:
		MykaEditor(ApplicationCommandLineArgs args) : Application("MykaEditor", args)
		{
			PushLayer(new EditorLayer());
		}

		~MykaEditor() {}
	};

	Myka::Application *Myka::CreateApplication(ApplicationCommandLineArgs args)
	{
		return new MykaEditor(args);
	}
} // namespace Myka
