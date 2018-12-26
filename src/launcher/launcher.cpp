#include <std_include.hpp>
#include "launcher.hpp"

launcher::launcher() : window_("Open-IW5", 615, 300), image_sp_(IMAGE_SP), image_mp_(IMAGE_MP)
{
	this->image_sp_.set_position({100, 90});
	this->image_mp_.set_position({401, 90});

	this->image_sp_.set_size({100, 100});
	this->image_mp_.set_size({100, 100});

	this->image_sp_.set_click_listener(std::bind(&launcher::select_mode, this, mode::singleplayer));
	this->image_mp_.set_click_listener(std::bind(&launcher::select_mode, this, mode::multiplayer));

	this->window_.set_callback(std::bind(&launcher::handler, this, std::placeholders::_1, std::placeholders::_2,
	                                     std::placeholders::_3));
}

launcher::mode launcher::run() const
{
	this->window_.run();

	return this->mode_;
}

LRESULT launcher::handler(const UINT message, const WPARAM w_param, const LPARAM l_param)
{
	if (message == WM_PAINT)
	{
		this->paint();
	}
	else if (message == WM_MOUSEMOVE)
	{
		this->mouse_move(l_param);
	}
	else if (message == WM_LBUTTONDOWN)
	{
		this->image_sp_.click(this->mouse_, true);
		this->image_mp_.click(this->mouse_, true);
	}
	else if (message == WM_LBUTTONUP)
	{
		this->image_sp_.click(this->mouse_, false);
		this->image_mp_.click(this->mouse_, false);
	}
	else if (message == WM_ERASEBKGND)
	{
		return TRUE;
	}

	return DefWindowProc(this->window_, message, w_param, l_param);
}

void launcher::select_mode(const mode mode)
{
	this->mode_ = mode;
	this->window_.close();
}

void launcher::draw_text(const HDC hdc)
{
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::SolidBrush color(Gdiplus::Color(255, 150, 150, 150));
	Gdiplus::FontFamily font_family(L"Segoe UI");
	Gdiplus::Font font(&font_family, 20, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	const auto stringformat = Gdiplus::StringFormat::GenericTypographic();

	std::wstring sp(L"Singleplayer");
	std::wstring mp(L"Multiplayer");

	Gdiplus::RectF rect{};
	graphics.MeasureString(sp.data(), -1, &font, rect, stringformat, &rect);

	Gdiplus::PointF pos{150 - (rect.Width / 2 + 2), 45};
	graphics.DrawString(sp.data(), -1, &font, pos, &color);

	rect = {};
	graphics.MeasureString(mp.data(), -1, &font, rect, stringformat, &rect);
	graphics.DrawString(mp.data(), -1, &font, {451 - (rect.Width / 2 + 4), 45}, &color);

	Gdiplus::Pen pen(Gdiplus::Color(50, 255, 255, 255), 1);
	graphics.DrawLine(&pen, 300, 0, 300, 600);
}

void launcher::paint() const
{
	RECT rect;
	GetClientRect(this->window_, &rect);
	const int width = rect.right - rect.left;
	const int height = rect.bottom + rect.left;

	PAINTSTRUCT ps;
	const auto hdc = BeginPaint(this->window_, &ps);

	const auto hdc_copy = CreateCompatibleDC(hdc);
	const auto bitmap = CreateCompatibleBitmap(hdc, width, height);
	SelectObject(hdc_copy, bitmap);

	this->window_.clear(hdc_copy);

	this->draw_text(hdc_copy);

	this->image_sp_.paint(hdc_copy, this->mouse_);
	this->image_mp_.paint(hdc_copy, this->mouse_);

	BitBlt(hdc, 0, 0, width, height, hdc_copy, 0, 0, SRCCOPY);

	DeleteObject(bitmap);
	DeleteDC(hdc_copy);

	EndPaint(this->window_, &ps);
}

void launcher::mouse_move(const LPARAM l_param)
{
	this->mouse_.x = GET_X_LPARAM(l_param);
	this->mouse_.y = GET_Y_LPARAM(l_param);

	InvalidateRect(this->window_, nullptr, FALSE);
}
