#include <std_include.hpp>
#include "loader/module_loader.hpp"
#include "notification.hpp"
#include "utils/hook.hpp"
#include "utils/string.hpp"
#include "scheduler.hpp"

std::mutex notification::mutex_;
std::vector<std::function<void(notification::event*)>> notification::callbacks_;

void notification::post_load()
{
	utils::hook(SELECT_VALUE(0x6109F3, 0x56B637, 0x4EDFF7), vm_notify_stub, HOOK_CALL).install()->quick();
	utils::hook(SELECT_VALUE(0x6128BE, 0x56D541, 0x4EFAF9), vm_notify_stub, HOOK_CALL).install()->quick();

	if (game::is_sp())
	{
		utils::hook(0x610970, vm_notify_stub, HOOK_JUMP).install()->quick();
	}

	//scripting::on_start(cleanup);
	scripting::on_stop(cleanup);
}

void notification::pre_destroy()
{
	cleanup();
}

void notification::listen(const std::function<void(event*)>& callback)
{
	std::lock_guard _(mutex_);
	callbacks_.push_back(callback);
}

void notification::cleanup()
{
	std::lock_guard _(mutex_);
	callbacks_.clear();
}

void notification::dispatch(event* event)
{
	decltype(callbacks_) copy;
	{
		std::lock_guard _(mutex_);
		copy = callbacks_;
	}

	for (const auto& callback : copy)
	{
		callback(event);
	}
}

void notification::vm_notify_stub(const unsigned int notify_id, const unsigned short type,
                                  game::native::VariableValue* stack)
{
	try
	{
		event e;
		e.name = game::native::SL_ConvertToString(type);
		e.entity_id = notify_id;

		if (e.name == "touch") return; // Skip that for now

		//printf("%X %X: %s\n", e.entity_id, *game::native::levelEntityId, e.name.data());

		for (auto value = stack; value->type != game::native::SCRIPT_END; --value)
		{
			e.arguments.emplace_back(*value);
		}

		dispatch(&e);
	}
	catch (std::exception& e)
	{
		scripting::propagate_error(e);
	}

	game::native::VM_Notify(notify_id, type, stack);
}

REGISTER_MODULE(notification)
