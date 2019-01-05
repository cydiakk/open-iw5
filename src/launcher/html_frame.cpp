#include <std_include.hpp>
#include "html_frame.hpp"

html_frame::html_frame() : in_place_frame_(this), in_place_site_(this), ui_handler_(this), client_site_(this)
{
	if (OleInitialize(nullptr) != S_OK)
	{
		throw std::runtime_error("Unable to initialize the OLE library");
	}
}

html_frame::~html_frame()
{
	OleUninitialize();
}

HWND html_frame::get_window() const
{
	return this->window_;
}

std::shared_ptr<IOleObject> html_frame::get_browser_object() const
{
	return this->browser_object_;
}

ole_in_place_frame* html_frame::get_in_place_frame()
{
	return &this->in_place_frame_;
}

ole_in_place_site* html_frame::get_in_place_site()
{
	return &this->in_place_site_;
}

doc_host_ui_handler* html_frame::get_ui_handler()
{
	return &this->ui_handler_;
}

ole_client_site* html_frame::get_client_site()
{
	return &this->client_site_;
}

std::shared_ptr<IWebBrowser2> html_frame::get_web_browser() const
{
	IWebBrowser2* web_browser;
	if (!this->browser_object_->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void**>(&web_browser)))
	{
		return std::shared_ptr<IWebBrowser2>(web_browser, [](IWebBrowser2* web_browser)
		{
			if (web_browser)
			{
				web_browser->Release();
			}
		});
	}

	return {};
}

void html_frame::initialize(const HWND window)
{
	if (this->window_) return;
	this->window_ = window;

	this->create_browser();
	this->initialize_browser();
}

void html_frame::create_browser()
{
	LPCLASSFACTORY class_factory = nullptr;
	if (CoGetClassObject(CLSID_WebBrowser, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, nullptr, IID_IClassFactory,
	                     reinterpret_cast<void **>(&class_factory)) || !class_factory)
	{
		throw std::runtime_error("Unable to get the class factory");
	}

	IOleObject* browser_object = nullptr;
	class_factory->CreateInstance(nullptr, IID_IOleObject, reinterpret_cast<void**>(&browser_object));
	class_factory->Release();

	if (!browser_object)
	{
		throw std::runtime_error("Unable to create browser object");
	}

	this->browser_object_ = std::shared_ptr<IOleObject>(browser_object, [](IOleObject* browser_object)
	{
		if (browser_object)
		{
			browser_object->Close(OLECLOSE_NOSAVE);
			browser_object->Release();
		}
	});
}

void html_frame::initialize_browser()
{
	this->browser_object_->SetClientSite(this->get_client_site());
	this->browser_object_->SetHostNames(L"Hostname", nullptr);

	RECT rect;
	GetClientRect(this->get_window(), &rect);
	OleSetContainedObject(this->browser_object_.get(), TRUE);

	this->browser_object_->DoVerb(OLEIVERB_SHOW, nullptr, this->get_client_site(), -1, this->get_window(), &rect);
	this->resize(rect.right, rect.bottom);
}

void html_frame::resize(const DWORD width, const DWORD height) const
{
	auto web_browser = this->get_web_browser();
	if (web_browser)
	{
		web_browser->put_Left(0);
		web_browser->put_Top(0);
		web_browser->put_Width(width);
		web_browser->put_Height(height);
	}
}

void html_frame::load_url(std::string url) const
{
	auto web_browser = this->get_web_browser();
	if (web_browser)
	{
		std::wstring wide_url(url.begin(), url.end());

		VARIANT my_url;
		VariantInit(&my_url);
		my_url.vt = VT_BSTR;
		my_url.bstrVal = SysAllocString(wide_url.data());

		web_browser->Navigate2(&my_url, nullptr, nullptr, nullptr, nullptr);

		VariantClear(&my_url);
	}
}
