#pragma once
#include "loader/module_loader.hpp"
#include "game/game.hpp"
#include "utils/hook.hpp"

class scripting final : public module
{
public:
	class variable final
	{
	public:
		variable(game::native::VariableValue value);
		~variable();

		operator game::native::VariableValue() const;

	private:
		game::native::VariableValue value_;
	};

	void post_start() override;
	void post_load() override;
	void pre_destroy() override;

	static void on_start(const std::function<void()>& callback);
	static void on_stop(const std::function<void()>& callback);

private:
	std::unique_ptr<chaiscript::ChaiScript> chai_;
	std::vector<std::function<void(const std::string&, const std::vector<chaiscript::Boxed_Value>&)>> callbacks_;

	void initialize();
	void load_scripts() const;

	static chaiscript::Boxed_Value make_boxed(game::native::VariableValue value);

	static utils::hook start_hook_;
	static utils::hook stop_hook_;

	static std::mutex mutex_;
	static std::vector<std::function<void()>> start_callbacks_;
	static std::vector<std::function<void()>> stop_callbacks_;

	static void start_execution();
	static void stop_execution();
};
