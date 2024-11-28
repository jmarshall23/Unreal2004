/*=============================================================================
	MasterServerSocket.h: Master server socket class
	Copyright 1997-2002 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Jack Porter

=============================================================================*/

//
// Defines.
//
#define MAX_CLIENTS_PER_THREAD 63
#define MAX_THREADS 60
#define MASTERSERVER_PROTOCOL_VER 1

// Client Ids

#define CLIENT_ID TEXT("UT2K4CLIENT")
#define SERVER_ID TEXT("UT2K4SERVER")

#define UDPWAITTIMEOUT 60

//
// Forward declarations
//
class FMasterServerClientLink;
class FMasterServerListenLink;


/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

extern TCHAR GDatabaseServer[];
extern UBOOL GRunHousekeeping;
extern UBOOL GAcceptStats;
extern UBOOL GExtendedLogging;
extern TCHAR* GDatabaseUser;
extern TCHAR* GDatabasePass;
extern INT GLatestVersion;

/*-----------------------------------------------------------------------------
	FMasterServerStats
-----------------------------------------------------------------------------*/

struct FMasterServerStats
{
	INT BytesSent;
	INT BytesReceived;
	INT ClientConnects;
	INT ServerConnects;
	INT DeniedConnects;
	INT Queries;
	INT MOTDs;
	INT AutoUpdates;
	INT OnlineServers;
	INT OnlinePlayers;

	FMasterServerStats()
	:	BytesSent(0)
	,	BytesReceived(0)
	,	ClientConnects(0)
	,	ServerConnects(0)
	,	DeniedConnects(0)
	,	Queries(0)
	,	MOTDs(0)
	,	AutoUpdates(0)
	,	OnlineServers(0)
	,	OnlinePlayers(0)
	{}

	FMasterServerStats operator+=( const FMasterServerStats& Other )
	{
		BytesSent += Other.BytesSent;
		BytesReceived += Other.BytesReceived;
		ClientConnects += Other.ClientConnects;
		ServerConnects += Other.ServerConnects;
		DeniedConnects += Other.DeniedConnects;
		Queries += Other.Queries;
		MOTDs += Other.MOTDs;
		AutoUpdates += Other.AutoUpdates;
		OnlineServers += Other.OnlineServers;
		OnlinePlayers += Other.OnlinePlayers;
		return *this;
	}

	void Average( FLOAT Time )
	{
		BytesSent = appRound((FLOAT)BytesSent/Time);
		BytesReceived = appRound((FLOAT)BytesReceived/Time);
		ClientConnects = appRound((FLOAT)ClientConnects/Time);
		ServerConnects = appRound((FLOAT)ServerConnects/Time);
		DeniedConnects = appRound((FLOAT)DeniedConnects/Time);
		Queries = appRound((FLOAT)Queries/Time);
		MOTDs = appRound((FLOAT)MOTDs/Time);
		AutoUpdates = appRound((FLOAT)AutoUpdates/Time);
		OnlineServers = appRound((FLOAT)OnlineServers/Time);
		OnlinePlayers = appRound((FLOAT)OnlinePlayers/Time);
	}

	void Clear()
	{
		BytesSent			= 0;
		BytesReceived		= 0;
		ClientConnects		= 0;
		ServerConnects		= 0;
		DeniedConnects		= 0;
		Queries				= 0;
		MOTDs				= 0;
		AutoUpdates			= 0;
		OnlineServers		= 0;
		OnlinePlayers		= 0;
	}
};

/*-----------------------------------------------------------------------------
	FMasterServerThread
-----------------------------------------------------------------------------*/

//
// FMasterServerSemaphore - threading mutual exclusion mechanism
//
class FMasterServerSemaphore
{
	HANDLE hMutex;
	TCHAR* SemaphoreName;
public:
	FMasterServerSemaphore( TCHAR* InSemName );
	FMasterServerSemaphore();
	~FMasterServerSemaphore();
	void Wait();
	void Signal();
};

//
// FMasterServerReadWriteLock - readers/writers mutual exclusion mechanism
//
class FMasterServerReadWriteLock
{
	FMasterServerSemaphore DBSem;
	INT ReaderCount;
public:
	FMasterServerReadWriteLock();
	void ReadLock();
	void ReadUnlock();
	void WriteLock();
	void WriteUnlock();
};

class FMasterServerThread
{
	HANDLE hThread;
	UBOOL Started;
public:
    FMasterServerThread();
	virtual ~FMasterServerThread();
	void StartThread();

	virtual void ThreadMain()=0;
};

/*-----------------------------------------------------------------------------
	FMasterServerCDKeyCacheThread
-----------------------------------------------------------------------------*/

struct FCDKeyCacheItem
{
	INT CDKeyID;
	TCHAR CDKey[24];
	DWORD ServerOnly:1;
	DWORD Disabled:1;
};

struct FCDKeyCacheNode
{
	BYTE CDKeyHash[16];
	DWORD UsingSpaces:1;
	FCDKeyCacheItem* Item;	
	FCDKeyCacheNode* Left;
	FCDKeyCacheNode* Right;
};

struct FCDKeyCacheTree
{
	FCDKeyCacheTree()
	:	Root(NULL)
	,	MaxDepth(0)
	,	AvgDepthNumerator(0)
	,	AvgDepthDenominator(0)
	,	NumKeys(0)
	,	MaxKeyID(0)
	{}

	static FCDKeyCacheTree* Cache;
	FCDKeyCacheNode* Root;
	INT MaxDepth;
	INT AvgDepthNumerator;
	INT AvgDepthDenominator;
	INT NumKeys;
	INT MaxKeyID;

	FCDKeyCacheNode** FindNodeForNewItem( BYTE* CDKeyHash );
	FCDKeyCacheNode* FindNode( BYTE* CDKeyHash );
private:
	FCDKeyCacheNode** FindNodePtr( BYTE* CDKeyHash );
};

class FMasterServerCDKeyCacheThread : public FMasterServerThread
{
	FMySQL MySQL;
	FCDKeyCacheTree Tree;
public:
	FMasterServerCDKeyCacheThread();
	virtual void ThreadMain();
};

/*-----------------------------------------------------------------------------
	FMasterServerClientThread
-----------------------------------------------------------------------------*/

struct FMasterServerListCache
{
	INT Version;
	INT MinVersion;
	INT MaxVersion;
	UBOOL DemoOnly;
	TArray<FQueryData> Query;
	TArray<FServerResponseLine> Result;
	DOUBLE LastUpdated;
};

struct FMasterServerVersionCache
{
	INT CurrentVersion;
	BYTE Platform;
	UBOOL IsServer;
	UBOOL OptionalRequest;
	UBOOL IsDemo;
	FString Language;

	// results
	INT Result;
	INT OutMinVersion;
	INT OutMaxVersion;
	FString MOTD;
	DOUBLE LastUpdated;
};


//
// FMasterServerClientThread - thread serving collection of client sockets
//
class FMasterServerClientThread : public FMasterServerThread
{
	friend class FMasterServerClientLink;
	friend class FMasterServerListenLink;

	FMasterServerSemaphore ClientsSem;
	TArray<FMasterServerClientLink*> Clients;
	TArray<FMasterServerListCache> ListCache;
	TArray<FMasterServerVersionCache> VersionCache;

	FMasterServerListenLink* MasterServerLink;
	FMySQL MySQL;
public:
	INT ClientCount() { return Clients.Num(); };
	DOUBLE LockupDetectTime;
	FString LockupInfo;
	FMasterServerStats Stats;
    FMasterServerClientThread( FMasterServerListenLink* InMasterServerLink );
	virtual void ThreadMain();
	void UpdateStats();
	UBOOL CheckReceiveHeartbeat( FIpAddr& Address, INT HeartbeatType, INT Code );
};


/*-----------------------------------------------------------------------------
	FMasterServerHousekeepingThread
-----------------------------------------------------------------------------*/

//
// FMasterServerHousekeepingThread - thread to perform periodic housekeeping
//
class FMasterServerHousekeepingThread : public FMasterServerThread
{
	FMasterServerListenLink* Parent;
	FMasterServerStats Stats;
	DOUBLE LastStatTime;
	FMySQL MySQL;
public:
	FMasterServerHousekeepingThread( FMasterServerListenLink* InParent );
	virtual void ThreadMain();
};

/*-----------------------------------------------------------------------------
	FMasterServerThrottleThread
-----------------------------------------------------------------------------*/

//
// FMasterServerThrottleThread - thread to perform bandwidth throttling.
//
class FMasterServerThrottleThread : public FMasterServerThread
{
	INT SendCPS;
	INT RecvCPS;
public:
	FMasterServerThrottleThread( INT InSendCPS, INT InRecvCPS )
	:	SendCPS(InSendCPS)
	,	RecvCPS(InRecvCPS)
	{
		StartThread();
	}
	virtual void ThreadMain()
	{
		while( !GIsRequestingExit )
		{
			FInternetLink::ThrottleBandwidth( SendCPS, RecvCPS );
			appSleep(1);
		}
	}
};

/*-----------------------------------------------------------------------------
	FMasterServerUdpLink - Listening UDP Socket.
-----------------------------------------------------------------------------*/

class FMasterServerUdpLink : public FUdpLink
{
	FMasterServerListenLink* Parent;
public:
	FMasterServerUdpLink( FMasterServerListenLink* InParent );
	virtual void OnReceivedData( FIpAddr Address, BYTE* Data, INT Length );
};

/*-----------------------------------------------------------------------------
	FMasterServerListenLink - Listening TCP Socket.
-----------------------------------------------------------------------------*/

class FMasterServerListenLink : public FTcpLink
{
	friend class FMasterServerClientThread;
	friend class FMasterServerHousekeepingThread;
	friend class FMasterServerUdpLink;

	FMasterServerUdpLink UdpSocket;
	FMasterServerSemaphore ThreadsSem;
	TArray<FMasterServerClientThread*> Threads;
public:
	FMasterServerSemaphore DBSem;
    
	// tors
	FMasterServerListenLink( INT ListenPort );
	virtual ~FMasterServerListenLink();

	// FTcpLink notifications
	virtual void WaitForConnections( INT WaitTime );
	virtual void OnIncomingConnection( FSocketData ConnectionData );

	// FMasterServerListenLink interface
	void UpdateStats();
	FMasterServerClientThread* FindThread();
};

/*-----------------------------------------------------------------------------
	FMasterServerClientLink - Client connection to master server.
-----------------------------------------------------------------------------*/

enum EClientState
{
	CL_Closed					= 0,
	CL_Challenged				= 1,		// Challenge sent.
	CL_Denied					= 2,		// Access denied.
	CL_ServerChannel			= 3,		// This is a server channel.  Process server messages.
	CL_ServerWaitingForUDP		= 4,		// Server, waiting for UDP response.
	CL_ClientChannel			= 5,		// This is a client channel.  Process client messages.
	CL_UnderReview				= 6,			// Client is under review
	CL_PendingClose				= 7
};

struct FHeartbeatInfo
{
	UBOOL WaitingReply;
	DOUBLE RequestSendTime;
	INT RequestCount;
	DWORD NatPort;
	
	FHeartbeatInfo()
	:	WaitingReply(0)
	,	RequestSendTime(0.0)
	,	RequestCount(0)
	,	NatPort(0)
	{}
};

struct FReceivedHeartbeatInfo
{
	FIpAddr Address;
	INT HeartbeatType;
	FReceivedHeartbeatInfo( FIpAddr InAddress, INT InHeartbeatType )
	:	Address(InAddress)
	,	HeartbeatType(InHeartbeatType)
	{}
};

class FMasterServerClientLink : public FTcpLink
{
	friend class FMasterServerClientThread;

	FMasterServerListenLink* Parent;
	FMasterServerClientThread* Thread;
	FSocketData* GetSocketData() {return &SocketData;}

	EClientState ClientState;
	FString Challenge;
	INT CDKeyID;
	INT RemoteVersion;
	INT RemotePlatform;
	INT RemoteGPUDeviceID;
	INT RemoteGPUVendorID;
	INT RemoteCPUType;
	INT RemoteCPUSpeed;
	INT CompatibleMinVersion;
	INT CompatibleMaxVersion;
	FString RemoteLanguage;
	UBOOL RemoteIsDemo;

	// server specific stuff
	FString ServerCDKeyHash;
	INT ServerID;
	INT MatchID;
	UBOOL AcceptStatsThisServer;

	// heartbeat stuff
	UBOOL ServerUsesNAT, ServerUsesGamespy;
	FHeartbeatInfo HeartbeatInfo[3];
	FMasterServerSemaphore ReceivedHeartbeatSem;
	TArray<FReceivedHeartbeatInfo> ReceivedHeartbeats;
	DOUBLE UDPWaitTime;
public:

	// tors
	FMasterServerClientLink( FSocketData InSocketData, FMasterServerListenLink* InParent, FMasterServerClientThread* InThread );
	virtual ~FMasterServerClientLink();

	// FTcpLink notifications
	virtual void OnDataReceived();

	// FMasterServerClientLink interface.
	void PerformHousekeeping();
	INT GenerateChallenge();
	INT VerifyChallenge( const TCHAR* CDKeyHash, const TCHAR* Challenge, const TCHAR* Response, UBOOL IsServer, const TCHAR* AuthIP );
	void GenerateServerList( INT Version,  INT MinVersion, INT MaxVersion, TArray<FQueryData>& Query, TArray<FServerResponseLine>& Result );
	void UpdateServerState( FServerResponseLine* ServerState, INT ConnectedPlayerCount );
	void UpdateClients( TArray<FString>& ClientIPs );
	void ValidateClientResponse( const TCHAR* ClientIP, const TCHAR* ClientCDKeyHash, const TCHAR* ClientResponse, const TCHAR* ClientGMD5, const TCHAR* ClientNick );
	void ReceiveStatsLine( const TCHAR* StatsLine );
	void CheckHeartbeats();
	void ReceivedHeartbeat( BYTE HeartbeatType, FIpAddr IPAddr );
	INT CheckVersion( INT Version, BYTE Platform, const TCHAR* Language, UBOOL IsDemo, UBOOL IsServer=0, UBOOL OptionalRequest=0, INT* OutMinVersion=NULL, INT* OutMaxVersion=NULL, FString* MOTD=NULL );
	void GetModMOTD(INT Version, BYTE Platform, const TCHAR* Language, UBOOL IsDemo, FString* MOTD=NULL );
	void GetDownloadSites( INT UpgradeVersion, BYTE Platform, TArray<FString>& DownloadSites );
	void AssignMatchID();
	void CheckPackageMD5Version( INT CurrentMaxRevision );
	void GenerateOwnageList(INT revision);
	UBOOL VerifyGlobalMD5(FString GlobalMD5, int CDKeyID);
	FString IsBanned(int CDKeyID);
	void AddBadGuy(int CDKeyID, FString Reason);
	UBOOL PatternMatch(FString Data, FString Pattern);
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/