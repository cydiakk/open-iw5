#pragma once
#include "loader/module_loader.hpp"
#include "game/game.hpp"
#include "utils/hook.hpp"
#include "utils/chain.hpp"

class scripting final : public module
{
public:
	class entity final
	{
	public:
		entity();
		entity(const entity& other) = default;
		entity(scripting* environment, unsigned int entity_id);

		void on_notify(const std::string& event,
		               const std::function<void(const std::vector<chaiscript::Boxed_Value>&)>& callback,
		               bool is_volatile) const;

		unsigned int get_entity_id() const;
		game::native::scr_entref_t get_entity_reference() const;

		void call(const std::string& function, const std::vector<chaiscript::Boxed_Value>& arguments);

	private:
		scripting* environment_;
		unsigned int entity_id_;
	};

	class variable final
	{
	public:
		variable(game::native::VariableValue value);
		~variable();

		operator game::native::VariableValue() const;

	private:
		game::native::VariableValue value_;
	};

	class event_listener final
	{
	public:
		std::string event = {};
		unsigned int entity_id = 0;
		std::function<void(const std::vector<chaiscript::Boxed_Value>&)> callback = {};
		bool is_volatile = false;
	};

	void post_start() override;
	void post_load() override;
	void pre_destroy() override;

	static void on_start(const std::function<void()>& callback);
	static void on_stop(const std::function<void()>& callback);

	static void propagate_error(const std::exception& e);

private:
	std::unique_ptr<chaiscript::ChaiScript> chai_;
	utils::chain<event_listener> event_listeners_;

	void add_event_listener(const event_listener& listener);

	void initialize();
	void load_scripts() const;

	chaiscript::Boxed_Value make_boxed(game::native::VariableValue value);

	static utils::hook start_hook_;
	static utils::hook stop_hook_;

	static std::mutex mutex_;
	static std::vector<std::function<void()>> start_callbacks_;
	static std::vector<std::function<void()>> stop_callbacks_;

	static void start_execution();
	static void stop_execution();

	static void call(const std::string& function, unsigned int entity_id, const std::vector<chaiscript::Boxed_Value>& arguments);
	static bool call_safe(game::native::scr_call_t function, game::native::scr_entref_t entref);
	static int find_function_index(const std::string& function);
};
