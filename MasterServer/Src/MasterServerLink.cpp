/*=============================================================================
	MasterServerLink.cpp: Unreal Master network / thread code
	Copyright 1997-2002 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Jack Porter.
=============================================================================*/

#include "MasterServer.h"
#include "MasterServerLink.h"


#define HEARTBEAT_TIMEOUT 5.f
#define HIGHEST_UT2003_VERSION 2225

/*-----------------------------------------------------------------------------
	FMasterServerSemaphore
-----------------------------------------------------------------------------*/

FMasterServerSemaphore::FMasterServerSemaphore()
:	SemaphoreName(TEXT("unnamed"))
{
	hMutex = CreateMutex( NULL, FALSE, SemaphoreName );
	check(hMutex);
}

FMasterServerSemaphore::FMasterServerSemaphore( TCHAR* InSemName )
:	SemaphoreName(InSemName)
{
	hMutex = CreateMutex( NULL, FALSE, SemaphoreName );
	check(hMutex);
}

FMasterServerSemaphore::~FMasterServerSemaphore()
{
	CloseHandle(hMutex);
}

void FMasterServerSemaphore::Wait()
{
	for(;;)
	{
		DWORD dwWaitResult = WaitForSingleObject( hMutex, 5000L );
		if( dwWaitResult == WAIT_OBJECT_0 )
			break;
		GWarn->Logf(TEXT("Timeout waiting for semaphore %s"), SemaphoreName );
		if( !appStrcmp(SemaphoreName, TEXT("ClientsSem")) )
			appErrorf(TEXT("How!?"));
	}
}

void FMasterServerSemaphore::Signal()
{
	 ReleaseMutex(hMutex);
}

/*-----------------------------------------------------------------------------
	FMasterServerThread
-----------------------------------------------------------------------------*/

//
// Windows threads entry point
//
static DWORD STDCALL ThreadEntry( void* Arg )
{
	((FMasterServerClientThread*)Arg)->ThreadMain();
	return 0;
}

FMasterServerThread::FMasterServerThread()
:	Started(0)
{
	// Create thread.
	DWORD ThreadId = 1;
	hThread = CreateThread( NULL, 0, ThreadEntry, this, CREATE_SUSPENDED, &ThreadId );
	check(hThread);
}

FMasterServerThread::~FMasterServerThread()
{
	CloseHandle( hThread );
}

void FMasterServerThread::StartThread()
{
	if( !Started )
	{
		Started = 1;
		ResumeThread(hThread);
	}
}

/*-----------------------------------------------------------------------------
	FMasterServerCDKeyCacheThread
-----------------------------------------------------------------------------*/

static INT HexDigit( TCHAR c )
{
	if( c>='0' && c<='9' )
		return c - '0';
	else if( c>='a' && c<='f' )
		return c + 10 - 'a';
	else if( c>='A' && c<='F' )
		return c + 10 - 'A';
	else
		return 0;
}

static void MD5StrToHash( const TCHAR* String, BYTE* MD5 )
{
	INT KeyLen = appStrlen(String);
	for( INT j=0;j<16;j++ )
		MD5[j] = (j*2+1) < KeyLen ? 16*HexDigit(String[j*2])+HexDigit(String[(j*2)+1]) : 0;
}

FMasterServerCDKeyCacheThread::FMasterServerCDKeyCacheThread()
:	FMasterServerThread()
{
	StartThread();
}

void FMasterServerCDKeyCacheThread::ThreadMain()
{
	try
	{
		guard(FMasterServerCDKeyCacheThread::ThreadMain);

		if( !MySQL.Connect( GDatabaseServer, GDatabaseUser, GDatabasePass, MASTER_SERVER_DATABASE, 0 ) )
			appErrorf(TEXT("FMasterServerCDKeyCacheThread: Database connect failed"));

		for(;;)
		{
			// Get server list.
			FQueryResult* CDKeyQuery = MySQL.Query(	"select cdkey.id, cdkey.cdkey, cdkey.disabled, cdkey.serveronly, cdkey.md5hash, cdkeyspaces.md5hashsp, "
													"cdkey.batchid from cdkey, cdkeyspaces where cdkey.id=cdkeyspaces.cdkeyid and id > %d order by id limit 50000", 
													Tree.MaxKeyID );
			FQueryField** CDKeyRow;

			INT NumKeys = CDKeyQuery->NumRows();
			if( NumKeys )
			{
				GWarn->Logf(TEXT("Caching %d cd keys starting at %d"), NumKeys, Tree.MaxKeyID );

				FCDKeyCacheNode* NewNode = (FCDKeyCacheNode*)appMalloc( NumKeys * sizeof(FCDKeyCacheNode) * 2, TEXT("FCDKeyCacheNode") );
				FCDKeyCacheItem* NewItem = (FCDKeyCacheItem*)appMalloc( NumKeys * sizeof(FCDKeyCacheItem)    , TEXT("FCDKeyCacheItem") );

				while( (CDKeyRow = CDKeyQuery->FetchNextRow()) != NULL )
				{
					NewItem->CDKeyID = CDKeyRow[0]->AsInt();
					Tree.MaxKeyID = NewItem->CDKeyID;
					appStrcpy( NewItem->CDKey, *CDKeyRow[1]->AsString().Left(23) );
					NewItem->Disabled = CDKeyRow[2]->AsString() == TEXT("T");
					NewItem->ServerOnly = CDKeyRow[3]->AsString() == TEXT("T");
					UBOOL IsGenServerOnlyKey = CDKeyRow[6]->AsInt() == 2;

					MD5StrToHash( *CDKeyRow[4]->AsString(), NewNode->CDKeyHash );
					NewNode->UsingSpaces = 0;
					NewNode->Item = NewItem;
					NewNode->Left = NULL;
					NewNode->Right = NULL;

					FCDKeyCacheNode** NewNodePtr = Tree.FindNodeForNewItem( NewNode->CDKeyHash );
					if( !NewNodePtr )
					{
						if( !IsGenServerOnlyKey )
							GWarn->Logf(TEXT("WARNING: detected duplicate cdkey hash in tree: %s"), *CDKeyRow[4]->AsString());
					}
					else
					{
						*NewNodePtr = NewNode;
						Tree.NumKeys++;
					}
					NewNode++;

					MD5StrToHash( *CDKeyRow[5]->AsString(), NewNode->CDKeyHash );
					NewNode->UsingSpaces = 1;
					NewNode->Item = NewItem;
					NewNode->Left = NULL;
					NewNode->Right = NULL;
					NewNodePtr = Tree.FindNodeForNewItem( NewNode->CDKeyHash );
					if( !NewNodePtr )
					{
						if( !IsGenServerOnlyKey )
							GWarn->Logf(TEXT("WARNING: detected duplicate cdkey hash in tree: %s (sp)"), *CDKeyRow[5]->AsString());
					}
					else
					{
						Tree.NumKeys++;
						*NewNodePtr = NewNode;
					}
					NewNode++;
					NewItem++;
				}

				GWarn->Logf(TEXT("Done.  MaxDepth=%d, AvgDepth=%f"), Tree.MaxDepth, (FLOAT)Tree.AvgDepthNumerator/(FLOAT)Tree.AvgDepthDenominator );

				delete CDKeyQuery;
			}
			else
			{
				delete CDKeyQuery;
				if( !FCDKeyCacheTree::Cache )
				{
					GWarn->Logf(TEXT("Completed initial cd key cache."));
					GWarn->Logf(TEXT("NumHashes=%d, MaxDepth=%d, AvgDepth=%f"), Tree.NumKeys, Tree.MaxDepth, (FLOAT)Tree.AvgDepthNumerator/(FLOAT)Tree.AvgDepthDenominator );
					FCDKeyCacheTree::Cache = &Tree;
				}
				appSleep(60);
			}
		}

		// done.

		unguard;
		delete this;
	}
	catch( ... )
	{
		// Crashed.
		GIsGuarded = 0;
		GError->HandleError();
		appRequestExit(1);
	}
}

// find place for a new node, or NULL if node already exists.
FCDKeyCacheNode** FCDKeyCacheTree::FindNodeForNewItem( BYTE* InCDKeyHash )
{
	guard(FCDKeyCacheTree::FindNodeForNewItem);
	FCDKeyCacheNode** Ptr = FindNodePtr( InCDKeyHash );
	if( *Ptr )
		return NULL;
	return Ptr;
	unguard;
}

// find an existing node, or NULL if not found.
FCDKeyCacheNode* FCDKeyCacheTree::FindNode( BYTE* InCDKeyHash )
{
	guard(FCDKeyCacheTree::FindNodeForNewItem);
	return *FindNodePtr(InCDKeyHash);
	unguard;
}

// find the parent to a potential node
FCDKeyCacheNode** FCDKeyCacheTree::FindNodePtr( BYTE* InCDKeyHash )
{
	guard(FCDKeyCacheTree::FindNodePtr);

	FCDKeyCacheNode** NodePtr = &Root;
	FCDKeyCacheNode* Node = Root;

	INT Depth = 0;
	AvgDepthDenominator++;

	while( Node )
	{
		INT i = appMemcmp( InCDKeyHash, Node->CDKeyHash, sizeof(Node->CDKeyHash) );
		if( i==0 )
			return NodePtr;
		else
		if( i < 0 )
		{
			NodePtr = &Node->Left;
			Node = Node->Left;
		}
		else
		{
			NodePtr = &Node->Right;
			Node = Node->Right;
		}
		Depth++;
		AvgDepthNumerator++;
		if( MaxDepth < Depth )
			MaxDepth = Depth;
	}

	return NodePtr;
	unguard;
}



/*-----------------------------------------------------------------------------
	FMasterServerClientThread
-----------------------------------------------------------------------------*/


FMasterServerClientThread::FMasterServerClientThread( FMasterServerListenLink* InMasterServerLink )
:	FMasterServerThread()
,	ClientsSem(TEXT("ClientsSem"))
,	MasterServerLink(InMasterServerLink)
{
	LockupDetectTime = appSeconds();

	// Add to masterserver's list of threads
	MasterServerLink->ThreadsSem.Wait();
	MasterServerLink->Threads.AddItem( this );
	MasterServerLink->ThreadsSem.Signal();  
}

void FMasterServerClientThread::ThreadMain()
{
	try
	{
		guard(FMasterServerClientThread::ThreadMain);
		if (GExtendedLogging)
			GWarn->Logf(TEXT("New thread...."));	

		if( !MySQL.Connect( GDatabaseServer, GDatabaseUser, GDatabasePass, MASTER_SERVER_DATABASE, 1 ) )
			appErrorf(TEXT("Database connect failed"));

		while( !GIsRequestingExit && Clients.Num() )
		{
			LockupDetectTime = appSeconds();
			LockupInfo = TEXT("Top of loop");

			// Check to see if clients have data waiting.
			INT SelectStatus;
			TIMEVAL SelectTime = {1, 0};
			fd_set ReadableSet, WritableSet;
			FD_ZERO( &ReadableSet );
			FD_ZERO( &WritableSet );
			ClientsSem.Wait();
			UBOOL CheckWritable=0;
			UBOOL CheckReadable=0;
			for( INT i=0;i<Clients.Num();i++ )
			{
				if( Clients(i)->HasBudgetToRecv() )
				{
					FD_SET( Clients(i)->GetSocketData()->Socket, &ReadableSet );
					CheckReadable = 1;
				}
				if( Clients(i)->HasSendPending() )
				{
					CheckWritable = 1;
					FD_SET( Clients(i)->GetSocketData()->Socket, &WritableSet );
				}
			}
			ClientsSem.Signal();

			if( CheckReadable || CheckWritable )
			{
				LockupInfo = TEXT("Select");
				SelectStatus = select( 0, CheckReadable ? &ReadableSet : 0, CheckWritable ? &WritableSet : 0, 0, &SelectTime );
				if( SelectStatus == SOCKET_ERROR )
				{
					GWarn->Logf( TEXT("!! Error selecting inbound sockets for data: %i"), WSAGetLastError());
					appSleep(1);
					continue;
				}

				if( SelectStatus )
				{
					ClientsSem.Wait();
					for( INT i=0;i<Clients.Num();i++ )
					{
						if( CheckReadable && FD_ISSET( Clients(i)->GetSocketData()->Socket, &ReadableSet ) )
						{
							// This client has data.
							LockupInfo = FString::Printf(TEXT("Reading socket for %s, state %d"), *Clients(i)->GetSocketData()->GetString(0), Clients(i)->LinkState );
							FMasterServerClientLink* Client = Clients(i);
							ClientsSem.Signal();
							Client->ReceivePendingData();  
							Stats.BytesReceived += Client->StatBytesReceived; Client->StatBytesReceived = 0;
							LockupInfo = FString::Printf(TEXT("Done reading socket for %s, state %d"), *Clients(i)->GetSocketData()->GetString(0), Clients(i)->LinkState );
							ClientsSem.Wait();
						}

						if( CheckWritable && FD_ISSET( Clients(i)->GetSocketData()->Socket, &WritableSet ) )
						{
							// This client has data to send.
							LockupInfo = FString::Printf(TEXT("Writing socket for %s"), *Clients(i)->GetSocketData()->GetString(0) );
							FMasterServerClientLink* Client = Clients(i);
							ClientsSem.Signal();
							Client->SendPendingData();
							Stats.BytesSent += Client->StatBytesSent; Client->StatBytesSent = 0;
							ClientsSem.Wait();
						}
						if ( Clients(i)->ClientState==CL_PendingClose && !Clients(i)->HasSendPending() )
						{
							Clients(i)->Close(true);
							Clients(i)->ClientState=CL_Closed;
						}
					}
					ClientsSem.Signal();
				}
			}
			else
				appSleep( 1 );
		
			// Check for closed connections
			LockupInfo = TEXT("Checking for closed connections");
			ClientsSem.Wait();
			for( INT i=0;i<Clients.Num();i++ )
			{
				// Timeout clients inactive after 90 seconds for servers, 15 for everything else.
//				DOUBLE timeout = (Clients(i)->ClientState==CL_ServerChannel) ? 300.0 : 60.0;
				DOUBLE timeout = (Clients(i)->ClientState==CL_ServerChannel) ? 300.0 : 120.0;
				if( Clients(i)->LastTrafficTime + timeout < appSeconds() )
				{
//					if (GExtendedLogging)
						GWarn->Logf( TEXT("Timed out client %s after %10.3f seconds (%d)."), *Clients(i)->GetSocketData()->GetString(0), (appSeconds()-Clients(i)->LastTrafficTime),appRound(timeout) );

					Clients(i)->Close(1);
				}

				if( Clients(i)->LinkState == LINK_Closed )
				{
					delete Clients(i);
					i--;
				}
			}
			ClientsSem.Signal();

			// Poll each connection
			LockupInfo = TEXT("Housekeeping");
			ClientsSem.Wait();
			for( INT i=0;i<Clients.Num();i++ )
			{
				FMasterServerClientLink* Client = Clients(i);
				ClientsSem.Signal();
				Client->PerformHousekeeping();
				ClientsSem.Wait();
			}
			ClientsSem.Signal();

		}

		// Shutdown.
		MasterServerLink->ThreadsSem.Wait();
		MasterServerLink->Threads.RemoveItem( this );
		MasterServerLink->ThreadsSem.Signal();
		unguard;
		delete this;
	}
	catch( ... )
	{
		// Crashed.
		GIsGuarded = 0;
		GError->HandleError();
		appRequestExit(1);
	}
}

void FMasterServerClientThread::UpdateStats()
{
//	GWarn->Logf( TEXT("  %d clients\r"), Clients.Num() );	
}

UBOOL FMasterServerClientThread::CheckReceiveHeartbeat( FIpAddr& Address, INT HeartbeatType, INT Code )
{
	guard(FMasterServerClientThread::CheckReceiveHeartbeat);
	
	ClientsSem.Wait();
	for( INT i=0;i<Clients.Num();i++ )
	{
		if( Code==Clients(i)->SocketData.Socket && Address.GetString(0)==Clients(i)->SocketData.GetString(0) )
		{
			Clients(i)->ReceivedHeartbeatSem.Wait();
			Clients(i)->ReceivedHeartbeats.AddItem( FReceivedHeartbeatInfo( Address, HeartbeatType ) );
			Clients(i)->ReceivedHeartbeatSem.Signal();
			ClientsSem.Signal();
			return 1;			
		}
	}
	ClientsSem.Signal();
	return 0;
	unguard;
}


/*-----------------------------------------------------------------------------
	FMasterServerHousekeepingThread
-----------------------------------------------------------------------------*/
FMasterServerHousekeepingThread::FMasterServerHousekeepingThread( FMasterServerListenLink* InParent )
:	FMasterServerThread()
,	Parent(InParent)
{
	StartThread();
}

void FMasterServerHousekeepingThread::ThreadMain()
{
	try
	{
		guard(FMasterServerHousekeepingThread::ThreadMain);

		GWarn->Logf(TEXT("Housekeeping thread starting up."));	

		if( !MySQL.Connect( GDatabaseServer, GDatabaseUser, GDatabasePass, MASTER_SERVER_DATABASE ) )
			appErrorf(TEXT("Database connect failed"));
	
		DOUBLE StatStartTime = appSeconds();
		DOUBLE MSStartupTime = appSeconds();
		INT HousekeepingCount = 0;
		while( !GIsRequestingExit )
		{
			HousekeepingCount++;
			appSleep(10);

			if( GRunHousekeeping && appSeconds() > MSStartupTime + 120.0 )	// If we crashed, give everyone a little chance to recover
			{
				{
					// Expire old servers
					TArray<INT> OldServerIDs;
					FQueryResult* OnlineServersQuery  = MySQL.Query( "select serverid from onlineservers where expires < NOW()" );
					FQueryField** Row;
					while( (Row = OnlineServersQuery->FetchNextRow())!=NULL )
						OldServerIDs.AddItem(Row[0]->AsInt());
					delete OnlineServersQuery;

					GWarn->Logf(TEXT("Expiring %i servers"), OldServerIDs.Num() );					

					for( INT i=0;i<OldServerIDs.Num();i++ )
					{
						if (GExtendedLogging)
							GWarn->Logf(TEXT("Expiring server %d"), OldServerIDs(i) );					
						MySQL.DoSQL( "delete from onlineservers where serverid='%d'", OldServerIDs(i) );
						MySQL.DoSQL( "delete from onlineserverdetails where serverid='%d'", OldServerIDs(i) );
						MySQL.DoSQL( "delete from onlineserverplayers where serverid='%d'", OldServerIDs(i) );
						MySQL.DoSQL( "delete from onlineplayers where serverid='%d'", OldServerIDs(i) );
					}
				}
			}

			// 10 minute processing.
			if( GRunHousekeeping && (HousekeepingCount%60) == 0 )
			{
				GWarn->Logf(TEXT("Expiring old servers records"));
				MySQL.DoSQL("delete from servers where banned is null and lastseen < date_sub(now(), interval 8 hour) and (matchid is null or matchid=0)");

				GWarn->Logf(TEXT("Expiring old badhash records"));
				MySQL.DoSQL("delete from badhash where lastseen < date_sub(now(), interval 30 minute)");

				GWarn->Logf(TEXT("Expiring incomplete stats matches"));
				TArray<INT> OldMatchIDs;
				FQueryResult* OldStatsQuery  = MySQL.Query( "select matchid from statsmatches where lastupdated < DATE_SUB(NOW(), INTERVAL 8 HOUR) and matchcomplete is null" );
				FQueryField** Row;
				while( (Row = OldStatsQuery->FetchNextRow())!=NULL )
					OldMatchIDs.AddItem(Row[0]->AsInt());
				delete OldStatsQuery;
				GWarn->Logf(TEXT("Expiring %i stats matchids"), OldMatchIDs.Num() );
				for( INT i=0;i<OldMatchIDs.Num();i++ )
				{
					if (GExtendedLogging)
						GWarn->Logf(TEXT("Expiring stats matchid %d"), OldMatchIDs(i) );
					MySQL.DoSQL( "delete from statsmatches where matchid='%d'", OldMatchIDs(i) );
					MySQL.DoSQL( "delete from statsline where matchid='%d'", OldMatchIDs(i) );
				}
			}

			// 60 minute processing.
			if( GRunHousekeeping && (HousekeepingCount%360) == 0 )
			{
				GWarn->Logf(TEXT("Expiring statslines with no matching staticmatch"));
				TArray<INT> BadMatchIDs;
				FQueryResult* UnmatchedStatsQuery  = MySQL.Query( "select distinct statsline.matchid from statsline left join statsmatches on statsline.matchid=statsmatches.matchid where statsmatches.matchid is null" );
				FQueryField** Row;
				while( (Row = UnmatchedStatsQuery->FetchNextRow())!=NULL )
					BadMatchIDs.AddItem(Row[0]->AsInt());
				delete UnmatchedStatsQuery;
				GWarn->Logf(TEXT("Removing statslines for %i matchids"),BadMatchIDs.Num() );
				for( INT i=0;i<BadMatchIDs.Num();i++ )
				{
					if (GExtendedLogging)
						GWarn->Logf(TEXT("Removing statslines for matchid %d"), BadMatchIDs(i) );

					MySQL.DoSQL( "delete from statsline where matchid='%d'", BadMatchIDs(i) );
				}
			}

			// Update stats
			INT ClientsConnected = 0;
			Parent->ThreadsSem.Wait();
			for( INT i=0;i<Parent->Threads.Num();i++ )
			{
				ClientsConnected += Parent->Threads(i)->ClientCount();
				Stats += Parent->Threads(i)->Stats;
				Parent->Threads(i)->Stats.Clear();
				FLOAT lut = (FLOAT)(appSeconds() - Parent->Threads(i)->LockupDetectTime);
				if( lut > 5.f )
					GWarn->Logf(TEXT("WARNING: Thread %d lockup: %f seconds (%s)"), i, lut, *Parent->Threads(i)->LockupInfo );
			}
			INT ThreadCount = Parent->Threads.Num();
			Parent->ThreadsSem.Signal();

			FMasterServerStats AvgStats;
			AvgStats = Stats;
			AvgStats .Average( appSeconds() - StatStartTime );
			GWarn->Logf(TEXT("-------------------------------------------------------------------------------"));
			GWarn->Logf( TEXT("%d th|%d cli|%d bpsOUT|%d bpsIN|%d Cli/s|%d Svrs/s|%d Dnd/s|%d Q/s|%d MOTD/s"), 
							ThreadCount,
							ClientsConnected,
							AvgStats.BytesSent,
							AvgStats.BytesReceived,
							AvgStats.ClientConnects,
							AvgStats.ServerConnects,
							AvgStats.DeniedConnects,
							AvgStats.Queries,
							AvgStats.MOTDs);
			Stats.Clear();
			GWarn->Logf(TEXT("-------------------------------------------------------------------------------"));
			StatStartTime = appSeconds();
		}

		unguard;
	}
	catch( ... )
	{
		// Crashed.
		GIsGuarded = 0;
		GError->HandleError();
		appRequestExit(1);
	}
}

/*-----------------------------------------------------------------------------
	FMasterServerUdpLink
-----------------------------------------------------------------------------*/

FMasterServerUdpLink::FMasterServerUdpLink( FMasterServerListenLink* InParent )
:	Parent(InParent)
{}

void FMasterServerUdpLink::OnReceivedData( FIpAddr Address, BYTE* Data, INT Length )
{
	guard(FMasterServerUdpLink::OnReceivedData);
	FArchiveUdpReader ArRecv( Data, Length );

	BYTE HeartbeatType;
	INT Code;
	ArRecv << HeartbeatType << Code;

	Parent->ThreadsSem.Wait();
	for( INT i=0;i<Parent->Threads.Num();i++ )
	{
		if( Parent->Threads(i)->CheckReceiveHeartbeat( Address, HeartbeatType, Code ) )
			break;
	}
	Parent->ThreadsSem.Signal();

	unguard;
}

/*-----------------------------------------------------------------------------
	FMasterServerListenLink
-----------------------------------------------------------------------------*/

FMasterServerListenLink::FMasterServerListenLink( INT ListenPort )
:	ThreadsSem(TEXT("ThreadsSem"))
,	DBSem(TEXT("DBSem"))
,	UdpSocket(this)
{
	// Listen on TCP
	Listen( ListenPort );

	// Listen on UDP
	UdpSocket.BindPort( ListenPort );
}

FMasterServerListenLink::~FMasterServerListenLink()
{
}

void FMasterServerListenLink::WaitForConnections( INT WaitTime )
{
	UdpSocket.Poll();
	FTcpLink::WaitForConnections(WaitTime);
}

void FMasterServerListenLink::OnIncomingConnection( FSocketData ConnectionData )
{
	guard(FMasterServerListenLink::OnIncomingConnection);

//	GWarn->Logf(TEXT("Received connection, creating new client to handle..."));

	// Create a new client to handle this incoming connection.
	FMasterServerClientThread* Thread = FindThread();
	new FMasterServerClientLink( ConnectionData, this, Thread );
	Thread->StartThread();
	unguard;
}

FMasterServerClientThread* FMasterServerListenLink::FindThread()
{
	FMasterServerClientThread* Result = NULL;
	for(;;)
	{
		ThreadsSem.Wait();
		for( INT i=Threads.Num()-1;i>=0;i-- )
		{
			if( Threads(i)->Clients.Num() < MAX_CLIENTS_PER_THREAD )
			{
				Result = Threads(i);
				break;
			}
		}
		ThreadsSem.Signal();

		if( Result )
			break;

		ThreadsSem.Wait();
		if( Threads.Num() < MAX_THREADS )
		{
			ThreadsSem.Signal();
			Result = new FMasterServerClientThread(this);
			break;
		}
		ThreadsSem.Signal();

		// Don't return until we can get a free thread.
		GWarn->Logf(TEXT("Waiting for a free thread."));
		appSleep(1);
	}

	return Result;
}

void FMasterServerListenLink::UpdateStats()
{
	ThreadsSem.Wait();
	for( INT i=0;i<Threads.Num();i++ )
		Threads(i)->UpdateStats();
	ThreadsSem.Signal();
}

/*-----------------------------------------------------------------------------
	FMasterServerClientLink
-----------------------------------------------------------------------------*/

FMasterServerClientLink::FMasterServerClientLink( FSocketData InSocketData, FMasterServerListenLink* InParent, FMasterServerClientThread* InThread )
:	FTcpLink(InSocketData)
,	Parent(InParent)
,	Thread(InThread)
,	ClientState(CL_Closed)
,	CDKeyID(-1)
,	ServerID(-1)
,	ServerUsesNAT(0)
,	ServerUsesGamespy(0)
,	ReceivedHeartbeatSem(TEXT("HeartbeatSem"))
,	AcceptStatsThisServer(1)
{
	guard(FMasterServerClientLink::FMasterServerClientLink);

	SetLinkMode( TCPLINK_FArchive );

	// Add to the thread's list of clients.
	Thread->ClientsSem.Wait();
	Thread->Clients.AddItem(this);
	Thread->ClientsSem.Signal();

	// Send challenge to client
	Challenge = FString::Printf(TEXT("%d"), GenerateChallenge());
	*ArSend << Challenge;
	ArSend->Flush();
    ClientState = CL_Challenged;

	UDPWaitTime=0.0f;

	unguard;
}

FMasterServerClientLink::~FMasterServerClientLink()
{
	// ClientSem is already acquired, so this is safe.
	Thread->Clients.RemoveItem(this);
}

void FMasterServerClientLink::OnDataReceived()
{
	guard(FMasterServerClientLink::OnDataReceived);

	INT LoopCount = 0;

	// data received from client
	while( DataAvailable() )
	{
		LoopCount++;
		Thread->LockupInfo = FString::Printf(TEXT("OnDataReceived %s loop %d state %d"), *GetSocketData()->GetString(0), LoopCount, ClientState );

		switch( ClientState )
		{
		case CL_Challenged:
			{
				FString CDKeyHash, Response, ClientType;
				FString Language;
				INT Version=0;
				BYTE Platform = OS_UNKNOWN;
				*ArRecv << CDKeyHash << Response << ClientType << Version << Platform << Language;

				INT		GPUDeviceID = 0,
						GPUVendorID	= 0,
						CPUSpeed	= 0;
				BYTE	CPUType		= 0;

				if( Version >= 3120 && ClientType != SERVER_ID )
				{
					*ArRecv << GPUDeviceID << GPUVendorID << CPUSpeed << CPUType;
				}

				if( ArRecv->IsCriticalError() )
				{
					GWarn->Logf(TEXT("FString serialize error reading challenge response from %s"), *SocketData.GetString(1) );
					Close();
					ClientState = CL_Closed;
					return;
				}
				if( ClientType == CLIENT_ID )				
					Thread->Stats.ClientConnects++;

				else if ( ClientType == SERVER_ID )
				{
					Thread->Stats.ServerConnects++;
					MatchID = 0;
					*ArRecv << MatchID; // will be checked after the UDP arrives
				}
				else		// Handle any legacy clients trying to connect
				{
					GWarn->Logf(TEXT("Denied Legacy Client from %s"),*SocketData.GetString(1));
					FString Denied = TEXT("DENIED");
					*ArSend << Denied;
					ArSend->Flush();
					Close();
					ClientState = CL_Closed;
					return;
				}

				// Handle hacks and cheats
/*
				if ( CDKeyHash==TEXT("5883d0aa3c13be790541b679cd0d0bb8") )
				{
					GWarn->Logf(TEXT("Denied Develop Mode Client from %s"),*SocketData.GetString(1));
					FString Denied = TEXT("DEV_CLIENT");
					*ArSend << Denied;
					ArSend->Flush();
					Close();
					ClientState = CL_Closed;
					return;
				}

				if ( CDKeyHash==TEXT("e0fabcb3639053594d070cf90e791fc4") )
				{
					GWarn->Logf(TEXT("Denied Hacked Client from %s"),*SocketData.GetString(1));
					FString Denied = TEXT("MODIFIED_CLIENT");
					*ArSend << Denied;
					ArSend->Flush();
					Close();
					ClientState = CL_Closed;
					return;
				}

*/				RemoteVersion = Version;
				RemotePlatform = Platform;
				RemoteLanguage = Language;

				RemoteCPUType = CPUType;
				RemoteCPUSpeed = CPUSpeed;
				RemoteGPUVendorID = GPUVendorID;
				RemoteGPUDeviceID = GPUDeviceID;

				if( ClientType!=CLIENT_ID && ClientType!=SERVER_ID )
					CDKeyID = -1;
				else
					CDKeyID = VerifyChallenge( *CDKeyHash, *Challenge, *Response, ClientType==SERVER_ID, *SocketData.GetString(0) );

				if( CDKeyID < 0 ) 
				{
					Thread->Stats.DeniedConnects++;
					//GWarn->Logf(TEXT("Denied client type \"%s\" with CD key hash \"%s\" version %d, platform %d from %s"), *ClientType, *CDKeyHash, RemoteVersion, RemotePlatform, *SocketData.GetString(1) );
					GWarn->Logf(TEXT("Unkown CD-Key \"%s\" from %s"),*CDKeyHash,*SocketData.GetString(1));
					FString Denied = TEXT("UNKNOWN_CDKEY");
					*ArSend << Denied;
					ArSend->Flush();
					Close();
					ClientState = CL_Closed;
					return;
				}
#if 0
				// update master server list
				FString List = TEXT("NEW_MASTER_SERVER_LIST");
				TArray<FString> MasterServers;
				TArray<INT> MasterServerPorts;

				GWarn->Logf(TEXT("Sending New Master Server List to %s"),*SocketData.GetString(1));

				new(MasterServers) FString(TEXT("ut2004master1.epicgames.com"));
				MasterServerPorts.AddItem(28902);
				new(MasterServers) FString(TEXT("ut2004master2.epicgames.com"));
				MasterServerPorts.AddItem(28902);

				*ArSend << List << MasterServers << MasterServerPorts;
				ArSend->Flush();
				Close();
				ClientState = CL_Closed;
				return;
#endif
				// Check if remote version is a demo version
				RemoteIsDemo = (CDKeyID == 0);

				// record visit
				if( CDKeyID != 0)
					Thread->MySQL.DoSQL( "update cdkey set version='%d', platform='%d', gpudeviceid='%d', gpuvendorid='%d', cputype='%d', cpuspeed='%d', lastseen=NOW(), lastseenip='%s', lastseenlang='%s', seencount=seencount+1 where id='%d'", 
						RemoteVersion, Platform, GPUDeviceID, GPUVendorID, CPUType, CPUSpeed, appToAnsi(*SocketData.GetString(0), Thread->MySQL.GetTempAnsiString(64)), Thread->MySQL.FormatSQL(*RemoteLanguage), CDKeyID );

				// Check version
				FString MOTD;
				INT UpgradeVersion = CheckVersion( Version, Platform, *RemoteLanguage, RemoteIsDemo, ClientType==SERVER_ID, 0, &CompatibleMinVersion, &CompatibleMaxVersion, &MOTD );
				if( UpgradeVersion )
				{
					GWarn->Logf(TEXT("Forcing upgrade...") );
					Thread->Stats.AutoUpdates++;
					FString Upgrade = TEXT("NEED_UPGRADE");
					*ArSend << Upgrade << UpgradeVersion;
					if( ClientType == CLIENT_ID )
					{
						TArray<FString> DownloadSites;
						GetDownloadSites( UpgradeVersion, Platform, DownloadSites );
						*ArSend << DownloadSites << MOTD;
					}

					ArSend->Flush();
					Close();
					ClientState = CL_Closed;
					return;
				}

				// Make sure the player isn't banned

				if (CDKeyID>0 && ClientType == CLIENT_ID)
				{
					FString Banned = IsBanned(CDKeyID);
					if (Banned != TEXT("") )
					{	
						GWarn->Logf(TEXT("Denied Banned Client from %s.  Banned until %s"),*SocketData.GetString(1), *Banned);
						FString Denied = TEXT("BANNED_CLIENT");
						*ArSend << Denied << Banned;
						ArSend->Flush();
						Close();
						ClientState = CL_Closed;
						return;
					}
				}

				FString Approved = TEXT("APPROVED");
				*ArSend << Approved;

				if( ClientType == CLIENT_ID ) 
				{

					ClientState = CL_UnderReview;
				}
				else
				{	
					ClientState = CL_ServerWaitingForUDP;
					UDPWaitTime=appSeconds();
					ServerCDKeyHash = CDKeyHash;
				}

				ArSend->Flush();
				if (GExtendedLogging)
					GWarn->Logf(TEXT("Approved client (%d) of type %s [%s]"), CDKeyID, *ClientType, *CDKeyHash );
			}
			break;

		case CL_UnderReview:
			{									
				FString GlobalMD5;

				*ArRecv << GlobalMD5; 

				if ( VerifyGlobalMD5(GlobalMD5, CDKeyID) )
				{
					FString Approved = TEXT("VERIFIED");
					*ArSend << Approved;
					ArSend->Flush();
				}
				else
				{
					FString Denied = TEXT("DENIED");
					*ArSend << Denied;
					ArSend->Flush();
					Close();
					ClientState = CL_Closed;
					return;
				}
				ClientState = CL_ClientChannel;
			}
			break;			

		case CL_ClientChannel:
			{
				BYTE ClientCommand;
				*ArRecv << ClientCommand;

				switch(ClientCommand)
				{
				case CTM_Query:
					{
						TArray<FQueryData> Query;
						*ArRecv << Query;

						Thread->Stats.Queries++;
						TArray<FServerResponseLine> Result;

						// Query database
						GenerateServerList(RemoteVersion, CompatibleMinVersion, CompatibleMaxVersion, Query, Result);


						INT Num = Result.Num();
						*ArSend << Num;

						// Compress and send Results
						BYTE Compressed = 1;
						*ArSend << Compressed;

						ArSend->Flush();

						for( INT i=0;i<Result.Num();i++ )
						{
							if( !Compressed )
							{
								*ArSend << Result(i);
								ArSend->Flush();
							}
							else
							{
								// Compress the FServerResponseLine
								DWORD CompIP			= htonl(FIpAddr( *Result(i).IP, 0 ).Addr);
								_WORD CompPort			= Result(i).Port;
								_WORD CompQueryPort		= Result(i).QueryPort;
								FString CompServerName	= Result(i).ServerName.Left(25);
								FString CompMapName		= Result(i).MapName.Left(16);

								INT j = CompMapName.InStr(TEXT("-"));		// strip extensions
								FString CompGameType = Result(i).GameType.Left(16);
								if( CompGameType == TEXT("xDeathMatch") )
								{
									CompGameType = TEXT("0");
									if( j!=-1 && CompMapName.Caps().Left(j) == TEXT("DM") )
										CompMapName = CompMapName.Mid(j+1);
								}
								else
								if( CompGameType == TEXT("xCTFGame") )
								{
									CompGameType = TEXT("1");
									if( j!=-1 && CompMapName.Caps().Left(j) == TEXT("CTF") )
										CompMapName = CompMapName.Mid(j+1);
								}
								else
								if( CompGameType == TEXT("xBombingRun") )
								{
									CompGameType = TEXT("2");
									if( j!=-1 && CompMapName.Caps().Left(j) == TEXT("BR") )
										CompMapName = CompMapName.Mid(j+1);
								}
								else
								if( CompGameType == TEXT("xTeamGame") )
								{
									CompGameType = TEXT("3");
									if( j!=-1 && CompMapName.Caps().Left(j) == TEXT("DM") )
										CompMapName = CompMapName.Mid(j+1);
								}
								else
								if( CompGameType == TEXT("xDoubleDom") )
								{
									CompGameType = TEXT("4");					
									if( j!=-1 && CompMapName.Caps().Left(j) == TEXT("DOM") )
										CompMapName = CompMapName.Mid(j+1);
								}
								BYTE    CompCurrentPlayers	= Result(i).CurrentPlayers;
								BYTE    CompMaxPlayers		= Result(i).MaxPlayers & 0xff;
								INT     CompFlags			= Result(i).Flags;
								FString CompLevel			= Result(i).SkillLevel;

								*ArSend << CompIP << CompPort << CompQueryPort
										<< CompServerName << CompMapName 
										<< CompGameType << CompCurrentPlayers
										<< CompMaxPlayers << CompFlags;

								// If not the Original Demo, send the Skill Level

								if ( RemoteVersion != 3120 )
									*ArSend << CompLevel;


								ArSend->Flush();
							}
						}
						ClientState = CL_PendingClose;
					}
					break;
				case CTM_GetMOTD:
					{
						Thread->Stats.MOTDs++;

						FString MOTD;
						INT UpgradeVersion = CheckVersion( RemoteVersion, RemotePlatform, *RemoteLanguage, RemoteIsDemo, 0, 1, NULL, NULL, &MOTD );
						*ArSend << MOTD << UpgradeVersion;
						if( UpgradeVersion )
						{
							TArray<FString> DownloadSites;
							GetDownloadSites( UpgradeVersion, RemotePlatform, DownloadSites );
							*ArSend << DownloadSites;
						}
						ArSend->Flush();
						ClientState = CL_PendingClose;
					}
					break;
				case CTM_GetModMOTD:
					{
						INT RevisionLevel;
						BYTE PacketType=0;
						FString MOTD;

						*ArRecv << RevisionLevel;		// Get Ownage Revision Level

						// Send the Message of the Day

						GetModMOTD(RemoteVersion, RemotePlatform, *RemoteLanguage, RemoteIsDemo, &MOTD);
						*ArSend << PacketType << MOTD;
						ArSend->Flush();

						// Send any ownage info

						PacketType++;
						GenerateOwnageList(RevisionLevel);

						// Signal end of Query

						PacketType++;
						*ArSend << PacketType;
						ArSend->Flush();
						ClientState = CL_PendingClose;
					}
					break;
				}
			}
			break;
		case CL_ServerWaitingForUDP:
			{

				if ( appSeconds() > UDPWaitTime + UDPWAITTIMEOUT )
				{

					GWarn->Logf(TEXT("Dropping Server [%s] after %10.3f"),*SocketData.GetString(1),appSeconds()-UDPWaitTime);
					Close();
					ClientState = CL_PendingClose;
					return;
				}

				// Receive server config.
				*ArRecv << ServerUsesNAT << ServerUsesGamespy;

				// Request heartbeats.
				HeartbeatInfo[HB_QueryInterface].WaitingReply	= 1;
				HeartbeatInfo[HB_GamePort].WaitingReply			= 1;
				HeartbeatInfo[HB_GamespyQueryPort].WaitingReply = ServerUsesGamespy;
			}
			break;
		case CL_ServerChannel:
			{
				BYTE Command;
				*ArRecv << Command;

				switch( Command )
				{
				case STM_ClientResponse:
					{
						FString ClientIP, ClientCDKeyHash, Response, GMD5, ClientNick;
						*ArRecv << ClientIP << ClientCDKeyHash << Response << GMD5 << ClientNick;
						ValidateClientResponse( *ClientIP, *ClientCDKeyHash, *Response, *GMD5, *ClientNick );
					}
					break;
				case STM_GameState:
					{
						// Update clients.
						TArray<FString> ClientIPs;
						*ArRecv << ClientIPs;
						INT ConnectedPlayerCount = ClientIPs.Num();
						UpdateClients( ClientIPs );

						// Update server state.
						FServerResponseLine ServerState;
						*ArRecv << ServerState;
						UpdateServerState( &ServerState, ConnectedPlayerCount );
					}
					break;
				case STM_Stats:
					{
						FString StatsLine;
						*ArRecv << StatsLine;
						ReceiveStatsLine( *StatsLine );
					}
					break;
				case STM_ClientDisconnectFailed:
					{
						FString Client;
						*ArRecv << Client;
						GWarn->Logf(TEXT("Server was unable to disconnected unauthenticated client %s."), *Client );
						//!! log
					}
					break;
				case STM_MD5Version:
					{
						INT MaxRevision;
						*ArRecv << MaxRevision;
						CheckPackageMD5Version(MaxRevision);
					}
					break;
				}
			}
			break;
		}
	}

	unguard;
}

void FMasterServerClientLink::PerformHousekeeping()
{
	guard(FMasterServerClientLink::PerformHousekeeping);

	EClientState OldClientState = ClientState;

	// Check to see if any heartbeats have arrived.
	ReceivedHeartbeatSem.Wait();
    while( ReceivedHeartbeats.Num() )
	{
		ReceivedHeartbeat( ReceivedHeartbeats(0).HeartbeatType, ReceivedHeartbeats(0).Address );
		ReceivedHeartbeats.Remove(0);
	}
	ReceivedHeartbeatSem.Signal();

	if( ClientState==CL_ServerChannel && OldClientState==CL_ServerWaitingForUDP )
	{
		// validate previously received MatchID and/or assign a new one.
		AssignMatchID();
		// Assign ServerID
		UpdateServerState( NULL, 0 );

		// Send the matchID to the client.
		BYTE SendCommand = MTS_MatchID;
		*ArSend << SendCommand << MatchID;
		ArSend->Flush();
	}

	// Check to see if we need to re-request any heartbeats we have yet to receive.
	if( ClientState == CL_ServerWaitingForUDP )
	{
		for( INT i=HB_QueryInterface;i<=HB_GamespyQueryPort;i++ )
		{
			if( HeartbeatInfo[i].WaitingReply && appSeconds()-HeartbeatInfo[i].RequestSendTime > HEARTBEAT_TIMEOUT )
			{
				HeartbeatInfo[i].RequestCount++;
				HeartbeatInfo[i].RequestSendTime = appSeconds();
				BYTE Success = 0;
				BYTE HeartbeatType = i;
				INT HeartbeatCode = SocketData.Socket;
				*ArSend << Success << HeartbeatType << HeartbeatCode;
				ArSend->Flush();
			}
		}
	}
	unguard;
}

void FMasterServerClientLink::ReceivedHeartbeat( BYTE HeartbeatType, FIpAddr Addr )
{
	if( ClientState == CL_ServerWaitingForUDP && HeartbeatType>=HB_QueryInterface && HeartbeatType<=HB_GamespyQueryPort )
	{
		HeartbeatInfo[HeartbeatType].WaitingReply = 0;
		HeartbeatInfo[HeartbeatType].NatPort = Addr.Port;

		// Check if there are any outstanding heartbeats.
		if( !HeartbeatInfo[HB_QueryInterface].WaitingReply &&
			!HeartbeatInfo[HB_GamePort].WaitingReply &&
			!HeartbeatInfo[HB_GamespyQueryPort].WaitingReply )
		{
			ClientState = CL_ServerChannel;
			BYTE Success = 1;
			INT HeartbeatPeriod = 120;
			*ArSend << Success << HeartbeatPeriod;
			*ArSend << HeartbeatInfo[HB_QueryInterface].NatPort << HeartbeatInfo[HB_GamePort].NatPort << HeartbeatInfo[HB_GamespyQueryPort].NatPort;
			ArSend->Flush();
		}
	}
}

void FMasterServerClientLink::ReceiveStatsLine( const TCHAR* StatsLine )
{
	guard(FMasterServerClientLink::ReceiveStatsLine);
	if( MatchID > 0 && GAcceptStats )
	{
		FString ip = SocketData.GetString(0) + FString(TEXT(":")) + FString(appItoa(HeartbeatInfo[HB_GamePort].NatPort));

		FString TempLine = StatsLine;

		UBOOL EndGame = 0;
		UBOOL NewGame = 0;
		if( TempLine.InStr(TEXT("\tC\t")) != -1 )
		{
			TArray<FString> LineParts;
			INT Count = SnipAppart( LineParts, TempLine, TEXT("\t") );
			//	0	1	2	3   4   5
			//	33	C	0	CDK UN  ENCPW
			if( Count==6 && LineParts(1)==TEXT("C") )
			{
				if( LineParts(3) == TEXT("") || LineParts(4) == TEXT("") || LineParts(5) == TEXT("") )
				{
					// if no cd key, username or password
					LineParts(4) = TEXT("");
					LineParts(5) = TEXT("");
				}
				else
				{
					// decode stats username/password.
					if( LineParts(3) == TEXT("238c7dd4ec4a065e2314c1c8b4d41ca6") ) //!! DEMO
					{
						LineParts(5) = DecryptWithCDKeyHash( *LineParts(5), TEXT("STATS"), TEXT("UT2004-UTDEMO-UTDEMO-UT2004") );
					}
					else
					{
						FQueryResult* CDKeyQuery = Thread->MySQL.Query( "select md5(concat(cdkey, 'STATS')) from cdkey where md5hash='%s'",
																		Thread->MySQL.FormatSQL(*LineParts(3)) );
						FQueryField** Row;
						if( (Row = CDKeyQuery->FetchNextRow())!=NULL )
							LineParts(5) = DecryptWithCDKeyHash( *LineParts(5), *Row[0]->AsString(), NULL );
						else
						{
							GWarn->Logf(TEXT("Couldn't find CD key for stats user %s/%s"), *LineParts(4), *LineParts(5) );
							//!! should never happen - master server has validated the clients by now.
							LineParts(4) = TEXT("");
							LineParts(5) = TEXT("");
						}
						delete CDKeyQuery;
					}
				}

				// if no username, set a fallback account
				if( LineParts(4) == TEXT("") )
				{
					LineParts(4) = TEXT("NOUSER");
					LineParts(5) = TEXT("00!!QWERTY!!00");	// random non-existent password
				}

				LineParts.Remove(3);          
				TempLine = JoinTogether( LineParts, TEXT("\t") );
			}
		}
		else
		if( TempLine.InStr(TEXT("\tSI\t")) != -1 )
		{
			TArray<FString> LineParts;
			INT Count = SnipAppart( LineParts, TempLine, TEXT("\t") );
			//	0	1	2			3   4   5					6			7
			//	33	SI	JoesServer	0	Joe	joe@epicgames.com	0.0.0.0:0	GameRules
			if( Count==8 && LineParts(1)==TEXT("SI") )
			{
				LineParts(6) = ip;
				TempLine = JoinTogether( LineParts, TEXT("\t") );
			}
		}
		else
		if( TempLine.InStr(TEXT("\tEG\t")) != -1 )
		{
			TArray<FString> LineParts;
			INT Count = SnipAppart( LineParts, TempLine, TEXT("\t") );
			//	0	1	
			//	33	EG	
			if( Count>=2 && LineParts(1)==TEXT("EG") )
				EndGame = 1;
		}
		else
		if( TempLine.InStr(TEXT("\tNG\t")) != -1 )
		{
			TArray<FString> LineParts;
			INT Count = SnipAppart( LineParts, TempLine, TEXT("\t") );
			//	0	1	
			//	33	NG	
			if( Count>=2 && LineParts(1)==TEXT("NG") )
				NewGame = 1;
		}

		if( ServerID <= 0 )
			GWarn->Logf( TEXT("Warning: stats line sent for ServerID %d"), ServerID );

		// Store stats line in the database ready for processing.
		Thread->MySQL.DoSQL("insert into statsline (matchid, serverid, line) values ('%d', '%d', '%s')", MatchID, ServerID, Thread->MySQL.FormatSQL(*TempLine) );

		if( NewGame )
			Thread->MySQL.DoSQL("insert into statsmatches (matchid, serverid, lastupdated, matchstarted, matchcomplete) values ('%d','%d', NOW(), NOW(), NULL)", MatchID, ServerID );
		else
		if( EndGame )
			Thread->MySQL.DoSQL("update statsmatches set lastupdated=NOW(), matchcomplete='T' where matchid='%d'", MatchID );
		else
			Thread->MySQL.DoSQL("update statsmatches set lastupdated=NOW() where matchid='%d'", MatchID );       
	}
	unguard;
}

INT FMasterServerClientLink::GenerateChallenge()
{
	INT Challenge = appRand();
	return Challenge;
}

INT FMasterServerClientLink::VerifyChallenge( const TCHAR* CDKeyHash, const TCHAR* Challenge, const TCHAR* Response, UBOOL IsServer, const TCHAR* AuthIP )
{
	guard(FMasterServerClientLink::VerifyChallenge);

	if( !appStrcmp( CDKeyHash, TEXT("238c7dd4ec4a065e2314c1c8b4d41ca6")) )	// UT2K4 Demo key - UT2004-UTDEMO-UTDEMO-UT2004
		return 0;	// demo
	else
	if( !appStrcmp( CDKeyHash, TEXT("5883d0aa3c13be790541b679cd0d0bb8")) )	// Temp approve the set command cheat
	{
		return 0;
	}
	else
	if( !appStrcmp( CDKeyHash, TEXT("d41d8cd98f00b204e9800998ecf8427e"))  )
	{
		// Blank CD key, retail.
		return -1;
	}

/*
	// Check CD key cache tree.

	if( FCDKeyCacheTree::Cache )
	{
		BYTE BinHash[16];
		MD5StrToHash( CDKeyHash, BinHash );
		FCDKeyCacheNode* Node = FCDKeyCacheTree::Cache->FindNode( BinHash );
		if( Node && !Node->Item->Disabled && (!Node->Item->ServerOnly || IsServer) )
		{
			TCHAR PotentialKey[24];
			appStrcpy( PotentialKey, Node->Item->CDKey );
			if( Node->UsingSpaces )
			{			
				PotentialKey[5] = ' ';
				PotentialKey[11] = ' ';
				PotentialKey[17] = ' ';
			}
		}
	}

	if( FCDKeyMemCache::Cache && FCDKeyMemCache::Find(CDKeyHash)==NULL )
		return -1;
*/	

	// Check for repeat offenders
	FQueryResult* BadHashQuery = Thread->MySQL.Query( "select id from badhash where ip='%s' and lastseen > date_sub(now(), interval 5 minute)",
														Thread->MySQL.FormatSQL(AuthIP) );
	if( BadHashQuery->NumRows() > 1 )
	{
		// repeat offender
		delete BadHashQuery;
		return -1;
	}
	delete BadHashQuery;

	FQueryResult* CDKeyQuery = Thread->MySQL.Query(	"select id, cdkey from cdkey "
													"where md5hash='%s' "
													"and md5(concat(cdkey, '%s'))='%s' "
													"and disabled is NULL " 
													"%s", 
													Thread->MySQL.FormatSQL(CDKeyHash), 
													Thread->MySQL.FormatSQL(Challenge),
													Thread->MySQL.FormatSQL(Response),
													IsServer ? "" : "and serveronly is NULL" );

	FQueryField** Row;
	if( (Row = CDKeyQuery->FetchNextRow())!=NULL )
	{
		INT id				= Row[0]->AsInt();
		GWarn->Logf(TEXT("Found: %i"),id);
		delete CDKeyQuery;
		return id;
	}

	delete CDKeyQuery;

	// log invalid login attempt
	Thread->MySQL.DoSQL( "insert into badhash (md5hash, ip, clienttype, lastseen, platform, version) values ('%s', '%s', '%s', NOW(), '%d', '%d')",
							Thread->MySQL.FormatSQL(CDKeyHash),
							Thread->MySQL.FormatSQL(AuthIP),
							Thread->MySQL.FormatSQL(IsServer ? SERVER_ID : CLIENT_ID),
							RemotePlatform,
							RemoteVersion											
						);
	return -1;

	unguard;
}

static const TCHAR* GetCondition( BYTE Q )
{
	switch( Q )
	{
	case QT_Equals:
		return TEXT("=");
	case QT_NotEquals:
		return TEXT("!=");
	case QT_LessThan:
		return TEXT("<");
	case QT_LessThanEquals:
		return TEXT("<=");
	case QT_GreaterThan:
		return TEXT(">");
	case QT_GreaterThanEquals:
		return TEXT(">=");
	}
	return TEXT("");
}

void FMasterServerClientLink::GenerateServerList( INT Version, INT MinVersion, INT MaxVersion, TArray<FQueryData>& Query, TArray<FServerResponseLine>& Result )
{
	guard(FMasterServerClientLink::ServerList);

	if( !Query.Num() )
		return;

	// !! Don't return servers for old demo.
	if( RemoteVersion < 2000 )
		return;

	INT cache;
	for( cache=0;cache<Thread->ListCache.Num();cache++ )
	{
		FMasterServerListCache& lc = Thread->ListCache(cache);
		if( lc.Version == Version && 
			lc.MinVersion == MinVersion && 
			lc.MaxVersion == MaxVersion &&
			lc.Query.Num() == Query.Num() &&
			lc.DemoOnly == RemoteIsDemo )
		{
			UBOOL SameQuery=1;
			for( INT i=0;i<Query.Num();i++ )
				if( Query(i).Key != lc.Query(i).Key || 
					Query(i).Value != lc.Query(i).Value || 
					Query(i).QueryType != lc.Query(i).QueryType )
				{
					SameQuery = 0;
					// If it's expired, remove old item from list cache.
					if( appSeconds() - lc.LastUpdated > 60.0 )
					{
						Thread->ListCache.Remove(cache);
						cache--;
					}
					break;
				}
			if( SameQuery )
			{
				if( appSeconds() - lc.LastUpdated < 60.0 )
				{
					Result = lc.Result;
					return;
				}
				break;
			}
		}
	}

	TCHAR Temp[256];
	UBOOL JoinPlayers = 0;
	FString WhereClause = FString::Printf(TEXT("where onlineservers.version>='%d' and servers.banned is null and (servers.tempbantil is null or servers.tempbantil<NOW()) and onlineservers.exclude is null "), MinVersion);
	if( MaxVersion )
		WhereClause += FString::Printf(TEXT("and onlineservers.version<='%d' "), MaxVersion);

	// Only return demo servers to demo clients.

	if( RemoteIsDemo )
		WhereClause += FString::Printf(TEXT("and onlineservers.cdkeyid=0 "));

	// search for buddy options
	INT BuddyCount = 0;
	for( INT i=0;i<Query.Num();i++ )
	{
		FString Buddy = Query(i).Value.Left(32);

		if( Query(i).Key.Locs() == TEXT("buddy") && BuddyCount < 32 )
		{
			if( !BuddyCount )
				WhereClause += TEXT(" and (");
			else
				WhereClause += TEXT(" or");
			
			// wildcards
			UBOOL HasLeftStar = 0;
			UBOOL HasRightStar = 0;
			if( Buddy.Left(1)==TEXT("*") )
			{
				HasLeftStar = 1;
				Buddy = Buddy.Mid(1);
			}
			if( Buddy.Right(1)==TEXT("*") )
			{
				HasRightStar = 1;
				Buddy = Buddy.Left(Buddy.Len()-1);
			}
			if( (HasLeftStar || HasRightStar) && Buddy.Len() > 2 )
			{
				WhereClause += FString::Printf(TEXT(" onlineserverplayers.playername like '%s%s%s'"), 
					HasLeftStar ? TEXT("%") : TEXT(""),
					appFromAnsi(Thread->MySQL.FormatSQL(*Buddy),Temp),
					HasRightStar ? TEXT("%") : TEXT("") );
			}
			else
				WhereClause += FString::Printf(TEXT(" onlineserverplayers.playername='%s'"), appFromAnsi(Thread->MySQL.FormatSQL(*Buddy),Temp) );
			BuddyCount++;
		}
	}
	if( BuddyCount )
	{
		WhereClause += TEXT(") ");
		JoinPlayers = 1;
		cache = -1;
	}

	// search for non-buddy options
	UBOOL RealPlayerCounts=0;
	for( INT i=0;i<Query.Num();i++ )
	{
		// rjp -- just skip if disabled
		if ( Query(i).QueryType == QT_Disabled )
			continue;

		if( Query(i).Key.Locs() == TEXT("custom") && Query(i).Value.Locs().InStr(TEXT("excludebots")) != -1 )
			RealPlayerCounts = 1;
		if( Query(i).Key.Locs() == TEXT("gametype") )
		{
			if( Query(i).Value.Locs() == TEXT("xDeathMatch") && (Version==1077||Version==1080) ) //!!OLDVER
				WhereClause += FString::Printf(TEXT(" and (onlineservers.gametype%s'xDeathMatch' or onlineservers.gametype%s'xTeamGame') "), GetCondition(Query(i).QueryType), GetCondition(Query(i).QueryType) );
			else
				WhereClause += FString::Printf(TEXT(" and (onlineservers.gametype%s'%s') "), GetCondition(Query(i).QueryType), appFromAnsi(Thread->MySQL.FormatSQL(*Query(i).Value.Left(64)),Temp) );
		}
		if( Query(i).Key.Locs() == TEXT("mapname") )
			WhereClause += FString::Printf(TEXT(" and (onlineservers.mapname%s'%s') "), GetCondition(Query(i).QueryType), appFromAnsi(Thread->MySQL.FormatSQL(*Query(i).Value.Left(64)),Temp) );
		if( Query(i).Key.Locs() == TEXT("currentplayers") )
			WhereClause += FString::Printf(TEXT(" and (onlineservers.realcurrentplayers%s%d) "), GetCondition(Query(i).QueryType), appAtoi(*Query(i).Value) );
		if( Query(i).Key.Locs() == TEXT("maxplayers") )
			WhereClause += FString::Printf(TEXT(" and (onlineservers.maxplayers%s%d) "), GetCondition(Query(i).QueryType), appAtoi(*Query(i).Value) );
		if( Query(i).Key.Locs() == TEXT("freespace") )
			WhereClause += FString::Printf(TEXT(" and (onlineservers.maxplayers-onlineservers.realcurrentplayers%s%d) "), GetCondition(Query(i).QueryType), appAtoi(*Query(i).Value) );
		if( Query(i).Key.Locs() == TEXT("password") && Query(i).Value.Locs() == TEXT("false") )
			WhereClause += TEXT(" and (onlineservers.haspassword is null) ");
		if( Query(i).Key.Locs() == TEXT("stats") )
		{
			if( Query(i).Value.Locs() == TEXT("true") )
				WhereClause += TEXT(" and (servers.matchid>0) ");
			else
			if( Query(i).Value.Locs() == TEXT("false") )
				WhereClause += TEXT(" and (servers.matchid=0) ");
		}
		if( Query(i).Key.Locs() == TEXT("custom") && Query(i).Value.Locs().InStr(TEXT("warez")) != -1 )
			WhereClause += FString::Printf(TEXT(" and (onlineservers.cdkeyid='1618615') ") );
		if( Query(i).Key.Locs() == TEXT("custom") && Query(i).Value.Locs().InStr(TEXT("dedicated")) != -1 )
			WhereClause += FString::Printf(TEXT(" and (onlineservers.dedicated='T') ") );
		if( Query(i).Key.Locs() == TEXT("custom") && Query(i).Value.Locs().InStr(TEXT("demoonly")) != -1 )
			WhereClause += FString::Printf(TEXT(" and onlineservers.cdkeyid=0 "));
		INT p=-1;
		if( Query(i).Key.Locs() == TEXT("custom") && (p=Query(i).Value.Locs().InStr(TEXT("platform="))) != -1 )
			WhereClause += FString::Printf(TEXT(" and servers.platform=%d "), appAtoi(*Query(i).Value.Mid(p+9)) );
		if( Query(i).Key.Locs() == TEXT("custom") && (p=Query(i).Value.Locs().InStr(TEXT("version="))) != -1 )
			WhereClause += FString::Printf(TEXT(" and servers.version=%d "), appAtoi(*Query(i).Value.Mid(p+8)) );
		if( Query(i).Key.Locs() == TEXT("custom") && Query(i).Value.Locs().InStr(TEXT("nobots")) != -1 )
			WhereClause += FString::Printf(TEXT(" and (onlineservers.realcurrentplayers>=onlineservers.currentplayers-1) "));
		if( Query(i).Key.Locs() == TEXT("mutator") )
		{
			if( Query(i).QueryType == QT_Equals )
			{
				if( Query(i).Value.Locs() == TEXT("") )
					WhereClause += FString::Printf(TEXT(" and (onlineservers.mutators='') ") );
				else
					WhereClause += FString::Printf(TEXT(" and (onlineservers.mutators like '%%|%s|%%') "), appFromAnsi(Thread->MySQL.FormatSQL(*Query(i).Value.Left(64)), Temp)  );
			}
			else
			if( Query(i).QueryType == QT_NotEquals )
			{
				if( Query(i).Value.Locs() != TEXT("") )
					WhereClause += FString::Printf(TEXT(" and (onlineservers.mutators not like '%%|%s|%%') "), appFromAnsi(Thread->MySQL.FormatSQL(*Query(i).Value.Left(64)), Temp)  );
			}
		}
		if( Query(i).Key.Locs()==TEXT("nobots") && Query(i).QueryType==QT_Equals && Query(i).Value.Locs() == TEXT("true") )
			WhereClause += FString::Printf(TEXT(" and (onlineservers.realcurrentplayers>=onlineservers.currentplayers-1) ") );
		if( Query(i).Key.Locs()==TEXT("weaponstay") && Query(i).QueryType==QT_Equals )
		{
			if( Query(i).Value.Locs() == TEXT("true") )
				WhereClause += FString::Printf(TEXT(" and (onlineservers.weaponstay='T') ") );
			else
			if( Query(i).Value.Locs() == TEXT("false") )
				WhereClause += FString::Printf(TEXT(" and (onlineservers.weaponstay is null) ") );
		}
		if( Query(i).Key.Locs()==TEXT("transloc") && Query(i).QueryType==QT_Equals )
		{
			if( Query(i).Value.Locs() == TEXT("true") )
				WhereClause += FString::Printf(TEXT(" and (onlineservers.transloc='T') ") );
			else
			if( Query(i).Value.Locs() == TEXT("false") )
				WhereClause += FString::Printf(TEXT(" and (onlineservers.transloc is null) ") );
		}
		if( Query(i).Key.Locs()==TEXT("standard") && Query(i).QueryType==QT_Equals && Query(i).Value.Locs()==TEXT("true") )
			WhereClause += FString::Printf(TEXT(" and (onlineservers.standard='T') ") );
		if( Query(i).Key.Locs() == TEXT("gamespeed") )
			WhereClause += FString::Printf(TEXT(" and (onlineservers.gamespeed%s%f) "), GetCondition(Query(i).QueryType), appAtof(*Query(i).Value) );
		if ( Query(i).Key == TEXT("category") )
			WhereClause += FString::Printf(TEXT(" and (onlineservers.skilllevel='%s') "), appFromAnsi(Thread->MySQL.FormatSQL(*Query(i).Value.Left(64)),Temp) );
	}

	Thread->MySQL.DoSQL(	"create temporary table _serverlist "
							"select distinct onlineservers.serverid, servers.ip, servers.port, "
							"onlineservers.queryport, "
							"onlineservers.servername, "
							"onlineservers.mapname, "
							"onlineservers.gametype, "
							"onlineservers.%scurrentplayers currentplayers, "
							"onlineservers.maxplayers, "
							"onlineservers.haspassword, "
							"servers.matchid, "
							"onlineservers.version, "
							"onlineservers.dedicated, "
							"onlineservers.mutators, "
							"onlineservers.standard, "
							"onlineservers.skilllevel from "
							"%s"
							"left join servers on servers.serverid=onlineservers.serverid "
							" %s order by serverid",
							RealPlayerCounts ? "real" : "",
							JoinPlayers
								? "onlineserverplayers left join onlineservers on onlineservers.serverid=onlineserverplayers.serverid " 
								: "onlineservers ",
							appToAnsi( *WhereClause, Thread->MySQL.GetTempAnsiString(8192) ) );


	// Get server list.
	FQueryResult* ServerQuery = Thread->MySQL.Query(	"select serverid, ip, port, queryport, servername, mapname, "
		"gametype, currentplayers, maxplayers, haspassword, matchid, version, dedicated, mutators, standard, skilllevel from _serverlist" );

	FQueryField** ServerRow;

	while( (ServerRow = ServerQuery->FetchNextRow()) != NULL )
	{
		INT i = Result.AddZeroed();
		Result(i).ServerID		= ServerRow[0]->AsInt();
		Result(i).IP			= ServerRow[1]->AsString();
		Result(i).Port			= ServerRow[2]->AsInt();
		Result(i).QueryPort		= ServerRow[3]->AsInt();
		Result(i).ServerName	= ServerRow[4]->AsString();
		Result(i).MapName		= ServerRow[5]->AsString();
		Result(i).GameType		= ServerRow[6]->AsString();
		Result(i).SkillLevel	= ServerRow[15]->AsString();
		Result(i).CurrentPlayers= ServerRow[7]->AsInt();

		INT Flags = 0;

		if( !ServerRow[9]->IsNull() )
			Flags |= 1;		// passworded

		if( ServerRow[10]->AsInt() != 0 )
			Flags |= (1 << 1);		// stats

		if( ServerRow[11]->AsInt() >= GLatestVersion )
			Flags |= (1 << 2);		// latest version

		if( ServerRow[12]->IsNull() )
			Flags |= (1 << 3);		// listen server

		FString Mutators = ServerRow[13]->AsString().Caps();
		if( Mutators.InStr(TEXT("|MUTINSTAGIB|")) != -1 || Mutators.InStr(TEXT("|MUTZOOMINSTAGIB|")) != -1 )
			Flags |= (1 << 4);		// instagib

		if( !ServerRow[14]->IsNull() )
			Flags |= (1 << 5);		// standard

		if( Mutators.InStr(TEXT("|MUTUTCLASSIC|")) != -1  )
			Flags |= (1 << 6);		// UTClassic

		Result(i).Flags = Flags;
		Result(i).MaxPlayers	= ServerRow[8]->AsInt();
	}

	delete ServerQuery;

	Thread->MySQL.DoSQL( "drop table _serverlist " );


	// Update cache
	if( cache == Thread->ListCache.Num() )
		cache = Thread->ListCache.AddZeroed();
	if( cache > 0 )
	{
		//GWarn->Logf(TEXT("Updating cache for %s/%s"), *Query(0).Key, *Query(0).Value );

		Thread->ListCache(cache).Version = Version;
		Thread->ListCache(cache).MinVersion = MinVersion;
		Thread->ListCache(cache).MaxVersion = MaxVersion;
		Thread->ListCache(cache).DemoOnly = RemoteIsDemo;
		Thread->ListCache(cache).Query = Query;
		Thread->ListCache(cache).Result = Result;
		Thread->ListCache(cache).LastUpdated = appSeconds();
	}
	unguard;
}

static int GetPlayerIndex( TArray<FPlayerResponseLine>& Array, INT PlayerNum )
{
	for( INT i=0;i<Array.Num();i++ )
		if( Array(i).PlayerNum == PlayerNum )
			return i;
	return INDEX_NONE;
}

#if 0
static INT GetKeyValueIndex( TArray<FKeyValuePair>& Array, const TCHAR* Key,const TCHAR* Value  )
{
	for( INT i=0;i<Array.Num();i++ )
		if( Array(i).Key == Key && Array(i).Value == Value )
			return i;
	return INDEX_NONE;
}
#endif

void FMasterServerClientLink::AssignMatchID()
{
	guard(FMasterServerClientLink::CheckPrevMatchID);

	if( MatchID < 0 || RemoteVersion < 2000 || !GAcceptStats || !AcceptStatsThisServer )
	{
		// Stats disabled.
		MatchID = 0;
	}
	else
	{
		if( MatchID != 0 )
		{
			// verify MatchID matches servers record.
			FQueryResult* ServersQuery = Thread->MySQL.Query(	"select serverid from servers where ip='%s' and port='%d' and matchid='%d' ",
																	Thread->MySQL.FormatSQL(*SocketData.GetString(0)),
																	HeartbeatInfo[HB_GamePort].NatPort,
																	MatchID );
			FQueryField** ServersRow = ServersQuery->FetchNextRow();
			if( !ServersRow )
				MatchID = 0;

			delete ServersQuery;
		}
		
		//!!We could assign MatchID = 0 here to deny stats

		if( MatchID == 0)
		{
			// assign a new matchid
			Thread->MySQL.DoSQL("insert into matchid values ()");
			MatchID = Thread->MySQL.GetInsertID();
			Thread->MySQL.DoSQL("delete from matchid where id < %d", MatchID);
		}
	}
	unguard;
}

void FMasterServerClientLink::UpdateServerState( FServerResponseLine* ServerState, INT ConnectedPlayerCount )
{
	guard(FMasterServerClientLink::UpdateServerState);

	FString TempIP = SocketData.GetString(0);
	INT TempPort = HeartbeatInfo[HB_GamePort].NatPort;

	if( ServerID == -1 )
	{
		// find serverid from IP/port.
		FQueryResult* ServersQuery = Thread->MySQL.Query(	"select serverid from servers where ip='%s' and port='%d' ",
															Thread->MySQL.FormatSQL(*TempIP),
															TempPort );

		FQueryField** ServersRow = ServersQuery->FetchNextRow();
		if( ServersRow )
		{
			ServerID = ServersRow[0]->AsInt();
			delete ServersQuery;
		}
		else
		{
            delete ServersQuery;
			// create a new servers record.
			Thread->MySQL.DoSQL(	"insert into servers (ip, port, lastseen) "
									"values('%s', '%d', NOW()) ",
									Thread->MySQL.FormatSQL(*TempIP),
									TempPort );
			ServerID = Thread->MySQL.GetInsertID();
		}
	}

	if( ServerState )
	{
		UBOOL IsDedicated=0;
		UBOOL HasPassword=0;
		UBOOL IsStandard=1;
		UBOOL HasBots=0;
		UBOOL HasWeaponStay=0;
		UBOOL HasTransloc=0;
		FString AdminName, AdminEmail, MutatorString;
		FLOAT GameSpeed=1.f;
		UBOOL ExcludeServer = 0;

		for( INT i=0;i<ServerState->ServerInfo.Num();i++ )
		{

//			GWarn->Logf(TEXT("Server State: [%s][%s]"),*ServerState->ServerInfo(i).Key, *ServerState->ServerInfo(i).Value);

			if( ServerState->ServerInfo(i).Key==TEXT("servermode") && ServerState->ServerInfo(i).Value==TEXT("dedicated") )
				IsDedicated = 1;
			if( ServerState->ServerInfo(i).Key==TEXT("gamepassword") && ServerState->ServerInfo(i).Value==TEXT("true") )
			{
				HasPassword = 1;
				IsStandard = 0;
			}
			if( ServerState->ServerInfo(i).Key==TEXT("gamespeed") )
			{
				GameSpeed = appAtof(*ServerState->ServerInfo(i).Value);
				if( GameSpeed != 1.f )
					IsStandard = 0;
			}
			if( ServerState->ServerInfo(i).Key==TEXT("adminname") )
				AdminName = ServerState->ServerInfo(i).Value;
			if( ServerState->ServerInfo(i).Key==TEXT("adminemail") )
				AdminEmail = ServerState->ServerInfo(i).Value;
			if( ServerState->ServerInfo(i).Key==TEXT("mutator") )
			{
				if( MutatorString.Len() < 2048 )
					MutatorString = MutatorString + TEXT("|") + ServerState->ServerInfo(i).Value + TEXT("|");
				FString CapsMut = ServerState->ServerInfo(i).Value.Caps();
				if( CapsMut != TEXT("MUTUTSECURE") &&
					CapsMut != TEXT("UT2VOTE") &&
					CapsMut != TEXT("MAPVOTE") &&
					CapsMut != TEXT("IRC_MUT") &&
					CapsMut != TEXT("IRC_SETTINGS") &&
					CapsMut != TEXT("MODOSSCOREBOARDMUT") )
					IsStandard = 0;
			}
			if( ServerState->ServerInfo(i).Key==TEXT("minplayers") && appAtoi(*ServerState->ServerInfo(i).Value) != 0 )
			{
				HasBots = 1;
//				IsStandard = 0;
			}
			if( ServerState->ServerInfo(i).Key==TEXT("weaponstay") )
			{
				HasWeaponStay = ServerState->ServerInfo(i).Value.Caps()==TEXT("TRUE");
				if( !HasWeaponStay )
					IsStandard = 0;
			}
			if( ServerState->ServerInfo(i).Key==TEXT("translocator") )
			{
				HasTransloc = ServerState->ServerInfo(i).Value.Caps()==TEXT("TRUE");
				if( ServerState->GameType == TEXT("xCTFGame") && !HasTransloc )
					IsStandard = 0;
				else
				if( ServerState->GameType == TEXT("xDeathMatch") && HasTransloc )
					IsStandard = 0;
				else
				if( ServerState->GameType == TEXT("xTeamGame") && HasTransloc )
					IsStandard = 0;
				else
				if( ServerState->GameType == TEXT("xDoubleDom") && HasTransloc )
					IsStandard = 0;
				else
				if( ServerState->GameType == TEXT("xBombingRun") && !HasTransloc )
					IsStandard = 0;
			}
		}
		// Hardcode IP based on connection.
		ServerState->IP = TempIP;
		ServerState->Port = TempPort;
		ServerState->QueryPort = HeartbeatInfo[HB_QueryInterface].NatPort;
		INT GSpyPort = HeartbeatInfo[HB_GamespyQueryPort].NatPort;
		// Exclude servers from list based on various criteria
		if( RemoteVersion==2122 || RemoteVersion==2126 )
			ExcludeServer = 1;

		// update last seen info in servers record.
		Thread->MySQL.DoSQL(	"update servers set "
								"lastseen=NOW(), "
								"cdkeyid='%d', "
								"servername='%s', "
								"adminname='%s', "
								"adminemail='%s', "
								"version='%d', "
								"matchid='%d', "
								"platform='%d' "
								"where serverid='%d' ",
								CDKeyID,
								Thread->MySQL.FormatSQL(*ServerState->ServerName),
								Thread->MySQL.FormatSQL(*AdminName),
								Thread->MySQL.FormatSQL(*AdminEmail),
								RemoteVersion,
								MatchID,
								RemotePlatform,
								ServerID );

		// Look for server in onlineservers
		FQueryResult* OnlineServersQuery = Thread->MySQL.Query(	"select serverid from onlineservers where serverid='%d'", ServerID );
		UBOOL ServerExists = OnlineServersQuery->NumRows()!=0;
		delete OnlineServersQuery;

		if( ServerExists )
		{
			// rjp --
			// Adjust the reported skill level to > 0 if the server isn't standard
			// Don't want to modify the real ServerState->SkillLevel value, so just copy it to another FString and modify that instead
			FString ServerSkillLevel = ServerState->SkillLevel;
			if ( !IsStandard && ServerSkillLevel == TEXT("0") )
				ServerSkillLevel = TEXT("1");

			if (CDKeyID==1)
				GWarn->Logf(TEXT("Updating Found a server (%s) that's Instagib %i"),*ServerState->ServerName,CDKeyID);

			// update
			Thread->MySQL.DoSQL(	"update onlineservers set "
									"expires=DATE_ADD(NOW(), INTERVAL 1 MINUTE), "
									"queryport='%d', "
									"gspyport='%d', "
									"servername='%s', "
									"mapname='%s', "
									"gametype='%s', "
									"currentplayers='%d', "
									"maxplayers='%d', "
									"cdkeyid='%d', "
									"servertype='%s', "
									"version='%d', "
									"dedicated=%s, "
									"haspassword=%s, "
									"standard=%s, "
									"mutators='%s', "
									"bots=%s, "
									"weaponstay=%s, "
									"transloc=%s, "
									"gamespeed='%f', "
									"exclude=%s, "
									"realcurrentplayers='%d', "
									"skilllevel='%s' "
									"where serverid='%d'",
									ServerState->QueryPort,
									GSpyPort,
									Thread->MySQL.FormatSQL(*ServerState->ServerName),
									Thread->MySQL.FormatSQL(*ServerState->MapName),
									Thread->MySQL.FormatSQL(*ServerState->GameType),
									ServerState->CurrentPlayers,
									ServerState->MaxPlayers,
									CDKeyID,
									"",
									RemoteVersion,
									IsDedicated ? "'T'" : "NULL",
									HasPassword ? "'T'" : "NULL",
									IsStandard ? "'T'" : "NULL",
									Thread->MySQL.FormatSQL(*MutatorString),
									HasBots ? "'T'" : "NULL",
									HasWeaponStay ? "'T'" : "NULL",
									HasTransloc ? "'T'" : "NULL",
									GameSpeed,
									ExcludeServer ? "'T'" : "NULL",
									IsDedicated ? ConnectedPlayerCount : ConnectedPlayerCount + 1,
									Thread->MySQL.FormatSQL(*ServerSkillLevel),
									ServerID );

			// Read database's versions of server and player details
			TArray<INT> DBServerDetailIDs;
			TArray<INT> DBPlayerDetailIDs;
		}
		else
		{
			//!! Check for an existing server with the same IP and the gameport the same as my query port
			FQueryResult* BadPortQuery = Thread->MySQL.Query(	"select servers.serverid from servers, onlineservers "
																"where onlineservers.serverid=servers.serverid "
																"and servers.ip='%s' and servers.port='%d' ",
																Thread->MySQL.FormatSQL(*TempIP),
																ServerState->QueryPort );
			UBOOL BadServer = BadPortQuery->NumRows() != 0;
			delete BadPortQuery;

			if( !BadServer )
			{
				// check the other way
				FQueryResult* BadPortQuery = Thread->MySQL.Query(	"select servers.serverid from servers, onlineservers "
																	"where onlineservers.serverid=servers.serverid "
																	"and servers.ip='%s' and onlineservers.queryport='%d' ",
																	Thread->MySQL.FormatSQL(*TempIP),
																	ServerState->Port );
				BadServer = BadPortQuery->NumRows() != 0;
				delete BadPortQuery;
			}


			if( BadServer )
			{
				// ban them all!
				Thread->MySQL.DoSQL( "update servers set tempbantil=DATE_ADD(NOW(), INTERVAL 1 HOUR) where ip='%s' ", Thread->MySQL.FormatSQL(*TempIP) );
			}

/*
			// insert.
			Thread->MySQL.DoSQL(	"insert into onlineservers (serverid, expires, queryport, gspyport, servername, mapname, gametype, currentplayers, maxplayers, cdkeyid, servertype, version, dedicated, haspassword, standard, mutators, bots, weaponstay, transloc, gamespeed, exclude, realcurrentplayers, skilllevel) "
									"values ('%d',DATE_ADD(NOW(), INTERVAL '1:10' MINUTE_SECOND),'%d','%d','%s','%s','%s','%d','%d','%d','%s','%d',%s,%s,%s,'%s',%s,%s,%s,'%f',%s,'%d','%s')",
									ServerID,
									ServerState->QueryPort,
									GSpyPort,
									Thread->MySQL.FormatSQL(*ServerState->ServerName),
									Thread->MySQL.FormatSQL(*ServerState->MapName),
									Thread->MySQL.FormatSQL(*ServerState->GameType),
									ServerState->CurrentPlayers,
									ServerState->MaxPlayers,
									CDKeyID,
									"",
									RemoteVersion,
									IsDedicated ? "'T'" : "NULL",
									HasPassword ? "'T'" : "NULL",
									IsStandard ? "'T'" : "NULL",
									Thread->MySQL.FormatSQL(*MutatorString),
									HasBots ? "'T'" : "NULL",
									HasWeaponStay ? "'T'" : "NULL",
									HasTransloc ? "'T'" : "NULL",
									GameSpeed,
									ExcludeServer ? "'T'" : "NULL",
									IsDedicated ? ConnectedPlayerCount : ConnectedPlayerCount + 1,
									Thread->MySQL.FormatSQL(*ServerState->SkillLevel)
								);
*/

			if (CDKeyID==1)
				GWarn->Logf(TEXT("Adding Found a server (%s) that's Instagib %i"),*ServerState->ServerName,CDKeyID);


			Thread->MySQL.DoSQL(	"insert into onlineservers (serverid, expires, queryport, gspyport, servername, mapname, gametype, currentplayers, maxplayers, cdkeyid, servertype, version, dedicated, haspassword, standard, mutators, bots, weaponstay, transloc, gamespeed, exclude, realcurrentplayers, skilllevel) "
									"values ('%d',DATE_ADD(NOW(), INTERVAL '1:10' MINUTE_SECOND),'%d','%d','%s','%s','%s','%d','%d','%d','%s','%d',%s,%s,%s,'%s',%s,%s,%s,'%f',%s,'%d','%s')",
									ServerID,
									ServerState->QueryPort,
									GSpyPort,
									Thread->MySQL.FormatSQL(*ServerState->ServerName),
									Thread->MySQL.FormatSQL(*ServerState->MapName),
									Thread->MySQL.FormatSQL(*ServerState->GameType),
									ServerState->CurrentPlayers,
									ServerState->MaxPlayers,
									CDKeyID,
									"",
									RemoteVersion,
									IsDedicated ? "'T'" : "NULL",
									HasPassword ? "'T'" : "NULL",
									IsStandard ? "'T'" : "NULL",
									Thread->MySQL.FormatSQL(*MutatorString),
									HasBots ? "'T'" : "NULL",
									HasWeaponStay ? "'T'" : "NULL",
									HasTransloc ? "'T'" : "NULL",
									GameSpeed,
									ExcludeServer ? "'T'" : "NULL",
									IsDedicated ? ConnectedPlayerCount : ConnectedPlayerCount + 1,
									Thread->MySQL.FormatSQL(*ServerState->SkillLevel)
								);
		}

		// Players
		FQueryResult* OnlineServerPlayersQuery = Thread->MySQL.Query( "select id, playernum from onlineserverplayers where serverid='%d'", ServerID );
		TArray<INT> PlayersToDelete;
		TArray<INT> PlayersToUpdate;
		TArray<INT> PlayersToUpdateIDs;

		FQueryField** ServerPlayersRow;
		while( (ServerPlayersRow=OnlineServerPlayersQuery->FetchNextRow()) != NULL )
		{
			INT i = GetPlayerIndex( ServerState->PlayerInfo, ServerPlayersRow[1]->AsInt() );
			if( i == -1 )
			{
				PlayersToDelete.AddItem( ServerPlayersRow[0]->AsInt() );
			}
			else
			{
				PlayersToUpdate.AddItem(i);
				PlayersToUpdateIDs.AddItem( ServerPlayersRow[0]->AsInt() );
			}
		}
		delete OnlineServerPlayersQuery;
		// delete old players
		for( INT i=0;i<PlayersToDelete.Num();i++ )	
			Thread->MySQL.DoSQL( "delete from onlineserverplayers where id='%d'", PlayersToDelete(i) );
		// update existing players
		for( INT i=0;i<PlayersToUpdate.Num();i++ )	
		{
			Thread->MySQL.DoSQL( "update onlineserverplayers set playername='%s', ping='%d', score='%d' where id='%d' ", 
				Thread->MySQL.FormatSQL(*ServerState->PlayerInfo(PlayersToUpdate(i)).PlayerName),
				ServerState->PlayerInfo(PlayersToUpdate(i)).Ping,
				ServerState->PlayerInfo(PlayersToUpdate(i)).Score,
				PlayersToUpdateIDs(i) );
		}
		// add new players
		for( INT i=0;i<ServerState->PlayerInfo.Num();i++ )
		{
			if( PlayersToUpdate.FindItemIndex(i) == INDEX_NONE )
			{
				// Add
				Thread->MySQL.DoSQL( "insert into onlineserverplayers (serverid, playernum, playername, ping, score) values ('%d', '%d', '%s', '%d', '%d') ", 
					ServerID, 
					ServerState->PlayerInfo(i).PlayerNum,
					Thread->MySQL.FormatSQL(*ServerState->PlayerInfo(i).PlayerName),
					ServerState->PlayerInfo(i).Ping,
					ServerState->PlayerInfo(i).Score );
			}
		}
	}
	unguard;
}

void FMasterServerClientLink::UpdateClients( TArray<FString>& ClientIPs )
{
	guard(FMasterServerClientLink::UpdateClients);

	// Don't track demo clients.
	if( RemoteVersion < 2000 )
		return;

	FQueryResult* ClientsQuery = Thread->MySQL.Query(	"select id, clientip, disconnectcount "
														"from onlineplayers "
														"where serverid='%d' ", ServerID );

	FString ToRemove;
	FQueryField** Row;
	while( (Row = ClientsQuery->FetchNextRow())!=NULL )
	{
		INT idx = ClientIPs.FindItemIndex(Row[1]->AsString());
		if( idx == INDEX_NONE )
		{
			// this client is no longer on this server.
			if( ToRemove.Len() )
				ToRemove = ToRemove + FString::Printf(TEXT(",%d"), Row[0]->AsInt());
			else
				ToRemove = ToRemove + FString::Printf(TEXT("%d"), Row[0]->AsInt());
		}
		else
		{
			if( Row[2]->AsInt() > 0 )
			{
				// Already requested a disconnect on this client.  Disconnect it again.
				BYTE Command = MTS_ClientAuthFailed;
				FString ClientIPString = Row[1]->AsString();
				*ArSend << Command << ClientIPString;
				ArSend->Flush();
				GWarn->Logf(TEXT("Server %d didn't disconnect client %s (request %d)."), ServerID, *ClientIPString, Row[2]->AsInt() );
				Thread->MySQL.DoSQL( "update onlineplayers set disconnectcount=disconnectcount+1 where id=%d", Row[0]->AsInt() );

				// After 5 disconnect ignores, exclude the server from the server list.
				if( Row[2]->AsInt() >= 5 )
					Thread->MySQL.DoSQL( "update servers set tempbantil=DATE_ADD(NOW(), INTERVAL 8 HOUR) where serverid=%d", ServerID );
			}

			// Client already challenged, remove it from the array
			ClientIPs.Remove(idx);
		}
	}
	delete ClientsQuery;

	if( ToRemove.Len() )
		Thread->MySQL.DoSQL( "delete from onlineplayers where id in (%s)",  Thread->MySQL.FormatSQL(*ToRemove) );

	for( INT i=0;i<ClientIPs.Num();i++ )
	{
		// Challenge any ClientIPs not in the database
		BYTE Command = MTS_ClientChallenge;
		FString ClientIP = ClientIPs(i);
		FString Challenge = FString::Printf( TEXT("%d"), GenerateChallenge() );
		*ArSend << Command << ClientIP << Challenge;
		ArSend->Flush();
		Thread->MySQL.DoSQL( "insert into onlineplayers (serverid, clientip, challenge, challengetime, challengecount) values ('%d', '%s', '%s', now(), 1)", ServerID, Thread->MySQL.FormatSQL(*ClientIPs(i)), Thread->MySQL.FormatSQL(*Challenge) ); 
	}

	unguard;
}

void FMasterServerClientLink::ValidateClientResponse( const TCHAR* ClientIP, const TCHAR* ClientCDKeyHash, const TCHAR* ClientResponse, const TCHAR* ClientGMD5, const TCHAR* ClientNick  ) 
{
	guard(FMasterServerClientLink::ValidateClientResponse);

	FQueryResult* ChallengeQuery = Thread->MySQL.Query(	"select id, challenge from onlineplayers "
														"where serverid='%d' and clientip='%s' ", 
														ServerID, Thread->MySQL.FormatSQL(ClientIP) );
	FString ClientIPNoPort = ClientIP;
	INT i = ClientIPNoPort.InStr(TEXT(":"));
	if( i!=-1 )
		ClientIPNoPort = ClientIPNoPort.Left(i);

	FQueryField** Row;
	if( (Row = ChallengeQuery->FetchNextRow())!=NULL )
	{
		INT OnlineID		= Row[0]->AsInt();
		FString Challenge	= Row[1]->AsString();
        delete ChallengeQuery;

		INT ClientCDKeyID = VerifyChallenge( ClientCDKeyHash, *Challenge, ClientResponse, 0, *ClientIPNoPort );

		// Update
		Thread->MySQL.DoSQL( "update onlineplayers set cdkeyid='%d' where id='%d'", ClientCDKeyID, OnlineID );

		// disconnect if failed
		if( ClientCDKeyID < 0 )
		{
			BYTE Command = MTS_ClientAuthFailed;
			FString ClientIPString = ClientIP;
			*ArSend << Command << ClientIPString;
			ArSend->Flush();
			// log
			Thread->MySQL.DoSQL( "update onlineplayers set disconnectcount=disconnectcount+1 where id=%d", OnlineID );
		}
		else
		{
			if( ClientCDKeyID > 0 )	// This is a registered version
			{
				// Check to see if the key is banned

				FString Banned = IsBanned(ClientCDKeyID);
				if (Banned != TEXT("") )
				{	
					GWarn->Logf(TEXT("Server %d denied banned client from %s.  Banned until %s"),ServerID,*SocketData.GetString(1), *Banned);
					BYTE Command = MTS_ClientBanned;
					FString ClientIPString = ClientIP;
					*ArSend << Command << ClientIPString << Banned;
					ArSend->Flush();
					Thread->MySQL.DoSQL( "update onlineplayers set disconnectcount=disconnectcount+1 where id=%d", OnlineID );

					return;
				}

				// Check for duplicate CD keys in use.
				TArray<FString> UniqueClientIPs;
				new(UniqueClientIPs) FString(ClientIPNoPort);

				FQueryResult* DupeQuery = Thread->MySQL.Query(	"select onlineplayers.clientip from onlineplayers, onlineservers "
																"where onlineplayers.serverid=onlineservers.serverid "
																"and onlineplayers.serverid!='%d' and onlineplayers.cdkeyid='%d' ", 
																ServerID, ClientCDKeyID );
				FQueryField** Row;
				INT DupeCount = 0;
				while( (Row = DupeQuery->FetchNextRow()) != NULL )
				{
					FString Temp = Row[0]->AsString();
					INT i = Temp.InStr(TEXT(":"));
					if( i != -1 )
						Temp = Temp.Left(i);
					if( UniqueClientIPs.FindItemIndex(Temp)==INDEX_NONE )
					{
						new(UniqueClientIPs) FString(Temp);
						DupeCount++;
					}
				}
				delete DupeQuery;
				if( DupeCount > 1 )
				{
					GWarn->Logf(TEXT("Server %d disconnecting client %s using key %d: %d simultaneous."), ServerID, ClientIP, ClientCDKeyID, DupeCount );
					BYTE Command = MTS_ClientDupKey;
					FString ClientIPString = ClientIP;
					*ArSend << Command << ClientIPString;
					ArSend->Flush();
					// log
					Thread->MySQL.DoSQL( "update onlineplayers set disconnectcount=disconnectcount+1 where id=%d", OnlineID );
					Thread->MySQL.DoSQL( "insert into simultaneous(cdkeyid, ip, count, at) values ('%d', '%s', '%d', NOW())", ClientCDKeyID, Thread->MySQL.FormatSQL(ClientIP), DupeCount );
				}

				// Not a dup key, check Global MD5

				if ( !VerifyGlobalMD5(ClientGMD5, ClientCDKeyID) )
				{
					BYTE Command = MTS_ClientMD5Fail;
					FString ClientIPString = ClientIP;
					*ArSend << Command << ClientIPString;
					ArSend->Flush();
					Thread->MySQL.DoSQL( "update onlineplayers set disconnectcount=disconnectcount+1 where id=%d", OnlineID );
				}

				// Log the last seen nick
				Thread->MySQL.DoSQL("update cdkey set LastNick='%s' where id=%i", Thread->MySQL.FormatSQL(ClientNick), ClientCDKeyID);

			}
			
			FQueryResult* WatchQuery = Thread->MySQL.Query("select * from watches");
			FQueryField** WatchRow;
			if( (WatchRow = WatchQuery->FetchNextRow())!=NULL )
			{
				FString CheckIP   = WatchRow[1]->AsString();
				FString CheckNick = WatchRow[2]->AsString();
				INT CheckID		  = WatchRow[3]->AsInt();

				FString IPString = ClientIP;
				FString NickStr = ClientNick;

				if ( PatternMatch(IPString,CheckIP) || PatternMatch(ClientNick, CheckNick) || (ClientCDKeyID>0 && ClientCDKeyID == CheckID) )
				{
					FString Msg = FString::Printf(TEXT("Detected Player [%s] @ %s [%i]"),ClientNick, ClientIP, ClientCDKeyID);

					GWarn->Logf(TEXT("[Watch Alert] %s"),*Msg);

					Thread->MySQL.DoSQL("insert into watchhits (hitdate, hitip, hitinfo) values ( NOW(), '%s', '%s')",
						Thread->MySQL.FormatSQL(*ClientIPNoPort), Thread->MySQL.FormatSQL(*Msg));
					
					FString Action = WatchRow[4]->AsString();
					FString Response = WatchRow[5]->AsString();

					if ( Action==TEXT("restrict") ) 
					{
						delete WatchQuery;
						
						GWarn->Logf(TEXT("Restricting %s"),*SocketData.GetString(1));
						BYTE Command = MTS_ClientBanned;
						FString ClientIPString = ClientIP;
						*ArSend << Command << ClientIPString << Response;
						ArSend->Flush();
						Thread->MySQL.DoSQL( "update onlineplayers set disconnectcount=disconnectcount+1 where id=%d", OnlineID );
						return;
					}
				}
			}
			delete WatchQuery;
		}
	}
	else
	{
		delete ChallengeQuery;
		GWarn->Logf(TEXT("Received unexpected challenge respone from %d/%s"), ServerID, ClientIP );
	}

	unguard;
}

INT FMasterServerClientLink::CheckVersion( INT CurrentVersion, BYTE Platform, const TCHAR* Language, UBOOL IsDemo, UBOOL IsServer, UBOOL OptionalRequest, INT* OutMinVersion, INT* OutMaxVersion, FString* MOTD )
{
	guard(FMasterServerClient::CheckVersion);

	// Version cache
	for( INT i=0;i<Thread->VersionCache.Num();i++ )
	{
		FMasterServerVersionCache* CheckCacheEntry = &Thread->VersionCache(i);
		if( appSeconds() > CheckCacheEntry->LastUpdated + 120.0 )
		{
			Thread->VersionCache.Remove(i);
			i--;
			continue;
		}

		if( CheckCacheEntry->CurrentVersion==CurrentVersion &&
			CheckCacheEntry->Platform==Platform &&
			CheckCacheEntry->IsDemo==IsDemo && 
			CheckCacheEntry->Language==FString(Language) &&
			CheckCacheEntry->IsServer==IsServer &&
			CheckCacheEntry->OptionalRequest==OptionalRequest )
		{
			if( OutMinVersion ) 
				*OutMinVersion = CheckCacheEntry->OutMinVersion;
			if( OutMaxVersion )
				*OutMaxVersion = CheckCacheEntry->OutMaxVersion;
			if( MOTD )
				*MOTD = CheckCacheEntry->MOTD;
			return CheckCacheEntry->Result;
		}
	}

	FMasterServerVersionCache* CacheEntry = &Thread->VersionCache(Thread->VersionCache.AddZeroed());

	FQueryResult* VersionQuery = Thread->MySQL.Query(	"select versions.version, versionupgrade.%s, versions.minnetver, versions.maxnetver "
														"from versions "
														"left join versionupgrade on versions.version = versionupgrade.version "
														"where (versionupgrade.id is null or "
														" (versionupgrade.minplatform <= '%d' and '%d' <= versionupgrade.maxplatform)) "
														"and versions.version <= '%d' and '%d' >= versions.minnetver "
														"and ('%d' <= versions.maxnetver or versions.maxnetver is null) "
														"order by versions.version desc limit 1",
														OptionalRequest ? 
														(IsServer ? "optionalupgradeserverto"	: "optionalupgradeclientto") :
														(IsServer ? "forceupgradeserverto"		: "forceupgradeclientto"),
														Platform, Platform, 
														CurrentVersion, CurrentVersion, CurrentVersion );

	INT Result=0;
	FQueryField** Row;
	if( (Row = VersionQuery->FetchNextRow())!=NULL )
	{
		if( !Row[1]->IsNull() )
			Result = Row[1]->AsInt();		// Upgrade

		CacheEntry->OutMinVersion = Row[2]->AsInt();
		CacheEntry->OutMaxVersion = Row[3]->AsInt();
		delete VersionQuery;

		// find MOTD
		FQueryResult* MOTDQuery = Thread->MySQL.Query( "select message, language, version from motd "
													   "where version<=%d and demo is%s null "
													   "order by version desc",
															CurrentVersion, 
															IsDemo ? " not" : "" );

		CacheEntry->MOTD = TEXT("");
		INT FoundVersion=0;
		FQueryField** MOTDRow;
		while( (MOTDRow = MOTDQuery->FetchNextRow())!=NULL )
		{
			if( MOTDRow[2]->AsInt() < FoundVersion )
				break;

			UBOOL LangMatch = !appStricmp( *MOTDRow[1]->AsString(), Language );
			if( LangMatch || !appStricmp( *MOTDRow[1]->AsString(), TEXT("int") ) )
			{
				FoundVersion = MOTDRow[2]->AsInt();
				CacheEntry->MOTD = MOTDRow[0]->AsString();
			}

			if( LangMatch )
				break;
		}
		delete MOTDQuery;

	}
	else
	{
		delete VersionQuery;
		//!!
		GWarn->Logf(TEXT("Couldn't find versions record for version %d platform %d"), CurrentVersion, Platform);
		CacheEntry->OutMinVersion = CurrentVersion;
		CacheEntry->OutMaxVersion = CurrentVersion;
		CacheEntry->MOTD = TEXT("");
	}

	// return results
	if( OutMinVersion )
		*OutMinVersion = CacheEntry->OutMinVersion;
	if( OutMaxVersion )
		*OutMaxVersion = CacheEntry->OutMaxVersion;
	if( MOTD )
		*MOTD = CacheEntry->MOTD;

	CacheEntry->CurrentVersion = CurrentVersion;
	CacheEntry->Platform = Platform;
	CacheEntry->Language = Language;
	CacheEntry->IsDemo = IsDemo;
	CacheEntry->IsServer = IsServer;
	CacheEntry->OptionalRequest = OptionalRequest;
	CacheEntry->LastUpdated = appSeconds();
	CacheEntry->Result = Result;

	return Result;
	unguard;
}

void FMasterServerClientLink::GetModMOTD( INT CurrentVersion, BYTE Platform, const TCHAR* Language, UBOOL IsDemo, FString* MOTD )
{
	guard(FMasterServerClient::GetModMOTD);

	ANSICHAR* l = TCHAR_TO_ANSI(Language);
	FQueryResult* ModMOTDQuery = Thread->MySQL.Query( "select motd from mod where language='%s'",l);
	FQueryField** ModMOTDRow;
	while( (ModMOTDRow = ModMOTDQuery->FetchNextRow())!=NULL )
	{
		*MOTD = ModMOTDRow[0]->AsString();
	}
	delete ModMOTDQuery;

	unguard;
}


void FMasterServerClientLink::GetDownloadSites( INT UpgradeVersion, BYTE Platform, TArray<FString>& DownloadSites )
{
	guard(FMasterServerClientLink::GetDownloadSites);

	FQueryResult* SiteQuery = Thread->MySQL.Query(	"select versionurls.url from versionurls, versionupgrade "
													"where versionurls.upgradeid = versionupgrade.id "
													"and versionupgrade.minplatform <= '%d' "
													"and '%d' <= versionupgrade.maxplatform "
													"and versionupgrade.version = '%d' ",
													Platform, Platform, UpgradeVersion );
	FQueryField** Row;
	while( (Row = SiteQuery->FetchNextRow())!=NULL )
	{
        INT i = DownloadSites.AddZeroed();
		DownloadSites(i) = Row[0]->AsString();
	}
	delete SiteQuery;

	//!! clients crash if you don't send them anything!
	check( DownloadSites.Num() );

	unguard;
}

void FMasterServerClientLink::CheckPackageMD5Version( INT CurrentMaxRevision )
{
	guard(FMasterServerClientLink::CheckPackageMD5Version);

	FQueryResult* PackageQuery = Thread->MySQL.Query( "select guid, md5, revision from packagemd5 where revision > %d ", CurrentMaxRevision );
	FQueryField** Row;

	TArray<FMD5UpdateData> UpdateData;
	while( (Row = PackageQuery->FetchNextRow())!=NULL )
	{
		INT i = UpdateData.AddZeroed();
		UpdateData(i).Guid		= Row[0]->AsString();
		UpdateData(i).MD5		= Row[1]->AsString();
		UpdateData(i).Revision	= Row[2]->AsInt();
	}
	delete PackageQuery;

	if( UpdateData.Num() )
	{
		BYTE Command = MTS_MD5Update;
		*ArSend << Command << UpdateData;
		ArSend->Flush();
	}
	unguard;
}

void FMasterServerClientLink::GenerateOwnageList(INT revision)
{
	guard(FMasterServerClientLink::GenerateOwnageList);

	FQueryResult* OwnageQuery = Thread->MySQL.Query( "select revision, mapname, mapdesc, mapurl from ownage where revision > %i ", revision);
	FQueryField** Row;

	// Send the Ownage Maps

	while( (Row = OwnageQuery->FetchNextRow())!=NULL )
	{
		INT	RL, OPT=0;
		BYTE PacketType=1;
		FString MapName, MapDesc, MapURL, MD;

		RL		= Row[0]->AsInt();
		MapName = Row[1]->AsString();
		MapDesc = Row[2]->AsString();
		MapURL	= Row[3]->AsString();


		*ArSend << PacketType << OPT << RL << MapName;
		ArSend->Flush();

		OPT++;

		while (MapDesc.Len() > 128)
		{
			MD = MapDesc.Left(128);
			MapDesc = MapDesc.Right(MapDesc.Len()-128);
			*ArSend << PacketType << OPT << RL << MD;
			ArSend->Flush();
		}

		if ( MapDesc != TEXT("") )	// Send final
		{
			*ArSend << PacketType << OPT << RL << MapDesc;
			ArSend->Flush();
		}

		OPT++;

		// Send the URL

		*ArSend << PacketType << OPT << RL << MapURL;
		ArSend->Flush();
	
	}
	delete OwnageQuery;

	unguard;
}

UBOOL FMasterServerClientLink::VerifyGlobalMD5(FString GlobalMD5, INT CDKeyID)
{
	guard(FMasterServerClientLink::VerifyGlobalMD5);

	if (CDKeyID==0)	// Demo Key
		return true;

	FQueryResult* GMD5Query = Thread->MySQL.Query( "select md5 from globalmd5 where md5='%s' and version=%i and platform=%i", Thread->MySQL.FormatSQL(*GlobalMD5), RemoteVersion, RemotePlatform);
	if ( GMD5Query->NumRows() ==0 )
	{
		AddBadGuy(CDKeyID,FString::Printf(TEXT("Failed Global MD5 Check with MD5 of [%s]"),GlobalMD5));
		delete GMD5Query;

		return true;			// @@fixme: Always returning true at the moment.
	}

	delete GMD5Query;
	return true;

	unguard;
}

FString FMasterServerClientLink::IsBanned(int CDKeyId)
{
	guard(FMasterServerClientLink::IsBanned);
	
	FString Result=TEXT("");
	FQueryResult* BannedQuery = Thread->MySQL.Query( "select banned from cdkey where id=%i and banned>NOW();",CDKeyId);	
	if (BannedQuery->NumRows() > 0)
	{
		FQueryField** Row;
		while( (Row = BannedQuery->FetchNextRow())!=NULL )
			Result = Row[0]->AsString();

	}

	delete BannedQuery;
	return Result;

	unguard;
}
		
void FMasterServerClientLink::AddBadGuy(int CDKeyID, FString Reason)
{
	guard(FMasterServerClientLink::AddBadGuy);

	return;

	FQueryResult* CDKeyQuery = Thread->MySQL.Query( "select md5hash, cdkey from cdkey where id=%i",CDKeyID);
	if (CDKeyQuery->NumRows() > 0)
	{
		FQueryField** CDKeyRow;
		CDKeyRow = CDKeyQuery->FetchNextRow();

		FString MD5Hash = CDKeyRow[0]->AsString();
		FString CDKey   = CDKeyRow[1]->AsString();

		delete CDKeyQuery;

		GWarn->Logf(TEXT("Client connection with cdkey [%s] %s"),*CDKey,*Reason);

		FQueryResult* BadGuyQuery  = Thread->MySQL.Query( "select cdkey, report from badguys where cdkey='%s'", Thread->MySQL.FormatSQL(*CDKey) );
		UBOOL b=false;
		if (BadGuyQuery->NumRows() >0)
		{
			FQueryField** Row;
			while( (Row = BadGuyQuery->FetchNextRow())!=NULL )
			{
				if (Row[1]->AsString() == Reason)
				{
					Thread->MySQL.DoSQL( "update badguys set lastupdate=NOW()");
					b=true;
				}
			}
		}

		if (!b)
			Thread->MySQL.DoSQL( "insert into badguys (cdkey, md5hash, report, lastupdate) values ('%s', '%s', '%s', NOW())",Thread->MySQL.FormatSQL(*CDKey),Thread->MySQL.FormatSQL(*MD5Hash),Thread->MySQL.FormatSQL(*Reason));

		delete BadGuyQuery;
	}
	else
		delete CDKeyQuery;
	
	unguard;
}

UBOOL FMasterServerClientLink::PatternMatch(FString Data, FString Pattern)
{
	guard(FMasterServerClientLink::PatternMatch);

	if ( !Pattern || Pattern == TEXT("") || !Data || Data == TEXT("") )
		return false;

	Data = Data.Caps();
	Pattern = Pattern.Caps();

	if ( Pattern==TEXT("*") )
		return true;

	UBOOL AnyLeft = Pattern.Left(1)==TEXT("*");
	if (AnyLeft)
		Pattern = Pattern.Right(Pattern.Len()-1);

	UBOOL AnyRight = Pattern.Right(1)==TEXT("*");
	if (AnyRight)
		Pattern = Pattern.Left(Pattern.Len()-1);

	INT Pos = Data.InStr(Pattern);
	if (Pos>=0)
	{
		if (!AnyLeft && Pos>0)
			return false;

		if (!AnyRight && Pos<Data.Len()-Pattern.Len()-1)
			return false;

		return true;
	}
	return false;

	unguard;
}
/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

