#pragma once
#include "window.hpp"
#include "html/ole_in_place_frame.hpp"
#include "html/ole_in_place_site.hpp"
#include "html/doc_host_ui_handler.hpp"
#include "html/ole_client_site.hpp"

class html_frame final
{
public:
	html_frame();
	~html_frame();

	void initialize(const HWND window);

	void resize(DWORD width, DWORD height) const;
	void load_url(std::string url) const;

	HWND get_window() const;

	std::shared_ptr<IOleObject> get_browser_object() const;
	std::shared_ptr<IWebBrowser2> get_web_browser() const;

	ole_in_place_frame* get_in_place_frame();
	ole_in_place_site* get_in_place_site();
	doc_host_ui_handler* get_ui_handler();
	ole_client_site* get_client_site();

private:
	HWND window_ = nullptr;
	std::shared_ptr<IOleObject> browser_object_;

	ole_in_place_frame in_place_frame_;
	ole_in_place_site in_place_site_;
	doc_host_ui_handler ui_handler_;
	ole_client_site client_site_;

	void create_browser();
	void initialize_browser();
};
