#include "EditorLayer.hpp"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

#include "MykaEngine/Scene/SceneSerializer.hpp"

#include "MykaEngine/Utils/PlatformUtils.hpp"

namespace Myka
{
    EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f) {}

    void EditorLayer::OnAttach()
    {
        MYKA_PROFILE_FUNCTION();

        m_BoxTexture = Texture2D::Create("../../MykaEditor/assets/textures/box.png");

        FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_ActiveScene = CreateRef<Scene>();

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnDetach()
    {
        MYKA_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        MYKA_PROFILE_FUNCTION();

        // Resize
        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        // Update
        if (m_ViewportFocused)
            m_CameraController.OnUpdate(ts);

        // Render
        Renderer2D::ResetStats();
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        RenderCommand::Clear();

        // Update scene
        m_ActiveScene->OnUpdate(ts);

        m_Framebuffer->Unbind();
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
            ImGuiStyle &style = ImGui::GetStyle();
            float minWinSizeX = style.WindowMinSize.x;
            style.WindowMinSize.x = 370.0f;
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2{0.0f, 0.0f}, dockspace_flags);
            }
            style.WindowMinSize.x = minWinSizeX;

            // Menu bar
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New", "Ctrl+N"))
                        NewScene();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Open...", "Ctrl+O"))
                        OpenScene();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                        SaveSceneAs();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Exit"))
                        Application::Get().Close();

                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
        }
        ImGui::End();

        // SceneHierarchyPanel Render
        m_SceneHierarchyPanel.OnImGuiRender();

        //  --- Renderer2D Stats ---
        ImGui::Begin("Renderer2D Stats");
        auto stats = Renderer2D::GetStats();
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
        ImGui::End();

        // --- Viewport ---
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");
        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused && !m_ViewportHovered);
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

        uint64_t textureID = m_Framebuffer->GetColorAttachment();
        ImGui::Image((void *)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});

        // ImGuizmo
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity && m_ImGuizmoType != -1)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            float windowWidth = static_cast<float>(ImGui::GetWindowWidth());
            float windowHeight = static_cast<float>(ImGui::GetWindowHeight());
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

            auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
            const auto &camera = cameraEntity.GetComponent<CameraComponent>().Camera;
            const glm::mat4 &cameraProjection = camera.GetProjection();
            glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

            // Entity transform
            auto &tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();

            // Snapping
            bool snap = Input::IsKeyPressed(MYKA_KEY_LEFT_CONTROL);

            float snapValue = 0.5f;
            if (m_ImGuizmoType == ImGuizmo::OPERATION::ROTATE)
                snapValue = 45.0f;

            float snapValues[3] = {snapValue, snapValue, snapValue};

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                                 (ImGuizmo::OPERATION)m_ImGuizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                                 nullptr, snap ? snapValues : nullptr);

            if (ImGuizmo::IsUsing())
            {
                float position[3], rotation[3], scale[3];

                ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), position, rotation, scale);

                glm::vec3 newRotation = glm::radians(glm::vec3(rotation[0], rotation[1], rotation[2]));
                glm::vec3 deltaRotation = newRotation - tc.Rotation;

                tc.Position = {position[0], position[1], position[2]};
                tc.Rotation += deltaRotation;
                tc.Scale = {scale[0], scale[1], scale[2]};
            }
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorLayer::OnEvent(Event &e)
    {
        m_CameraController.OnEvent(e);

        EventDispatcher dp(e);
        dp.Dispatch<KeyPressedEvent>(MYKA_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent &e)
    {
        // Shortcuts
        if (e.GetRepeatCount() > 0)
            return false;

        bool ctrl = Input::IsKeyPressed(MYKA_KEY_LEFT_CONTROL) || Input::IsKeyPressed(MYKA_KEY_RIGHT_CONTROL);
        bool shift = Input::IsKeyPressed(MYKA_KEY_LEFT_SHIFT) || Input::IsKeyPressed(MYKA_KEY_RIGHT_SHIFT);

        switch (e.GetKeyCode())
        {
        // File dialogs
        case MYKA_KEY_N:
        {
            if (ctrl)
            {
                NewScene();
            }
            break;
        }

        case MYKA_KEY_O:
        {
            if (ctrl)
            {
                OpenScene();
            }
            break;
        }

        case MYKA_KEY_S:
        {
            if (ctrl && shift)
            {
                SaveSceneAs();
            }
            break;
        }
        // ImGuizmo
        case MYKA_KEY_Q:
        {
            m_ImGuizmoType = -1;
            break;
        }
        case MYKA_KEY_W:
        {
            m_ImGuizmoType = ImGuizmo::TRANSLATE;
            break;
        }
        case MYKA_KEY_E:
        {
            m_ImGuizmoType = ImGuizmo::ROTATE;
            break;
        }
        case MYKA_KEY_R:
        {
            m_ImGuizmoType = ImGuizmo::SCALE;
            break;
        }
        }
    }

    void EditorLayer::NewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OpenScene()
    {
        std::string filepath = FileDialogs::OpenFile("MykaEngine Scene (*.myka)\0*.myka\0"); // JSON (*.json)\0*.json\0
        if (!filepath.empty())
        {
            m_ActiveScene = CreateRef<Scene>();
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);

            SceneSerializer serializer(m_ActiveScene);
            serializer.DeserializeJSON(filepath);
        }
    }

    void EditorLayer::SaveSceneAs()
    {
        std::string filepath = FileDialogs::SaveFile("MykaEngine Scene (*.myka)\0*.myka\0");

        if (!filepath.empty())
        {
            SceneSerializer serializer(m_ActiveScene);
            serializer.SerializeJSON(filepath);
        }
    }

} // namespace Myka
