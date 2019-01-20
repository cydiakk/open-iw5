#pragma once
#include "loader/module_loader.hpp"
#include "utils/hook.hpp"
#include "game/scripting/context.hpp"

class scripting final : public module
{
public:
	void post_start() override;
	void post_load() override;
	void pre_destroy() override;

	static void on_start(const std::function<void()>& callback);
	static void on_stop(const std::function<void()>& callback);

	static void propagate_error(const std::exception& e);

private:
	std::vector<std::unique_ptr<game::scripting::context>> scripts_;

	void load_scripts();

	static utils::hook start_hook_;
	static utils::hook stop_hook_;

	static std::mutex mutex_;
	static std::vector<std::function<void()>> start_callbacks_;
	static std::vector<std::function<void()>> stop_callbacks_;

	static void start_execution();
	static void stop_execution();
};
