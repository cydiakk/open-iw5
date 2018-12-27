#include <std_include.hpp>
#include "scheduler.hpp"

std::mutex scheduler::mutex_;
std::vector<std::function<void()>> scheduler::callbacks_;

void scheduler::on_frame(const std::function<void()>& callback)
{
	std::lock_guard _(mutex_);
	callbacks_.push_back(callback);
}

void scheduler::execute()
{
	std::vector<std::function<void()>> callbacks_copy;

	{
		std::lock_guard _(mutex_);
		callbacks_copy = callbacks_;
	}

	for (const auto& callback : callbacks_copy)
	{
		callback();
	}
}

void scheduler::pre_destroy()
{
	std::lock_guard _(mutex_);
	callbacks_.clear();
}

REGISTER_MODULE(scheduler);
