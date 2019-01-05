#include <std_include.hpp>
#include "window.hpp"

window::window()
{
	ZeroMemory(&this->wc_, sizeof(this->wc_));

	this->classname_ = "window-base-" + std::to_string(time(nullptr));

	this->wc_.cbSize = sizeof(this->wc_);
	this->wc_.style = CS_HREDRAW | CS_VREDRAW;
	this->wc_.lpfnWndProc = static_processor;
	this->wc_.hInstance = GetModuleHandle(nullptr);
	this->wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);
	this->wc_.hIcon = LoadIcon(this->wc_.hInstance, MAKEINTRESOURCE(102));
	this->wc_.hIconSm = this->wc_.hIcon;
	this->wc_.hbrBackground = HBRUSH(COLOR_WINDOW);
	this->wc_.lpszClassName = this->classname_.data();
	RegisterClassEx(&this->wc_);
}

void window::create(const std::string& title, const int width, const int height)
{
	const auto x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	const auto y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	this->handle_ = CreateWindowExA(NULL, this->wc_.lpszClassName, title.data(),
	                                (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~(WS_THICKFRAME | WS_MAXIMIZEBOX), x, y, width,
	                                height, nullptr, nullptr, this->wc_.hInstance, this);
}

window::~window()
{
	this->close();
	UnregisterClass(this->wc_.lpszClassName, this->wc_.hInstance);
	DeleteObject(this->wc_.hbrBackground);
}

void window::close()
{
	if (!this->handle_) return;

	SendMessageA(this->handle_, WM_KILL_WINDOW, NULL, NULL);
	this->handle_ = nullptr;
}

void window::run() const
{
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		//if(!this->handle_) break;
	}
}

void window::set_callback(const std::function<LRESULT(UINT, WPARAM, LPARAM)>& callback)
{
	this->callback_ = callback;
}

LRESULT CALLBACK window::processor(const UINT message, const WPARAM w_param, const LPARAM l_param) const
{
	if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
		return TRUE;
	}

	if (message == WM_KILL_WINDOW)
	{
		DestroyWindow(*this);
		return 0;
	}

	if (this->callback_)
	{
		return this->callback_(message, w_param, l_param);
	}

	return DefWindowProc(*this, message, w_param, l_param);
}

LRESULT CALLBACK window::static_processor(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
	if (message == WM_CREATE)
	{
		auto data = reinterpret_cast<LPCREATESTRUCT>(l_param);
		SetWindowLongPtrA(hwnd, GWLP_USERDATA, LONG_PTR(data->lpCreateParams));

		reinterpret_cast<window*>(data->lpCreateParams)->handle_ = hwnd;
	}

	const auto self = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (self) return self->processor(message, w_param, l_param);

	return DefWindowProc(hwnd, message, w_param, l_param);
}


window::operator HWND() const
{
	return this->handle_;
}
