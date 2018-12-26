#include <std_include.hpp>
#include "image.hpp"

image::image(const size_t resource)
{
	this->bitmap_ = LoadBitmapA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(resource));
}

image::~image()
{
	DeleteBitmap(this->bitmap_);
}

void image::set_position(const POINT& position)
{
	this->position_ = position;
}

void image::set_size(const POINT& size)
{
	this->size_ = size;
}

bool image::is_hovered(const POINT& mouse) const
{
	return mouse.x >= this->position_.x && mouse.x < (this->position_.x + this->size_.x)
		&& mouse.y >= this->position_.y && mouse.y < (this->position_.y + this->size_.y);
}

void image::paint(const HDC hdc, const POINT& mouse) const
{
	BITMAP bitmap;
	GetObject(this->bitmap_, sizeof(bitmap), &bitmap);

	const auto dc = CreateCompatibleDC(hdc);
	SelectObject(dc, this->bitmap_);

	const LONG modifier = 2;
	LONG size_offset = 0;
	LONG position_offset = 0;
	if (this->is_hovered(mouse))
	{
		size_offset = modifier * 2;
		position_offset = -modifier;
	}

	SetStretchBltMode(hdc, HALFTONE);
	StretchBlt(hdc, this->position_.x + position_offset, this->position_.y + position_offset,
	           this->size_.x + size_offset, this->size_.y + size_offset, dc, 0, 0, bitmap.bmWidth,
	           bitmap.bmHeight, SRCCOPY);

	DeleteDC(dc);
}

void image::set_click_listener(std::function<void()> callback)
{
	this->callback_ = callback;
}

void image::click(const POINT& mouse, const bool down)
{
	if (down)
	{
		this->down_handled_ = this->is_hovered(mouse);
	}
	else
	{
		if (this->down_handled_ && this->is_hovered(mouse) && this->callback_)
		{
			this->callback_();
		}

		this->down_handled_ = false;
	}
}
