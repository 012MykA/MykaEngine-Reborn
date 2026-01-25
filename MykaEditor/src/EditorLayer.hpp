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

        void OnOverlayRender();

        void NewScene();
        void OpenScene();
        void OpenScene(const std::filesystem::path &path);
        void SaveScene();
        void SaveSceneAs();

        void SerializeScene(Ref<Scene> scene, const std::filesystem::path &path);

        void OnScenePlay();
        void OnSceneSimulate();
        void OnSceneStop();

        void OnDuplicateEntity();

        // UI Panels
        void UI_ToolBar();

    private:
        Ref<Mesh> m_CubeMesh;     // TODO: remove
        Ref<Material> m_Material; // TODO: remove

        Ref<Texture2D> m_IconPlay, m_IconSimulate, m_IconStop;
        Ref<Framebuffer> m_Framebuffer;
        Ref<Scene> m_ActiveScene;
        Ref<Scene> m_EditorScene;
        std::filesystem::path m_EditorScenePath;

        EditorCamera m_EditorCamera;

        bool m_ViewportFocused = false, m_ViewportHovered = false;
        glm::vec2 m_ViewportSize = {0.0f, 0.0f};
        glm::vec2 m_ViewportBounds[2];

        int m_ImGuizmoType = -1;
        int m_ImGuizmoMode = 0;
        bool m_ShowPhysicsColliders = false;
        bool m_ShowTriangles = false;

        ContentBrowserPanel m_ContentBrowserPanel;
        SceneHierarchyPanel m_SceneHierarchyPanel;
        Entity m_HoveredEntity;
        enum class SceneState
        {
            Edit = 0,
            Play,
            Simulate
        };
        SceneState m_SceneState = SceneState::Edit;
        glm::vec4 colliders2DColor = {0.0f, 0.5f, 1.0f, 1.0f};
    };
} // namespace Myka
