#include <std_include.hpp>
#include "module_loader.hpp"

std::vector<std::unique_ptr<module>>* module_loader::modules_ = nullptr;

void module_loader::register_module(std::unique_ptr<module>&& module_)
{
	if (!module_loader::modules_)
	{
		module_loader::modules_ = new std::vector<std::unique_ptr<module>>();
		atexit(module_loader::destroy_modules);
	}

	module_loader::modules_->push_back(std::move(module_));
}

void module_loader::post_load()
{
	static auto handled = false;
	if (handled || !module_loader::modules_) return;
	handled = true;

	for (const auto& module_ : *module_loader::modules_)
	{
		module_->post_load();
	}
}

void module_loader::pre_destroy()
{
	static auto handled = false;
	if (handled || !module_loader::modules_) return;
	handled = true;

	for (const auto& module_ : *module_loader::modules_)
	{
		module_->pre_destroy();
	}
}

void module_loader::destroy_modules()
{
	module_loader::pre_destroy();

	if (!module_loader::modules_) return;

	delete module_loader::modules_;
	module_loader::modules_ = nullptr;
}
