#include "EditorLayer.hpp"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

#include "MykaEngine/Scene/SceneSerializer.hpp"
#include "MykaEngine/Renderer/Renderer3D.hpp"
#include "MykaEngine/Core/MouseCodes.hpp"
#include "MykaEngine/Utils/PlatformUtils.hpp"

namespace Myka
{
    EditorLayer::EditorLayer() : Layer("EditorLayer") {}

    void EditorLayer::OnAttach()
    {
        MYKA_PROFILE_FUNCTION();

        m_IconPlay = Texture2D::Create("Resources/icons/PlayButton.png");
        m_IconSimulate = Texture2D::Create("Resources/icons/SimulateButton.png");
        m_IconStop = Texture2D::Create("Resources/icons/StopButton.png");

        FramebufferSpecification fbSpec;
        fbSpec.Attachments = {FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth};
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_EditorScene = CreateRef<Scene>();
        m_ActiveScene = m_EditorScene;

        // TODO: projects
        // auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
        // if (commandLineArgs.Count > 1)
        // {
        //     auto projectFilepath = commandLineArgs[1];
        //     OpenProject(projectFilepath);
        // }
        // else
        // {
        //     if (!OpenProject())
        //         Application::Get().Close();
        // }

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

        m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f);
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
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);

            if (m_ActiveScene)
                m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        if (!m_ActiveScene)
        {
            m_Framebuffer->Bind();
            RenderCommand::Clear();
            m_Framebuffer->Unbind();
            return;
        }

        // Render
        Renderer2D::ResetStats();
        Renderer3D::ResetStats();
        m_Framebuffer->Bind();
        RenderCommand::Clear();

        m_Framebuffer->ClearAttachment(1, -1);

        switch (m_SceneState)
        {
        case SceneState::Edit:
        {
            m_EditorCamera.OnUpdate(ts);
            m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
            break;
        }
        case SceneState::Simulate:
        {
            m_EditorCamera.OnUpdate(ts);
            m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
            break;
        }
        case SceneState::Play:
        {
            m_ActiveScene->OnUpdateRuntime(ts);
            break;
        }
        }

        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;

        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

        my = viewportSize.y - my;

        int mouseX = (int)mx;
        int mouseY = (int)my;

        if (mouseX >= 0 && mouseY >= 0 &&
            mouseX < (int)viewportSize.x &&
            mouseY < (int)viewportSize.y)
        {
            int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
            m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
        }

        OnOverlayRender();

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
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
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

                if (ImGui::MenuItem("Save", "Ctrl+S"))
                    SaveScene();

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                ImGui::Separator();

                if (ImGui::MenuItem("Exit"))
                    Application::Get().Close();

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();

        // SceneHierarchyPanel Render
        m_SceneHierarchyPanel.OnImGuiRender();
        m_ContentBrowserPanel.OnImGuiRender();

        //  --- Renderer2D Stats ---
        if (ImGui::Begin("Renderer2D Stats"))
        {
            auto stats = Renderer2D::GetStats();
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Quads: %d", stats.QuadCount);
            ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
            ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
        }
        ImGui::End();

        //  --- Rendere3D Stats ---
        if (ImGui::Begin("Renderer3D Stats"))
        {
            auto stats = Renderer3D::GetStats();
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Triangles: %d", stats.Triangles);
            ImGui::Text("Models: %d", stats.ModelsCount);
        }
        ImGui::End();

        // Settings
        ImGui::Begin("Settings");
        std::string name = "None";
        if (static_cast<bool>(m_HoveredEntity))
            name = m_HoveredEntity.GetComponent<TagComponent>().Tag;

        ImGui::Text("Hovered Entity: %s", name.c_str());
        ImGui::Checkbox("Show Physics Colliders", &m_ShowPhysicsColliders);
        if (ImGui::Checkbox("Show Triangles", &m_ShowTriangles))
        {
            RenderCommand::ShowTriangles(m_ShowTriangles);
        }
        ImGui::End();

        // --- Viewport ---
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused && !m_ViewportHovered);

        ImVec2 viewportOffset = ImGui::GetCursorPos();
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

        uint64_t textureID = m_Framebuffer->GetColorAttachment(0);
        ImGui::Image(reinterpret_cast<void *>(textureID), ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const wchar_t *path = (const wchar_t *)payload->Data;
                OpenScene(path);
            }

            ImGui::EndDragDropTarget();
        }

        ImVec2 minBound = ImGui::GetWindowPos();
        minBound.x += viewportOffset.x;
        minBound.y += viewportOffset.y;

        ImVec2 maxBound = {minBound.x + m_ViewportSize.x, minBound.y + m_ViewportSize.y};

        m_ViewportBounds[0] = {minBound.x, minBound.y};
        m_ViewportBounds[1] = {maxBound.x, maxBound.y};

        // ImGuizmo
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity && m_ImGuizmoType != -1)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            float windowWidth = static_cast<float>(ImGui::GetWindowWidth());
            float windowHeight = static_cast<float>(ImGui::GetWindowHeight());
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

            // Runtime camera
            // auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
            // const auto &camera = cameraEntity.GetComponent<CameraComponent>().Camera;
            // const glm::mat4 &cameraProjection = camera.GetProjection();
            // glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

            const glm::mat4 &cameraProjection = m_EditorCamera.GetProjection();
            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

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
                                 (ImGuizmo::OPERATION)m_ImGuizmoType, (ImGuizmo::MODE)m_ImGuizmoMode, glm::value_ptr(transform),
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

        UI_ToolBar();

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorLayer::UI_ToolBar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        auto &colors = ImGui::GetStyle().Colors;

        const auto &buttonHovered = colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto &activeHovered = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(activeHovered.x, activeHovered.y, activeHovered.z, 0.5f));

        ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize);

        float size = ImGui::GetWindowHeight() - 4.0f;

        bool hasScene = m_EditorScene != nullptr;

        // Play Button
        {
            Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
            ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f) - size * 0.6f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

            if (!hasScene)
                ImGui::BeginDisabled();

            if (ImGui::ImageButton("##playOrStopButton", (ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1)))
            {
                if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
                    OnScenePlay();
                else if (m_SceneState == SceneState::Play)
                    OnSceneStop();
            }

            if (!hasScene)
                ImGui::EndDisabled();

            ImGui::PopStyleVar();
        }

        ImGui::SameLine();

        // Simulate Button
        {
            Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

            if (!hasScene)
                ImGui::BeginDisabled();

            if (ImGui::ImageButton("##SimulateButton", (ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1)))
            {
                if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
                    OnSceneSimulate();
                else if (m_SceneState == SceneState::Simulate)
                    OnSceneStop();
            }

            if (!hasScene)
                ImGui::EndDisabled();

            ImGui::PopStyleVar();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::End();
    }

    void EditorLayer::OnEvent(Event &e)
    {
        m_EditorCamera.OnEvent(e);

        EventDispatcher dp(e);
        dp.Dispatch<KeyPressedEvent>(MYKA_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dp.Dispatch<MouseButtonPressedEvent>(MYKA_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
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
            if (ctrl)
            {
                if (shift)
                    SaveSceneAs();
                else
                    SaveScene();
            }
            break;
        }

        // Scene Commands
        case MYKA_KEY_D:
        {
            if (ctrl)
                OnDuplicateEntity();

            break;
        }

        // ImGuizmo
        case MYKA_KEY_X:
        {
            m_ImGuizmoMode = m_ImGuizmoMode == ImGuizmo::MODE::LOCAL ? ImGuizmo::MODE::WORLD : ImGuizmo::MODE::LOCAL;
            break;
        }
        case MYKA_KEY_Q:
        {
            m_ImGuizmoType = -1;
            break;
        }
        case MYKA_KEY_W:
        {
            m_ImGuizmoType = ImGuizmo::OPERATION::TRANSLATE;
            break;
        }
        case MYKA_KEY_E:
        {
            m_ImGuizmoType = ImGuizmo::OPERATION::ROTATE;
            break;
        }
        case MYKA_KEY_R:
        {
            m_ImGuizmoType = ImGuizmo::OPERATION::SCALE;
            break;
        }
        }
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent &e)
    {
        if (e.GetMouseButton() == Mouse::ButtonLeft)
        {
            if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(MYKA_KEY_LEFT_ALT))
                m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
        }

        return false;
    }

    void EditorLayer::OnOverlayRender()
    {
        if (!m_ActiveScene)
            return;

        if (m_SceneState == SceneState::Play)
        {
            Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
            if (!camera)
                return;

            Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
        }
        else
        {
            Renderer2D::BeginScene(m_EditorCamera);
        }

        if (m_ShowPhysicsColliders)
        {
            { // Quad Colliders Render
                auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
                for (auto entity : view)
                {
                    auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

                    glm::mat4 entityTransform = tc.GetTransform();
                    glm::mat4 overlayTransform = glm::translate(entityTransform, glm::vec3(bc2d.Offset, 0.01f)) * glm::scale(glm::mat4(1.0f), glm::vec3(bc2d.Size, 1.0f));

                    Renderer2D::DrawRect(overlayTransform, colliders2DColor);
                }
            }

            { // Circle Colliders Render
                auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
                for (auto entity : view)
                {
                    auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

                    glm::mat4 entityTransform = tc.GetTransform();
                    glm::mat4 overlayTransform = glm::translate(entityTransform, glm::vec3(cc2d.Offset, 0.01f)) * glm::scale(glm::mat4(1.0f), glm::vec3(cc2d.Radius * 2.0f));

                    Renderer2D::DrawCircle(overlayTransform, colliders2DColor, 0.01f);
                }
            }
        }

        Renderer2D::EndScene();
    }

    void EditorLayer::NewScene()
    {
        m_EditorScene = CreateRef<Scene>();
        m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

        if (m_SceneState != SceneState::Edit)
            OnSceneStop();

        m_ActiveScene = m_EditorScene;
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        m_EditorScenePath = std::filesystem::path();
    }

    void EditorLayer::OpenScene()
    {
        auto filepath = FileDialogs::OpenFile("MykaEngine Scene (*.myka)\0*.myka\0");
        if (filepath != std::nullopt)
            OpenScene(filepath.value());
    }

    void EditorLayer::OpenScene(const std::filesystem::path &path)
    {
        if (m_SceneState != SceneState::Edit)
            OnSceneStop();

        if (path.extension().string() != ".myka")
        {
            // TODO: make a file signature with OpenSSL and then check the signature instead of validating whole file
            MYKA_WARN("Could not load {0} - not a scene file", path.filename().string());
            return;
        }

        Ref<Scene> newScene = CreateRef<Scene>();

        SceneSerializer serializer(newScene);
        if (serializer.DeserializeJSON(path))
        {
            m_EditorScene = newScene;
            m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

            m_ActiveScene = m_EditorScene;
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);
            m_EditorScenePath = path;
        }
    }

    void EditorLayer::SaveScene()
    {
        if (!m_EditorScenePath.empty())
        {
            SerializeScene(m_EditorScene, m_EditorScenePath);
        }
    }

    void EditorLayer::SaveSceneAs()
    {
        auto filepath = FileDialogs::SaveFile("MykaEngine Scene (*.myka)\0*.myka\0");

        if (filepath != std::nullopt)
        {
            SerializeScene(m_ActiveScene, filepath.value());

            m_EditorScenePath = filepath.value();
        }
    }

    void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path &path)
    {
        SceneSerializer serializer(scene);
        serializer.SerializeJSON(path);
    }

    void EditorLayer::OnScenePlay()
    {
        if (m_SceneState == SceneState::Simulate)
            OnSceneStop();

        m_SceneState = SceneState::Play;

        if (m_EditorScene)
        {
            m_ActiveScene = Scene::Copy(m_EditorScene);
            m_ActiveScene->OnRuntimeStart();
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        }
        else
        {
            MYKA_ERROR("Cannot play: no active scene!");
            m_SceneState = SceneState::Edit;
        }
    }

    void EditorLayer::OnSceneSimulate()
    {
        if (m_SceneState == SceneState::Play)
            OnSceneStop();

        m_SceneState = SceneState::Simulate;

        if (m_EditorScene)
        {
            m_ActiveScene = Scene::Copy(m_EditorScene);
            m_ActiveScene->OnSimulationStart();
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        }
        else
        {
            MYKA_ERROR("Cannot simulate: no active scene!");
            m_SceneState = SceneState::Edit;
        }
    }

    void EditorLayer::OnSceneStop()
    {
        MYKA_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate, "Unknown SceneState");

        if (m_SceneState == SceneState::Play)
            m_ActiveScene->OnRuntimeStop();
        else if (m_SceneState == SceneState::Simulate)
            m_ActiveScene->OnSimulationStop();

        m_SceneState = SceneState::Edit;

        m_ActiveScene = m_EditorScene;

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnDuplicateEntity()
    {
        if (m_SceneState != SceneState::Edit)
            return;

        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity)
        {
            Entity newEntity = m_EditorScene->DuplicateEntity(selectedEntity);
            m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
        }
    }

} // namespace Myka
