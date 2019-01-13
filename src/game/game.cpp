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

		RemoveRefToValue_t RemoveRefToValue;

		Sys_ShowConsole_t Sys_ShowConsole;

		VM_Notify_t VM_Notify;

		int* cmd_args;
		int* cmd_argc;
		const char*** cmd_argv;

		short* scrVarGlob;
		char** scrMemTreePub;

		void AddRefToValue(VariableValue* value)
		{
			if (value->type == SCRIPT_OBJECT)
			{
				++scrVarGlob[4 * value->u.entityId];
			}
			else if ((value->type & ~1) == SCRIPT_STRING)
			{
				static const auto size = is_sp() ? 16 : 12;
				const auto ref_count = reinterpret_cast<unsigned volatile *>(*scrMemTreePub + size * value
				                                                                                     ->u.stringValue);
				InterlockedIncrement(ref_count);
			}
			else if (value->type == SCRIPT_VECTOR)
			{
				if (!*PBYTE(value->u.vectorValue - 1))
				{
					++*PWORD(value->u.vectorValue - 4);
				}
			}
		}

		scr_entref_t Scr_GetEntityIdRef(const unsigned int id)
		{
			static auto class_array = reinterpret_cast<DWORD*>(SELECT_VALUE(0x19AFC84, 0x1E72184, 0x1D3C804));
			static auto ent_array = reinterpret_cast<WORD*>(SELECT_VALUE(0x19AFC82, 0x1E72182, 0x1D3C802));

			scr_entref_t result{};
			result.raw.classnum = static_cast<unsigned short>(class_array[2 * id]) >> 8;
			result.raw.entnum = ent_array[4 * id];

			return result;
		}

		const char* SL_ConvertToString(unsigned int stringValue)
		{
			if (!stringValue) return nullptr;

			static const auto size = is_sp() ? 16 : 12;
			return *scrMemTreePub + size * stringValue + 4;
		}
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

		native::RemoveRefToValue = native::RemoveRefToValue_t(SELECT_VALUE(0x477EA0, 0x565730, 0x4E8A40));

		native::Sys_ShowConsole = native::Sys_ShowConsole_t(SELECT_VALUE(0x470AF0, 0x5CF590, 0));

		native::VM_Notify = native::VM_Notify_t(SELECT_VALUE(0x610200, 0x569720, 0x4EF450));

		native::cmd_args = reinterpret_cast<int*>(SELECT_VALUE(0x1750750, 0x1C978D0, 0x1B455F8));
		native::cmd_argc = reinterpret_cast<int*>(SELECT_VALUE(0x1750794, 0x1C97914, 0x1B4563C));
		native::cmd_argv = reinterpret_cast<const char***>(SELECT_VALUE(0x17507B4, 0x1C97934, 0x1B4565C));

		native::scrVarGlob = reinterpret_cast<short*>(SELECT_VALUE(0x19AFC80, 0x1E72180, 0x1D3C800));
		native::scrMemTreePub = reinterpret_cast<char**>(SELECT_VALUE(0x196FB00, 0x1E32000, 0x1C152A4));
	}
}
