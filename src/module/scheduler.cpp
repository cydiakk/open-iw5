#include <std_include.hpp>
#include "scheduler.hpp"
#include "utils/string.hpp"
#include "game/structs.hpp"
#include "game/game.hpp"

std::mutex scheduler::mutex_;
std::queue<std::pair<std::string, int>> scheduler::errors_;
std::vector<std::function<void()>> scheduler::callbacks_;
std::vector<std::function<void()>> scheduler::single_callbacks_;

void scheduler::on_frame(const std::function<void()>& callback)
{
	std::lock_guard _(mutex_);
	callbacks_.push_back(callback);
}

void scheduler::once(const std::function<void()>& callback)
{
	std::lock_guard _(mutex_);
	single_callbacks_.push_back(callback);
}

void scheduler::error(const std::string& message, int level)
{
	std::lock_guard _(mutex_);
	errors_.emplace(message, level);
}

__declspec(naked) void scheduler::execute()
{
	__asm
	{
		call execute_error
		call execute_safe
		retn
	}
}

void scheduler::execute_safe()
{
	std::vector<std::function<void()>> callbacks_copy;
	std::vector<std::function<void()>> single_callbacks_copy;

	{
		std::lock_guard _(mutex_);
		callbacks_copy = callbacks_;
		single_callbacks_copy = single_callbacks_;
		single_callbacks_.clear();
	}

	for (const auto& callback : callbacks_copy)
	{
		callback();
	}

	for (const auto& callback : single_callbacks_copy)
	{
		callback();
	}
}

void scheduler::execute_error()
{
	const char* message;
	int level;

	if(get_next_error(&message, &level) && message)
	{
		game::native::Com_Error(level, "%s", message);
	}
}

bool scheduler::get_next_error(const char** error_message, int* error_level)
{
	std::lock_guard _(mutex_);
	if(errors_.empty())
	{
		*error_message = nullptr;
		return false;
	}

	const auto error = errors_.front();
	errors_.pop();

	*error_level = error.second;
	*error_message = utils::string::va("%s", error.first.data());

	return true;
}

void scheduler::pre_destroy()
{
	std::lock_guard _(mutex_);
	callbacks_.clear();
	single_callbacks_.clear();
}

REGISTER_MODULE(scheduler);
