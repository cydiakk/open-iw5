#pragma once
#include "game/game.hpp"

namespace game
{
	namespace scripting
	{
		class event final
		{
		public:
			std::string name;
			unsigned int entity_id;
			std::vector<native::VariableValue> arguments;
		};
	}
}
