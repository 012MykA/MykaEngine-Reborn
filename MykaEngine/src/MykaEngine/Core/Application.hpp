#pragma once

#include "MykaEngine/Core/Base.hpp"
#include "MykaEngine/Core/Window.hpp"
#include "MykaEngine/Events/ApplicationEvent.hpp"
#include "MykaEngine/Core/LayerStack.hpp"
#include "MykaEngine/ImGui/ImGuiLayer.hpp"

int main(int argc, char **argv);

namespace Myka
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char **Args = nullptr;

		const char *operator[](int index) const
		{
			MYKA_CORE_ASSERT(index < Count, "ApplicationCommandLineArgs::operator[] index out of range");
			return Args[index];
		}
	};

	class Application
	{
		friend int ::main(int argc, char **argv);

	public:
		Application(const std::string &name = "MykaEngine App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application();

		void OnEvent(Event &e);

		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);

		Window &GetWindow() { return *m_Window; }

		void Close();

		ImGuiLayer *GetImGuiLayer() { return m_ImGuiLayer; }

		static Application &Get() { return *s_Instance; }

	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent &e);
		bool OnWindowResize(WindowResizeEvent &e);

	private:
		ApplicationCommandLineArgs m_CommandLineArgs;
		Scope<Window> m_Window;
		ImGuiLayer *m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

	private:
		static Application *s_Instance;
	};

	// To be defined in a CLIENT
	Application *CreateApplication(ApplicationCommandLineArgs args);

} // namespace Myka