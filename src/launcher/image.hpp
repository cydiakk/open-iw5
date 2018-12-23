#pragma once

class image final
{
public:
	image(size_t resource);
	~image();

	void set_position(const POINT& position);
	void set_size(const POINT& size);

	bool is_hovered(const POINT& mouse) const;

	void paint(HDC hdc, const POINT& mouse) const;

	void set_click_listener(std::function<void()> callback);

	void click(const POINT& mouse, bool down);

private:
	POINT size_{};
	POINT position_{};
	HBITMAP bitmap_;

	bool down_handled_ = false;
	std::function<void()> callback_;
};
