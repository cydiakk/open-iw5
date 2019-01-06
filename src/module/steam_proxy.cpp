#include <std_include.hpp>
#include <discord_rpc.h>
#include "loader/module_loader.hpp"
#include "game/game.hpp"
#include "utils/nt.hpp"
#include "steam/steam.hpp"
#include "steam/interface.hpp"
#include "utils/string.hpp"

class steam_proxy final : public module
{
public:
	void post_start() override
	{
		if (game::is_dedi()) return;

		this->run_mod();
		this->load_client();
	}

	void pre_destroy() override
	{

	}

private:
	utils::nt::module steam_client_module_;

	void run_mod()
	{
		const char* command = "-proc ";
		const char* parent_proc = strstr(GetCommandLineA(), command);

		if (parent_proc)
		{
			const int pid = atoi(parent_proc + strlen(command));

			const HANDLE process_handle = OpenProcess(SYNCHRONIZE, FALSE, pid);
			if (process_handle && process_handle != INVALID_HANDLE_VALUE)
			{
				WaitForSingleObject(process_handle, INFINITE);
				CloseHandle(process_handle);
			}

			module_loader::trigger_premature_shutdown();
		}
	}

	void* load_client_engine()
	{
		if (!this->steam_client_module_.is_valid()) return nullptr;

		for(int i = 1;; ++i)
		{
			std::string name = utils::string::va("CLIENTENGINE_INTERFACE_VERSION%03i", i);
			void* client_engine = this->steam_client_module_.invoke<void*>("CreateInterface", name.data(), nullptr);
			if (client_engine) return client_engine;
		}
	}

	void load_client()
	{
		const auto steam_path = ::steam::get_steam_install_directory();
		if (steam_path.empty()) return;

		SetDllDirectoryA(steam_path.data());

		this->steam_client_module_ = utils::nt::module::load("steamclient.dll");
		if (!this->steam_client_module_.is_valid()) return;

		steam::interface steam_client = this->steam_client_module_.invoke<void*>("CreateInterface", "SteamClient008", nullptr);
		steam::interface client_engine = load_client_engine();

		const auto pipe = steam_client.invoke<void*>(0); // CreateSteamPipe
		const auto gobal_user = steam_client.invoke<void*>(2, pipe); // ConnectToGlobalUser
	}
};

//REGISTER_MODULE(steam_proxy)
