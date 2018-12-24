#include <std_include.hpp>
#include "launcher/launcher.hpp"
#include "loader/loader.hpp"
#include "loader/module_loader.hpp"
#include "game/game.hpp"

void exit_hook(const int code)
{
	module_loader::pre_destroy();
	exit(code);
}

int CALLBACK WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	FARPROC entry_point = nullptr;

	{
		launcher launcher;
		const auto mode = launcher.run();

		if (mode == launcher::mode::NONE) return 0;

		loader loader(mode);
		loader.set_import_resolver([](const std::string& module, const std::string& function) -> FARPROC
		{
			if (module == "steam_api.dll")
			{
				return utils::nt::module().get_proc<FARPROC>(function);
			}
			else if (function == "ExitProcess")
			{
				return FARPROC(exit_hook);
			}

			return nullptr;
		});

		entry_point = loader.load({});
		if (!entry_point) return 1;

		game::initialize(mode);
		module_loader::post_load();
	}

	return entry_point();
}
