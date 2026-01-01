#include <MykaEngine.hpp>
#include <MykaEngine/Core/EntryPoint.hpp>

#include "Platform/OpenGL/OpenGLShader.hpp"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EditorLayer.hpp"

namespace Myka
{
	class MykaEditor : public Myka::Application
	{
	public:
		MykaEditor() : Application()
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

