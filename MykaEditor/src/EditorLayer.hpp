#pragma once

#include <MykaEngine.hpp>

#include "MykaEngine/Renderer/EditorCamera.hpp"

#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"

#include <filesystem>

namespace Myka
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        virtual void OnUpdate(Timestep ts) override;
        virtual void OnImGuiRender() override;
        virtual void OnEvent(Event &e) override;

    private:
        bool OnKeyPressed(KeyPressedEvent &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);

        void NewScene();
        void OpenScene();
        void OpenScene(const std::filesystem::path &path);
        void SaveSceneAs();

        void OnScenePlay();
        void OnSceneStop();

        // UI Panels
        void UI_ToolBar();

    private:
        OrthographicCameraController m_CameraController;

        // Temp
        Ref<VertexArray> m_VertexArray;
        Ref<Shader> m_FlatColorShader;
        Ref<Framebuffer> m_Framebuffer;

        Ref<Scene> m_ActiveScene;
        Entity m_SquareEntity;
        Entity m_CameraEntity;
        Entity m_SecondCamera;

        Entity m_HoveredEntity;

        bool m_PrimaryCamera = true;
        EditorCamera m_EditorCamera;

        bool m_ViewportFocused = false, m_ViewportHovered = false;
        glm::vec2 m_ViewportSize = {0.0f, 0.0f};
        glm::vec2 m_ViewportBounds[2];

        enum class SceneState
        {
            Edit = 0,
            Play = 1
        };

        SceneState m_SceneState = SceneState::Edit;

        int m_ImGuizmoType = -1;
        int m_ImGuizmoMode = 0;

        SceneHierarchyPanel m_SceneHierarchyPanel;
        ContentBrowserPanel m_ContentBrowserPanel;

        // Editor resources
        Ref<Texture2D> m_IconPlay, m_IconStop;
    };
} // namespace Myka
