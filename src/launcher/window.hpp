#pragma once

#define WM_KILL_WINDOW (WM_USER+0)

class window
{
public:
	window();
	~window();

	void create(const std::string& title, int width, int height);

	void close();

	void show() const;
	void hide() const;

	void set_hide_on_close(bool value);
	void set_close_all_on_close(bool value);

	void set_callback(const std::function<LRESULT(UINT, WPARAM, LPARAM)>& callback);

	operator HWND() const;

	static void run();
	static void close_all();

private:
	bool hide_on_close_ = false;
	bool close_all_on_close_ = false;

	WNDCLASSEX wc_{};
	HWND handle_ = nullptr;
	std::string classname_;
	std::function<LRESULT(UINT, WPARAM, LPARAM)> callback_;

	LRESULT CALLBACK processor(UINT message, WPARAM w_param, LPARAM l_param) const;
	static LRESULT CALLBACK static_processor(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);

	static std::mutex mutex_;
	static std::vector<window*> windows_;

	static void remove_window(const window* window);
	static int get_window_count();
};
