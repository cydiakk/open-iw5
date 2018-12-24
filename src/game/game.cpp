#include <std_include.hpp>
#include "game.hpp"

namespace game
{
	namespace native
	{
		Sys_ShowConsole_t Sys_ShowConsole;
	}


	launcher::mode mode = launcher::mode::NONE;

	bool is_mp()
	{
		return mode == launcher::mode::MULTIPLAYER;
	}

	bool is_sp()
	{
		return mode == launcher::mode::SINGLEPLAYER;
	}

	bool is_dedi()
	{
		return mode == launcher::mode::SERVER;
	}

	void initialize(const launcher::mode _mode)
	{
		mode = _mode;

		native::Sys_ShowConsole = native::Sys_ShowConsole_t(SELECT_VALUE(0x470AF0, 0x5CF590, 0));
	}
}
