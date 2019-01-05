#include <std_include.hpp>
#include "launcher.hpp"
#include "html_frame.hpp"
#include "utils/nt.hpp"

launcher::launcher()
{
	this->window_.set_callback(std::bind(&launcher::handler, this, std::placeholders::_1, std::placeholders::_2,
	                                     std::placeholders::_3));

	this->html_frame_.register_callback("selectMode", [this](html_frame::callback_params* params)
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

	this->window_.create("Open-IW5", 615, 300);
	this->html_frame_.load_html(load_content());
}

launcher::mode launcher::run() const
{
	this->window_.run();

	return this->mode_;
}

LRESULT launcher::handler(const UINT message, const WPARAM w_param, const LPARAM l_param)
{
	if (message == WM_SIZE)
	{
		this->html_frame_.resize(LOWORD(l_param), HIWORD(l_param));
		return 0;
	}

	if (message == WM_CREATE)
	{
		this->html_frame_.initialize(this->window_);
		return 0;
	}

	return DefWindowProc(this->window_, message, w_param, l_param);
}

void launcher::select_mode(const mode mode)
{
	this->mode_ = mode;
	this->window_.close();
}

std::string launcher::load_content()
{
	const auto res = FindResource(::utils::nt::module(), MAKEINTRESOURCE(MAIN_MENU), RT_RCDATA);
	if (!res) return {};

	const auto handle = LoadResource(nullptr, res);
	if (!handle) return {};

	return std::string(LPSTR(LockResource(handle)), SizeofResource(nullptr, res));
}
