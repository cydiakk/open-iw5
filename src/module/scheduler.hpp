#pragma once
#include "loader/module_loader.hpp"

class scheduler final : public module
{
public:
	static void on_frame(const std::function<void()>& callback);
	static void once(const std::function<void()>& callback);
	static void execute();

	static void error(const std::string& message, int level);

	void pre_destroy() override;

private:
	static std::mutex mutex_;
	static std::queue<std::pair<std::string, int>> errors_;
	static std::vector<std::function<void()>> callbacks_;
	static std::vector<std::function<void()>> single_callbacks_;

	static void execute_safe();
	static void execute_error();
	static bool get_next_error(const char** error_message, int* error_level);
};
