#include <std_include.hpp>
#include "launcher.hpp"
#include "utils/nt.hpp"

launcher::launcher()
{
	this->main_window_.register_callback("selectMode", [this](html_frame::callback_params* params)
	{
		if(params->arguments.empty()) return;

		const auto param = params->arguments[0];
		if(!param.is_number()) return;

		const auto number = param.get_number();
		if(number == singleplayer || number == multiplayer)
		{
			this->select_mode(static_cast<mode>(number));
		}
	});

	this->main_window_.register_callback("showSettings", [this](html_frame::callback_params*)
	{
		this->settings_window_.show();
	});

	this->settings_window_.set_hide_on_close(true);
	this->settings_window_.create("Open-IW5 Settings", 615, 300);
	this->settings_window_.load_html(load_content(MENU_SETTINGS));

	this->main_window_.set_close_all_on_close(true);
	this->main_window_.create("Open-IW5", 615, 300);
	this->main_window_.load_html(load_content(MENU_MAIN));
	this->main_window_.show();
}

launcher::mode launcher::run() const
{
	window::run();

	return this->mode_;
}



void launcher::select_mode(const mode mode)
{
	this->mode_ = mode;
	this->settings_window_.close();
	this->main_window_.close();
}

std::string launcher::load_content(int res)
{
	const auto resource = FindResource(::utils::nt::module(), MAKEINTRESOURCE(res), RT_RCDATA);
	if (!res) return {};

	const auto handle = LoadResource(nullptr, resource);
	if (!handle) return {};

	return std::string(LPSTR(LockResource(handle)), SizeofResource(nullptr, resource));
}
