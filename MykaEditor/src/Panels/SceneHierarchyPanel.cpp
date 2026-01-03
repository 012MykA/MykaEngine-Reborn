#include "mykapch.hpp"
#include "SceneHierarchyPanel.hpp"

#include <imgui.h>

namespace Myka
{
    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &scene)
    {
        SetContext(scene);
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene> &context)
    {
        m_Context = context;
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");

        auto view = m_Context->m_Registry.view<entt::entity>();
        for (auto entity : view)
        {
            DrawEntityNode(Entity(entity, m_Context.get()));
        }

        ImGui::End(); // Scene Hierarchy
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        const auto &tag = entity.GetComponent<TagComponent>().Tag;

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        bool opened = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, flags, tag.c_str());
        if (ImGui::IsItemClicked())
        {
            m_SelectionContext = entity;
        }

        if (opened)
        {
            ImGui::TreePop();
        }
    }

} // namespace Myka