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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		main::initialize();
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		main::uninitialize();
	}

	return TRUE;
}
