#pragma once
#include "loader/module_loader.hpp"

class scheduler final : public module
{
public:
	static void on_frame(const std::function<void()>& callback);
	static void once(const std::function<void()>& callback);
	static void execute();

	void pre_destroy() override;

private:
	static std::mutex mutex_;
	static std::vector<std::function<void()>> callbacks_;
	static std::vector<std::function<void()>> single_callbacks_;
};
