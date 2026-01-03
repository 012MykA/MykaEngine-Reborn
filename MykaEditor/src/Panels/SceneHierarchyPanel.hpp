#pragma once

#include <MykaEngine.hpp>

namespace Myka
{
    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene> &scene);

        void SetContext(const Ref<Scene> &context);

        void OnImGuiRender();

        void DrawEntityNode(Entity entity);

    private:
        Ref<Scene> m_Context;
        Entity m_SelectionContext;
    };
} // namespace Myka
