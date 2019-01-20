#pragma once
#include "utils/chain.hpp"
#include "entity.hpp"
#include "event.hpp"

namespace game
{
	namespace scripting
	{
		class context;

		class event_listener final
		{
		public:
			std::string event = {};
			unsigned int entity_id = 0;
			std::function<void(const std::vector<chaiscript::Boxed_Value>&)> callback = {};
			bool is_volatile = false;
		};

		class generic_event_listener final
		{
		public:
			std::string event = {};
			std::function<void(const entity&, const std::vector<chaiscript::Boxed_Value>&)> callback = {};
			bool is_volatile = false;
		};

		class event_handler final
		{
		public:
			explicit event_handler(context* context);

			void dispatch(event* event);

			void add_event_listener(const event_listener& listener);
			void add_event_listener(const generic_event_listener& listener);

		private:
			context* context_;

			void dispatch_to_specific_listeners(event* event, const std::vector<chaiscript::Boxed_Value>& arguments);
			void dispatch_to_generic_listeners(event* event, const std::vector<chaiscript::Boxed_Value>& arguments);

			utils::chain<event_listener> event_listeners_;
			utils::chain<generic_event_listener> generic_event_listeners_;
		};
	}
}
