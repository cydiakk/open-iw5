#include <std_include.hpp>
#include "notification.hpp"
#include "utils/io.hpp"
#include "utils/string.hpp"
#include "scheduler.hpp"
#include "game/scripting/functions.hpp"

utils::hook scripting::start_hook_;
utils::hook scripting::stop_hook_;

std::mutex scripting::mutex_;
std::vector<std::function<void()>> scripting::start_callbacks_;
std::vector<std::function<void()>> scripting::stop_callbacks_;

scripting::entity::entity() : environment_(nullptr), entity_id_(0)
{
}

scripting::entity::entity(scripting* environment, const unsigned int entity_id) : environment_(environment),
                                                                                  entity_id_(entity_id)
{
}

void scripting::entity::on_notify(const std::string& event,
                                  const std::function<void(const std::vector<chaiscript::Boxed_Value>&)>& callback,
                                  const bool is_volatile)
const
{
	event_listener listener;
	listener.event = event;
	listener.callback = callback;
	listener.entity_id = this->entity_id_;
	listener.is_volatile = is_volatile;

	this->environment_->add_event_listener(listener);
}

unsigned int scripting::entity::get_entity_id() const
{
	return this->entity_id_;
}

game::native::scr_entref_t scripting::entity::get_entity_reference() const
{
	return game::native::Scr_GetEntityIdRef(this->get_entity_id());
}

void scripting::entity::call(const std::string& function, const std::vector<chaiscript::Boxed_Value>& arguments)
{
	scripting::call(function, this->get_entity_id(), arguments);
}

scripting::variable::variable(game::native::VariableValue value) : value_(value)
{
	game::native::AddRefToValue(&value);
}

scripting::variable::~variable()
{
	game::native::RemoveRefToValue(this->value_.type, this->value_.u);
}

scripting::variable::operator game::native::VariableValue() const
{
	return this->value_;
}

void scripting::post_start()
{
	on_start([this]()
	{
		try
		{
			this->initialize();
		}
		catch (std::exception& e)
		{
			propagate_error(e);
		}
	});
	on_stop([this]()
	{
		this->chai_ = {};
	});
}

void scripting::post_load()
{
	start_hook_.initialize(SELECT_VALUE(0x50C575, 0x50D4F2, 0x48A026), []()
	{
		start_execution();
		static_cast<void(*)()>(start_hook_.get_original())();
	}, HOOK_CALL)->install()->quick();

	stop_hook_.initialize(SELECT_VALUE(0x528B04, 0x569E46, 0x4F03FA), []()
	{
		stop_execution();
		static_cast<void(*)()>(stop_hook_.get_original())();
	}, HOOK_CALL)->install()->quick();
}

void scripting::pre_destroy()
{
	this->chai_ = {};
	start_callbacks_.clear();
	stop_callbacks_.clear();
}

void scripting::add_event_listener(const event_listener& listener)
{
	this->event_listeners_.add(listener);
}

void scripting::initialize()
{
	this->chai_ = std::make_unique<chaiscript::ChaiScript>();
	this->chai_->add(chaiscript::user_type<entity>(), "entity");
	this->chai_->add(chaiscript::constructor<entity()>(), "entity");
	this->chai_->add(chaiscript::constructor<entity(const entity&)>(), "entity");
	this->chai_->add(chaiscript::fun(&entity::on_notify), "onNotify");
	this->chai_->add(chaiscript::fun(&entity::call), "call");
	this->chai_->add(chaiscript::fun([](entity& lhs, const entity& rhs) -> entity&
	{
		return lhs = rhs;
	}), "=");

	this->chai_->add(chaiscript::fun([](const std::string& string)
	{
		printf("%s\n", string.data());
	}), "print");

	this->chai_->add(chaiscript::fun([](const std::string& string)
	{
		MessageBoxA(nullptr, string.data(), nullptr, 0);
	}), "alert");

	const auto level_id = *game::native::levelEntityId;
	this->chai_->add_global(chaiscript::var(entity(this, level_id)), "level");

	this->load_scripts();

	notification::listen([this](notification::event* event)
	{
		std::vector<chaiscript::Boxed_Value> arguments;

		for (const auto& argument : event->arguments)
		{
			arguments.push_back(this->make_boxed(argument));
		}

		for (auto listener = this->event_listeners_.begin(); listener.is_valid(); ++listener)
		{
			try
			{
				if (listener->event == event->name && listener->entity_id == event->entity_id)
				{
					if (listener->is_volatile)
					{
						this->event_listeners_.remove(listener);
					}

					listener->callback(arguments);
				}
			}
			catch (chaiscript::exception::eval_error& e)
			{
				throw std::runtime_error(e.pretty_print());
			}
		}
	});
}

void scripting::load_scripts() const
{
	const auto scripts = utils::io::list_files("open-iw5/scripts/");

	for (const auto& script : scripts)
	{
		if (script.substr(script.find_last_of('.') + 1) == "chai")
		{
			try
			{
				this->chai_->eval_file(script);
			}
			catch (chaiscript::exception::eval_error& e)
			{
				throw std::runtime_error(e.pretty_print());
			}
		}
	}
}

chaiscript::Boxed_Value scripting::make_boxed(const game::native::VariableValue value)
{
	if (value.type == game::native::SCRIPT_STRING)
	{
		const std::string string = game::native::SL_ConvertToString(value.u.stringValue);
		return chaiscript::var(string);
	}
	else if (value.type == game::native::SCRIPT_FLOAT)
	{
		return chaiscript::var(value.u.floatValue);
	}
	else if (value.type == game::native::SCRIPT_INTEGER)
	{
		return chaiscript::var(value.u.intValue);
	}
	else if (value.type == game::native::SCRIPT_OBJECT)
	{
		return chaiscript::var(entity(this, value.u.entityId));
	}

	return {};
}

void scripting::on_start(const std::function<void()>& callback)
{
	std::lock_guard _(mutex_);
	start_callbacks_.push_back(callback);
}

void scripting::on_stop(const std::function<void()>& callback)
{
	std::lock_guard _(mutex_);
	stop_callbacks_.push_back(callback);
}

void scripting::propagate_error(const std::exception& e)
{
	printf("\n******* Script execution error *******\n");
	printf("%s\n", e.what());
	printf("**************************************\n\n");

	scheduler::error("Script execution error\n(see console for actual details)\n", 5);
}

void scripting::start_execution()
{
	decltype(start_callbacks_) copy;
	{
		std::lock_guard _(mutex_);
		copy = start_callbacks_;
	}

	for (const auto& callback : copy)
	{
		callback();
	}
}

void scripting::stop_execution()
{
	decltype(stop_callbacks_) copy;
	{
		std::lock_guard _(mutex_);
		copy = stop_callbacks_;
		std::reverse(copy.begin(), copy.end());
	}

	for (const auto& callback : copy)
	{
		callback();
	}
}

void scripting::call(const std::string& function, const unsigned int entity_id,
                     const std::vector<chaiscript::Boxed_Value>& /*arguments*/)
{
	const int function_index = find_function_index(function);
	if (function_index < 0)
	{
		throw std::runtime_error("No function found for name '" + function + "'");
	}

	const game::native::scr_entref_t entity = function_index > 0x1C7
		                                    ? game::native::Scr_GetEntityIdRef(entity_id)
		                                    : game::native::scr_entref_t{~0u};

	const auto function_ptr = game::native::Scr_GetFunc(function_index);

	if(!call_safe(function_ptr, entity))
	{
		throw std::runtime_error("Error executing function '" + function + "'");
	}
}

#pragma warning(push)
#pragma warning(disable: 4611)
bool scripting::call_safe(const game::native::scr_call_t function, const game::native::scr_entref_t entref)
{
	static_assert(sizeof(jmp_buf) == 64);

	*game::native::g_script_error_level += 1;
	if (setjmp(game::native::g_script_error[*game::native::g_script_error_level]))
	{
		*game::native::g_script_error_level -= 1;
		return false;
	}

	function(entref.val);

	*game::native::g_script_error_level -= 1;
	return true;
}
#pragma warning(pop)

int scripting::find_function_index(const std::string& function)
{
	auto function_entry = game::scripting::instance_function_map.find(function);
	if (function_entry != game::scripting::instance_function_map.end())
	{
		return function_entry->second;
	}

	function_entry = game::scripting::global_function_map.find(function);
	if (function_entry != game::scripting::global_function_map.end())
	{
		return function_entry->second;
	}

	return -1;
}


REGISTER_MODULE(scripting)
