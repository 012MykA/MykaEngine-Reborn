#include "mykapch.hpp"
#include "MykaEngine/Utils/PlatformUtils.hpp"
#include "MykaEngine/Core/Application.hpp"

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Myka
{
    std::optional<std::filesystem::path> FileDialogs::OpenFile(const char *filter)
    {
        OPENFILENAME ofn;
        CHAR szFile[260] = {0};
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow *)Application::Get().GetWindow().GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (GetOpenFileName(&ofn) == TRUE)
        {
            return std::filesystem::path(ofn.lpstrFile);
        }
        return std::nullopt;
    }

    std::optional<std::filesystem::path> FileDialogs::SaveFile(const char *filter)
    {
        OPENFILENAME ofn;
        CHAR szFile[260] = {0};
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow *)Application::Get().GetWindow().GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (GetSaveFileName(&ofn) == TRUE)
        {
            return std::filesystem::path(ofn.lpstrFile);
        }
        return std::nullopt;
    }

} // namespace Myka
