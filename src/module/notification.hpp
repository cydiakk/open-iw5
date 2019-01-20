#pragma once
#include "loader/module_loader.hpp"
#include "game/scripting/event.hpp"
#include "scripting.hpp"

class notification final : public module
{
public:
	void post_load() override;
	void pre_destroy() override;

	static void listen(const std::function<void(game::scripting::event*)>& callback);

private:
	static std::mutex mutex_;
	static std::vector<std::function<void(game::scripting::event*)>> callbacks_;

	static void cleanup();
	static void dispatch(game::scripting::event* event);

	static void vm_notify_stub(unsigned int notify_id, unsigned short type, game::native::VariableValue* stack);
};
