#include <std_include.hpp>

namespace Steam
{
	bool Apps::BIsSubscribed()
	{
		return true;
	}

	bool Apps::BIsLowViolence()
	{
		return false;
	}

	bool Apps::BIsCybercafe()
	{
		return false;
	}

	bool Apps::BIsVACBanned()
	{
		return false;
	}

	const char *Apps::GetCurrentGameLanguage()
	{
		return "english";
	}

	const char *Apps::GetAvailableGameLanguages()
	{
		return "english";
	}

	bool Apps::BIsSubscribedApp(unsigned int appID)
	{
		return true;
	}

	bool Apps::BIsDlcInstalled(unsigned int appID)
	{
		return true;
	}

	unsigned int Apps::GetEarliestPurchaseUnixTime(unsigned int nAppID)
	{
		return 0;
	}

	bool Apps::BIsSubscribedFromFreeWeekend()
	{
		return false;
	}

	int Apps::GetDLCCount()
	{
		return 0;
	}

	bool Apps::BGetDLCDataByIndex(int iDLC, unsigned int *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize)
	{
		return false;
	}

	void Apps::InstallDLC(unsigned int nAppID)
	{
	}

	void Apps::UninstallDLC(unsigned int nAppID)
	{
	}
}
