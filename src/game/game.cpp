#include <std_include.hpp>
#include "game.hpp"

namespace game
{
	namespace native
	{
		Cmd_AddCommand_t Cmd_AddCommand;

		Conbuf_AppendText_t Conbuf_AppendText;

		DB_LoadXAssets_t DB_LoadXAssets;

		MSG_ReadData_t MSG_ReadData;

		Sys_ShowConsole_t Sys_ShowConsole;

		int* cmd_args;
		int* cmd_argc;
		const char*** cmd_argv;
	}


	launcher::mode mode = launcher::mode::none;

	bool is_mp()
	{
		return mode == launcher::mode::multiplayer;
	}

	bool is_sp()
	{
		return mode == launcher::mode::singleplayer;
	}

	bool is_dedi()
	{
		return mode == launcher::mode::server;
	}

	void initialize(const launcher::mode _mode)
	{
		mode = _mode;

		native::Cmd_AddCommand = native::Cmd_AddCommand_t(SELECT_VALUE(0x558820, 0x545DF0, 0));

		native::Conbuf_AppendText = native::Conbuf_AppendText_t(SELECT_VALUE(0x4C84E0, 0x5CF610, 0x53C790));

		native::DB_LoadXAssets = native::DB_LoadXAssets_t(SELECT_VALUE(0x48A8E0, 0x4CD020, 0x44F770));

		native::MSG_ReadData = native::MSG_ReadData_t(SELECT_VALUE(0, 0x5592A0, 0));

		native::Sys_ShowConsole = native::Sys_ShowConsole_t(SELECT_VALUE(0x470AF0, 0x5CF590, 0));

		native::cmd_args = reinterpret_cast<int*>(SELECT_VALUE(0x1750750, 0x1C978D0, 0x1B455F8));
		native::cmd_argc = reinterpret_cast<int*>(SELECT_VALUE(0x1750794, 0x1C97914, 0x1B4563C));
		native::cmd_argv = reinterpret_cast<const char***>(SELECT_VALUE(0x17507B4, 0x1C97934, 0x1B4565C));
	}
}
