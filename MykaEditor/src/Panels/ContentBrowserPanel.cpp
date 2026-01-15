#include "mykapch.hpp"
#include "ContentBrowserPanel.hpp"

#include <imgui.h>

namespace Myka
{
    ContentBrowserPanel::ContentBrowserPanel() : m_CurrentDirectory("assets")
    {
        m_DirectoryIcon = Texture2D::Create("Resources/icons/ContentBrowser/folder-icon.png");
        m_FileIcon = Texture2D::Create("Resources/icons/ContentBrowser/file-icon.png");
    }

    void ContentBrowserPanel::OnImGuiRender()
    {
        ImGui::Begin("Content Browser");

        if (m_CurrentDirectory != std::filesystem::path("assets"))
        {
            if (ImGui::Button("<-"))
            {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
        }

        ImGui::SameLine();

        // Assets
        std::string pathString = m_CurrentDirectory.generic_string();
        char buffer[512]{};
        strncpy_s(buffer, pathString.c_str(), sizeof(buffer));

        ImGui::PushItemWidth(-1);
        if (ImGui::InputText("##PathBar", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            std::filesystem::path newPath(buffer);
            if (std::filesystem::exists(newPath) && std::filesystem::is_directory(newPath))
            {
                m_CurrentDirectory = newPath;
            }
        }
        ImGui::PopItemWidth();

        ImGui::Separator();

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
            std::string filenameString = path.filename().string();

            Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
            ImGui::ImageButton(filenameString.c_str(), (ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

            if (ImGui::BeginDragDropSource())
            {
                const wchar_t *itemPath = path.c_str();
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
