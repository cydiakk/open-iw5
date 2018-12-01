#include <std_include.hpp>

namespace Steam
{
	int User::GetHSteamUser()
	{
		return NULL;
	}

	bool User::LoggedOn()
	{
		return true;
	}

	SteamID User::GetSteamID()
	{
		SteamID id;
		id.Bits = 0x110000100000000 | (0x1377 & ~0x80000000);
		return id;
	}

	int User::InitiateGameConnection(void *pAuthBlob, int cbMaxAuthBlob, SteamID steamIDGameServer, unsigned int unIPServer, unsigned short usPortServer, bool bSecure)
	{
		return 0;
	}

	void User::TerminateGameConnection(unsigned int unIPServer, unsigned short usPortServer)
	{
	}

	void User::TrackAppUsageEvent(SteamID gameID, int eAppUsageEvent, const char *pchExtraInfo)
	{
	}

	bool User::GetUserDataFolder(char *pchBuffer, int cubBuffer)
	{
		return false;
	}

	void User::StartVoiceRecording()
	{
	}

	void User::StopVoiceRecording()
	{
	}

	int User::GetAvailableVoice(unsigned int *pcbCompressed, unsigned int *pcbUncompressed, unsigned int nUncompressedVoiceDesiredSampleRate)
	{
		return 0;
	}

	int User::GetVoice(bool bWantCompressed, void *pDestBuffer, unsigned int cbDestBufferSize, unsigned int *nBytesWritten, bool bWantUncompressed, void *pUncompressedDestBuffer, unsigned int cbUncompressedDestBufferSize, unsigned int *nUncompressBytesWritten, unsigned int nUncompressedVoiceDesiredSampleRate)
	{
		return 0;
	}

	int User::DecompressVoice(void *pCompressed, unsigned int cbCompressed, void *pDestBuffer, unsigned int cbDestBufferSize, unsigned int *nBytesWritten)
	{
		return 0;
	}

	unsigned int User::GetVoiceOptimalSampleRate()
	{
		return 0;
	}

	unsigned int User::GetAuthSessionTicket(void *pTicket, int cbMaxTicket, unsigned int *pcbTicket)
	{
		return 0;
	}

	int User::BeginAuthSession(const void *pAuthTicket, int cbAuthTicket, SteamID steamID)
	{
		return 0;
	}

	void User::EndAuthSession(SteamID steamID)
	{
	}

	void User::CancelAuthTicket(unsigned int hAuthTicket)
	{
	}

	unsigned int User::UserHasLicenseForApp(SteamID steamID, unsigned int appID)
	{
		return 0;
	}

	bool User::BIsBehindNAT()
	{
		return false;
	}

	void User::AdvertiseGame(SteamID steamIDGameServer, unsigned int unIPServer, unsigned short usPortServer)
	{
	}

	unsigned __int64 User::RequestEncryptedAppTicket(void *pUserData, int cbUserData)
	{
		// Generate the authentication ticket
		//Components::DemonWare::GenerateAuthTicket(std::string(reinterpret_cast<char*>(pUserData), cbUserData));

		// Create the call response
		uint64_t result = Callbacks::RegisterCall();
		EncryptedAppTicketResponse* retvals = (EncryptedAppTicketResponse*)calloc(1, sizeof(EncryptedAppTicketResponse));//::Utils::Memory::AllocateArray<EncryptedAppTicketResponse>();
		retvals->m_eResult = 1;

		// Return the call response
		Callbacks::ReturnCall(retvals, sizeof(EncryptedAppTicketResponse), EncryptedAppTicketResponse::CallbackID, result);

		return result;
	}

	bool User::GetEncryptedAppTicket(void *pTicket, int cbMaxTicket, unsigned int *pcbTicket)
	{
		if (cbMaxTicket < 0) return false;
		return false;//Components::DemonWare::GetAuthTicket(pTicket, static_cast<unsigned int>(cbMaxTicket), pcbTicket);
	}
}
