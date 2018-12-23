#include <std_include.hpp>
#include "launcher/launcher.hpp"

int CALLBACK WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	launcher launcher;
	const auto mode = launcher.run();

	if(mode == launcher::mode::NONE)
	{
		return 0;
	}
	else if(mode == launcher::mode::SINGLEPLAYER)
	{
		OutputDebugStringA("\n\nSINGLEPLAYER\n\n");
	}
	else if(mode == launcher::mode::MULTIPLAYER)
	{
		MessageBoxA(nullptr, "Multiplayer not supported yet!", "ERROR", MB_ICONEXCLAMATION);
	}

	return 0;
}
