#include <std_include.hpp>
#include <discord_rpc.h>
#include "loader/module_loader.hpp"
#include "game/game.hpp"
#include "utils/nt.hpp"
#include "steam/steam.hpp"
#include "steam/interface.hpp"
#include "utils/string.hpp"
#include "utils/io.hpp"

class steam_proxy final : public module
{
public:
	void post_start() override
	{
		if (game::is_dedi()) return;

		this->run_mod();
		this->load_client();
	}

	void post_load() override
	{
		if (game::is_dedi()) return;

		try
		{
			this->start_mod(game::is_mp() ? "LUUUUUL" : "LEEEEL", 480);
		}
		catch (std::exception& e)
		{
			printf("Steam: %s\n", e.what());
		}
	}

	void pre_destroy() override
	{
	}

private:
	utils::nt::module steam_client_module_;

	steam::interface steam_client_;
	steam::interface client_engine_;
	steam::interface client_user_;
	steam::interface client_utils_;

	void* steam_pipe_ = nullptr;
	void* global_user_ = nullptr;

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

		for (int i = 1; i > 0; ++i)
		{
			std::string name = utils::string::va("CLIENTENGINE_INTERFACE_VERSION%03i", i);
			void* client_engine = this->steam_client_module_.invoke<void*>("CreateInterface", name.data(), nullptr);
			if (client_engine) return client_engine;
		}

		return nullptr;
	}

	void load_client()
	{
		const auto steam_path = ::steam::get_steam_install_directory();
		if (steam_path.empty()) return;

		utils::nt::module::load(steam_path + "tier0_s.dll");
		utils::nt::module::load(steam_path + "vstdlib_s.dll");
		this->steam_client_module_ = utils::nt::module::load(steam_path + "steamclient.dll");

		if (!this->steam_client_module_) return;

		this->steam_client_ = this->steam_client_module_.invoke<void*>("CreateInterface", "SteamClient008", nullptr);
		this->client_engine_ = load_client_engine();

		if (!this->client_engine_) return;

		this->steam_pipe_ = this->steam_client_.invoke<void*>(0); // CreateSteamPipe
		this->global_user_ = this->steam_client_.invoke<void*>(2, this->steam_pipe_); // ConnectToGlobalUser
		this->client_user_ = this->client_engine_.invoke<void*>(8, this->steam_pipe_, this->global_user_,
		                                                        "CLIENTUSER_INTERFACE_VERSION001"); // GetIClientUser
		this->client_utils_ = this->client_engine_.invoke<void*>(13, this->steam_pipe_,
		                                                         "CLIENTUTILS_INTERFACE_VERSION001"); // GetIClientUtils
	}

	void start_mod(const std::string& title, const size_t app_id)
	{
		if (!this->client_utils_ || !this->client_user_) return;

		this->client_utils_.invoke<void>("SetAppIDForCurrentPipe", app_id, false);

		utils::nt::module self;
		std::string path = self.get_path();

		char our_directory[MAX_PATH] = {0};
		GetCurrentDirectoryA(sizeof(our_directory), our_directory);

		std::string cmdline = utils::string::va("\"%s\" -proc %d", path.data(), GetCurrentProcessId());

		game_id game_id;
		game_id.raw.type = 1; // k_EGameIDTypeGameMod
		game_id.raw.app_id = app_id & 0xFFFFFF;

		const char* mod_id = "OIW5";
		game_id.raw.mod_id = *reinterpret_cast<const unsigned int*>(mod_id) | 0x80000000;

		this->client_user_.invoke<bool>("SpawnProcess", self.get_path().data(), cmdline.data(), 0, our_directory,
		                                game_id.bits, title.data(), app_id, 0, 0);
	}
};

//REGISTER_MODULE(steam_proxy)
