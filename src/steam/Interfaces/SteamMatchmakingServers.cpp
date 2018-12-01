#include <std_include.hpp>

namespace Steam
{
	void* MatchmakingServers::RequestInternetServerList(unsigned int iApp, void **ppchFilters, unsigned int nFilters, void *pRequestServersResponse)
	{
		return nullptr;
	}

	void* MatchmakingServers::RequestLANServerList(unsigned int iApp, void *pRequestServersResponse)
	{
		return nullptr;
	}

	void* MatchmakingServers::RequestFriendsServerList(unsigned int iApp, void **ppchFilters, unsigned int nFilters, void *pRequestServersResponse)
	{
		return nullptr;
	}

	void* MatchmakingServers::RequestFavoritesServerList(unsigned int iApp, void **ppchFilters, unsigned int nFilters, void *pRequestServersResponse)
	{
		return nullptr;
	}

	void* MatchmakingServers::RequestHistoryServerList(unsigned int iApp, void **ppchFilters, unsigned int nFilters, void *pRequestServersResponse)
	{
		return nullptr;
	}

	void* MatchmakingServers::RequestSpectatorServerList(unsigned int iApp, void **ppchFilters, unsigned int nFilters, void *pRequestServersResponse)
	{
		return nullptr;
	}

	void MatchmakingServers::ReleaseRequest(void* hServerListRequest)
	{
	}

	void *MatchmakingServers::GetServerDetails(void* hRequest, int iServer)
	{
		return nullptr;
	}

	void MatchmakingServers::CancelQuery(void* hRequest)
	{
	}

	void MatchmakingServers::RefreshQuery(void* hRequest)
	{
	}

	bool MatchmakingServers::IsRefreshing(void* hRequest)
	{
		return false;
	}

	int MatchmakingServers::GetServerCount(void* hRequest)
	{
		return 0;
	}

	void MatchmakingServers::RefreshServer(void* hRequest, int iServer)
	{
	}

	int MatchmakingServers::PingServer(unsigned int unIP, unsigned short usPort, void *pRequestServersResponse)
	{
		return 0;
	}

	int MatchmakingServers::PlayerDetails(unsigned int unIP, unsigned short usPort, void *pRequestServersResponse)
	{
		return 0;
	}

	int MatchmakingServers::ServerRules(unsigned int unIP, unsigned short usPort, void *pRequestServersResponse)
	{
		return 0;
	}

	void MatchmakingServers::CancelServerQuery(int hServerQuery)
	{
	}
}
