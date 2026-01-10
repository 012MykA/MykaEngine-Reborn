#include "mykapch.hpp"
#include "Application.hpp"

#include "MykaEngine/Core/Log.hpp"
#include "MykaEngine/Core/Timestep.hpp"

#include "MykaEngine/Renderer/Renderer.hpp"

#include "MykaEngine/Core/Input.hpp"

#include <GLFW/glfw3.h> // TODO: remove

namespace Myka
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application *Application::s_Instance = nullptr;

	Application::Application(const std::string &name, ApplicationCommandLineArgs args)
	{
		MYKA_PROFILE_FUNCTION();

		MYKA_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create(WindowProps(name));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		MYKA_PROFILE_FUNCTION();

		Renderer::Shutdown();
	}

	void Application::OnEvent(Event &e)
	{
		MYKA_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::PushLayer(Layer *layer)
	{
		MYKA_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer *overlay)
	{
		MYKA_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::Run()
	{
		MYKA_PROFILE_FUNCTION();

		while (m_Running)
		{
			MYKA_PROFILE_SCOPE("RunLoop");

			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			// OnUpdate
			if (!m_Minimized)
			{
				MYKA_PROFILE_SCOPE("LayerStack OnUpdate");

				for (Layer *layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			// OnImGuiRender
			m_ImGuiLayer->Begin();
			{
				MYKA_PROFILE_SCOPE("LayerStack OnImGuiRender");

				for (Layer *layer : m_LayerStack)
					layer->OnImGuiRender();
				m_ImGuiLayer->End();
			}
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent &e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent &e)
	{
		MYKA_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

} // namespace Myka