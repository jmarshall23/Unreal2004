#include "ReplicatorEngine.h"
#include "UnLinker.h"
#include "UTVPackageMap.h"
#include "BunchDelayer.h"
#include "FConfigCacheIni.h"
#include "utvremotecontroll.h"

///////////////////////////////////////////////////////

void UUTVStats::Clear () 
{
	for (INT i = 0; i < 20; i++)
		Times[i] = 0;
}

void UUTVStats::Add (INT Index, INT Time)
{
	Times[Index] += Time / 1000;
}

void UUTVStats::Show ()
{
	float sleep = ((float)Times[1] / Times[0]) * 100;
	float run = ((float)Times[2] / Times[0]) * 100;

	float lrecv = ((float)Times[5] / Times[0]) * 100;
	float lsend = ((float)Times[6] / Times[0]) * 100;

	float connectmaint = ((float)Times[13] / Times[0]) * 100;
	float delayer = ((float)Times[11] / Times[0]) * 100;
	float net = ((float)Times[12] / Times[0]) * 100;

	debugf (TEXT ("Sleep: %.1f Run: %.1f Net: %.1f Recv: %.1f Send: %.1f Con: %.1f Delay: %.1f"), sleep, run, net, lrecv, lsend, connectmaint, delayer);
	//debugf (TEXT ("1: %f 2: %f 3: %f 4: %f"), , (float)Times[2] / Times[0], (float)Times[3] / Times[0], (float)Times[4] / Times[0]);
	Clear ();
}

UUTVStats* UTVStats;

///////////////////////////////////////////////////////

UReplicatorEngine::UReplicatorEngine()
: LastURL(TEXT(""))
{
}

UReplicatorEngine* UtvEngine;

void UReplicatorEngine::Init(FString IniFileName)
{
	guard(UReplicatorEngine::Init);
	iniFile=IniFileName;

	Delayer=new BunchDelayer();

	UtvEngine=this;
	ServerConnReady=false;
	DoRestart=false;
	DoRestartIn=-1;
	MsgTimeOut=-10;
	GameEnded=false;
	PrimaryConnection=0;
	ConnectStatus=0;
	ConnectedToUtvProxy=0;
	SeeAll=0;
	NoPrimary=0;
	TestedForUtvProxy=false;
	TotalClients=0;
	TotalDelay=0;
	LastTotalClientsUpdate=0;
	needStats=0;
	LoggedInPlayers=0;
	IgnoreChainedChat=0;

	FConfigCacheIni CacheIni;
	ServerAdress="127.0.0.1";
	ServerPort=7777;
	ListenPort=7780;
	debugf (TEXT ("Ini file %s "),*iniFile);
	CacheIni.GetInt(TEXT("UTV2004"),TEXT("ServerPort"),ServerPort,*iniFile);
	CacheIni.GetInt(TEXT("UTV2004"),TEXT("ListenPort"),ListenPort,*iniFile);
	CacheIni.GetInt(TEXT("UTV2004"),TEXT("MaxClients"),MaxClients,*iniFile);
	CacheIni.GetFloat(TEXT("UTV2004"),TEXT("Delay"),Delayer->DelayTime,*iniFile);
	if(Delayer->DelayTime<5)
		Delayer->DelayTime=5;
	CacheIni.GetString(TEXT("UTV2004"),TEXT("ServerAddress"),ServerAdress,*iniFile);
	CacheIni.GetString(TEXT("UTV2004"),TEXT("PrimaryPassword"),PrimaryPassword,*iniFile);
	CacheIni.GetString(TEXT("UTV2004"),TEXT("NormalPassword"),NormalPassword,*iniFile);
	CacheIni.GetString(TEXT("UTV2004"),TEXT("VipPassword"),VipPassword,*iniFile);
	CacheIni.GetString(TEXT("UTV2004"),TEXT("JoinPassword"),JoinPassword,*iniFile);
	CacheIni.GetInt(TEXT("UTV2004"),TEXT("SeeAll"),SeeAll,*iniFile);
	CacheIni.GetInt(TEXT("UTV2004"),TEXT("IgnoreChainedChat"),IgnoreChainedChat,*iniFile);
	CacheIni.GetInt(TEXT("UTV2004"),TEXT("DelayPrimaryVoice"),DelayPrimaryVoice,*iniFile);

	CacheIni.GetInt(TEXT("UTV2004"),TEXT("ClockInterval"),clockInterval,*iniFile);
	CacheIni.GetInt(TEXT("UTV2004"),TEXT("TickRate"),tickRate,*iniFile);

	CacheIni.GetInt(TEXT("UTV2004"),TEXT("NoPrimary"),NoPrimary,*iniFile);
	if(NoPrimary && !SeeAll){
		debugf(TEXT("Cant run NoPimary without SeeAll mode"));
		NoPrimary=false;
	}

	INT RemoteControllPort;
	CacheIni.GetInt(TEXT("UTV2004"),TEXT("RemoteControllPort"),RemoteControllPort,*iniFile);
	if(RemoteControllPort){
		RemoteController=new UTVRemoteControll(RemoteControllPort);
	} else {
		RemoteController=0;
	}

	//Parse the download manager part of the ini file
	INT numDl = 0;
	CacheIni.GetInt (TEXT ("UTV2004"), TEXT ("DLManagerCount"), numDl, *iniFile);
	for (INT i = 0; i < numDl; ++i) {
		FString url;
		FString key = FString::Printf (TEXT ("DLManager%i"), i);
		CacheIni.GetString (TEXT ("UTV2004"), *key, url, *iniFile);
		
		//Make sure bogus ones are not sent
		if ((url.Len () > 5) && (appStrfind (*url, TEXT ("HTTP")))) {
			//INT a=DownloadManagers.AddZeroed();
			new(DownloadManagers) FString(FString::Printf (TEXT ("DLMGR CLASS=IpDrv.HTTPDownload PARAMS=%s COMPRESSION=1"), *url));
		}
	}
	
	debugf (TEXT ("The proxy knows about %i extra HTTP Download redirects"), DownloadManagers.Num ());
	for (int i = 0; i < DownloadManagers.Num (); ++i) {
		debugf (TEXT ("URL: %s"), *(DownloadManagers(i).Mid (38)));
	}

	CacheSizeMegs = 8;

	UChannel::ChannelClasses[CHTYPE_Actor]=UChannel::ChannelClasses[7];
	UChannel::ChannelClasses[CHTYPE_Voice]=UChannel::ChannelClasses[6];

	//Create the package map
	ServerPackageMap = new UUTVPackageMap ();

	//And load our custom client package
	debugf (TEXT ("Loading UT2004 Clientside Mod"));
	BeginLoad();
	ClientPackage = UObject::GetPackageLinker(NULL,UTV2004C,LOAD_NoFail, NULL,NULL);
	debugf (TEXT ("Loaded ") UTV2004C TEXT(" with guid: %s"), ClientPackage->Summary.Guid.String ());
	EndLoad();
	if (ClientPackage == NULL)
		appErrorf(TEXT("Failed to load package ") UTV2004C);

	check(sizeof(*this)==GetClass()->GetPropertiesSize());

	// Call base.
	UEngine::Init();

	ListenDriver=0;
	ConnectDriver=0;
	uplink=0;

	InitServerState (iniFile);

	unguard;
}

void UReplicatorEngine::InitServerState (const FString iniName)
{
	int i;
	FConfigCacheIni CacheIni;

	//Start with filling in defaults
	useMaster = false;
	useGamespy = false;

	CacheIni.GetBool (TEXT ("MasterServer"), TEXT ("ConnectToMaster"), useMaster, *iniName);
	CacheIni.GetBool (TEXT ("MasterServer"), TEXT ("UseGamespy"), useGamespy, *iniName);

	if (!useMaster)
		return;

	ServerState.ServerID = 4711;
	ServerState.IP = "127.0.0.1";
	ServerState.Port = ServerPort;
	ServerState.QueryPort = ServerPort + 1;
	ServerState.ServerName = "Random UTV server";
	ServerState.MapName = "unknown";
	//ServerState.GameType = "UTVGame";
	ServerState.GameType = "xDoubleDom";
	ServerState.CurrentPlayers = 0;
	ServerState.MaxPlayers = MaxClients;
	ServerState.Ping = 0;

	ServerState.ServerInfo.Empty ();
	i = 0;
	ServerState.ServerInfo.AddZeroed ();
	ServerState.ServerInfo(i).Key = FString (TEXT ("servermode"));
	ServerState.ServerInfo(i++).Value = "dedicated";
	ServerState.ServerInfo.AddZeroed ();
	ServerState.ServerInfo(i).Key = FString (TEXT ("ServerVersion"));
	ServerState.ServerInfo(i++).Value = UTVVERSION;
	ServerState.ServerInfo.AddZeroed ();
	ServerState.ServerInfo(i).Key = FString (TEXT ("gamestats"));
	ServerState.ServerInfo(i++).Value = "false";
	if (NormalPassword != "") {
		ServerState.ServerInfo.AddZeroed ();
		ServerState.ServerInfo(i).Key = FString (TEXT ("password"));
		ServerState.ServerInfo(i++).Value = "true";
	}

	//Now update with things from the ini
	FString tmp;
	CacheIni.GetString (TEXT ("MasterServer"), TEXT ("ServerName"), tmp, *iniName);
	if (tmp != "")
		ServerState.ServerName = tmp;

	CacheIni.GetString (TEXT ("MasterServer"), TEXT ("AdminName"), tmp, *iniName);
	if (tmp != "") {
		ServerState.ServerInfo.AddZeroed ();
		ServerState.ServerInfo(i).Key = FString (TEXT ("adminname"));
		ServerState.ServerInfo(i++).Value = tmp;
	}
	CacheIni.GetString (TEXT ("MasterServer"), TEXT ("AdminEmail"), tmp, *iniName);
	if (tmp != "") {
		ServerState.ServerInfo.AddZeroed ();
		ServerState.ServerInfo(i).Key = FString (TEXT ("adminemail"));
		ServerState.ServerInfo(i++).Value = tmp;
	}
}

void UReplicatorEngine::Destroy()
{
	guard(UReplicatorEngine::Destroy);
	Super::Destroy();
	delete Delayer;
	delete RemoteController;
	unguard;
}

//
// Update everything.
//
void UReplicatorEngine::Tick( FLOAT DeltaSeconds )
{
	static bool firstTick=true;
	guard(UReplicatorEngine::Tick);
	//moved here so we can set the port in commandlet;
	if(firstTick){
		firstTick=false;
		FURL URL;
		FString Error;

		// Create net driver.
		UClass* NetDriverClass = StaticLoadClass( UNetDriver::StaticClass(), NULL, TEXT("IpDrv.TcpNetDriver"/*/"utv2004.TVNetDriver"/**/), NULL, LOAD_NoFail, NULL );

		URL.Port = ListenPort;
		ListenDriver = (UNetDriver*)StaticConstructObject( NetDriverClass );
		ListenDriver->InitialConnectTimeout = 30;
		ListenDriver->ConnectionTimeout = 30;	//not used?
		if( !ListenDriver->InitListen( this, URL, Error ) )
		{
			debugf (TEXT ("Failed to start listen driver"));
			debugf( TEXT("Failed to listen: %s"), *Error );
			delete ListenDriver;
			ListenDriver=NULL;
		}

		//Now we can create the master server uplink
		if (useMaster && ListenDriver) {
			if (uplink)
				delete uplink;

			//Create master server uplink
			uplink = new UTVUplink ();
			uplink->TryConnect ();
		}
	}

	INT LocalTickCycles=0;
	clock(LocalTickCycles);

    if( DeltaSeconds < 0.0f ) // sjs temp
        appErrorf(TEXT("Negative delta time!"));

	// Update subsystems.
	UObject::StaticTick();				
	GCache.Tick();

	if(RemoteController)
		RemoteController->Tick();

	// Update the level.
	guard(TickLevel);
	GameCycles=0;
	clock(GameCycles);
/*
	static int oldStatus=0;
	if(oldStatus!=ConnectStatus){
		oldStatus=ConnectStatus;
		debugf(TEXT("ConnectStatus %i"),ConnectStatus);
	}*/

	INT ConnectTick = 0;
	clock (ConnectTick);

	switch(ConnectStatus){
	case 0:
		if(ServerAdress=="" || TestedForUtvProxy)
			break;
	case 1:{
		UClass* NetDriverClass = StaticLoadClass( UNetDriver::StaticClass(), NULL, TEXT("IpDrv.TcpNetDriver"/*/"utv2004.TVNetDriver"/**/), NULL, LOAD_NoFail, NULL );
		ConnectDriver = (UNetDriver*)StaticConstructObject( NetDriverClass );
		FURL URL;
		FString Error;
		URL.Host=ServerAdress;
		URL.Port = ServerPort;
		if( ConnectDriver->InitConnect( this, URL, Error ) )
		{
			// Send initial message.
			ConnectDriver->ServerConnection->Logf( TEXT("HELLO REVISION=0 MINVER=%i VER=%i"), ENGINE_MIN_NET_VERSION, ENGINE_VERSION );
			ConnectDriver->ServerConnection->FlushNet();
			debugf (TEXT ("Starting server connect"));
		}
		else
		{
			debugf (TEXT ("Connect failed"));
			delete ConnectDriver;
			ConnectDriver=NULL;
		}
		ConnectStatus=2;
		break;}
	case 3:
		if(PrimaryConnection){		
			ServerPackageMap->FixLinkers (PrimaryConnection->PackageMap);
			
			SendDownloadManagers (PrimaryConnection);
			PrimaryConnection->SendPackageMap(0);
			ServerPackageMap->PostFixLinkers (PrimaryConnection->PackageMap);
			PrimaryConnection->Logf( TEXT("WELCOME LEVEL=%s GAME=%s LONE=%i"), ServerMap, GameType, 0 );
			PrimaryConnection->FlushNet();
			ConnectStatus=4;
		} else {
			for( TArray<FPackageInfo>::TIterator It(ServerPackageMap->List); It; ++It ){
				//not used any more ?
		//		ConnectDriver->ServerConnection->Logf(TEXT("HAVE GUID=%s GEN=%i"),It->Guid.String(),It->LocalGeneration);
				if(It->RemoteGeneration>It->LocalGeneration)
					It->RemoteGeneration=It->LocalGeneration;
			}
			ConnectDriver->ServerConnection->FlushNet();
			ConnectStatus=5;
		}
		break;
	case 5:
		ServerPackageMap->Compute ();
		if (ServerPackageMap->ComputeError ()) {
			debugf (TEXT ("Fatal error during Compute, restarting proxy"));
			DoRestart = true;
			break;
		}
		ServerPackageMap->ShowStatus ();

		debugf (TEXT ("Joining server"));
		ConnectDriver->ServerConnection->Logf( TEXT("JOIN") );
		ConnectDriver->ServerConnection->FlushNet();
		if(ConnectedToUtvProxy){
			ConnectDriver->ServerConnection->Logf( TEXT("OPENVOICE 1") );
			ConnectDriver->ServerConnection->FlushNet();			
		}
		MsgTimeOut=-10;
		ConnectStatus=6;
		break;
	case 6:
		if(!WaitingAfterLogin.Empty()){
			UNetConnection* Connection=WaitingAfterLogin.Front();
			WaitingAfterLogin.PopFront();
			if(OpenConnections.Find(Connection)!=0){
				ServerPackageMap->FixLinkers (Connection->PackageMap);

				SendDownloadManagers (Connection);
				Connection->SendPackageMap(0);
				ServerPackageMap->PostFixLinkers (Connection->PackageMap);
				Connection->Logf( TEXT("WELCOME LEVEL=%s GAME=%s LONE=%i"), ServerMap, GameType, 0 );
				Connection->FlushNet();
			}
		}
		break;
	default:
		break;
	}

	unclock (ConnectTick);
	UTVStats->Add (10, ConnectTick);

	INT DelayerTick = 0;
	clock (DelayerTick);

	Delayer->Tick(DeltaSeconds);
	mDelayer.Tick (DeltaSeconds);
	vDelayer.Tick (DeltaSeconds);
	
	unclock (DelayerTick);
	UTVStats->Add (11, DelayerTick);

	INT NetTick = 0;
	clock (NetTick);

	if (ListenDriver) {
		ListenDriver->TickDispatch (DeltaSeconds);
		ListenDriver->TickFlush();

		INT ConTick = 0;
		clock (ConTick);

		for( INT i=0; i<ListenDriver->ClientConnections.Num(); i++ ){
			OpenConnections.Find(ListenDriver->ClientConnections(i))->doRemove=false;
		}

		for(TMap<UNetConnection*,OpenConnection>::TIterator oi(OpenConnections);oi;++oi){
			OpenConnection* oc=&oi.Value();
			UNetConnection* conn=oi.Key();
			if(oc->doRemove){
				if(conn==PrimaryConnection){
					debugf (TEXT ("Lost primary connection"));
					PrimaryConnection=0;
					if(ConnectStatus==6){
						if(DoRestartIn<0)
							DoRestartIn=Delayer->DelayTime+1;
					} else {
						DoRestart=true;
						return;
					}
				}
				if(oc->loggedIn)
					LoggedInPlayers--;
				oi.RemoveCurrent();
				continue;
			}
			oc->doRemove=true;

			for(TMap<INT,INT>::TIterator wi(oc->waitingChannels);wi;++wi){
				int channel=wi.Key();
				if(conn->Channels[channel]==0){
//					new(OpenConnections.Find(conn)->ChannelHistory[channel]) FString("Waiting channel activated");
					Delayer->OpenChannel(conn,channel,true);
					wi.RemoveCurrent();
//					new(OpenConnections.Find(conn)->ChannelHistory[channel]) FString("Waiting channel packets sent");
//					wi=oc->waitingChannels.erase(wi);
//					if(wi==oc->waitingChannels.end())
//						break;
				}
			}
			for(TMap<int,OpenConnection::StalledChannel>::TIterator si(oc->StalledChannels);si;++si){
				while(!si.Value().bunches.Empty() && SendBunch(conn,&si.Value().bunches.Front(),true)){
					si.Value().bunches.PopFront();
				}
				if(si.Value().bunches.Empty()){
		//			debugf(TEXT("Stalled channel %i unstalled"),si->first);
					si.RemoveCurrent();
//					si=oc->StalledChannels.erase(si);
//					if(si==oc->StalledChannels.end())
//						break;
				}
			}
		}
		for( INT i=0; i<ListenDriver->ClientConnections.Num(); i++ ) {	
			//Det h�r g�r att en filtransfer g�r i maxfart.. kanske man ska �ndra
			/*guard(FileTransfer);
			if (ListenDriver->ClientConnections(i)->OpenChannels.Num() == 2) {
				//			printf ("running file transfer fix\n");
				//ListenDriver->ClientConnections(i)->Tick ();
				ListenDriver->ClientConnections(i)->QueuedBytes = 0;
			}
			unguard; */
			if(ListenDriver->ClientConnections(i)->Out.GetNumBits() && (SentClientData || ListenDriver->ClientConnections(i)==PrimaryConnection))
				ListenDriver->ClientConnections(i)->FlushNet();
		}

		unclock (ConTick);
		UTVStats->Add (13, ConTick);

		SentClientData=false;
		UTVStats->Add (5, ListenDriver->RecvCycles);
		UTVStats->Add (6, ListenDriver->SendCycles);
	}

	if (ConnectDriver) {
		ConnectDriver->TickDispatch (DeltaSeconds);

		//Need to tick the download
		if (ConnectDriver->ServerConnection)
			if (ConnectDriver->ServerConnection->Download)
				ConnectDriver->ServerConnection->Download->Tick ();

		if(ConnectDriver->ServerConnection->Out.GetNumBits())
			ConnectDriver->ServerConnection->FlushNet();
		ConnectDriver->TickFlush();
		UTVStats->Add (7, ConnectDriver->RecvCycles);
		UTVStats->Add (8, ConnectDriver->SendCycles);
	}

	unclock (NetTick);
	UTVStats->Add (12, NetTick);

	MsgTimeOut+=DeltaSeconds;

	if((MsgTimeOut>10 && DoRestartIn<0 && ConnectStatus==6 && !ConnectedToUtvProxy && GameEnded) ||
		 (MsgTimeOut>60 && DoRestartIn<0 && (ConnectedToUtvProxy || ConnectStatus!=6))){
		debugf (TEXT ("Server timeout, level restart?"));
		if(NoPrimary)
			DoRestartIn=10;
		else
			DoRestartIn=max(5,Delayer->DelayTime-10);
		if(ConnectStatus!=6 && !ConnectedToUtvProxy)
			TestedForUtvProxy=true;
	}

	if(DoRestartIn>0){
		DoRestartIn-=DeltaSeconds;
		if(DoRestartIn<=0){
			for( INT i=0; i<ListenDriver->ClientConnections.Num(); i++ ) {
				if(OpenConnections.Find(ListenDriver->ClientConnections(i))->utvProxy){
					ListenDriver->ClientConnections(i)->Logf(TEXT("UTV TOTALCLIENTS=%i DELAY=%.1f DORESTARTIN=%.1f"),TotalClients,Delayer->DelayTime,5.0);
					ListenDriver->ClientConnections(i)->FlushNet();
				}
			}

			DoRestart=true;
		}
	}

	LastTotalClientsUpdate+=DeltaSeconds;
	if(LastTotalClientsUpdate>10 && ConnectStatus==6){
		LastTotalClientsUpdate=0;
		int newTotal=0;

		for(TMap<UNetConnection*,OpenConnection>::TIterator oi(OpenConnections);oi;++oi){
			if(oi.Value().isReady)
				newTotal+=oi.Value().TotalSubClients;
		}
		if(ConnectedToUtvProxy){
			ConnectDriver->ServerConnection->Logf(TEXT("UTV TOTALCLIENTS=%i"),newTotal);
			ConnectDriver->ServerConnection->FlushNet();
		} else {
			TotalClients=newTotal;
			for( INT i=0; i<ListenDriver->ClientConnections.Num(); i++ ) {
				if(OpenConnections.Find(ListenDriver->ClientConnections(i))->utvProxy){
					ListenDriver->ClientConnections(i)->Logf(TEXT("UTV TOTALCLIENTS=%i DELAY=%.1f DORESTARTIN=%.1f"),TotalClients,Delayer->DelayTime,DoRestartIn);
					ListenDriver->ClientConnections(i)->FlushNet();
				} else {
					SendStatusToClient (ListenDriver->ClientConnections(i));
				}
			}
			if(PrimaryConnection){
				TCHAR ch[1024];
				appSprintf(ch,TEXT("7 %s %i %i %s %s %s %s %f %i"),*ServerAdress,ServerPort,ListenPort,*JoinPassword,*PrimaryPassword,*VipPassword,*NormalPassword,Delayer->DelayTime,MaxClients);
				FString s=ch;

				SendMessageToClient(PrimaryConnection,s);
//				debugf(TEXT("Primary info %s"),*s);
			}
		}
	}

	if (uplink)
		uplink->Poll (0);

	unclock(GameCycles);
	unclock(LocalTickCycles);
	TickCycles=LocalTickCycles;
	GTicks++;

	unguard;
	unguard;
}


void UReplicatorEngine::Draw( UViewport* Viewport, UBOOL Blit, BYTE* HitData, INT* HitSize )
{
}

UBOOL UReplicatorEngine::Exec( const TCHAR* Cmd, FOutputDevice& Ar )
{
	return false;
}

void UReplicatorEngine::MouseDelta( UViewport*, DWORD, FLOAT, FLOAT )
{
}

void UReplicatorEngine::MousePosition( class UViewport*, DWORD, FLOAT, FLOAT )
{
}

void UReplicatorEngine::MouseWheel( UViewport* Viewport, DWORD Buttons, INT Delta )
{
}

void UReplicatorEngine::Click( UViewport*, DWORD, FLOAT, FLOAT )
{
}

void UReplicatorEngine::UnClick( UViewport*, DWORD, INT, INT )
{
}

void UReplicatorEngine::SetClientTravel( UPlayer* Viewport, const TCHAR* NextURL, UBOOL bItems, ETravelType TravelType )
{
}

FLOAT UReplicatorEngine::GetMaxTickRate()
{
	if ((tickRate < 5) || (tickRate > 200))
		return 30;
	else
		return tickRate;
}

EAcceptConnection UReplicatorEngine::NotifyAcceptingConnection()
{
	//Ignore if we are currently testing if we are a chained proxy
	if(ConnectStatus!=6 && !TestedForUtvProxy && ServerAdress!="" && !PrimaryConnection && !NoPrimary)
		return ACCEPTC_Ignore;

	//Ignore connections if we are about to restart
	if(DoRestartIn>0)
		return ACCEPTC_Ignore;

	//Stop clients from rejoining to early at map restart
	if(GameEnded || (MsgTimeOut>10 && ConnectStatus==6))
		return ACCEPTC_Ignore;

	return ACCEPTC_Accept;
}

void UReplicatorEngine::NotifyAcceptedConnection( class UNetConnection* Connection )
{
	guard(UReplicatorEngine::NotifyAcceptedConnection);
	OpenConnection oc;
	oc.isReady=false;
	oc.utvProxy=false;
	oc.loggedIn=false;
	oc.numErrs=0;
	oc.primaryConnection=false;
	oc.vipConnection=false;
	oc.TotalSubClients=1;
	oc.doRemove=false;
	OpenConnections.Set(Connection,oc);
	debugf(TEXT("New connection: %i"),(int)Connection);
	unguard;
}

UBOOL UReplicatorEngine::NotifyAcceptingChannel( class UChannel* Channel )
{
	return true;
}

ULevel* UReplicatorEngine::NotifyGetLevel() 
{
	return NULL;
}

//fix?
INT UReplicatorEngine::ChallengeResponse( INT Challenge )
{
	guard(UGameEngine::ChallengeResponse);
	return (Challenge*237) ^ (0x93fe92Ce) ^ (Challenge>>16) ^ (Challenge<<16);
	unguard;
}

void UReplicatorEngine::NotifyReceivedText( UNetConnection* Connection, const TCHAR* Text )
{
	guard (UReplicatorEngine::NotifyReceivedText);

	if(Connection->Driver->ServerConnection){
		MsgTimeOut=0;

		FString s=Text;
		if ((s.InStr(TEXT("USES"))==-1) && (s.InStr(TEXT("DLMGR"))==-1))
			debugf(TEXT("Server sends: %s"),Text);

		if( ParseCommand( &Text, TEXT("CHALLENGE") ) )
		{
			// Challenged by server.
			//INT RemoteStats = 0;
			Parse( Text, TEXT("VER="), Connection->NegotiatedVer );
			Parse( Text, TEXT("CHALLENGE="), Connection->Challenge );
			Parse( Text, TEXT("STATS="), needStats );

			if(PrimaryConnection){
				if(needStats && PrimaryConnection->EncStatsUsername=="")
					Connection->Logf( TEXT("AUTH HASH=%s USERNAME=%s PASSWORD=%s"), *PrimaryConnection->CDKeyHash,TEXT("UtvTempClient"), TEXT("123456789012"));
				else
					Connection->Logf( TEXT("AUTH HASH=%s USERNAME=%s PASSWORD=%s"), *PrimaryConnection->CDKeyHash,*PrimaryConnection->EncStatsUsername, *PrimaryConnection->EncStatsPassword);
				Connection->Logf( TEXT("NETSPEED %i"), PrimaryConnection->CurrentNetSpeed );
				//Connection->Logf( TEXT("NETSPEED %i"), 5000 );
			} else {
				if(needStats)
					Connection->Logf( TEXT("AUTH HASH=%s USERNAME=%s PASSWORD=%s"), *GetCDKeyHash(),TEXT("UtvTempClient"), TEXT("123456789012"));
				else
					Connection->Logf( TEXT("AUTH HASH=%s USERNAME= PASSWORD="), *GetCDKeyHash() );
				Connection->Logf( TEXT("NETSPEED %i"), 5000 );
			}

			Connection->Logf( TEXT("UTVPROXY SEEALL=%i"),SeeAll );

			FString s=TEXT("Index.ut2?SpectatorOnly=True?Name=UTV2004");
			if(JoinPassword!=""){
				s+="?password=";
				s+=JoinPassword;
			}
			if(SeeAll){
				s+=TEXT("?UTVSeeAll=true");
			}
			Connection->Logf( TEXT("LOGIN RESPONSE=%i URL=%s"), ChallengeResponse(Connection->Challenge), *s);
			Connection->FlushNet();

			//Remove old download manager from the connection (if any)
			Connection->DownloadInfo.Empty ();
			RetryManagers.Empty ();

			//Add our preferred download managers:
			for (INT i = 0; i < DownloadManagers.Num(); ++i) 
				NotifyReceivedText (Connection, *DownloadManagers(i));			

			ServerDLManagers.Empty ();
		}
		else if (ParseCommand (&Text, TEXT ("DLMGR"))) {

			INT i = Connection->DownloadInfo.AddZeroed();
			Parse( Text, TEXT("CLASS="), Connection->DownloadInfo(i).ClassName );
			Parse( Text, TEXT("PARAMS="), Connection->DownloadInfo(i).Params );
			ParseUBOOL( Text, TEXT("COMPRESSION="), Connection->DownloadInfo(i).Compression );

			debugf (TEXT ("Adding dlmanager: %s"), *Connection->DownloadInfo(i).ClassName);
			debugf (TEXT ("Dlmanager params: %s"), *Connection->DownloadInfo(i).Params);

			//add it to our list of serverdlmanagers if it is a http redirect
			//if (appStrfind (*Connection->DownloadInfo(i).ClassName, TEXT ("HTTP"))) {
				//INT j = ServerDLManagers.AddZeroed ();
				new(ServerDLManagers) FString(FString::Printf (TEXT ("DLMGR %s"), Text));
			//}

			Connection->DownloadInfo(i).Class = StaticLoadClass( UDownload::StaticClass(), NULL, *Connection->DownloadInfo(i).ClassName, NULL, LOAD_NoWarn | LOAD_Quiet, NULL );
			if( !Connection->DownloadInfo(i).Class )
				Connection->DownloadInfo.Remove(i);
		}
		else if( ParseCommand(&Text,TEXT("UTVPROXY")) )
		{
			ConnectedToUtvProxy=true;
			Delayer->DelayTime=0;
			Parse( Text, TEXT("SEEALL="), SeeAll );
		}
		else if( ParseCommand(&Text,TEXT("UTV")) )
		{
			Parse( Text, TEXT("TOTALCLIENTS="), TotalClients );
			Parse( Text, TEXT("DELAY="), TotalDelay );
			Parse( Text, TEXT("DORESTARTIN="), DoRestartIn );
			for( INT i=0; i<ListenDriver->ClientConnections.Num(); i++ ) {
				if(OpenConnections.Find(ListenDriver->ClientConnections(i))->utvProxy){
					ListenDriver->ClientConnections(i)->Logf(TEXT("UTV TOTALCLIENTS=%i DELAY=%.1f DORESTARTIN=%.1f"),TotalClients,TotalDelay,DoRestartIn);
					ListenDriver->ClientConnections(i)->FlushNet();
				} else {
					if(OpenConnections.Find(ListenDriver->ClientConnections(i))->isReady)
						SendStatusToClient (ListenDriver->ClientConnections(i));
				}
			}
		}
		else if( ParseCommand( &Text, TEXT("USES") ) )
		{
			// Dependency information.

			FString s=Text;
			FPackageInfo& Info = *new(ServerPackageMap->List)FPackageInfo(NULL);
			TCHAR PackageName[NAME_SIZE]=TEXT("");
			Parse( Text, TEXT("GUID=" ), Info.Guid );
			Parse( Text, TEXT("GEN=" ),  Info.RemoteGeneration );
			//Parse( Text, TEXT("GEN=" ),  Info.LocalGeneration );
			Parse( Text, TEXT("SIZE="),  Info.FileSize );
			Parse( Text, TEXT("FLAGS="), Info.PackageFlags );
			Parse( Text, TEXT("PKG="), PackageName, ARRAY_COUNT(PackageName) );
			Parse( Text, TEXT("FNAME="), Info.URL );
			//Info.RemoteGeneration = Info.LocalGeneration;

			guard(CreateUsesPackage);
			Info.Parent = CreatePackage(NULL,PackageName);

			unguardf((TEXT("PackageName=(%s) Text=(%s)"),PackageName,Text));
		}
		else if( ParseCommand( &Text, TEXT("FAILCODE") ) || ParseCommand(&Text, TEXT("BRAWL")) || ParseCommand(&Text, TEXT("FAILURE")))
		{
			//Send the same failure message to the primary so he knows what is going on
			if(PrimaryConnection){
				debugf (TEXT("Failed login as primary (%s)"), *s);
				PrimaryConnection->Logf( TEXT("%s"),*s);
				PrimaryConnection->FlushNet();
			}
			TestedForUtvProxy=true;
			DoRestartIn=0.01f;
			debugf (TEXT ("Connection failed"));
			return;
		}
		else if( ParseCommand( &Text, TEXT("UPGRADE") ))
		{
			if(PrimaryConnection){
				PrimaryConnection->Logf( TEXT("UPGRADE %s"),Text);
				PrimaryConnection->FlushNet();
			}
			TestedForUtvProxy=true;
			DoRestartIn=0.01f;
			debugf(TEXT("Server sends upgrade %s\n"),Text);
			return;
		}
		else if( ParseCommand( &Text, TEXT("WELCOME") ) )
		{
			if(!PrimaryConnection && !ConnectedToUtvProxy){
				if(NoPrimary){
					debugf (TEXT ("None utv server, continuing without primary connection"));
				} else {
					TestedForUtvProxy=true;
					Connection->Channels[0]->Close();
					Connection->FlushNet();
					DoRestartIn=0.01f;
					debugf (TEXT ("None utv server, waiting for primary client"));
					return;
				}
			}
			//Trick clients into loading an additional packet
			if(!ConnectedToUtvProxy){
				FString S = FString::Printf(TEXT("USES GUID=%s PKG=") UTV2004C TEXT(" FLAGS=1 SIZE=%i GEN=1 FNAME=") UTV2004C TEXT(".u"),
					ClientPackage->Summary.Guid.String(), GFileManager->FileSize (*ClientPackage->Filename));
				NotifyReceivedText (Connection, *S);
			}

			// Parse welcome message.
			Parse( Text, TEXT("LEVEL="), ServerMap ,ARRAY_COUNT(ServerMap));
			Parse( Text, TEXT("GAME="), GameType ,ARRAY_COUNT(ServerMap));
			//ParseUBOOL( Text, TEXT("LONE="), LonePlayer );
			Parse( Text, TEXT("CHALLENGE="), Connection->Challenge );

			//Connection->PackageMap->CopyLinkers (ServerPackageMap);
			//Connection->PackageMap->List = ServerPackageMap->List;
			ServerPackageMap->FixLinkers (Connection->PackageMap);

			//Fix the packagemap
			if (!ServerPackageMap->CreateLinkers ()) {
				//This function will lead to a received-notify eventually, both on success and failure
				Connection->ReceiveFile (ServerPackageMap->NeededPackage (),0);//todo: something other as attempt?
			}
			else {
				ConnectStatus=3;
				ServerConnReady=true;
			}
		}
	} else {
		FString s=Text;
		if((PrimaryConnection==0 && ConnectStatus!=6) || Connection==PrimaryConnection)
			if(s.InStr(TEXT("REPEAT"))==-1 && s.InStr(TEXT("HAVE"))==-1)
				debugf(TEXT("Client sends: %s"),Text);

		if( ParseCommand(&Text,TEXT("HELLO")) )
		{
			// Versions.
			INT RemoteMinVer=219, RemoteVer=219;
			Parse( Text, TEXT("MINVER="), RemoteMinVer );
			Parse( Text, TEXT("VER="),    RemoteVer    );

			if( RemoteVer<ENGINE_MIN_NET_VERSION || RemoteMinVer>ENGINE_VERSION )
			{
				Connection->Logf( TEXT("UPGRADE MINVER=%i VER=%i"), ENGINE_MIN_NET_VERSION, ENGINE_VERSION );
				Connection->FlushNet();
				Connection->State = USOCK_Closed;
				return;
			}
			Connection->NegotiatedVer = Min(RemoteVer,ENGINE_VERSION);

			Connection->Challenge = appCycles();
			Connection->Logf( TEXT("CHALLENGE VER=%i CHALLENGE=%i STATS=%i"), Connection->NegotiatedVer, Connection->Challenge, needStats );
			Connection->FlushNet();
		}
		else if( ParseCommand(&Text,TEXT("UTVPROXY")) )
		{
			OpenConnections.Find(Connection)->utvProxy=true;
			OpenConnections.Find(Connection)->TotalSubClients=0;
			Connection->Logf( TEXT("UTVPROXY SEEALL=%i"),SeeAll );
			Connection->FlushNet();			
		}
		else if( ParseCommand(&Text,TEXT("UTV")) )
		{
			Parse( Text, TEXT("TOTALCLIENTS="), OpenConnections.Find(Connection)->TotalSubClients );
		}
		else if( ParseCommand(&Text,TEXT("AUTH")) )
		{
			Parse( Text, TEXT("HASH="), Connection->CDKeyHash );
			Parse( Text, TEXT("USERNAME="), Connection->EncStatsUsername );
			Parse( Text, TEXT("PASSWORD="), Connection->EncStatsPassword );
		}
		else if( ParseCommand(&Text,TEXT("NETSPEED")) )
		{
			INT Rate = appAtoi(Text);
			if( Rate>=500 )
				Connection->CurrentNetSpeed = Clamp( Rate, 1500, ListenDriver->MaxClientRate );
			if(Connection==PrimaryConnection)
				debugf( TEXT("Primary netspeed is %i"), Connection->CurrentNetSpeed );
		}
		else if( ParseCommand(&Text,TEXT("LOGIN")) )
		{
			INT Response=0;
			if(	!Parse(Text,TEXT("RESPONSE="),Response) || ChallengeResponse(Connection->Challenge)!=Response )
			{
				debugf( NAME_DevNet, TEXT("Client %s failed CHALLENGE."), *Connection->LowLevelGetRemoteAddress() );
				Connection->Logf( TEXT("FAILURE CHALLENGE") );
				Connection->FlushNet();
				Connection->State = USOCK_Closed;
				return;
			}
			TCHAR Str[1024]=TEXT("");
			FString Error, FailCode;
			Parse( Text, TEXT("URL="), Str, ARRAY_COUNT(Str) );
			Connection->RequestURL = Str;

			FString s=Connection->RequestURL;
			FString Password;
			if(s.InStr(TEXT("password="))!=-1){
				Password=s.Mid(9+s.InStr("password="));
				if(Password.InStr("?")!=-1)
					Password=Password.Left(Password.InStr("?"));
			}

			//Determine what kind of connection this is (primary, vip, normal)
			int ctype=0;

			if(!PrimaryConnection && DoRestartIn<0 && !OpenConnections.Find(Connection)->utvProxy && !ConnectedToUtvProxy && (PrimaryPassword=="" || PrimaryPassword==Password) && ConnectStatus!=6){
				debugf (TEXT ("Got primary client"));
				PrimaryConnection=Connection;
				OpenConnections.Find(Connection)->primaryConnection=true;
				OpenConnections.Find(Connection)->vipConnection=true;
				ctype = 1;
			}
			else {
				//If vip password is not set, do not use this
				if ((VipPassword != "") && (VipPassword == Password)) {
					debugf (TEXT ("Got VIP client"));
					OpenConnections.Find(Connection)->vipConnection=true;
					ctype = 2;
				}
				else {
					if((PrimaryPassword=="" || PrimaryPassword==Password) || (NormalPassword=="" || NormalPassword==Password)){
						ctype = 3;
						debugf (TEXT ("Got normal client"));
					}
					else {
						//The client did not know any of the passwords
						debugf (TEXT ("Rejected client due to bad password"));
						if(Password=="")
							Connection->Logf( TEXT("FAILCODE NEEDPW"));
						else
							Connection->Logf( TEXT("FAILCODE WRONGPW"));
						Connection->FlushNet();
						Connection->State = USOCK_Closed;
						return;
					}
				}
			}

			//Now the type of client is determined, make sure the proxy has room for this one
			//This check only affects normal clients, primary and vip are always allowed
			if ((MaxClients > 0) && (MaxClients <= LoggedInPlayers) && (ctype > 2)) {
				Connection->Logf( TEXT("FAILURE UTV Server is already at capacity."));
				Connection->FlushNet();
				Connection->State = USOCK_Closed;
				debugf (TEXT ("Rejected normal client due to connection limit reached"));
				return;
			}

			//Now do appropriate processing
			if (ConnectStatus==6) {
				ServerPackageMap->FixLinkers (Connection->PackageMap);
				SendDownloadManagers (Connection);
				Connection->SendPackageMap(0);
				ServerPackageMap->PostFixLinkers (Connection->PackageMap);
				Connection->Logf( TEXT("WELCOME LEVEL=%s GAME=%s LONE=%i"), ServerMap, GameType, 0 );
				Connection->FlushNet();
			} else if(Connection==PrimaryConnection){
				ParseCmdLine(Str);
				ConnectStatus=1;
			} else {
				WaitingAfterLogin.PushBack(Connection);
			}

			OpenConnections.Find(Connection)->loggedIn=true;
			LoggedInPlayers++;
			debugf (TEXT ("New connection opened %i/%i") ,LoggedInPlayers,MaxClients);
		}
		else if( ParseCommand(&Text,TEXT("JOIN")) )
		{
			debugf (TEXT ("New client ready"));
			OpenConnections.Find(Connection)->isReady=true;

			if(PrimaryConnection==Connection){
				ConnectStatus=5;
			} else {
				Delayer->NewConnection(Connection);
			}

		}
		else if( ParseCommand(&Text,TEXT("PETE")) )
		{
			if(Connection==PrimaryConnection)
			{
				INT PktCnt, PkgCnt;
				Parse(Text,TEXT("PKT="),PktCnt);
				Parse(Text,TEXT("PKG="),PkgCnt);
				Connection->CurrentPackageCount=0;
				Connection->ExpectedPackageCount=PktCnt;
				Connection->FinalPackageCount=PkgCnt;

				ConnectDriver->ServerConnection->Logf(TEXT("PETE PKT=%i PKG=%i"),PktCnt,PkgCnt);
				ConnectDriver->ServerConnection->FlushNet();

			}
		}
		else if( ParseCommand(&Text,TEXT("REPEAT")) )
		{
			if(Connection==PrimaryConnection && Connection->CurrentPackageCount!=Connection->ExpectedPackageCount){
				Connection->CurrentPackageCount++;
				ConnectDriver->ServerConnection->Logf(TEXT("REPEAT %s"),Text);
				ConnectDriver->ServerConnection->FlushNet();
			}
		}
		//not used any more ?
/*		else if( ParseCommand(&Text,TEXT("HAVE")) )
		{
			if(Connection==PrimaryConnection){
				ConnectDriver->ServerConnection->Logf(TEXT("HAVE %s"),Text);
				ConnectDriver->ServerConnection->FlushNet();
				FGuid Guid(0,0,0,0);
				Parse( Text, TEXT("GUID=" ), Guid );
				int gen;
				Parse( Text, TEXT("GEN=" ),gen  );
				for( TArray<FPackageInfo>::TIterator It(ServerPackageMap->List); It; ++It )
					if( It->Guid==Guid ){
						//debugf(TEXT("Primary sends HAVE %s %i %i"),Text,gen,It->RemoteGeneration);
						//Only update if lower since it's only used then
						if (gen < It->RemoteGeneration)
							It->RemoteGeneration=gen;
					}
			} else if(Connection->State != USOCK_Closed){
				//			debugf (TEXT ("Checking for old client"));

				FGuid Guid(0,0,0,0);
				Parse( Text, TEXT("GUID=" ), Guid );
				int gen;
				Parse( Text, TEXT("GEN=" ),gen  );
				for( TArray<FPackageInfo>::TIterator It(ServerPackageMap->List); It; ++It ){
					if( It->Guid==Guid ){
						if (gen < It->RemoteGeneration) {
							Connection->Logf( TEXT("UPGRADE"));
							Connection->FlushNet();
							Connection->State = USOCK_Closed;
							debugf (TEXT ("Old client detected! Packet %s Our version %i Client version %i"),*It->URL,It->RemoteGeneration,gen);
							break;
						}
					}
				}
			}
		}
*/		else if ( ParseCommand(&Text,TEXT("OPENVOICE")) )
		{
			//			UGameEngine* GameEngine                     = CastChecked<UGameEngine>( Engine );
			//			AVoiceChatReplicationInfo* VoiceChatManager = CastChecked<AVoiceChatReplicationInfo>( AVoiceChatReplicationInfo::StaticClass()->GetDefaultObject() );	

			if(/* VoiceChatManager->bEnableVoiceChat && */(appStrcmp(Text,TEXT("")) != 0) )
			{
				//				static UBOOL			LanPlay			= ParseParam(appCmdLine(),TEXT("LANPLAY"));
				EVoiceCodec				VoiceCodec		= CODEC_48NB;//(EVoiceCodec) appAtoi(Text);
				/*			DWORD					AllowedCodecs	= 0;
				const TArray<FString>&	Codecs			= LanPlay ? VoiceChatManager->VoIPLANCodecs : VoiceChatManager->VoIPInternetCodecs;

				// Find codec to use.
				for( INT i=0; i<Codecs.Num(); i++ )
				{
				if( appStricmp(*Codecs(i),TEXT("CODEC_48NB")) == 0 )
				AllowedCodecs |= CODEC_48NB;
				if( appStricmp(*Codecs(i),TEXT("CODEC_96WB")) == 0 )
				AllowedCodecs |= CODEC_96WB;
				}

				// Use 4.8kbs narrow band as safety fallback codec.
				if( !(VoiceCodec & AllowedCodecs) )
				VoiceCodec = CODEC_48NB;
				*/
				// Create and set up voice channel.
				/*				if( Connection->Actor && Connection->Actor->PlayerReplicationInfo )
				{
				INT PlayerID = Connection->Actor->PlayerReplicationInfo->PlayerID;
				check(PlayerID);
				debugf(TEXT("OPENVOICE for PlayerID: %i with codec: %i"), PlayerID, VoiceCodec );
				*/				
				// See whether this player already has a voice channel.
				UVoiceChannel* VoiceChannel = Connection->VoiceChannel;

				// Create channel for client if there isn't already one.
				if( !VoiceChannel )
				{
					UVoiceChannel* VoiceChannel = CastChecked<UVoiceChannel>(Connection->CreateChannel( CHTYPE_Voice, 1, UNetConnection::MAX_CHANNELS-2));

					// Set server options.
					DWORD ServerOptions = 0;
					//						if( VoiceChatManager->VoIPAllowSpatialization )
					//							ServerOptions |= VOICE_AllowSpatialization;
					//						if( GameEngine->VoIPAllowVAD )
					//							ServerOptions |= VOICE_AllowVAD;

					// Find free voice slot.
					INT i;
					for( i=1; i<VOICE_MAX_CHATTERS; i++ )
						if( Connection->Driver->VoiceInfos[i] == NULL )
							break;

					if(i>=VOICE_MAX_CHATTERS){
						return;
					}

					Connection->Driver->VoiceInfos[i] = new FVoiceInfo;

					Connection->Driver->VoiceInfos[i]->VoiceSound		= NULL;
					Connection->Driver->VoiceInfos[i]->VoiceCodec		= VoiceCodec;
					Connection->Driver->VoiceInfos[i]->PlayerID			= 1;//PlayerID;
					Connection->Driver->VoiceInfos[i]->PacketSize		= UAudioSubsystem::CodecPacketSize[CODEC_TO_INDEX(VoiceCodec)];
					Connection->Driver->VoiceInfos[i]->ServerOptions	= ServerOptions;
					Connection->Driver->VoiceInfos[i]->PacketData		= new BYTE[Connection->Driver->VoiceInfos[i]->PacketSize];
					Connection->Driver->VoiceInfos[i]->VoiceIndex		= i;

					VoiceChannel->SendStatusPacket( Connection->Driver->VoiceInfos[i], VOICEPACKET_Initialize );
					VoiceChannel->VoiceIndex = i;

					debugf(TEXT("Opening voice channel %i"),i);
					//			}
				}
			}
		}
	}
	unguard;
}

void UReplicatorEngine::SendDownloadManagers (UNetConnection* Connection)
{
	//Send the ones we have defined locally
	for (INT i = 0; i < DownloadManagers.Num(); ++i) 
		Connection->Logf ( *DownloadManagers (i));

	//And the ones that the real server (or the upstream proxy) told us about
	for (INT i = 0; i < ServerDLManagers.Num (); ++i) {
		Connection->Logf ( *ServerDLManagers (i));
	}

	Connection->Driver->DownloadManagers.Empty();
}

void UReplicatorEngine::NotifyReceivedFile( UNetConnection* Connection, INT PackageIndex, const TCHAR* Error, UBOOL Skipped, INT attempt)
{
	UBOOL abort = false;

	//When failing to receive something we wanted, always abort
	if (*Error) {
		if ((ServerPackageMap->List.IsValidIndex (ServerPackageMap->NeededPackage()) && Connection->DownloadInfo.Num () > 1)) {

			// Try with the next download method.
			debugf (TEXT ("Downloadmanager failed, trying next one"));

			INT i = RetryManagers.AddZeroed ();
			RetryManagers(i).ClassName = Connection->DownloadInfo(0).ClassName;
			RetryManagers(i).Params = Connection->DownloadInfo(0).Params;
			RetryManagers(i).Compression = Connection->DownloadInfo(0).Compression;
			RetryManagers(i).Class = Connection->DownloadInfo(0).Class;

			Connection->DownloadInfo.Remove(0);
			Connection->ReceiveFile (ServerPackageMap->NeededPackage (),0);//todo: something other as attempt ?
		}
		else {
			debugf (TEXT ("Error when receiving a file, aborting.."));
			abort = true;
		}
	}
	else {
		//Check that it was the file we wanted
		if (PackageIndex != ServerPackageMap->NeededPackage ()) {
			debugf (TEXT ("Received unwanted file (%i - %i), aborting.."), PackageIndex, ServerPackageMap->NeededPackage ());
			abort = true;
		}
		else {
			//Perhaps we can try to load now!			
			if (!ServerPackageMap->CreateLinkers ()) {
				//One more file needed, continue with this..
				debugf (TEXT ("File downloaded, continuing with next needed file"));

				//Maybe we had to remove some downloadmanager.. if so readd them
				while (RetryManagers.Num () > 0) {
					INT i = RetryManagers.Num () - 1;

					Connection->DownloadInfo.InsertZeroed (0, 1);
                    Connection->DownloadInfo(0).ClassName = RetryManagers(i).ClassName;
					Connection->DownloadInfo(0).Params = RetryManagers(i).Params;
					Connection->DownloadInfo(0).Compression = RetryManagers(i).Compression;
					Connection->DownloadInfo(0).Class = RetryManagers(i).Class;

					RetryManagers.Remove (i);
				}

				Connection->ReceiveFile (ServerPackageMap->NeededPackage (),0);//todo: something other as attempt?
				return;
			}
			else {
	
				//This seems to be needed, strange things happen otherwise
				//DoRestart=true;
				ConnectStatus=3;
				ServerConnReady=true;
			}
		}
	}

	if (abort) {
		DoRestart = true;
	}
}

UBOOL UReplicatorEngine::NotifySendingFile( UNetConnection* Connection, FGuid GUID )
{
//	debugf (TEXT ("Allowing file"));
	return true;
}

void UReplicatorEngine::NotifyProgress(  const TCHAR* CmdStr, const TCHAR* Str1, const TCHAR* Str2, FLOAT Seconds )
{
	//Only think this is called when we are downloading things
	//cmdstr = ""
	//str1 = Receiving 'UTSecure205' (F10 Cancels)		/  Success
	//str2 = Size 47K, Complete 27.9%					/  Received 'UTSecure205'
	//Seconds = 0 (?)

	//Don't show first line since http-download will output stuff after that one
	if (appStrfind (Str2, TEXT ("0.0%")) || appStrfind (Str2, TEXT ("1.$"))) {
		return;
	}

	//Warning: very primitive :)
	if (Str1[0] == 'R')
		printf ("\015%S", Str2);
	else
		printf ("\nDownloading finished\n");
}

void UReplicatorEngine::OpenChannel(UNetConnection* Connection,FInBunch* Bunch)
{
//	if(Connection==PrimaryConnection){
//		debugf(TEXT("Open %i %i %i"),Bunch->bOpen,Bunch->bClose,Bunch->bReliable);
//	}
	if(Connection->Channels[Bunch->ChIndex]){
		if(Bunch->bClose){
			if(Bunch->IsError()){
				debugf (TEXT ("Erroneous bunch in remapped open"));
				return;
			}
			for(int a=UNetConnection::MAX_CHANNELS-2;a<UNetConnection::MAX_CHANNELS-20;--a){
				if(!Connection->Channels[a]){
					//printf("Remapping temporary channel %i to %i\n",Bunch->ChIndex,a);
					Connection->CreateChannel(CHTYPE_Actor,true,a);  // Bunch->ChType
					INT OrgIndex=Bunch->ChIndex;
					Bunch->ChIndex=a;
					SendBunch(Connection,Bunch);
					Bunch->ChIndex=OrgIndex;
					return;
				}
			}
		}
		((UtvChannel*)Connection->Channels[Bunch->ChIndex])->UtvClose();
		if(Connection==PrimaryConnection){
//			printf("Opening open channel %i\n",Bunch->ChIndex);
		}
		if(Connection->Channels[Bunch->ChIndex]->OpenTemporary)
			delete Connection->Channels[Bunch->ChIndex];

		OpenConnections.Find(Connection)->waitingChannels.Set(Bunch->ChIndex,0);
		return;
	} 
	Connection->CreateChannel(CHTYPE_Actor/*Bunch->ChType*/,true,Bunch->ChIndex);
//	new(OpenConnections.Find(Connection)->ChannelHistory[Bunch->ChIndex]) FString("Channel opened");
}

void UReplicatorEngine::CreateFakeActor (UNetConnection* Connection)
{
	guard(UReplicatorEngine::CreateFakeActor);

	if(Connection->Channels[UNetConnection::MAX_CHANNELS-1])
		return;

	UChannel* ch = Connection->CreateChannel (CHTYPE_Actor, true, UNetConnection::MAX_CHANNELS-1);

	FOutBunch ob(ch, false);
	ob.bOpen = true;
	ob.bClose = false;
	ob.bReliable = true;

	BYTE B = 0;
	DWORD Index = ServerPackageMap->GetReplicationId ();
	ob.SerializeBits (&B, 1);
	ob.SerializeInt( Index, ServerPackageMap->GetMaxObjectIndex () );

	SendBunch (Connection, &ob);

	FString s2;
	if(NoPrimary)
		s2=TEXT("2");
	else if(SeeAll)
		s2=TEXT("1");
	else
		s2=TEXT("0");

	//Tell it whether it is primary or not and if we are in see all mode
	if (Connection == PrimaryConnection)
		SendMessageToClient (Connection, FString (TEXT ("8 1 "))+s2);
	else
		SendMessageToClient (Connection, FString (TEXT ("8 2 "))+s2);

	unguard;
}

void UReplicatorEngine::ParseReplicationBunch (UNetConnection* Connection, FInBunch* tempBunch)
{
	guard (UReplicatorEngine::ParseReplicationBunch);

	INT index = tempBunch->ReadInt (ServerPackageMap->GetReplicationMax ());
	while (!tempBunch->IsError()) {
		tempBunch->ReadBit ();

		//A text message?
		if ((index == ServerPackageMap->GetReplicationSend ()) || (index == ServerPackageMap->GetReplicationGet ())) {

			FString msg;
			*tempBunch << msg;

			switch ((*msg)[0]) {
				case '1':	//Chatmessage

					if(OpenConnections.Find(Connection)!=0 && OpenConnections.Find(Connection)->utvProxy && IgnoreChainedChat)
						break;
					//Send the message to all clients on this proxy
					//ratecontrol the sender?
					debugf(TEXT("Utvsay: %s"),*msg);
					for (int i = 0; i < ListenDriver->ClientConnections.Num(); ++i) {
						UNetConnection* cc = ListenDriver->ClientConnections(i);
	
						if ((OpenConnections.Find(cc)->isReady) && (cc != Connection) )
							SendMessageToClient (cc, msg);
					}

					//If we are connected to a proxy, send the message upwards
					if (ConnectedToUtvProxy) {
						if (ConnectDriver->ServerConnection != Connection)
							SendMessageToClient (ConnectDriver->ServerConnection, msg);
					}

					break;
				case '2':	//Primary client position
					if(Connection==PrimaryConnection || Connection==ConnectDriver->ServerConnection){
						mDelayer.AddMove (msg);
					}
					break;
				case '3':	//Primary client game end indication
					if(Connection==PrimaryConnection || Connection==ConnectDriver->ServerConnection || NoPrimary){
						GameEnded=true;
						debugf (TEXT ("Game ended"));
					}
					break;
				case '5':	//Primary client settings
					if(Connection==PrimaryConnection){
						ParseCmdLine(*msg);
					}
					break;
				case '6':	//Primary client want restart
					if(Connection==PrimaryConnection){
						debugf (TEXT ("Primary sends reset"));
						DoRestartIn=1;
					}
					break;
			}
		}

		//or an actor message?
		if ((index == ServerPackageMap->GetReplicationSendTarget ()) || (index == ServerPackageMap->GetReplicationGetTarget ())) {
			DWORD Actor;

			BYTE b = 1;
			tempBunch->SerializeBits (&b, 1);
			tempBunch->SerializeInt (Actor, UNetConnection::MAX_CHANNELS);
			mDelayer.AddActor (Actor);
		}

		//Read the index for next iteration of the loop
		index = tempBunch->ReadInt (ServerPackageMap->GetReplicationMax ());
	}
	
	unguard;
}

void UReplicatorEngine::SendMessageToClient (UNetConnection* Connection, const FString& msg)
{
	UChannel* ch = Connection->Channels[UNetConnection::MAX_CHANNELS-1];

	if(Connection!=ConnectDriver->ServerConnection && !OpenConnections.Find(Connection)->isReady)
		return;

	if (!ch) {
		//debugf(TEXT("Retrying create fake actor"));
		//CreateFakeActor(Connection);
		return;
	}

	FOutBunch ob(ch, false);
	ob.bOpen = false;
	ob.bClose = false;
	ob.bReliable = true;

	UBOOL b = true;
	DWORD get = ServerPackageMap->GetReplicationGet();
	ob.SerializeInt( get, ServerPackageMap->GetReplicationMax () );
	ob.SerializeBits (&b, 1);

	FString m (msg);
	ob << m;

//	debugf (TEXT("Trying to send string (%s) to client"), *msg);
	SendBunch (Connection, &ob);
}

void UReplicatorEngine::SendActorToClient (UNetConnection* Connection, INT Actor)
{
	UChannel* ch = Connection->Channels[UNetConnection::MAX_CHANNELS-1];

	if (!ch) {
		//debugf(TEXT("Retrying create fake actor"));
		//CreateFakeActor(Connection);
		return;
	}

	FOutBunch ob(ch, false);
	ob.bOpen = false;
	ob.bClose = false;
	ob.bReliable = true;

	UBOOL b = true;
	DWORD get = ServerPackageMap->GetReplicationGetTarget();
	ob.SerializeInt( get, ServerPackageMap->GetReplicationMax () );
	ob.SerializeBits (&b, 1);

	BYTE B = 1;
	ob.SerializeBits (&B, 1);
	DWORD index = Actor;
	ob.SerializeInt (index, UNetConnection::MAX_CHANNELS);	
	SendBunch (Connection, &ob);
}

void UReplicatorEngine::SendStatusToClient (UNetConnection* Connection)
{
	FString msg = FString::Printf (TEXT ("4 %i %.0f %.0f"), TotalClients, TotalDelay+Delayer->DelayTime, DoRestartIn);
	SendMessageToClient (Connection, msg);	
}

bool UReplicatorEngine::SendBunch(UNetConnection* Connection,InternalBunch* IntBunch,bool fromQue)
{
	guard(UReplicatorEngine::SendBunch);
	FInBunch* Bunch=IntBunch->bunch;
	OpenConnection* oc=OpenConnections.Find(Connection);

	if(Connection->Channels[Bunch->ChIndex]==0){
		return true;
	}

	if(Connection->Channels[Bunch->ChIndex]->Closing){
		if(Connection->Channels[Bunch->ChIndex]->OpenTemporary){
			delete Connection->Channels[Bunch->ChIndex];
			return false;
		}
		return false;
	}

	if(!fromQue && oc->StalledChannels.Find(Bunch->ChIndex)!=0){  //check if channel is stalled
		if(Bunch->bClose){
			oc->StalledChannels.Remove(Bunch->ChIndex);
	//		debugf(TEXT("Stalled channel %i erased"),Bunch->ChIndex);
		} else {
			oc->StalledChannels.Find(Bunch->ChIndex)->bunches.PushBack(*IntBunch);  //annars l�gg till i k�n
			return false;
		}
	}
	for(INT i=0;i<IntBunch->dependsOn.Num();++i){	
		int c=IntBunch->dependsOn(i);
		if(!Connection->Channels[c]/* && fromQue*/){
			continue;
		}
		if(!Connection->Channels[c] || Connection->Channels[c]->Closing/* || oc->StalledChannels.find(c)!=oc->StalledChannels.end()*/){  //seems to get deadlocks sometimes if we stall on stalled channels
			if(!fromQue){
				OpenConnection::StalledChannel* sc=oc->StalledChannels.Find(Bunch->ChIndex);
				if(!sc){
					OpenConnection::StalledChannel s;
					oc->StalledChannels.Set(Bunch->ChIndex,s);
					sc=oc->StalledChannels.Find(Bunch->ChIndex);
				}
				sc->bunches.PushBack(*IntBunch);	
	//			debugf(TEXT("Channel %i stalled on channel %i"),Bunch->ChIndex,c);
			}
			return false;
		}
	}

	SendBunch(Connection,Bunch);

	return true;
	unguard;
}

void UReplicatorEngine::SendBunch(UNetConnection* Connection,FInBunch* InBunch)
{
	guard(UReplicatorEngine::SendBunch);

	if(Connection->Channels[InBunch->ChIndex]==0){
	//	printf("Send on null channel %i\n",InBunch->ChIndex);
		return;
	}
	if(Connection->Channels[InBunch->ChIndex]->Closing){
		if(Connection->Channels[InBunch->ChIndex]->OpenTemporary){
	//		printf("Deleting temp channel %i\n",InBunch->ChIndex);
			delete Connection->Channels[InBunch->ChIndex];
			return;
		}
	//	if(Connection==PrimaryConnection){
	//		printf("Send on closing channel %i\n",InBunch->ChIndex);
	//	}
		return;
	}

	if(InBunch->IsError()){
		debugf (TEXT ("Errenous inbunch in SendBunch %i %i %i"),InBunch->ChIndex,(int)InBunch->bOpen,(int)InBunch->bClose);
		return;
	}

	FInBunch tempBunch(*InBunch);//no way to reset position of bunch
	FOutBunch outBunch(Connection->Channels[InBunch->ChIndex],InBunch->bClose);

	outBunch.bOpen=tempBunch.bOpen;
	outBunch.bClose=tempBunch.bClose;
	outBunch.bReliable=tempBunch.bReliable;

	for(int a=0;a<tempBunch.GetNumBits();++a)
		outBunch.WriteBit(tempBunch.ReadBit());

	SendBunch(Connection,&outBunch);
	
	if ((InBunch->ChIndex == 1) && (InBunch->bOpen) && Connection==PrimaryConnection) {
		CreateFakeActor (Connection);
	}
	unguard;
}

void UReplicatorEngine::SendBunch(UNetConnection* Connection,FOutBunch* OutBunch)
{
	guard(UReplicatorEngine::SendBunch);
//	debugf(TEXT("Send on connection: %i"),(int)Connection);
//	check(OpenConnections.Find(Connection)!=0);

	if(Connection->Channels[OutBunch->ChIndex]==0){
	//	printf("Send on null channel %i\n",OutBunch->ChIndex);
		return;
	}
	if(Connection->Channels[OutBunch->ChIndex]->Closing){
		if(Connection->Channels[OutBunch->ChIndex]->OpenTemporary){
	//		printf("Deleting temp channel %i\n",OutBunch->ChIndex);
			delete Connection->Channels[OutBunch->ChIndex];
			return;
		}
	//	if(Connection==PrimaryConnection){
	//		printf("Send on closing channel %i\n",OutBunch->ChIndex);
	//	}
		return;
	}
	if(Connection->State==USOCK_Closed)
		return;

	//Error-flagged bunches will crash us if they go through.
	if(OutBunch->IsError()){
		if(OpenConnections.Find(Connection)==0)	//will not be found if server conn
			return;
		if (OpenConnections.Find(Connection)->numErrs < 3) {
			debugf (TEXT ("Erroneous outbunch nr %i, waiting a while"), OpenConnections.Find(Connection)->numErrs);
			OpenConnections.Find(Connection)->numErrs++;
		}
		else {
			debugf (TEXT ("Erroneous outbunch in SendBunch %i %i %i. Killing client"),OutBunch->ChIndex,(int)OutBunch->bOpen,(int)OutBunch->bClose);
			Connection->State=USOCK_Closed;
		}
		return;
	}

//	FString s=FString::Printf(TEXT("%i %i %i %i"),OutBunch->bOpen,OutBunch->bClose,OutBunch->bReliable,Connection->Channels[OutBunch->ChIndex]->OpenTemporary);
//	new(OpenConnections.Find(Connection)->ChannelHistory[OutBunch->ChIndex]) FString(s);

	if(OutBunch->bReliable && Connection->Channels[OutBunch->ChIndex]->OpenTemporary){
		debugf (TEXT ("Reliable bunch on temp channel ?? %i %i %i. Killing client"),OutBunch->ChIndex,(int)OutBunch->bOpen,(int)OutBunch->bClose);
		if(Delayer->DelayedOpenChannels[OutBunch->ChIndex]){
			for(TArray<InternalBunch>::TIterator opi(*Delayer->DelayedOpenChannels[OutBunch->ChIndex]);opi;++opi)
				debugf (TEXT ("Data %i %i %i"),opi->bunch->bOpen,opi->bunch->bClose,opi->bunch->bReliable);
		} else {
			debugf (TEXT ("No delayed channel"));
		}
//		for(TArray<FString>::TIterator opi(OpenConnections.Find(Connection)->ChannelHistory[OutBunch->ChIndex]);opi;++opi)
//			debugf (TEXT ("Channel Data %s"),*opi);
		Connection->State=USOCK_Closed;
		return;
	}
	FOutBunch tempBunch(*OutBunch);
	
	if (tempBunch.IsError ()) {
		debugf (TEXT ("Outbunch suddenly turned erroneous. This is very strange!"));
	}
	else {
		Connection->Channels[tempBunch.ChIndex]->SendBunch(&tempBunch,false);
	}
	
	unguard;
}

void UReplicatorEngine::Restart(void)
{
	debugf (TEXT ("Restarting"));

	guard(UReplicatorEngine::Restart);
	debugf (TEXT ("Unloading"));
	if(ConnectStatus!=6){
		for( INT i=0; i<ListenDriver->ClientConnections.Num(); i++ ){
			UNetConnection* Connection=ListenDriver->ClientConnections(i);
			Connection->Logf( TEXT("FAILURE Server restart. Please reconnect"));
			Connection->FlushNet();
		}
	}
	delete ListenDriver;
	if(ConnectDriver){
		delete ConnectDriver;
		ConnectDriver=0;
	}
	OpenConnections.Empty();
	delete ServerPackageMap;
	
	Delayer->Restart();
	mDelayer.Restart ();
	vDelayer.Restart ();
	UtvEngine=this;
	ServerConnReady=false;
	DoRestart=false;
	DoRestartIn=-1;
	MsgTimeOut=-10;
	GameEnded=false;
	PrimaryConnection=0;
	ConnectStatus=0;
	TestedForUtvProxy=TestedForUtvProxy && !ConnectedToUtvProxy;
	ConnectedToUtvProxy=0;
	TotalClients=0;
	TotalDelay=0;
	LastTotalClientsUpdate=0;
	LoggedInPlayers=0;

	//Create the package map
	ServerPackageMap = new UUTVPackageMap ();

	FURL URL;
	FString Error;

	check(sizeof(*this)==GetClass()->GetPropertiesSize());

	// Create net driver.
	UClass* NetDriverClass = StaticLoadClass( UNetDriver::StaticClass(), NULL, TEXT("IpDrv.TcpNetDriver"/*/"utv2004.TVNetDriver"/**/), NULL, LOAD_NoFail, NULL );

	URL.Port = ListenPort;
	ListenDriver = (UNetDriver*)StaticConstructObject( NetDriverClass );
	if( !ListenDriver->InitListen( this, URL, Error ) )
	{
		debugf( TEXT("Failed to listen: %s"), *Error );
		delete ListenDriver;
		ListenDriver=NULL;
	}

	unguard;
}

static void GetValue(FString& s,const TCHAR* key,FString& value)
{
	if(s.Locs().InStr(key)!=-1){
		FString k=key;
		FString s2=s.Mid(s.Locs().InStr(key)+k.Len());
		if(s2.InStr(TEXT(" "))!=-1)
			s2=s2.Left(s2.InStr(TEXT(" ")));
		if(s2.InStr(TEXT("?"))!=-1)
			s2=s2.Left(s2.InStr(TEXT("?")));
		value=s2;
		debugf(TEXT("%s %s"),key,value);
	}
}

static void GetValue(FString& s,const TCHAR* key,float& value)
{
	if(s.Locs().InStr(key)!=-1){
		FString k=key;
		FString s2=s.Mid(s.Locs().InStr(key)+k.Len());
		if(s2.InStr(TEXT(" "))!=-1)
			s2=s2.Left(s2.InStr(TEXT(" ")));
		if(s2.InStr(TEXT("?"))!=-1)
			s2=s2.Left(s2.InStr(TEXT("?")));
		value=appAtof(*s2);
		debugf(TEXT("%s %f"),key,value);
	}
}

static void GetValue(FString& s,const TCHAR* key,int& value)
{
	if(s.Locs().InStr(key)!=-1){
		FString k=key;
		FString s2=s.Mid(s.Locs().InStr(key)+k.Len());
		if(s2.InStr(TEXT(" "))!=-1)
			s2=s2.Left(s2.InStr(TEXT(" ")));
		if(s2.InStr(TEXT("?"))!=-1)
			s2=s2.Left(s2.InStr(TEXT("?")));
		value=appAtoi(*s2);
		debugf(TEXT("%s %i"),key,value);
	}
}

void UReplicatorEngine::ParseCmdLine(const TCHAR* Parms)
{
	guard(UReplicatorEngine::ParseCmdLine);
	FString s=Parms;
	GetValue( s, TEXT("serveraddress="), ServerAdress );
	GetValue( s, TEXT("serverport="), ServerPort );
	GetValue( s, TEXT("listenport="), ListenPort );
	GetValue( s, TEXT("delay="), Delayer->DelayTime );
	if(Delayer->DelayTime<5 && !ConnectedToUtvProxy)
		Delayer->DelayTime=5;
	GetValue( s, TEXT("maxclients="), MaxClients );
	GetValue( s, TEXT("primarypassword="), PrimaryPassword );
	GetValue( s, TEXT("normalpassword="), NormalPassword );
	GetValue( s, TEXT("vippassword="), VipPassword );
	GetValue( s, TEXT("joinpassword="), JoinPassword );
	GetValue( s, TEXT("serverpassword="), JoinPassword );
	GetValue( s, TEXT("seeall="), SeeAll );
	GetValue( s, TEXT("noprimary="), NoPrimary );
	GetValue( s, TEXT("ignorechainedchat="), IgnoreChainedChat );

	if(NoPrimary && !SeeAll)
		NoPrimary=0;

	if(s.Locs().InStr(TEXT("restart"))!=-1)
		DoRestartIn=0.1f;

	unguard;
}

IMPLEMENT_CLASS(UReplicatorEngine);
