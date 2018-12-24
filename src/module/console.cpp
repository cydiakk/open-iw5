#include <std_include.hpp>
#include "loader/module_loader.hpp"
#include "console.hpp"
#include "game/game.hpp"

class console final : public module
{
public:
	void post_load() override
	{
		if (game::is_dedi()) return;

		game::native::Sys_ShowConsole();
	}
};

REGISTER_MODULE(console)
