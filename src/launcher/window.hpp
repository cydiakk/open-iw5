#pragma once

#define WM_KILL_WINDOW (WM_USER+0)

class window final
{
public:
	window(const std::string& title, int width, int height);
	~window();

	void close();
	void run() const;

	operator HWND() const;

private:
	WNDCLASSEX wc_{};
	HWND handle_ = nullptr;

	LRESULT CALLBACK processor(UINT message, WPARAM w_param, LPARAM l_param) const;
	static LRESULT CALLBACK static_processor(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
};
