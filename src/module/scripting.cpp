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

scripting::entity::entity() : entity(nullptr, 0)
{
}

scripting::entity::entity(const entity& other) : entity(other.environment_, other.entity_id_)
{
}

scripting::entity::entity(scripting* environment, const unsigned int entity_id) : environment_(environment),
                                                                                  entity_id_(entity_id)
{
	if (this->entity_id_)
	{
		game::native::VariableValue value;
		value.type = game::native::SCRIPT_OBJECT;
		value.u.entityId = this->entity_id_;
		game::native::AddRefToValue(&value);
	}
}

scripting::entity::~entity()
{
	if (this->entity_id_)
	{
		game::native::RemoveRefToValue(game::native::SCRIPT_OBJECT, {static_cast<int>(this->entity_id_)});
	}
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

chaiscript::Boxed_Value scripting::entity::call(const std::string& function,
                                                const std::vector<chaiscript::Boxed_Value>& arguments) const
{
	return this->environment_->call(function, this->get_entity_id(), arguments);
}

void scripting::entity::notify(const std::string& event, const std::vector<chaiscript::Boxed_Value>& arguments) const
{
	this->environment_->notify(event, this->get_entity_id(), arguments);
}

void scripting::entity::set(const std::string& field, const chaiscript::Boxed_Value& value) const
{
	this->environment_->set_entity_field(field, this->get_entity_id(), value);
}

chaiscript::Boxed_Value scripting::entity::get(const std::string& field) const
{
	return this->environment_->get_entity_field(field, this->get_entity_id());
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

scripting::stack_context::stack_context()
{
	this->inparamcount_ = game::native::scr_VmPub->inparamcount;
	this->outparamcount_ = game::native::scr_VmPub->outparamcount;
	this->top_ = game::native::scr_VmPub->top;
	this->maxstack_ = game::native::scr_VmPub->maxstack;

	game::native::scr_VmPub->top = this->stack_;
	game::native::scr_VmPub->maxstack = &this->stack_[ARRAYSIZE(this->stack_) - 1];
	game::native::scr_VmPub->inparamcount = 0;
	game::native::scr_VmPub->outparamcount = 0;
}

scripting::stack_context::~stack_context()
{
	game::native::Scr_ClearOutParams();
	game::native::scr_VmPub->inparamcount = this->inparamcount_;
	game::native::scr_VmPub->outparamcount = this->outparamcount_;
	game::native::scr_VmPub->top = this->top_;
	game::native::scr_VmPub->maxstack = this->maxstack_;
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
	this->chai_->add(chaiscript::fun([](const std::string& string)
	{
		printf("%s\n", string.data());
	}), "print");

	this->chai_->add(chaiscript::fun([](const std::string& string)
	{
		MessageBoxA(nullptr, string.data(), nullptr, 0);
	}), "alert");

	this->initialize_entity();

	const auto level_id = *game::native::levelEntityId;
	this->chai_->add_global(chaiscript::var(entity(this, level_id)), "level");

	this->load_scripts();

	notification::listen([this](notification::event* event)
	{
		try
		{
			std::vector<chaiscript::Boxed_Value> arguments;

			for (const auto& argument : event->arguments)
			{
				arguments.push_back(this->make_boxed(argument));
			}

			for (auto listener = this->event_listeners_.begin(); listener.is_valid(); ++listener)
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

			for (auto listener = this->generic_event_listeners_.begin(); listener.is_valid(); ++listener)
			{
				if (listener->event == event->name)
				{
					if (listener->is_volatile)
					{
						this->generic_event_listeners_.remove(listener);
					}

					listener->callback(entity(this, event->entity_id), arguments);
				}
			}
		}
		catch (chaiscript::exception::eval_error& e)
		{
			throw std::runtime_error(e.pretty_print());
		}
	});
}

void scripting::initialize_entity()
{
	this->chai_->add(chaiscript::user_type<entity>(), "entity");
	this->chai_->add(chaiscript::constructor<entity()>(), "entity");
	this->chai_->add(chaiscript::constructor<entity(const entity&)>(), "entity");

	this->chai_->add(chaiscript::fun(&entity::get), "get");
	this->chai_->add(chaiscript::fun(&entity::set), "set");

	this->chai_->add(chaiscript::fun(&entity::on_notify), "onNotify");
	this->chai_->add(chaiscript::fun([](const entity& ent, const std::string& event,
	                                    const std::function<void(const std::vector<chaiscript::Boxed_Value>&)>&
	                                    callback)
	{
		return ent.on_notify(event, callback, false);
	}), "onNotify");

	this->chai_->add(chaiscript::fun([](entity& lhs, const entity& rhs) -> entity&
	{
		return lhs = rhs;
	}), "=");

	this->chai_->add(chaiscript::fun([this](const std::string& event,
	                                        const std::function<void(const entity&,
	                                                                 const std::vector<chaiscript::Boxed_Value>&)>&
	                                        callback)
	{
		generic_event_listener listener;
		listener.event = event;
		listener.is_volatile = false;
		listener.callback = callback;

		this->generic_event_listeners_.add(listener);
	}), "onNotify");

	this->chai_->add(chaiscript::fun([this](const std::string& event,
	                                        const std::function<void(const entity&,
	                                                                 const std::vector<chaiscript::Boxed_Value>&)>&
	                                        callback, const bool is_volatile)
	{
		generic_event_listener listener;
		listener.event = event;
		listener.is_volatile = is_volatile;
		listener.callback = callback;

		this->generic_event_listeners_.add(listener);
	}), "onNotify");

	// Notification
	this->chai_->add(chaiscript::fun(&entity::notify), "vectorNotify");
	this->chai_->add(chaiscript::fun([](const entity& ent, const std::string& event)
	{
		return ent.notify(event, {});
	}), "notify");

	this->chai_->add(chaiscript::fun(
		                 [](const entity& ent, const std::string& event,
		                    const chaiscript::Boxed_Value& a1)
		                 {
			                 return ent.notify(event, {a1});
		                 }), "notify");

	this->chai_->add(chaiscript::fun(
		                 [](const entity& ent, const std::string& event,
		                    const chaiscript::Boxed_Value& a1,
		                    const chaiscript::Boxed_Value& a2)
		                 {
			                 return ent.notify(event, {a1, a2});
		                 }), "notify");

	this->chai_->add(chaiscript::fun(
		                 [](const entity& ent, const std::string& event,
		                    const chaiscript::Boxed_Value& a1,
		                    const chaiscript::Boxed_Value& a2,
		                    const chaiscript::Boxed_Value& a3)
		                 {
			                 return ent.notify(event, {a1, a2, a3});
		                 }), "notify");

	this->chai_->add(chaiscript::fun(
		                 [](const entity& ent, const std::string& event,
		                    const chaiscript::Boxed_Value& a1,
		                    const chaiscript::Boxed_Value& a2,
		                    const chaiscript::Boxed_Value& a3,
		                    const chaiscript::Boxed_Value& a4)
		                 {
			                 return ent.notify(event, {a1, a2, a3, a4});
		                 }), "notify");

	this->chai_->add(chaiscript::fun(
		                 [](const entity& ent, const std::string& event,
		                    const chaiscript::Boxed_Value& a1,
		                    const chaiscript::Boxed_Value& a2,
		                    const chaiscript::Boxed_Value& a3,
		                    const chaiscript::Boxed_Value& a4,
		                    const chaiscript::Boxed_Value& a5)
		                 {
			                 return ent.notify(event, {a1, a2, a3, a4, a5});
		                 }), "notify");

	// Instance call
	this->chai_->add(chaiscript::fun(&entity::call), "vectorCall");
	this->chai_->add(chaiscript::fun([](const entity& ent, const std::string& function)
	{
		return ent.call(function, {});
	}), "call");

	this->chai_->add(chaiscript::fun(
		                 [](const entity& ent, const std::string& function,
		                    const chaiscript::Boxed_Value& a1)
		                 {
			                 return ent.call(function, {a1});
		                 }), "call");

	this->chai_->add(chaiscript::fun(
		                 [](const entity& ent, const std::string& function,
		                    const chaiscript::Boxed_Value& a1,
		                    const chaiscript::Boxed_Value& a2)
		                 {
			                 return ent.call(function, {a1, a2});
		                 }), "call");

	this->chai_->add(chaiscript::fun(
		                 [](const entity& ent, const std::string& function,
		                    const chaiscript::Boxed_Value& a1,
		                    const chaiscript::Boxed_Value& a2,
		                    const chaiscript::Boxed_Value& a3)
		                 {
			                 return ent.call(function, {a1, a2, a3});
		                 }), "call");

	this->chai_->add(chaiscript::fun(
		                 [](const entity& ent, const std::string& function,
		                    const chaiscript::Boxed_Value& a1,
		                    const chaiscript::Boxed_Value& a2,
		                    const chaiscript::Boxed_Value& a3,
		                    const chaiscript::Boxed_Value& a4)
		                 {
			                 return ent.call(function, {a1, a2, a3, a4});
		                 }), "call");

	this->chai_->add(chaiscript::fun(
		                 [](const entity& ent, const std::string& function,
		                    const chaiscript::Boxed_Value& a1,
		                    const chaiscript::Boxed_Value& a2,
		                    const chaiscript::Boxed_Value& a3,
		                    const chaiscript::Boxed_Value& a4,
		                    const chaiscript::Boxed_Value& a5)
		                 {
			                 return ent.call(function, {a1, a2, a3, a4, a5});
		                 }), "call");

	// Global call
	this->chai_->add(chaiscript::fun(
		                 [this](const std::string& function, const std::vector<chaiscript::Boxed_Value>& arguments)
		                 {
			                 return this->call(function, 0, arguments);
		                 }), "vectorCall");
	this->chai_->add(chaiscript::fun([this](const std::string& function)
	{
		return this->call(function, 0, {});
	}), "call");

	this->chai_->add(chaiscript::fun(
		                 [this](const std::string& function,
		                        const chaiscript::Boxed_Value& a1)
		                 {
			                 return this->call(function, 0, {a1});
		                 }), "call");

	this->chai_->add(chaiscript::fun(
		                 [this](const std::string& function,
		                        const chaiscript::Boxed_Value& a1,
		                        const chaiscript::Boxed_Value& a2)
		                 {
			                 return this->call(function, 0, {a1, a2});
		                 }), "call");

	this->chai_->add(chaiscript::fun(
		                 [this](const std::string& function,
		                        const chaiscript::Boxed_Value& a1,
		                        const chaiscript::Boxed_Value& a2,
		                        const chaiscript::Boxed_Value& a3)
		                 {
			                 return this->call(function, 0, {a1, a2, a3});
		                 }), "call");

	this->chai_->add(chaiscript::fun(
		                 [this](const std::string& function,
		                        const chaiscript::Boxed_Value& a1,
		                        const chaiscript::Boxed_Value& a2,
		                        const chaiscript::Boxed_Value& a3,
		                        const chaiscript::Boxed_Value& a4)
		                 {
			                 return this->call(function, 0, {a1, a2, a3, a4});
		                 }), "call");

	this->chai_->add(chaiscript::fun(
		                 [this](const std::string& function,
		                        const chaiscript::Boxed_Value& a1,
		                        const chaiscript::Boxed_Value& a2,
		                        const chaiscript::Boxed_Value& a3,
		                        const chaiscript::Boxed_Value& a4,
		                        const chaiscript::Boxed_Value& a5)
		                 {
			                 return this->call(function, 0, {a1, a2, a3, a4, a5});
		                 }), "call");
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
	else if (value.type == game::native::SCRIPT_VECTOR)
	{
		std::vector<chaiscript::Boxed_Value> values;
		values.push_back(chaiscript::var(value.u.vectorValue[0]));
		values.push_back(chaiscript::var(value.u.vectorValue[1]));
		values.push_back(chaiscript::var(value.u.vectorValue[2]));

		return chaiscript::var(values);
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

int scripting::get_field_id(const int classnum, const std::string& field) const
{
	const auto field_name = utils::string::to_lower(field);
	const auto class_id = game::native::g_classMap[classnum].id;
	const auto field_str = game::native::SL_GetString(field_name.data(), 1);
	const auto _ = gsl::finally([field_str]()
	{
		game::native::VariableUnion u{};
		u.stringValue = field_str;
		game::native::RemoveRefToValue(game::native::SCRIPT_STRING, u);
	});

	const auto offset = game::native::FindVariable(class_id, field_str);
	if (offset)
	{
		const auto index = 4 * (offset + 0xC800 * (class_id & 1));
		return PINT(SELECT_VALUE(0x1A3BC80, 0x1EFE180, 0x1DC8800))[index];
	}

	return -1;
}

void scripting::set_entity_field(const std::string& field, const unsigned int entity_id,
                                 const chaiscript::Boxed_Value& value)
{
	const auto entref = game::native::Scr_GetEntityIdRef(entity_id);
	const int id = get_field_id(entref.raw.classnum, field);

	if (id != -1)
	{
		stack_context _;
		this->push_param(value);

		game::native::scr_VmPub->outparamcount = game::native::scr_VmPub->inparamcount;
		game::native::scr_VmPub->inparamcount = 0;

		if (!set_entity_field_safe(entref, id))
		{
			throw std::runtime_error("Failed to set value for field '" + field + "'");
		}
	}
	else
	{
		this->entity_fields_[entity_id][field] = value;
	}
}

chaiscript::Boxed_Value scripting::get_entity_field(const std::string& field, const unsigned int entity_id)
{
	const auto entref = game::native::Scr_GetEntityIdRef(entity_id);
	const auto id = this->get_field_id(entref.raw.classnum, field);

	if (id != -1)
	{
		stack_context _;

		game::native::VariableValue value{};
		if (!get_entity_field_safe(entref, id, &value))
		{
			throw std::runtime_error("Failed to get value for field '" + field + "'");
		}

		const auto $ = gsl::finally([value]()
		{
			game::native::RemoveRefToValue(value.type, value.u);
		});

		return this->make_boxed(value);
	}
	else
	{
		const auto& map = this->entity_fields_[entity_id];
		const auto value = map.find(field);
		if (value != map.end())
		{
			return value->second;
		}
	}

	return {};
}

void scripting::notify(const std::string& event, const unsigned int entity_id,
                       std::vector<chaiscript::Boxed_Value> arguments) const
{
	stack_context _;

	std::reverse(arguments.begin(), arguments.end());
	for (const auto& argument : arguments)
	{
		this->push_param(argument);
	}

	const auto event_id = game::native::SL_GetString(event.data(), 0);
	game::native::Scr_NotifyId(entity_id, event_id, game::native::scr_VmPub->inparamcount);
}

chaiscript::Boxed_Value scripting::call(const std::string& function, const unsigned int entity_id,
                                        std::vector<chaiscript::Boxed_Value> arguments)
{
	const auto function_index = find_function_index(function, entity_id == 0);
	if (function_index < 0)
	{
		throw std::runtime_error("No function found for name '" + function + "'");
	}

	const auto entity = function_index > 0x1C7
		                    ? game::native::Scr_GetEntityIdRef(entity_id)
		                    : game::native::scr_entref_t{~0u};

	const auto function_ptr = game::native::Scr_GetFunc(function_index);

	stack_context _;

	std::reverse(arguments.begin(), arguments.end());
	for (const auto& argument : arguments)
	{
		this->push_param(argument);
	}

	game::native::scr_VmPub->outparamcount = game::native::scr_VmPub->inparamcount;
	game::native::scr_VmPub->inparamcount = 0;

	if (!call_safe(function_ptr, entity))
	{
		throw std::runtime_error("Error executing function '" + function + "'");
	}

	return this->get_return_value();
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

bool scripting::set_entity_field_safe(game::native::scr_entref_t entref, int offset)
{
	*game::native::g_script_error_level += 1;
	if (setjmp(game::native::g_script_error[*game::native::g_script_error_level]))
	{
		*game::native::g_script_error_level -= 1;
		return false;
	}

	game::native::Scr_SetObjectField(entref.raw.classnum, entref.raw.entnum, offset);

	*game::native::g_script_error_level -= 1;
	return true;
}

bool scripting::get_entity_field_safe(game::native::scr_entref_t entref, int offset, game::native::VariableValue* value)
{
	*game::native::g_script_error_level += 1;
	if (setjmp(game::native::g_script_error[*game::native::g_script_error_level]))
	{
		value->type = game::native::SCRIPT_NONE;
		value->u.intValue = 0;
		*game::native::g_script_error_level -= 1;
		return false;
	}

	*value = game::native::GetEntityFieldValue(entref.raw.classnum, entref.raw.entnum, offset);

	*game::native::g_script_error_level -= 1;
	return true;
}
#pragma warning(pop)

int scripting::find_function_index(const std::string& function, const bool prefer_global)
{
	const auto target = utils::string::to_lower(function);

	const auto primary_map = prefer_global
		                         ? &game::scripting::global_function_map
		                         : &game::scripting::instance_function_map;
	const auto secondary_map = !prefer_global
		                           ? &game::scripting::global_function_map
		                           : &game::scripting::instance_function_map;

	auto function_entry = primary_map->find(target);
	if (function_entry != primary_map->end())
	{
		return function_entry->second;
	}

	function_entry = secondary_map->find(target);
	if (function_entry != secondary_map->end())
	{
		return function_entry->second;
	}

	return -1;
}

void scripting::push_param(const chaiscript::Boxed_Value& value) const
{
	if (game::native::scr_VmPub->outparamcount)
	{
		game::native::Scr_ClearOutParams();
	}

	if (game::native::scr_VmPub->top == game::native::scr_VmPub->maxstack)
	{
		throw std::runtime_error("Internal script stack overflow");
	}

	game::native::VariableValue* value_ptr = ++game::native::scr_VmPub->top;
	++game::native::scr_VmPub->inparamcount;

	value_ptr->type = game::native::SCRIPT_NONE;
	value_ptr->u.intValue = 0;

	if (value.get_type_info() == typeid(float))
	{
		const auto real_value = this->chai_->boxed_cast<float>(value);
		value_ptr->type = game::native::SCRIPT_FLOAT;
		value_ptr->u.floatValue = real_value;
	}
	else if (value.get_type_info() == typeid(double))
	{
		const auto real_value = this->chai_->boxed_cast<double>(value);
		value_ptr->type = game::native::SCRIPT_FLOAT;
		value_ptr->u.floatValue = static_cast<float>(real_value);
	}
	else if (value.get_type_info() == typeid(int))
	{
		const auto real_value = this->chai_->boxed_cast<int>(value);
		value_ptr->type = game::native::SCRIPT_INTEGER;
		value_ptr->u.intValue = real_value;
	}
	else if (value.get_type_info() == typeid(bool))
	{
		const auto real_value = this->chai_->boxed_cast<bool>(value);
		value_ptr->type = game::native::SCRIPT_INTEGER;
		value_ptr->u.intValue = real_value;
	}
	else if (value.get_type_info() == typeid(entity))
	{
		const auto real_value = this->chai_->boxed_cast<entity>(value);
		value_ptr->type = game::native::SCRIPT_OBJECT;
		value_ptr->u.entityId = real_value.get_entity_id();

		game::native::AddRefToValue(value_ptr);
	}
	else if (value.get_type_info() == typeid(std::string))
	{
		const auto real_value = this->chai_->boxed_cast<std::string>(value);
		value_ptr->type = game::native::SCRIPT_STRING;
		value_ptr->u.stringValue = game::native::SL_GetString(real_value.data(), 0);
	}
	else if (value.get_type_info() == typeid(std::vector<chaiscript::Boxed_Value>))
	{
		float values[3];
		const auto real_value = this->chai_->boxed_cast<std::vector<chaiscript::Boxed_Value>>(value);
		if (real_value.size() != 3)
		{
			throw std::runtime_error("Invalid vector length. Size must be exactly 3");
		}

		const auto unbox_float = [&real_value, this](const size_t index) -> float
		{
			const auto value = real_value[index];
			if (value.get_type_info() == typeid(float))
			{
				return this->chai_->boxed_cast<float>(value);
			}
			else if (value.get_type_info() == typeid(double))
			{
				return float(this->chai_->boxed_cast<double>(value));
			}
			else if (value.get_type_info() == typeid(int))
			{
				return float(this->chai_->boxed_cast<int>(value));
			}

			throw std::runtime_error("Vector element at index " + std::to_string(index) + " is not a number");
		};

		values[0] = unbox_float(0);
		values[1] = unbox_float(1);
		values[2] = unbox_float(2);

		value_ptr->type = game::native::SCRIPT_VECTOR;
		value_ptr->u.vectorValue = game::native::Scr_AllocVector(values);
	}
	else
	{
		throw std::runtime_error("Unable to unbox value of type '" + value.get_type_info().bare_name() + "'");
	}
}

chaiscript::Boxed_Value scripting::get_return_value()
{
	if (game::native::scr_VmPub->inparamcount == 0) return {};

	game::native::Scr_ClearOutParams();
	game::native::scr_VmPub->outparamcount = game::native::scr_VmPub->inparamcount;
	game::native::scr_VmPub->inparamcount = 0;

	return this->make_boxed(game::native::scr_VmPub->top[1 - game::native::scr_VmPub->outparamcount]);
}


REGISTER_MODULE(scripting)
