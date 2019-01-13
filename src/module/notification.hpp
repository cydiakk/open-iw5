#pragma once
#include "loader/module_loader.hpp"
#include "scripting.hpp"

class notification final : public module
{
public:
	class event final
	{
	public:
		std::string name;
		game::native::scr_entref_t entity;
		//std::vector<scripting::variable> arguments;
		std::vector<game::native::VariableValue> arguments;
	};

	void post_load() override;
	void pre_destroy() override;

	static void listen(const std::function<void(event*)>& callback);

private:
	static std::mutex mutex_;
	static std::vector<std::function<void(event*)>> callbacks_;

	static void cleanup();
	static void dispatch(event* event);

	static void vm_notify_stub(unsigned int notify_id, unsigned short type, game::native::VariableValue* stack);
};
