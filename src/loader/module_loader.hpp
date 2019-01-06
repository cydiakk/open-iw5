#pragma once
#include "module.hpp"

class module_loader final
{
public:
	class premature_shutdown_trigger final : public std::exception
	{
		const char* what() const noexcept override
		{
			return "Premature shutdown requested";
		}
	};

	template <typename T>
	class installer final
	{
		static_assert(std::is_base_of<module, T>::value, "Module has invalid base class");

	public:
		installer()
		{
			register_module(std::make_unique<T>());
		}
	};

	static void register_module(std::unique_ptr<module>&& module);

	static bool post_start();
	static bool post_load();
	static void pre_destroy();

	static void trigger_premature_shutdown();

private:
	static std::vector<std::unique_ptr<module>>* modules_;

	static void destroy_modules();
};

#define REGISTER_MODULE(name)                       \
namespace                                           \
{                                                   \
	static module_loader::installer<name> $_##name; \
}
