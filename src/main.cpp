#include <std_include.hpp>
#include "launcher/launcher.hpp"

int CALLBACK WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	launcher launcher;
	launcher.run();
}
