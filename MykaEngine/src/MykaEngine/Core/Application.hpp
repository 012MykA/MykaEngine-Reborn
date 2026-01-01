#pragma once

#include "MykaEngine/Core/Base.hpp"
#include "MykaEngine/Core/Window.hpp"
#include "MykaEngine/Events/ApplicationEvent.hpp"
#include "MykaEngine/Core/LayerStack.hpp"
#include "MykaEngine/ImGui/ImGuiLayer.hpp"

namespace Myka
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event &e);

		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);

		Window &GetWindow() { return *m_Window; }

		static Application &Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent &e);
		bool OnWindowResize(WindowResizeEvent &e);

	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer *m_ImGuiLayer;

		bool m_Running = true;
		bool m_Minimized = false;

		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

	private:
		static Application *s_Instance;
	};

	// To be defined in a CLIENT
	Application *CreateApplication();

} // namespace Myka