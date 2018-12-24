#include <std_include.hpp>
#include "loader/module_loader.hpp"
#include "utils/hook.hpp"
#include "game/game.hpp"

class stats final : public module
{
public:
	void post_load() override
	{
		if (!game::is_sp()) return;

		// Disable remote storage
		utils::hook::set<BYTE>(0x663B5A, 0xEB);
		utils::hook::set<BYTE>(0x663C54, 0xEB);
	}
};

REGISTER_MODULE(stats)
