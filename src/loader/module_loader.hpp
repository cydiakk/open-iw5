#pragma once
#include "module.hpp"

class module_loader final
{
public:
	template <typename T>
	class installer
	{
		static_assert(std::is_base_of<module, T>::value, "Module has invalid base class");

	public:
		installer()
		{
			register_module(std::make_unique<T>());
		}
	};

	static void register_module(std::unique_ptr<module>&& module);

	static void post_load();
	static void pre_destroy();

	static launcher::mode get_mode();
	static void set_mode(launcher::mode mode);

private:
	static launcher::mode mode_;
	static std::vector<std::unique_ptr<module>>* modules_;

	static void destroy_modules();
};

#define REGISTER_MODULE(name)                       \
namespace                                           \
{                                                   \
	static module_loader::installer<name> $_##name; \
}
