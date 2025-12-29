#pragma once

#ifdef MYKA_PLATFORM_WINDOWS

#include "MykaEngine/Core/Application.hpp"
#include "MykaEngine/Core/Log.hpp"

extern Myka::Application *Myka::CreateApplication();

int main(int argc, char **argv)
{
	Myka::Log::Init();

	MYKA_PROFILE_BEGIN_SESSION("Startup", "MykaEngineProfile-Startup.json");
	auto *app = Myka::CreateApplication();
	MYKA_PROFILE_END_SESSION();

	MYKA_PROFILE_BEGIN_SESSION("Runtime", "MykaEngineProfile-Runtime.json");
	app->Run();
	MYKA_PROFILE_END_SESSION();

	MYKA_PROFILE_BEGIN_SESSION("Shutdown", "MykaEngineProfile-Shutdown.json");
	delete app;
	MYKA_PROFILE_END_SESSION();
}

#endif