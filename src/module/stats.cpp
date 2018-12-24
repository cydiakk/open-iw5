#include <std_include.hpp>
#include "loader/module_loader.hpp"
#include "utils/hook.hpp"

class stats final : public module
{
public:
	void post_load() override
	{
		if (module_loader::get_mode() != launcher::mode::SINGLEPLAYER) return;

		// Disable remote storage
		utils::hook::set<BYTE>(0x663B5A, 0xEB);
		utils::hook::set<BYTE>(0x663C54, 0xEB);
	}
};

REGISTER_MODULE(stats)
