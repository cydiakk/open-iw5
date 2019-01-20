#include <std_include.hpp>
#include "utils/hook.hpp"
#include "utils/io.hpp"

#include "scheduler.hpp"
#include "scripting.hpp"

utils::hook scripting::start_hook_;
utils::hook scripting::stop_hook_;

void scripting::post_load()
{
	start_hook_.initialize(SELECT_VALUE(0x50C575, 0x50D4F2, 0x48A026), &start_execution_stub, HOOK_CALL) //
	           ->install() //
	           ->quick();

	stop_hook_.initialize(SELECT_VALUE(0x528B04, 0x569E46, 0x4F03FA), &stop_execution_stub, HOOK_CALL) //
	          ->install() // 
	          ->quick();

	utils::hook(SELECT_VALUE(0x6109F3, 0x56B637, 0x4EDFF7), &vm_notify_stub, HOOK_CALL).install()->quick();
	utils::hook(SELECT_VALUE(0x6128BE, 0x56D541, 0x4EFAF9), &vm_notify_stub, HOOK_CALL).install()->quick();

	if (game::is_sp())
	{
		utils::hook(0x610970, &vm_notify_stub, HOOK_JUMP).install()->quick();
	}
}

void scripting::pre_destroy()
{
	this->scripts_.clear();
}

void scripting::load_scripts()
{
	const auto scripts = utils::io::list_files("open-iw5/scripts/");

	for (const auto& script : scripts)
	{
		if (script.substr(script.find_last_of('.') + 1) == "chai")
		{
			try
			{
				auto context = std::make_unique<game::scripting::context>();
				context->get_chai()->eval_file(script);
				this->scripts_.push_back(std::move(context));
			}
			catch (chaiscript::exception::eval_error& e)
			{
				throw std::runtime_error(e.pretty_print());
			}
		}
	}
}

void scripting::start_execution()
{
	try
	{
		this->load_scripts();
	}
	catch (std::exception& e)
	{
		propagate_error(e);
	}
}

void scripting::stop_execution()
{
	this->scripts_.clear();
}

void scripting::dispatch(game::scripting::event* event)
{
	for (const auto& script : this->scripts_)
	{
		script->get_event_handler()->dispatch(event);
	}
}

void scripting::propagate_error(const std::exception& e)
{
	printf("\n******* Script execution error *******\n");
	printf("%s\n", e.what());
	printf("**************************************\n\n");

	scheduler::error("Script execution error\n(see console for actual details)\n", 5);
}

void scripting::start_execution_stub()
{
	module_loader::get<scripting>()->start_execution();
	reinterpret_cast<void(*)()>(start_hook_.get_original())();
}

void scripting::stop_execution_stub()
{
	module_loader::get<scripting>()->stop_execution();
	reinterpret_cast<void(*)()>(stop_hook_.get_original())();
}

void scripting::vm_notify_stub(const unsigned int notify_id, const unsigned short type,
                               game::native::VariableValue* stack)
{
	try
	{
		game::scripting::event e;
		e.name = game::native::SL_ConvertToString(type);
		e.entity_id = notify_id;

		if (e.name == "touch") return; // Skip that for now

		//printf("%X: %s\n", e.entity_id, e.name.data());

		for (auto value = stack; value->type != game::native::SCRIPT_END; --value)
		{
			e.arguments.emplace_back(*value);
		}

		module_loader::get<scripting>()->dispatch(&e);
	}
	catch (std::exception& e)
	{
		propagate_error(e);
	}

	game::native::VM_Notify(notify_id, type, stack);
}

REGISTER_MODULE(scripting)
