#include <std_include.hpp>
#include "launcher.hpp"
#include "html_frame.hpp"

launcher::launcher()
{
	this->window_.set_callback(std::bind(&launcher::handler, this, std::placeholders::_1, std::placeholders::_2,
	                                     std::placeholders::_3));

	this->window_.create("Open-IW5", 615, 300);
	this->html_frame_.load_url("https://google.com");
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
