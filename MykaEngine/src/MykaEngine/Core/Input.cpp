#include "mykapch.hpp"
#include "Input.hpp"

#ifdef MYKA_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsInput.hpp"
#endif

namespace Myka
{
	Scope<Input> Input::s_Instance = Input::Create();

	Scope<Input> Input::Create()
	{
	#ifdef MYKA_PLATFORM_WINDOWS
		return CreateScope<WindowsInput>();
	#else
		MYKA_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
	#endif
    }
} // namespace Myka
