#include <std_include.hpp>

namespace main
{
	void initialize()
	{

	}

	void uninitialize()
	{

	}
}

BOOL APIENTRY DllMain(HMODULE, DWORD call_reason, LPVOID)
{
	if (call_reason == DLL_PROCESS_ATTACH)
	{
		main::initialize();
	}
	else if (call_reason == DLL_PROCESS_DETACH)
	{
		main::uninitialize();
	}

	return TRUE;
}
