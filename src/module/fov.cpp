#include <std_include.hpp>
#include "loader/module_loader.hpp"
#include "utils/hook.hpp"
#include "game/game.hpp"

class fov final : public module
{
public:
	void post_load() override
	{
		if (game::is_dedi()) return;

		utils::hook::set<BYTE>(SELECT_VALUE(0x4302C5, 0x455155, 0), 0x41);

		if (game::is_mp())
		{
			static const auto cg_fov_limit = 90.0f;
			utils::hook::set(0x455148, &cg_fov_limit);
		}
	}
};

REGISTER_MODULE(fov)
