#pragma once
#include "executer.hpp"
#include "parameters.hpp"
#include "event_handler.hpp"

namespace game
{
	namespace scripting
	{
		class context final
		{
		public:
			context();

			executer* get_executer();
			parameters* get_parameters();
			event_handler* get_event_handler();
			chaiscript::ChaiScript* get_chai();

		private:
			executer executer_;
			parameters parameters_;
			event_handler event_handler_;
			chaiscript::ChaiScript chai_;
		};
	}
}
