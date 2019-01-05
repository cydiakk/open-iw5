#pragma once

#define WM_KILL_WINDOW (WM_USER+0)

class window final
{
public:
	window();
	~window();

	void create(const std::string& title, int width, int height);

	void close();
	void run() const;

	void set_callback(const std::function<LRESULT(UINT, WPARAM, LPARAM)>& callback);

	operator HWND() const;

private:
	WNDCLASSEX wc_{};
	HWND handle_ = nullptr;
	std::string classname_;
	std::function<LRESULT(UINT, WPARAM, LPARAM)> callback_;

	LRESULT CALLBACK processor(UINT message, WPARAM w_param, LPARAM l_param) const;
	static LRESULT CALLBACK static_processor(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
};
