#include "std_include.hpp"
#include "context.hpp"

namespace game
{
	namespace scripting
	{
		event_handler::event_handler(context* context) : context_(context)
		{
		}

		void event_handler::run_frame()
		{

		}

		void event_handler::dispatch(event* event)
		{
			try
			{
				std::vector<chaiscript::Boxed_Value> arguments;

				for (const auto& argument : event->arguments)
				{
					arguments.push_back(this->context_->get_parameters()->load(argument));
				}

				this->dispatch_to_specific_listeners(event, arguments);
				this->dispatch_to_generic_listeners(event, arguments);
			}
			catch (chaiscript::exception::eval_error& e)
			{
				throw std::runtime_error(e.pretty_print());
			}
		}

		void event_handler::dispatch_to_specific_listeners(event* event,
		                                                   const std::vector<chaiscript::Boxed_Value>& arguments)
		{
			for (auto listener = this->event_listeners_.begin(); listener.is_valid(); ++listener)
			{
				if (listener->event == event->name && listener->entity_id == event->entity_id)
				{
					if (listener->is_volatile)
					{
						this->event_listeners_.remove(listener);
					}

					listener->callback(arguments);
				}
			}
		}

		void event_handler::dispatch_to_generic_listeners(event* event,
		                                                  const std::vector<chaiscript::Boxed_Value>& arguments)
		{
			for (auto listener = this->generic_event_listeners_.begin(); listener.is_valid(); ++listener)
			{
				if (listener->event == event->name)
				{
					if (listener->is_volatile)
					{
						this->generic_event_listeners_.remove(listener);
					}

					listener->callback(entity(this->context_, event->entity_id), arguments);
				}
			}
		}

		void event_handler::add_event_listener(const event_listener& listener)
		{
			this->event_listeners_.add(listener);
		}

		void event_handler::add_event_listener(const generic_event_listener& listener)
		{
			this->generic_event_listeners_.add(listener);
		}
	}
}
