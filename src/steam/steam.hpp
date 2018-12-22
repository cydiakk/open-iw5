#pragma once

#define STEAM_EXPORT extern "C" __declspec(dllexport)

struct raw_steam_id final
{
	unsigned int account_id : 32;
	unsigned int account_instance : 20;
	unsigned int account_type : 4;
	int          universe : 8;
};

typedef union
{
	raw_steam_id raw;
	unsigned long long bits;
} steam_id;

#include "interfaces/SteamApps.hpp"
#include "interfaces/SteamUser.hpp"
#include "interfaces/SteamUtils.hpp"
#include "interfaces/SteamFriends.hpp"
#include "interfaces/SteamUserStats.hpp"
#include "interfaces/SteamGameServer.hpp"
#include "interfaces/SteamNetworking.hpp"
#include "interfaces/SteamMatchmaking.hpp"
#include "interfaces/SteamRemoteStorage.hpp"
#include "interfaces/SteamMatchmakingServers.hpp"
#include "interfaces/SteamMasterServerUpdater.hpp"

namespace steam
{
	class callbacks
	{
	public:
		class base
		{
		public:
			base() : flags_(0), callback_(0) {};

			virtual void run(void *pv_param) = 0;
			virtual void run(void *pv_param, bool failure, uint64_t handle) = 0;
			virtual int get_callback_size_bytes() = 0;

			int get_i_callback() const { return callback_; }
			void set_i_callback(const int i_callback) { callback_ = i_callback; }

		protected:
			unsigned char flags_;
			int callback_;
		};

		struct result final
		{
			void* data;
			int size;
			int type;
			uint64_t call;
		};

		static uint64_t register_call();
		static void register_callback(base* handler, int callback);
		static void register_call_result(uint64_t call, base* result);
		static void return_call(void* data, int size, int type, uint64_t call);
		static void run_callbacks();

	private:
		static uint64_t call_id_;
		static std::recursive_mutex mutex_;
		static std::map<uint64_t, bool> calls_;
		static std::map<uint64_t, base*> result_handlers_;
		static std::vector<result> results_;
		static std::vector<base*> callback_list_;
	};

	STEAM_EXPORT bool SteamAPI_RestartAppIfNecessary();
	STEAM_EXPORT bool SteamAPI_Init();
	STEAM_EXPORT void SteamAPI_RegisterCallResult(callbacks::base* result, uint64_t call);
	STEAM_EXPORT void SteamAPI_RegisterCallback(callbacks::base* handler, int callback);
	STEAM_EXPORT void SteamAPI_RunCallbacks();
	STEAM_EXPORT void SteamAPI_Shutdown();
	STEAM_EXPORT void SteamAPI_UnregisterCallResult();
	STEAM_EXPORT void SteamAPI_UnregisterCallback();

	STEAM_EXPORT bool SteamGameServer_Init();
	STEAM_EXPORT void SteamGameServer_RunCallbacks();
	STEAM_EXPORT void SteamGameServer_Shutdown();

	STEAM_EXPORT steam::friends* SteamFriends();
	STEAM_EXPORT steam::matchmaking* SteamMatchmaking();
	STEAM_EXPORT steam::matchmaking_servers* SteamMatchmakingServers();
	STEAM_EXPORT steam::game_server* SteamGameServer();
	STEAM_EXPORT steam::master_server_updater* SteamMasterServerUpdater();
	STEAM_EXPORT steam::networking* SteamNetworking();
	STEAM_EXPORT steam::remote_storage* SteamRemoteStorage();
	STEAM_EXPORT steam::user* SteamUser();
	STEAM_EXPORT steam::utils* SteamUtils();
	STEAM_EXPORT steam::apps* SteamApps();
	STEAM_EXPORT steam::user_stats* SteamUserStats();

	extern HMODULE overlay;
}
