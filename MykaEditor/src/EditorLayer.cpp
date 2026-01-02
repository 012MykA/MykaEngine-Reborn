#include "EditorLayer.hpp"

#include <imgui.h>

namespace Myka
{
    EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f) {}

    void EditorLayer::OnAttach()
    {
        MYKA_PROFILE_FUNCTION();

        m_BoxTexture = Texture2D::Create("C:/dev/MykaEngine-Reborn/MykaEditor/assets/textures/box.png");

        FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);
    }

    void EditorLayer::OnDetach()
    {
        MYKA_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        MYKA_PROFILE_FUNCTION();

        // Update
        m_CameraController.OnUpdate(ts);

        // Render
        Renderer2D::ResetStats();
        {
            MYKA_PROFILE_SCOPE("Renderer Prep");

            m_Framebuffer->Bind();
            RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
            RenderCommand::Clear();
        }

        {
            static float rotation = 0.0f;
            rotation += ts * 50.0f;

            MYKA_PROFILE_SCOPE("Renderer Draw");

            Renderer2D::BeginScene(m_CameraController.GetCamera());

            Renderer2D::DrawRotatedQuad({0.0f, 0.0f, -0.1f}, {1.0f, 1.0f}, glm::radians(rotation), m_BoxTexture);

            for (float y = -5.0f; y < 5.0f; y += 0.5f)
            {
                for (float x = -5.0f; x < 5.0f; x += 0.5f)
                {
                    glm::vec4 color = {(x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f};
                    Renderer2D::DrawQuad({x, y}, {0.45f, 0.45f}, color);
                }
            }

            Renderer2D::EndScene();
            m_Framebuffer->Unbind();
        }
    }

    void EditorLayer::OnImGuiRender()
    {
        MYKA_PROFILE_FUNCTION();

        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // Parent window flags
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        if (opt_fullscreen)
        {
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // --- Dockspace ---
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
        if (ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags))
        {
            ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // DockSpace
            ImGuiIO &io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2{0.0f, 0.0f}, dockspace_flags);
            }

            // Menu bar
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Exit"))
                        Application::Get().Close();

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            ImGui::End();
        }

        //  --- Renderer2D Stats ---
        if (ImGui::Begin("Renderer2D Stats"))
        {
            auto stats = Renderer2D::GetStats();
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Quads: %d", stats.QuadCount);
            ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
            ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
            ImGui::End();
        }

        // --- Viewport ---
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        if (ImGui::Begin("Viewport"))
        {
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            glm::vec2 viewportSize = {viewportPanelSize.x, viewportPanelSize.y};
            if (m_ViewportSize != viewportSize)
            {
                m_Framebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
                m_ViewportSize = viewportSize;

            m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);
            }

            ImGui::Image(m_Framebuffer->GetColorAttachment(), ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});
            ImGui::End();
        }
        ImGui::PopStyleVar();
    }

    void EditorLayer::OnEvent(Event &e)
    {
        m_CameraController.OnEvent(e);
    }
} // namespace Myka
