#include "mykapch.hpp"
#include "ContentBrowserPanel.hpp"

#include <imgui.h>

namespace Myka
{
    // TODO: change
    extern const std::filesystem::path g_AssetsDirectory = std::filesystem::current_path().parent_path().parent_path() / "MykaEditor/assets";

    ContentBrowserPanel::ContentBrowserPanel() : m_CurrentDirectory(g_AssetsDirectory)
    {
        m_DirectoryIcon = Texture2D::Create((g_AssetsDirectory / "textures/folder-icon.png").string());
        m_FileIcon = Texture2D::Create((g_AssetsDirectory / "textures/file-icon.png").string());
    }

    void ContentBrowserPanel::OnImGuiRender()
    {
        ImGui::Begin("Explorer");

        if (m_CurrentDirectory != std::filesystem::path(g_AssetsDirectory))
        {
            if (ImGui::Button("<-"))
            {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
        }

        static float padding = 20.0f;
        static float thumbnailSize = 80.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, 0, false);
        static ImVec4 color(1.0f, 0.0f, 0.0f, 1.0f);
        for (auto &directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
        {
            const auto &path = directoryEntry.path();
            auto relativePath = std::filesystem::relative(path, g_AssetsDirectory);
            std::string filenameString = path.filename().string();

            Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
            ImGui::ImageButton(filenameString.c_str(), (ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

            if (ImGui::BeginDragDropSource())
            {
                const wchar_t* itemPath = relativePath.c_str();
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
                ImGui::EndDragDropSource();
            }
            
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (directoryEntry.is_directory())
                    m_CurrentDirectory /= path.filename();
            }
            ImGui::TextWrapped(filenameString.c_str());

            ImGui::NextColumn();
        }

        ImGui::Columns(1);
        ImGui::End();
    }

} // namespace Myka
