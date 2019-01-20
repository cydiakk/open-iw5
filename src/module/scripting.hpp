#pragma once
#include "loader/module_loader.hpp"
#include "utils/hook.hpp"
#include "game/scripting/context.hpp"

class scripting final : public module
{
public:
	void post_load() override;
	void pre_destroy() override;

	static void propagate_error(const std::exception& e);

private:
	std::vector<std::unique_ptr<game::scripting::context>> scripts_;

	void load_scripts();

	void start_execution();
	void stop_execution();

	void dispatch(game::scripting::event* event);

	static utils::hook start_hook_;
	static utils::hook stop_hook_;

	static void start_execution_stub();
	static void stop_execution_stub();
	static void vm_notify_stub(const unsigned int notify_id, const unsigned short type,
                                  game::native::VariableValue* stack);
};
