#pragma once
#include "window.hpp"
#include "html/html_frame.hpp"

class html_window final : public window, public html_frame
{
public:
	html_window();
	~html_window() = default;

	window* get_window();
	html_frame* get_html_frame();

private:
	LRESULT handler(const UINT message, const WPARAM w_param, const LPARAM l_param);
};
