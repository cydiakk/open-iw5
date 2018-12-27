#include <std_include.hpp>
#include "launcher/launcher.hpp"
#include "loader/loader.hpp"
#include "loader/module_loader.hpp"
#include "game/game.hpp"
#include "loader/binary_loader.hpp"
#include "utils/string.hpp"

//#define GENERATE_DIFFS

__declspec(thread) char tls_data[TLS_PAYLOAD_SIZE];

void exit_hook(const int code)
{
	module_loader::pre_destroy();
	exit(code);
}

void verify_tls()
{
	utils::nt::module self;
	const auto self_tls = reinterpret_cast<PIMAGE_TLS_DIRECTORY>(self.get_ptr() + self
	                                                                              .get_optional_header()
	                                                                              ->
	                                                                              DataDirectory
		[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

	const auto ref = DWORD(&tls_data);
	const auto tls_index = *reinterpret_cast<DWORD*>(self_tls->AddressOfIndex);
	const auto tls_vector = *reinterpret_cast<DWORD*>(__readfsdword(0x2C) + 4 * tls_index);
	const auto offset = ref - tls_vector;

	if (offset != 0 && offset != 8) // Actually 8 is bad, but I think msvc places custom stuff before
	{
		throw std::runtime_error(utils::string::va("TLS payload is at offset 0x%X, but should be at 0!",
		                                           offset));
	}
}

int main()
{
	FARPROC entry_point = nullptr;

	try
	{
#ifdef GENERATE_DIFFS
		binary_loader::create();
		return 0;
#endif

		verify_tls();

		module_loader::post_start();

		launcher launcher;
		const auto mode = launcher.run();

		if (mode == launcher::mode::none) return 0;

		loader loader(mode);
		loader.set_import_resolver([](const std::string& module, const std::string& function) -> FARPROC
		{
			if (module == "steam_api.dll")
			{
				return utils::nt::module().get_proc<FARPROC>(function);
			}
			else if (function == "ExitProcess")
			{
				return FARPROC(exit_hook);
			}

			return nullptr;
		});

		entry_point = loader.load({});
		if (!entry_point) throw std::runtime_error("Unable to load inject binary into memory");

		game::initialize(mode);
		module_loader::post_load();
	}
	catch (std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR);
		return 1;
	}

	return entry_point();
}
