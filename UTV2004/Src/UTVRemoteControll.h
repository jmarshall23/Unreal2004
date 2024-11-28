#ifndef UTVREMOTECONTROLL
#define UTVREMOTECONTROLL

#include "UnIpDrv.h"
#include "ReplicatorEngine.h"

class UTVRemoteControll : public FUdpLink
{
public:
	UTVRemoteControll(INT port);
	~UTVRemoteControll(void);
	void Tick(void);
	virtual void OnReceivedData( FIpAddr SrcAddr, BYTE* Data, INT Count );

	struct WaitingClient {
		DWORD ChallengeGiven;
		FIpAddr address;
	};

	UTVList<WaitingClient> WaitingClients;
};

#endif