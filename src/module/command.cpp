#include <std_include.hpp>
#include "command.hpp"
#include "utils/string.hpp"
#include "game/structs.hpp"
#include "game/game.hpp"
#include "scheduler.hpp"

utils::memory::allocator command::allocator_;
std::mutex command::mutex_;
std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> command::callbacks_;

void command::add(const std::string& name, const std::function<void(const std::vector<std::string>&)>& callback)
{
	std::lock_guard _(mutex_);
	callbacks_[utils::string::to_lower(name)] = callback;

	const auto cmd_name = allocator_.duplicate_string(name);
	const auto cmd_function = allocator_.allocate<game::native::cmd_function_t>();

	game::native::Cmd_AddCommand(cmd_name, dispatcher, cmd_function);
}

void command::pre_destroy()
{
	std::lock_guard _(mutex_);
	if(!callbacks_.empty())
	{
		callbacks_.clear();
	}
}

void command::dispatcher()
{
	if (game::native::cmd_argc[*game::native::cmd_args] < 1) return;

	const auto command = utils::string::to_lower(game::native::cmd_argv[*game::native::cmd_args][0]);
	const auto handler = callbacks_.find(command);
	if (handler == callbacks_.end()) return;

	std::vector<std::string> arguments;

	for (auto i = 0; i < game::native::cmd_argc[*game::native::cmd_args]; ++i)
	{
		arguments.emplace_back(game::native::cmd_argv[*game::native::cmd_args][i]);
	}

	handler->second(arguments);
}

REGISTER_MODULE(command);
