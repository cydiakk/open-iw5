#include <std_include.hpp>
#include "notification.hpp"
#include "utils/io.hpp"
#include "scheduler.hpp"

utils::hook scripting::start_hook_;
utils::hook scripting::stop_hook_;

std::mutex scripting::mutex_;
std::vector<std::function<void()>> scripting::start_callbacks_;
std::vector<std::function<void()>> scripting::stop_callbacks_;

void scripting::post_start()
{
	on_start([this]()
	{
		try
		{
			this->load_scripts();
			notification::listen([this](game::scripting::event* event)
			{
				for(const auto& script : this->scripts_)
				{
					script->get_event_handler()->dispatch(event);
				}
			});
		}
		catch (std::exception& e)
		{
			propagate_error(e);
		}
	});
	on_stop([this]()
	{
		this->scripts_.clear();
	});
}

void scripting::post_load()
{
	start_hook_.initialize(SELECT_VALUE(0x50C575, 0x50D4F2, 0x48A026), []()
	{
		start_execution();
		reinterpret_cast<void(*)()>(start_hook_.get_original())();
	}, HOOK_CALL)->install()->quick();

	stop_hook_.initialize(SELECT_VALUE(0x528B04, 0x569E46, 0x4F03FA), []()
	{
		stop_execution();
		reinterpret_cast<void(*)()>(stop_hook_.get_original())();
	}, HOOK_CALL)->install()->quick();
}

void scripting::pre_destroy()
{
	this->scripts_.clear();
	start_callbacks_.clear();
	stop_callbacks_.clear();
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

REGISTER_MODULE(scripting)
