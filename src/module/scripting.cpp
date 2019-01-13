#include <std_include.hpp>
#include "notification.hpp"
#include "utils/io.hpp"
#include "utils/string.hpp"

utils::hook scripting::start_hook_;
utils::hook scripting::stop_hook_;

std::mutex scripting::mutex_;
std::vector<std::function<void()>> scripting::start_callbacks_;
std::vector<std::function<void()>> scripting::stop_callbacks_;

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
	on_start(std::bind(&scripting::initialize, this));
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

void scripting::initialize()
{
	this->chai_ = std::make_unique<chaiscript::ChaiScript>();
	this->chai_->add(chaiscript::fun([](const std::string& string)
	{
		printf("%s\n", string.data());
	}), "print");

	this->chai_->add(chaiscript::fun(
		                 [this](const std::function<void(const std::string&,
		                                                 const std::vector<chaiscript::Boxed_Value>&)>& callback)
		                 {
			                 std::lock_guard _(mutex_);
			                 this->callbacks_.push_back(callback);
		                 }), "onNotify");

	this->load_scripts();

	notification::listen([this](notification::event* event)
	{
		std::vector<chaiscript::Boxed_Value> arguments;

		for (const auto& argument : event->arguments)
		{
			arguments.push_back(make_boxed(argument));
		}

		decltype(this->callbacks_) copy;
		{
			std::lock_guard _(mutex_);
			copy = this->callbacks_;
		}

		for (const auto& callback : copy)
		{
			try
			{
				callback(event->name, arguments);
			}
			catch (std::exception& e)
			{
				printf("Failed to handle event: %s\n", e.what());
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
			catch (std::exception& e)
			{
				printf("Failed to load script %s: %s\n", script.data(), e.what());
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


REGISTER_MODULE(scripting)
