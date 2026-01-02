#include <MykaEngine.hpp>
#include <MykaEngine/Core/EntryPoint.hpp>

#include "EditorLayer.hpp"

namespace Myka
{
	class MykaEditor : public Myka::Application
	{
	public:
		MykaEditor() : Application("MykaEditor")
		{
			PushLayer(new EditorLayer());
		}

		~MykaEditor() {}
	};

	Myka::Application *Myka::CreateApplication()
	{
		return new MykaEditor();
	}
} // namespace Myka
