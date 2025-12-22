#pragma once

#include "MykaEngine/Core.hpp"
#include "MykaEngine/Window.hpp"
#include "MykaEngine/Events/ApplicationEvent.hpp"
#include "MykaEngine/LayerStack.hpp"
#include "MykaEngine/ImGui/ImGuiLayer.hpp"

// TODO: remove
#include "MykaEngine/Renderer/Shader.hpp"
#include "MykaEngine/Renderer/Buffer.hpp"
#include "MykaEngine/Renderer/VertexArray.hpp"

#include <memory>

namespace Myka
{
	class MYKA_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event &e);

		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);

		inline Window &GetWindow() { return *m_Window; }

		inline static Application &Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent &e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		// TODO: remove
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	private:
		static Application *s_Instance;
	};

	// To be defined in a CLIENT
	Application *CreateApplication();

} // namespace Myka