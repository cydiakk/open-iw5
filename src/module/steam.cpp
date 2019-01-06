#include <std_include.hpp>
#include <discord_rpc.h>
#include "loader/module_loader.hpp"
#include "game/game.hpp"

class steam final : public module
{
public:
	void post_load() override
	{
		if (game::is_dedi()) return;
	}

	void pre_destroy() override
	{

	}
};

REGISTER_MODULE(steam)
