#pragma once
#include "window.hpp"
#include "html_frame.hpp"

class launcher final
{
public:
	enum mode
	{
		none,
		singleplayer,
		multiplayer,
		server,
	};

	launcher();

	mode run() const;

private:
	mode mode_ = none;

	window window_;
	html_frame html_frame_;

	LRESULT handler(const UINT message, const WPARAM w_param, const LPARAM l_param);
	void select_mode(mode mode);

	static std::string load_content();
};
