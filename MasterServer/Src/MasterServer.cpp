/*=============================================================================
	MasterServer.cpp: Unreal Master, CD Key and Stats Server
	Copyright 1997-2002 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Jack Porter.
=============================================================================*/

#include "MasterServer.h"
#include "MasterServerLink.h"

/*-----------------------------------------------------------------------------
	Globals
-----------------------------------------------------------------------------*/

TCHAR GDatabaseServer[64] = {0};
UBOOL GRunHousekeeping = 0;
UBOOL GAcceptStats = 0;
UBOOL GExtendedLogging = 0;
TCHAR* GDatabaseUser = TEXT("ut2004user");
TCHAR* GDatabasePass = TEXT("pkv9j23k");
INT GLatestVersion = 0;
FCDKeyCacheTree* FCDKeyCacheTree::Cache = NULL;

/*-----------------------------------------------------------------------------
	UMasterServerCommandlet
-----------------------------------------------------------------------------*/

void UMasterServerCommandlet::StaticConstructor()
{
	guard(UMasterServerCommandlet::StaticConstructor);
	LogToStdout     = 0;
	IsClient        = 1;
	IsEditor        = 1;
	IsServer        = 1;
	LazyLoad        = 0;
	ShowErrorCount  = 1;
	ListeningSocket = NULL;
	unguard;
}

//
// Main - entry point
//
INT UMasterServerCommandlet::Main( const TCHAR* Parms )
{
	guard(UMasterServerCommandlet::Main);

	INT ListenPort, RestartTime;
	FString DBServer;

	// load config
	if( !GConfig->GetInt( TEXT("MasterServer"), TEXT("ListenPort"), ListenPort, TEXT("masterserver.ini") ) )
		appErrorf(TEXT("Missing ListenPort in INI"));

	if( !GConfig->GetString( TEXT("MasterServer"), TEXT("DBServer"), DBServer, TEXT("masterserver.ini") ) )
		appErrorf(TEXT("Missing DBServer in INI"));
	appStrcpy( GDatabaseServer, *DBServer );

	if( !GConfig->GetBool( TEXT("MasterServer"), TEXT("RunHousekeeping"), GRunHousekeeping, TEXT("masterserver.ini") ) )
		appErrorf(TEXT("Missing RunHousekeeping in INI"));

	if( !GConfig->GetBool( TEXT("MasterServer"), TEXT("AcceptStats"), GAcceptStats, TEXT("masterserver.ini") ) )
		appErrorf(TEXT("Missing AcceptStats in INI"));

	GConfig->GetBool( TEXT("MasterServer"), TEXT("GExtendedLogging"), GExtendedLogging, TEXT("masterserver.ini") ); 

	if( !GConfig->GetInt( TEXT("MasterServer"), TEXT("RestartTime"), RestartTime, TEXT("masterserver.ini") ) )
		appErrorf(TEXT("Missing RestartTime in INI"));

	if( !GConfig->GetInt( TEXT("MasterServer"), TEXT("LatestVersion"), GLatestVersion, TEXT("masterserver.ini") ) )
		appErrorf(TEXT("Missing LatestVersion in INI"));
	

	GWarn->Logf(TEXT("INI: ListenPort=%d"),ListenPort);
	GWarn->Logf(TEXT("INI: DBServer=%s"),*DBServer);
	GWarn->Logf(TEXT("INI: RunHousekeeping=%d"),GRunHousekeeping);
	GWarn->Logf(TEXT("INI: AcceptStats=%d"),GAcceptStats);
	GWarn->Logf(TEXT("INI: RestartTime=%d"), RestartTime);
	GWarn->Logf(TEXT("INI: LatestVersion=%d"), GLatestVersion);

	MainLoop(ListenPort, RestartTime);

	return 0;
	unguard;
}

void UMasterServerCommandlet::MainLoop( INT ListenPort, INT RestartTime )
{
	guard(UMasterServerCommandlet::MainLoop);

	// Create bandwidth throttling thread.
	//FMasterServerThrottleThread Throttling( 5000, 5000 );

	// Create the master server and listen on port 28902.
	ListeningSocket = new FMasterServerListenLink(ListenPort);

	// Create housekeeping thread.
	FMasterServerHousekeepingThread Housekeeping(ListeningSocket);

	// Create CDKey memory cache thread.
	//new FMasterServerCDKeyCacheThread();

	DOUBLE LastTime = appSeconds();
	INT tYear, tMonth, tWeek, tDay, tHour, tMin, tSec, tMSec,tLastHour;
	appSystemTime(tYear, tMonth, tWeek, tDay, tLastHour, tMin, tSec, tMSec);	// Prime for restarts
	while(!GIsRequestingExit)
	{
		GCurrentTime = appSeconds();
		ListeningSocket->WaitForConnections(1);

		if( appSeconds() - LastTime > 5.0 )
		{
			UpdateStats();
			LastTime = appSeconds();
		}

		appSystemTime(tYear, tMonth, tWeek, tDay, tHour, tMin, tSec, tMSec);	// Prime for restarts
		if( tHour != tLastHour && tHour%6==0 )
		{
			GWarn->Logf(TEXT("Performing 6 hour restart") );
			break;
		}
		else
			tLastHour = tHour;
	}
	
	delete ListeningSocket;
	ListeningSocket = NULL;
	unguard;
}

void UMasterServerCommandlet::UpdateStats()
{
	guard(UMasterServerCommandlet::UpdateStats);
	ListeningSocket->UpdateStats();
	unguard;
}

IMPLEMENT_CLASS(UMasterServerCommandlet)

/*-----------------------------------------------------------------------------
	Misc helper functions
-----------------------------------------------------------------------------*/

INT SnipAppart( TArray<FString>& LineParts, FString Line, const TCHAR* Separator )
{
	// Chop the line up into bits on tabs.
	// Returns line in LineParts.Num() elements in LineParts( 0 to Num()-1 )
	INT j=0;
	for(;;)
	{
		// Find a tab
		INT i = Line.InStr( Separator );		//e.g. TEXT("\t")
			j = LineParts.AddZeroed();
		if( i==-1 )
		{
			// No tabs - just add the line.
			LineParts(j) = Line;
			break;
		}
		else
		{
			// Add the first bit of the line and continue chopping.
			LineParts(j) = Line.Left(i);
			Line = Line.Mid(i+1);
		}
	}
	return j+1;
}

FString JoinTogether( TArray<FString>& LineParts, const TCHAR* Separator )
{
	FString Result;
    for( INT i=0;i<LineParts.Num();i++ )    
	{
		if( i > 0 )
			Result = Result + Separator;
		Result = Result + LineParts(i);
	}
	return Result;
}

/*-----------------------------------------------------------------------------
	Package.
-----------------------------------------------------------------------------*/

IMPLEMENT_PACKAGE(MasterServer);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

