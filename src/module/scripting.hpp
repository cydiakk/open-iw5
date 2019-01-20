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
		entity(const entity& other);
		entity(scripting* environment, unsigned int entity_id);
		~entity();

		void on_notify(const std::string& event,
		               const std::function<void(const std::vector<chaiscript::Boxed_Value>&)>& callback,
		               bool is_volatile) const;

		unsigned int get_entity_id() const;
		game::native::scr_entref_t get_entity_reference() const;

		chaiscript::Boxed_Value call(const std::string& function, const std::vector<chaiscript::Boxed_Value>& arguments) const;
		void notify(const std::string& event, const std::vector<chaiscript::Boxed_Value>& arguments) const;

		void set(const std::string& field, const chaiscript::Boxed_Value& value) const;
		chaiscript::Boxed_Value get(const std::string& field) const;

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

	class generic_event_listener final
	{
	public:
		std::string event = {};
		std::function<void(const entity&, const std::vector<chaiscript::Boxed_Value>&)> callback = {};
		bool is_volatile = false;
	};

	void post_start() override;
	void post_load() override;
	void pre_destroy() override;

	static void on_start(const std::function<void()>& callback);
	static void on_stop(const std::function<void()>& callback);

	static void propagate_error(const std::exception& e);

private:
	class stack_context final
	{
	public:
		stack_context();
		~stack_context();

	private:
		game::native::VariableValue stack_[512]{};

		game::native::VariableValue *max_stack_;
		game::native::VariableValue *top_;
		unsigned int in_param_count_;
		unsigned int out_param_count_;
	};

	std::unique_ptr<chaiscript::ChaiScript> chai_;
	utils::chain<event_listener> event_listeners_;
	utils::chain<generic_event_listener> generic_event_listeners_;

	std::unordered_map<unsigned int, std::unordered_map<std::string, chaiscript::Boxed_Value>> entity_fields_;

	void add_event_listener(const event_listener& listener);

	void initialize();
	void initialize_entity();
	void load_scripts() const;

	chaiscript::Boxed_Value make_boxed(game::native::VariableValue value);

	static utils::hook start_hook_;
	static utils::hook stop_hook_;

	static std::mutex mutex_;
	static std::vector<std::function<void()>> start_callbacks_;
	static std::vector<std::function<void()>> stop_callbacks_;

	static void start_execution();
	static void stop_execution();

	int get_field_id(int classnum, const std::string& field) const;
	void set_entity_field(const std::string& field, unsigned int entity_id, const chaiscript::Boxed_Value& value);
	chaiscript::Boxed_Value get_entity_field(const std::string& field, unsigned int entity_id);

	static bool set_entity_field_safe(game::native::scr_entref_t entref, int offset);
	static bool get_entity_field_safe(game::native::scr_entref_t entref, int offset, game::native::VariableValue* value);

	void notify(const std::string& event, unsigned int entity_id, std::vector<chaiscript::Boxed_Value> arguments) const;

	void push_param(const chaiscript::Boxed_Value& value) const;
	chaiscript::Boxed_Value get_return_value();
	chaiscript::Boxed_Value call(const std::string& function, unsigned int entity_id, std::vector<chaiscript::Boxed_Value> arguments);
	static bool call_safe(game::native::scr_call_t function, game::native::scr_entref_t entref);
	static int find_function_index(const std::string& function, bool prefer_global);
};
