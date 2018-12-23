#pragma once
#include "window.hpp"
#include "image.hpp"

class launcher final
{
public:
	enum mode
	{
		NONE,
		SINGLEPLAYER,
		MULTIPLAYER,
	};

	launcher();

	mode run() const;

private:
	mode mode_ = NONE;

	window window_;

	image image_sp_;
	image image_mp_;

	POINT mouse_{};

	LRESULT handler(const UINT message, const WPARAM w_param, const LPARAM l_param);

	void select_mode(mode mode);

	static void draw_text(const HDC hdc);
	void paint() const;
	void mouse_move(LPARAM l_param);
};
