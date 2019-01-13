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
		std::vector<scripting::variable> arguments;
	};

	void post_load() override;

private:
	static void vm_notify_stub(unsigned int notify_id, unsigned short type, game::native::VariableValue* stack);
};
