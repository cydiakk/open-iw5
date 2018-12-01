#include <std_include.hpp>

namespace Steam
{
	bool UserStats::RequestCurrentStats()
	{
		return true;
	}

	bool UserStats::GetStat(const char *pchName, int *pData)
	{
		return false;
	}

	bool UserStats::GetStat(const char *pchName, float *pData)
	{
		return false;
	}

	bool UserStats::SetStat(const char *pchName, int nData)
	{
		return false;
	}

	bool UserStats::SetStat(const char *pchName, float fData)
	{
		return false;
	}

	bool UserStats::UpdateAvgRateStat(const char *pchName, float flCountThisSession, double dSessionLength)
	{
		return false;
	}

	bool UserStats::GetAchievement(const char *pchName, bool *pbAchieved)
	{
		return false;
	}

	bool UserStats::SetAchievement(const char *pchName)
	{
		return false;
	}

	bool UserStats::ClearAchievement(const char *pchName)
	{
		return false;
	}

	bool UserStats::GetAchievementAndUnlockTime(const char *pchName, bool *pbAchieved, unsigned int *punUnlockTime)
	{
		return false;
	}

	bool UserStats::StoreStats()
	{
		return false;
	}

	int UserStats::GetAchievementIcon(const char *pchName)
	{
		return 0;
	}

	const char *UserStats::GetAchievementDisplayAttribute(const char *pchName, const char *pchKey)
	{
		return "";
	}

	bool UserStats::IndicateAchievementProgress(const char *pchName, unsigned int nCurProgress, unsigned int nMaxProgress)
	{
		return false;
	}

	unsigned __int64 UserStats::RequestUserStats(SteamID steamIDUser)
	{
		return 0;
	}

	bool UserStats::GetUserStat(SteamID steamIDUser, const char *pchName, int *pData)
	{
		return false;
	}

	bool UserStats::GetUserStat(SteamID steamIDUser, const char *pchName, float *pData)
	{
		return false;
	}

	bool UserStats::GetUserAchievement(SteamID steamIDUser, const char *pchName, bool *pbAchieved)
	{
		return false;
	}

	bool UserStats::GetUserAchievementAndUnlockTime(SteamID steamIDUser, const char *pchName, bool *pbAchieved, unsigned int *punUnlockTime)
	{
		return false;
	}

	bool UserStats::ResetAllStats(bool bAchievementsToo)
	{
		return false;
	}

	unsigned __int64 UserStats::FindOrCreateLeaderboard(const char *pchLeaderboardName, int eLeaderboardSortMethod, int eLeaderboardDisplayType)
	{
		return 0;
	}

	unsigned __int64 UserStats::FindLeaderboard(const char *pchLeaderboardName)
	{
		return 0;
	}

	const char *UserStats::GetLeaderboardName(unsigned __int64 hSteamLeaderboard)
	{
		return "";
	}

	int UserStats::GetLeaderboardEntryCount(unsigned __int64 hSteamLeaderboard)
	{
		return 0;
	}

	int UserStats::GetLeaderboardSortMethod(unsigned __int64 hSteamLeaderboard)
	{
		return 0;
	}

	int UserStats::GetLeaderboardDisplayType(unsigned __int64 hSteamLeaderboard)
	{
		return 0;
	}

	unsigned __int64 UserStats::DownloadLeaderboardEntries(unsigned __int64 hSteamLeaderboard, int eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
	{
		return 0;
	}

	unsigned __int64 UserStats::DownloadLeaderboardEntriesForUsers(unsigned __int64 hSteamLeaderboard, SteamID *prgUsers, int cUsers)
	{
		return 0;
	}

	bool UserStats::GetDownloadedLeaderboardEntry(unsigned __int64 hSteamLeaderboardEntries, int index, int *pLeaderboardEntry, int *pDetails, int cDetailsMax)
	{
		return false;
	}

	unsigned __int64 UserStats::UploadLeaderboardScore(unsigned __int64 hSteamLeaderboard, int eLeaderboardUploadScoreMethod, int nScore, const int *pScoreDetails, int cScoreDetailsCount)
	{
		return 0;
	}

	unsigned __int64 UserStats::AttachLeaderboardUGC(unsigned __int64 hSteamLeaderboard, unsigned __int64 hUGC)
	{
		return 0;
	}

	unsigned __int64 UserStats::GetNumberOfCurrentPlayers()
	{
		return 0;
	}

	unsigned __int64 UserStats::RequestGlobalAchievementPercentages()
	{
		return 0;
	}

	int UserStats::GetMostAchievedAchievementInfo(char *pchName, unsigned int unNameBufLen, float *pflPercent, bool *pbAchieved)
	{
		return 0;
	}

	int UserStats::GetNextMostAchievedAchievementInfo(int iIteratorPrevious, char *pchName, unsigned int unNameBufLen, float *pflPercent, bool *pbAchieved)
	{
		return 0;
	}

	bool UserStats::GetAchievementAchievedPercent(const char *pchName, float *pflPercent)
	{
		return false;
	}

	unsigned __int64 UserStats::RequestGlobalStats(int nHistoryDays)
	{
		return 0;
	}

	bool UserStats::GetGlobalStat(const char *pchStatName, __int64 *pData)
	{
		return false;
	}

	bool UserStats::GetGlobalStat(const char *pchStatName, double *pData)
	{
		return false;
	}

	int UserStats::GetGlobalStatHistory(const char *pchStatName, __int64 *pData, unsigned int cubData)
	{
		return 0;
	}

	int UserStats::GetGlobalStatHistory(const char *pchStatName, double *pData, unsigned int cubData)
	{
		return 0;
	}
}
