#include <std_include.hpp>

namespace steam
{
	HMODULE overlay = nullptr;

	uint64_t callbacks::call_id_ = 0;
	std::recursive_mutex callbacks::mutex_;
	std::map<uint64_t, bool> callbacks::calls_;
	std::map<uint64_t, callbacks::base*> callbacks::result_handlers_;
	std::vector<callbacks::result> callbacks::results_;
	std::vector<callbacks::base*> callbacks::callback_list_;

	uint64_t callbacks::register_call()
	{
		std::lock_guard _(callbacks::mutex_);
		callbacks::calls_[++callbacks::call_id_] = false;
		return callbacks::call_id_;
	}

	void callbacks::register_callback(callbacks::base* handler, const int callback)
	{
		std::lock_guard _(callbacks::mutex_);
		handler->set_i_callback(callback);
		callbacks::callback_list_.push_back(handler);
	}

	void callbacks::register_call_result(const uint64_t call, callbacks::base* result)
	{
		std::lock_guard _(callbacks::mutex_);
		callbacks::result_handlers_[call] = result;
	}

	void callbacks::return_call(void* data, const int size, const int type, const uint64_t call)
	{
		std::lock_guard _(callbacks::mutex_);

		callbacks::result result;

		result.call = call;
		result.data = data;
		result.size = size;
		result.type = type;

		callbacks::calls_[call] = true;

		callbacks::results_.push_back(result);
	}

	void callbacks::run_callbacks()
	{
		std::lock_guard _(callbacks::mutex_);

		for (auto result : callbacks::results_)
		{
			if (callbacks::result_handlers_.find(result.call) != callbacks::result_handlers_.end())
			{
				callbacks::result_handlers_[result.call]->run(result.data, false, result.call);
			}

			for (auto callback : callbacks::callback_list_)
			{
				if (callback && callback->get_i_callback() == result.type)
				{
					callback->run(result.data, false, 0);
				}
			}

			if (result.data)
			{
				free(result.data);
			}
		}

		callbacks::results_.clear();
	}

	extern "C"
	{
		bool SteamAPI_RestartAppIfNecessary()
		{
			return false;
		}

		bool SteamAPI_Init()
		{
			overlay = GetModuleHandleA("gameoverlayrenderer.dll");

			if (!overlay)
			{
				HKEY reg_key;
				if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Valve\\Steam", 0, KEY_QUERY_VALUE, &reg_key) == ERROR_SUCCESS)
				{
					char steam_path[MAX_PATH] = { 0 };
					DWORD length = sizeof(steam_path);
					RegQueryValueExA(reg_key, "InstallPath", nullptr, nullptr, reinterpret_cast<BYTE*>(steam_path), &length);
					RegCloseKey(reg_key);

					SetDllDirectoryA(steam_path);

					strcat_s(steam_path, "gameoverlayrenderer.dll");

					overlay = LoadLibraryA(steam_path);
				}
			}

			return true;
		}

		void SteamAPI_RegisterCallResult(callbacks::base* result, uint64_t call)
		{
			callbacks::register_call_result(call, result);
		}

		void SteamAPI_RegisterCallback(callbacks::base* handler, int callback)
		{
			callbacks::register_callback(handler, callback);
		}

		void SteamAPI_RunCallbacks()
		{
			callbacks::run_callbacks();
		}

		void SteamAPI_Shutdown()
		{
		}

		void SteamAPI_UnregisterCallResult()
		{
		}

		void SteamAPI_UnregisterCallback()
		{
		}


		bool SteamGameServer_Init()
		{
			return true;
		}

		void SteamGameServer_RunCallbacks()
		{
		}

		void SteamGameServer_Shutdown()
		{
		}


		steam::friends* SteamFriends()
		{
			static steam::friends friends;
			return &friends;
		}

		steam::matchmaking* SteamMatchmaking()
		{
			static steam::matchmaking matchmaking;
			return &matchmaking;
		}

		steam::matchmaking_servers* SteamMatchmakingServers()
		{
			static steam::matchmaking_servers matchmaking_servers;
			return &matchmaking_servers;
		}

		steam::game_server* SteamGameServer()
		{
			static steam::game_server game_server;
			return &game_server;
		}

		steam::master_server_updater* SteamMasterServerUpdater()
		{
			static steam::master_server_updater master_server_updater;
			return &master_server_updater;
		}

		steam::networking* SteamNetworking()
		{
			static steam::networking networking;
			return &networking;
		}

		steam::remote_storage* SteamRemoteStorage()
		{
			static steam::remote_storage remote_storage;
			return &remote_storage;
		}

		steam::user* SteamUser()
		{
			static steam::user user;
			return &user;
		}

		steam::utils* SteamUtils()
		{
			static steam::utils utils;
			return &utils;
		}

		steam::apps* SteamApps()
		{
			static steam::apps apps;
			return &apps;
		}

		steam::user_stats* SteamUserStats()
		{
			static steam::user_stats user_stats;
			return &user_stats;
		}
	}
}
