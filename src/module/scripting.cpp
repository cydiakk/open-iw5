#include <std_include.hpp>
#include "loader/module_loader.hpp"
#include "game/game.hpp"

class scripting final : public module
{
public:
	void post_load() override
	{
		
	}
};

REGISTER_MODULE(scripting)
