#pragma once

#include "MykaEngine/Renderer/Texture.hpp"

namespace Myka
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel();

        void OnImGuiRender();

    private:
        std::filesystem::path m_CurrentDirectory;
        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;
    };
} // namespace Myka
