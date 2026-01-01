#include "mykapch.hpp"
#include "Window.hpp"

#ifdef MYKA_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsWindow.hpp"
#endif

namespace Myka
{
    Scope<Window> Myka::Window::Create(const WindowProps &props)
    {
    #ifdef MYKA_PLATFORM_WINDOWS
        return CreateScope<WindowsWindow>(props);
    #else
        MYKA_CORE_ASSERT(false, "Unknown platform!");
        return nullptr;
    #endif
    }
} // namespace Myka
