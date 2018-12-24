#pragma once

#include "launcher/launcher.hpp"

#define SELECT_VALUE(sp, mp, dedi) (game::is_sp() ? (sp) : (game::is_mp() ? (mp) : (dedi)))

namespace game
{
	namespace native
	{
		typedef void(*Sys_ShowConsole_t)();
		extern Sys_ShowConsole_t Sys_ShowConsole;
	}
	
	bool is_mp();
	bool is_sp();
	bool is_dedi();

	void initialize(launcher::mode mode);
}