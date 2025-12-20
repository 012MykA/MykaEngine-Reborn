#pragma once

#ifdef MYKA_PLATFORM_WINDOWS

#include "MykaEngine/Log.hpp"

extern Myka::Application* Myka::CreateApplication();

int main(int argc, char** argv)
{
	Myka::Log::Init();

	auto* app = Myka::CreateApplication();
	app->Run();
	delete app;
}

#endif