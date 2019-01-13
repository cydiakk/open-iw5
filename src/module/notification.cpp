#include <std_include.hpp>
#include "loader/module_loader.hpp"
#include "notification.hpp"
#include "utils/hook.hpp"

void notification::post_load()
{
	utils::hook(SELECT_VALUE(0x6109F3, 0x56B637, 0x4EDFF7), vm_notify_stub, HOOK_CALL).install()->quick();
	utils::hook(SELECT_VALUE(0x6128BE, 0x56D541, 0x4EFAF9), vm_notify_stub, HOOK_CALL).install()->quick();

	if (game::is_sp())
	{
		utils::hook(0x610970, vm_notify_stub, HOOK_JUMP).install()->quick();
	}
}

void notification::vm_notify_stub(const unsigned int notify_id, const unsigned short type,
                                  game::native::VariableValue* stack)
{
	event e;
	e.name = game::native::SL_ConvertToString(type);
	e.entity = game::native::Scr_GetEntityIdRef(notify_id);

	for (auto value = stack; value->type != game::native::SCRIPT_END; --value)
	{
		e.arguments.emplace_back(*value);
	}

	if(!e.arguments.empty())
	{
		printf("");
	}

	game::native::VM_Notify(notify_id, type, stack);
}

REGISTER_MODULE(notification)
