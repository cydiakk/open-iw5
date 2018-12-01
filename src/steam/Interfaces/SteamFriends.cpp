#include <std_include.hpp>

namespace Steam
{
	const char *Friends::GetPersonaName()
	{
		return "GlaDos";
	}

	void Friends::SetPersonaName(const char *pchPersonaName)
	{

	}

	int Friends::GetPersonaState()
	{
		return 1;
	}

	int Friends::GetFriendCount(int eFriendFlags)
	{
		return 0;
	}

	SteamID Friends::GetFriendByIndex(int iFriend, int iFriendFlags)
	{
		return SteamID();
	}

	int Friends::GetFriendRelationship(SteamID steamIDFriend)
	{
		return 0;
	}

	int Friends::GetFriendPersonaState(SteamID steamIDFriend)
	{
		return 0;
	}

	const char *Friends::GetFriendPersonaName(SteamID steamIDFriend)
	{
		return "";
	}

	bool Friends::GetFriendGamePlayed(SteamID steamIDFriend, void *pFriendGameInfo)
	{
		return false;
	}

	const char *Friends::GetFriendPersonaNameHistory(SteamID steamIDFriend, int iPersonaName)
	{
		return "";
	}

	bool Friends::HasFriend(SteamID steamIDFriend, int eFriendFlags)
	{
		return false;
	}

	int Friends::GetClanCount()
	{
		return 0;
	}

	SteamID Friends::GetClanByIndex(int iClan)
	{
		return SteamID();
	}

	const char *Friends::GetClanName(SteamID steamIDClan)
	{
		return "3arc";
	}

	const char *Friends::GetClanTag(SteamID steamIDClan)
	{
		return this->GetClanName(steamIDClan);
	}

	int Friends::GetFriendCountFromSource(SteamID steamIDSource)
	{
		return 0;
	}

	SteamID Friends::GetFriendFromSourceByIndex(SteamID steamIDSource, int iFriend)
	{
		return SteamID();
	}

	bool Friends::IsUserInSource(SteamID steamIDUser, SteamID steamIDSource)
	{
		return false;
	}

	void Friends::SetInGameVoiceSpeaking(SteamID steamIDUser, bool bSpeaking)
	{
	}

	void Friends::ActivateGameOverlay(const char *pchDialog)
	{
	}

	void Friends::ActivateGameOverlayToUser(const char *pchDialog, SteamID steamID)
	{
	}

	void Friends::ActivateGameOverlayToWebPage(const char *pchURL)
	{
	}

	void Friends::ActivateGameOverlayToStore(unsigned int nAppID)
	{
		OutputDebugStringA("Store requested!");
	}

	void Friends::SetPlayedWith(SteamID steamIDUserPlayedWith)
	{
	}

	void Friends::ActivateGameOverlayInviteDialog(SteamID steamIDLobby)
	{
	}

	int Friends::GetSmallFriendAvatar(SteamID steamIDFriend)
	{
		return 0;
	}

	int Friends::GetMediumFriendAvatar(SteamID steamIDFriend)
	{
		return 0;
	}

	int Friends::GetLargeFriendAvatar(SteamID steamIDFriend)
	{
		return 0;
	}

	bool Friends::RequestUserInformation(SteamID steamIDUser, bool bRequireNameOnly)
	{
		return false;
	}

	unsigned __int64 Friends::RequestClanOfficerList(SteamID steamIDClan)
	{
		return 0;
	}

	SteamID Friends::GetClanOwner(SteamID steamIDClan)
	{
		return SteamID();
	}

	int Friends::GetClanOfficerCount(SteamID steamIDClan)
	{
		return 0;
	}

	SteamID Friends::GetClanOfficerByIndex(SteamID steamIDClan, int iOfficer)
	{
		return SteamID();
	}

	int Friends::GetUserRestrictions()
	{
		return 0;
	}

	bool Friends::SetRichPresence(const char *pchKey, const char *pchValue)
	{
		return true;
	}

	void Friends::ClearRichPresence()
	{
	}

	const char *Friends::GetFriendRichPresence(SteamID steamIDFriend, const char *pchKey)
	{
		return "";
	}

	int Friends::GetFriendRichPresenceKeyCount(SteamID steamIDFriend)
	{
		return 0;
	}

	const char *Friends::GetFriendRichPresenceKeyByIndex(SteamID steamIDFriend, int iKey)
	{
		return "a";
	}

	bool Friends::InviteUserToGame(SteamID steamIDFriend, const char *pchConnectString)
	{
		return false;
	}

	int Friends::GetCoplayFriendCount()
	{
		return 0;
	}

	SteamID Friends::GetCoplayFriend(int iCoplayFriend)
	{
		return SteamID();
	}

	int Friends::GetFriendCoplayTime(SteamID steamIDFriend)
	{
		return 0;
	}

	unsigned int Friends::GetFriendCoplayGame(SteamID steamIDFriend)
	{
		return 0;
	}
}
