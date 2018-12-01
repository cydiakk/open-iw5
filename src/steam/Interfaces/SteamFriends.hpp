namespace Steam
{
	class Friends
	{
	public:
		virtual const char *GetPersonaName();
		virtual void SetPersonaName(const char *pchPersonaName);
		virtual int GetPersonaState();
		virtual int GetFriendCount(int eFriendFlags);
		virtual SteamID GetFriendByIndex(int iFriend, int iFriendFlags);
		virtual int GetFriendRelationship(SteamID steamIDFriend);
		virtual int GetFriendPersonaState(SteamID steamIDFriend);
		virtual const char *GetFriendPersonaName(SteamID steamIDFriend);
		virtual bool GetFriendGamePlayed(SteamID steamIDFriend, void *pFriendGameInfo);
		virtual const char *GetFriendPersonaNameHistory(SteamID steamIDFriend, int iPersonaName);
		virtual bool HasFriend(SteamID steamIDFriend, int eFriendFlags);
		virtual int GetClanCount();
		virtual SteamID GetClanByIndex(int iClan);
		virtual const char *GetClanName(SteamID steamIDClan);
		virtual const char *GetClanTag(SteamID steamIDClan);
		virtual int GetFriendCountFromSource(SteamID steamIDSource);
		virtual SteamID GetFriendFromSourceByIndex(SteamID steamIDSource, int iFriend);
		virtual bool IsUserInSource(SteamID steamIDUser, SteamID steamIDSource);
		virtual void SetInGameVoiceSpeaking(SteamID steamIDUser, bool bSpeaking);
		virtual void ActivateGameOverlay(const char *pchDialog);
		virtual void ActivateGameOverlayToUser(const char *pchDialog, SteamID steamID);
		virtual void ActivateGameOverlayToWebPage(const char *pchURL);
		virtual void ActivateGameOverlayToStore(unsigned int nAppID);
		virtual void SetPlayedWith(SteamID steamIDUserPlayedWith);
		virtual void ActivateGameOverlayInviteDialog(SteamID steamIDLobby);
		virtual int GetSmallFriendAvatar(SteamID steamIDFriend);
		virtual int GetMediumFriendAvatar(SteamID steamIDFriend);
		virtual int GetLargeFriendAvatar(SteamID steamIDFriend);
		virtual bool RequestUserInformation(SteamID steamIDUser, bool bRequireNameOnly);
		virtual unsigned __int64 RequestClanOfficerList(SteamID steamIDClan);
		virtual SteamID GetClanOwner(SteamID steamIDClan);
		virtual int GetClanOfficerCount(SteamID steamIDClan);
		virtual SteamID GetClanOfficerByIndex(SteamID steamIDClan, int iOfficer);
		virtual int GetUserRestrictions();
		virtual bool SetRichPresence(const char *pchKey, const char *pchValue);
		virtual void ClearRichPresence();
		virtual const char *GetFriendRichPresence(SteamID steamIDFriend, const char *pchKey);
		virtual int GetFriendRichPresenceKeyCount(SteamID steamIDFriend);
		virtual const char *GetFriendRichPresenceKeyByIndex(SteamID steamIDFriend, int iKey);
		virtual bool InviteUserToGame(SteamID steamIDFriend, const char *pchConnectString);
		virtual int GetCoplayFriendCount();
		virtual SteamID GetCoplayFriend(int iCoplayFriend);
		virtual int GetFriendCoplayTime(SteamID steamIDFriend);
		virtual unsigned int GetFriendCoplayGame(SteamID steamIDFriend);
	};
}
