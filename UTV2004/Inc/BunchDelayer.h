#ifndef BUNCHDELAYER
#define BUNCHDELAYER

#include "UnIpDrv.h"
#include "UtvChannel.h"
#include "replicatorengine.h"

struct InternalBunch;

//"Piggybacks" on the bunchdelayer.. Actually it doesn't
class MoveDelayer
{
public:
	struct WaitingMove {
		FString Move;
		float OutTime;
	};
	UTVList<WaitingMove> WaitingMoves;

	struct WaitingActor {
		INT Actor;
		float OutTime;
	};
	UTVList<WaitingActor> WaitingActors;

	MoveDelayer();
	void Tick(float DeltaSeconds);
	void AddMove(FString Move);
	void AddActor (INT Actor);
	void Restart();
};

extern MoveDelayer mDelayer;

class VoiceDelayer
{
public:
	struct WaitingVoice {
		FVoiceInfo VoiceInfo;
		float OutTime;
	};
	UTVList<WaitingVoice> WaitingVoices;

	VoiceDelayer();
	void Tick(float DeltaSeconds);
	void AddVoice(FVoiceInfo* VoiceInfo);
	void Restart();
};

extern VoiceDelayer vDelayer;

class BunchDelayer : public UObject
{
	DECLARE_CLASS(BunchDelayer,UObject,CLASS_Transient,utv2004)
public:
	float DelayTime;
	float CurrentTime;
	unsigned int MaxChannelQueSize;

	struct WaitingBunch {
		InternalBunch* Bunch;
		float OutTime;
		bool Important;
	};
	UTVList<WaitingBunch> WaitingBunches;

	typedef TArray<InternalBunch> OpenPackets;

	OpenPackets* OpenChannels[UNetConnection::MAX_CHANNELS+1];
	OpenPackets* DelayedOpenChannels[UNetConnection::MAX_CHANNELS+1];

	struct WatcherActor {
		TArray<BYTE *> Data;
		UClass* ActorClass;
	};
	WatcherActor WActors[UNetConnection::MAX_CHANNELS+1];

	struct ActorProps {
		UClass* ActorClass;
		TMap<int,int> PropertiesUsed; //used as a set, value doesnt matter
		bool worthTracking;
	};
	ActorProps actors[UNetConnection::MAX_CHANNELS+1];

	BunchDelayer(void);
	void Destroy(void);
	void Tick(float DeltaSeconds);
	void ClearActor (DWORD index);
	void ParseWatcherProperties (InternalBunch* Bunch);
	bool ParseProperties (InternalBunch* Bunch);
	void AddBunch(FInBunch* Bunch);
	void NewConnection(UNetConnection* Connection);
	void OpenChannel(UNetConnection* Connection,INT ChIndex,bool SendQuedBunches=false);
	void OpenChannelOnClients(int ChIndex);
	void Restart(void);
};

extern BunchDelayer* Delayer;

#endif