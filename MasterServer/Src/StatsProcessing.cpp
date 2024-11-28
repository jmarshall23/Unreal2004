/*=============================================================================
StatsProcessing.cpp: Stats processing code
Copyright 1997-2002 Epic Games, Inc. All Rights Reserved.

Revision history:
* Created by Jack Porter (Framework) and Christoph A. Loewe (Meat)
=============================================================================*/

#include "MasterServer.h"
#include "StatsProcessing.h"

/*-----------------------------------------------------------------------------
Main loop
-----------------------------------------------------------------------------*/
void UProcessStatsCommandlet::StaticConstructor()
{
	LogToStdout     = 0;
	IsClient        = 1;
	IsEditor        = 1;
	IsServer        = 1;
	LazyLoad        = 0;
	ShowErrorCount  = 1;  
}

INT UProcessStatsCommandlet::Main( const TCHAR* Parms )
{
	guard(UProcessStatsCommandlet::Main);

	FString StatsDBServer;
	if( !GConfig->GetString( TEXT("Stats"), TEXT("StatsDBServer"), StatsDBServer, TEXT("stats.ini") ) )
		appErrorf(TEXT("Missing StatsDBServer in stats.ini"));

#if CAL_NOTLOCAL_MYSQL_TESTING
	FString SLServer;
	if( !GConfig->GetString( TEXT("Stats"), TEXT("StatsLineDBServer"), SLServer, TEXT("stats.ini") ) )
		appErrorf(TEXT("Missing StatsLineDBServer in stats.ini"));

	// StatsLine database
	FMySQL SLMySQL;
	if( !SLMySQL.Connect( *SLServer, TEXT("statsuser"), TEXT("n387cjs"), TEXT("ut2003") ) )
		appErrorf(TEXT("Stats: statsline database connect failed"));
#endif
    
	// Stats database
	if( !MySQL.Connect( *StatsDBServer, TEXT("root"), TEXT(""), TEXT("ut2003stats") ) )
		appErrorf(TEXT("Stats: database connect failed"));

	DEBUG_LOGLINENUMBER=0;

	// Pre-Cache once (updates in Housekeeping)
	THIS_MS_DOES_HOUSEKEEPING	= 1;
	TIME_MATCHID_TIMEOUT		= 600;			// Time a MatchID times out... 600 = 10 Minutes! - CheckRemoveMatchIDs()
	TIME_BETWEEN_HOUSEKEEPINGS	= 21600;		// Time between each Housekeeping, e.g. 1800 = 30 Minutes //!! 6 houres for now
	LAST_HOUSEKEEPING_TIME		= appSeconds();	// Remember when last Housekeeping happend (on server start!).
#if CAL_NOTLOCAL_MYSQL_TESTING
	CacheAll();									// If housekeeping testing, do not cache here! Only cache in HK.
#endif

	// Setting Maximum number of matches per server (Housekeeping)
	MAX_NUM_MATCHES_PER_SERVER = 60;

#if CAL_NOTLOCAL_MYSQL_TESTING
	// Loop forever waiting for data and process it.
	while( !GIsRequestingExit )
	{
		INT LastID = 0;
		INT LineCount = 0;
#if CAL_LIVECODE
		FQueryResult* LineQuery = SLMySQL.Query( "select slid, matchid, serverid, line from statsline order by slid limit 10000" );
#else
		FQueryResult* LineQuery = SLMySQL.Query( "select slid, matchid, serverid, line from statsline order by slid limit 30000" );
#endif
		FQueryField** Row;
		DOUBLE START_100LOG_TIME = appSeconds();			// Adding some more timing code
		while( (Row = LineQuery->FetchNextRow())!=NULL )
		{
			LastID					= Row[0]->AsInt();
			INT Current_MatchID		= Row[1]->AsInt();
			INT Current_ServerID	= Row[2]->AsInt();
			FString Current_Line	= Row[3]->AsString();

			DEBUG_LOGLINENUMBER++;
			LineCount++;

			ProcessStatsLine( Current_ServerID, Current_MatchID, Current_Line );
		};
		delete LineQuery;

		if( LastID )
		{
#if !CAL_LIVECODE
			ProcessDumpRankTeam();				// Dump all rankteam values from ram to database.
			ProcessDumpRankELO();
#if CAL_MATCHPLAYERS_SUMMING
			ProcessDumpMPSum();		
#endif
#if CAL_HK_TEST
			CalcPlayersSummary();				// Direct calls to test _ALT2 versus normal HK.
			CalcGlobalSummary();
#endif
#endif
			DOUBLE TIME_DIFF = appSeconds()-START_100LOG_TIME;
#if CAL_LIVECODE
			SLMySQL.DoSQL( "delete from statsline where slid <= %d", LastID );
#endif
			MySQL.MySQLDumpQueryStats();		// New tracking of MYSQL call usage

			GWarn->Logf(TEXT("Processed %d stats lines in (%f) seconds."), LineCount, TIME_DIFF );
#if CAL_CALC_LOGLINE_TIMES
			DumpLogLineSummary( LineCount, TIME_DIFF );	// Tracking the time spent in certain log lines.
#endif
#if !CAL_LIVECODE
			appSleep(3600);						// Local tests, run above and then wait,
#endif
		}
		else
		{
			GWarn->Logf(TEXT("No stats lines to process"));
			appSleep(10);
		}

		// Housekeeping done every ca. 30 minutes
		StatsHousekeeping();
	}
#else
	DOUBLE START_100LOG_TIME = appSeconds();			// Adding some more timing code
	StatsHousekeeping();
	DOUBLE TIME_DIFF = appSeconds()-START_100LOG_TIME;
	MySQL.MySQLDumpQueryStats();						// New tracking of MYSQL call usage
	GWarn->Logf(TEXT("Housekept 100,000 stats lines in (%f) seconds."), TIME_DIFF );
	appSleep(3600);
#endif
	return 0;
	unguard;
}


/*-----------------------------------------------------------------------------
Processing Lines
-----------------------------------------------------------------------------*/

void UProcessStatsCommandlet::ProcessStatsLine( INT ServerID, INT MatchID, FString& StatsLine )
{
	guard(UProcessStatsCommandlet::ProcessStatsLine);
	// Naming Shortcuts to reduce traffic
	//	NewGame		NG
	//	ServerInfo	SI
	//	Connect		C
	//	GSpecial	G
	//	PSpecial	P
	//	Disconnect	D
	//	StartGame	SG
	//	EndGame		EG
	//	Kill		K
	//	Teamkill	TK
	//	Score		S
	//	TeamScore	T

	if( ServerID <= 0 )
		GWarn->Logf(TEXT("WARNING: Jack's Fault!! Received ServerID %d for MatchID %d"), ServerID, MatchID);

	// Chop the log line up into bits on tabs
	TArray<FString> LineParts;
	INT ColumnNumber;
	ColumnNumber = SnipAppart( LineParts, StatsLine, TEXT("\t") );	//ColNum + \t count +1 = data columns

	// Checking for "lost" MatchLookup infos due to server crash.
	//	Only accept a new MatchID and set up all the lookups, if this is a NG.
	//	Else check if MatchID is in list: If so all is well, else ignore log line.
	if( LineParts(1) != TEXT("NG") )
	{
		INT MatchIDinvalid = 1;
		for( INT m=0; m<MatchIDs.Num(); m++ )				// Looking through global "MatchIDs"
		{
			if( MatchID == MatchIDs(m) )
			{
				MatchIDinvalid = 0;							// MatchID exits in RAM, all is well :)
				break;
			}
		}
		if( MatchIDinvalid == 1 )							// MatchID did not exist in ram
		{
			// Warning turned of, since this would cause to much spam.
			// GWarn->Logf(TEXT("Warning: MatchID %d log line number %d invalid, ignoring it."), MatchID, DEBUG_LOGLINENUMBER );
			return;
		}
	}

	// Initialize the MatchLookup with default data, add MatchID to global MatchIDs list
	// If MatchLookup exists, do nothing.
	SetMatchInitGet( MatchID );			

	// Updating the time counter "lastUsed" for this MatchID, to be able to check for timeOuts.
	SetMatchLastUsed( MatchID );							// Actually does the SetMatchInitGet() already

#if CAL_CALC_LOGLINE_TIMES
	// Remember start time for the logline type
	DOUBLE LOGLINE_STARTTIME = appSeconds();
#endif

	if( LineParts(1) == TEXT("K") && ColumnNumber == 6 )	//"Kill"
	{
		//	0	1	2	3					4	5
	  	//	71	K	0	DamTypeFlakChunk	4	None
		INT TeamKill = 0;											//Normal kills, not Teamkills
		ProcessKill(	MatchID,
						appAtoi(*LineParts(2)), *LineParts(3),		//KillerNumber, DamageType
						appAtoi(*LineParts(4)),	*LineParts(5),		//VictimNumber, VictimWeapon
						TeamKill );
	}
	else
	if( LineParts(1) == TEXT("TK") && ColumnNumber == 6 )	//"Teamkill"
	{
		//	0	1	2	3					4	5
	  	//	71	TK	0	DamTypeFlakChunk	4	None
		INT TeamKill = 1;
		ProcessKill(	MatchID,
						appAtoi(*LineParts(2)), *LineParts(3),		//KillerNumber, DamageType
						appAtoi(*LineParts(4)),	*LineParts(5),		//VictimNumber, VictimWeapon
						TeamKill );
	}
	else
	if( LineParts(1) == TEXT("S") && ColumnNumber == 5 )	//"Score"
	{
		//	0	1	2	3		4
		//	71	S	0	1.00	frag
		ProcessScore(	MatchID,
						appAtoi(*LineParts(2)),				// PlayerNumber
						appAtof(*LineParts(3)),				// Score
						*LineParts(4),						// ScoreCode
						appAtoi(*LineParts(0))	);			// Seconds
	}
	else
	if( LineParts(1) == TEXT("T") && ColumnNumber == 5 )	// "TeamScore"
	{
		//	0	1	2	3		4
		//	231	T	0	1.00	flag_cap
		ProcessTeamScore(	MatchID,					
							appAtoi(*LineParts(2)),			// TeamID
							appAtof(*LineParts(3)),			// Score
							*LineParts(4),					// TeamScoreCode
							appAtoi(*LineParts(0))	);		// Seconds				
	}
	else
	if( LineParts(1) == TEXT("P") && ColumnNumber == 4 )	// Special Player Events "PSpecial"
	{
		//	1	2	3
		//	P	0	spree_1
		ProcessGSpecial( MatchID, 
						*LineParts(3),						// EventCode
						appAtoi(*LineParts(2)),				// PlayerNumber
						-1 );								// TeamFlagID - Not used
	}
	else
	if( LineParts(1) == TEXT("G") && ColumnNumber == 5 )	// Special Game Events "GSpecial"
	{
		if( LineParts(2) == TEXT("NameChange") )
		{
			//	1	2			3	4
			//	G	NameChange	0	Christoph
			ProcessNameChange(	MatchID,
								appAtoi(*LineParts(3)),		// PlayerNumber
								*LineParts(4) );			// NickName
		}
		else
		if( LineParts(2) == TEXT("TeamChange") )
		{
			//	1	2			3	4
			//	G	TeamChange	0	0
			ProcessTeamChange(	MatchID, 
								appAtoi(*LineParts(3)),		// PlayerNumber
								appAtoi(*LineParts(4))	);	// TeamID
		}
		else
		{ 
			//	1	2			3	4
			//	G	flag_taken	0	1						// All other game events
			ProcessGSpecial(	MatchID, 
								*LineParts(2),				// EventCode
								appAtoi(*LineParts(3)),		// PlayerNumber
								appAtoi(*LineParts(4)) );	// TeamFlagID - Not used
		}
	}
	else
	if( LineParts(1) == TEXT("C") && ColumnNumber == 5  )	//"Connect"
	{
		//	0	1	2	3   4
		//	33	C	0	UN  PW
		ProcessConnect(	MatchID,
						appAtoi(*LineParts(0)),				// Seconds
						appAtoi(*LineParts(2)),				// PlayerNumber
						*LineParts(3),						// StatsUsername
						*LineParts(4) );	 				// StatsPassword
	}
	else
	if( LineParts(1) == TEXT("D") && ColumnNumber == 3 )	//"Disconnect"
	{
		//	0	1	2
		//	138	D	0
		ProcessDisconnect(	MatchID,
							appAtoi(*LineParts(0)),			// Seconds
							appAtoi(*LineParts(2))	);		// PlayerNumber
	}
	else
	if( LineParts(1) == TEXT("NG") && ColumnNumber >= 9 )	// >= Mutators could possibly be empty
	{
		//	0	1	2					3	4				5			6
		//	0	NG	2002-8-22 17:33:27	0	CTF-Geothermal	Geothermal	Bastiaan (Checker) Frank	
		//	7				8					9
		//	XGame.xCTFGame	Capture the Flag	Mutators=UnrealGame.DMMutator|UnrealGame.MutBigHead|XGame.MutHeliumCorpses|UnrealGame.BigHeadRules
		ProcessNewGame(	MatchID,
						*LineParts(2),	appAtoi(*LineParts(3)),			// DateTime, TimeZone
						*LineParts(4),	*LineParts(5), *LineParts(6), 	// Mapname, MapTitle, MapAuthor
						*LineParts(7),	*LineParts(8),					// GameClass, GameName
						*LineParts(9) );								// Mutators

		// Triggering StartGame time for NG as well, to make sure we have a startgame match time.
		ProcessStartGame(	MatchID,
							appAtoi(*LineParts(0)) );		// Seconds
	}
	else
	if( LineParts(1) == TEXT("SI") && ColumnNumber == 8 )	// "ServerInfo"
	{
		//	0	1	2			3   4   5					6			7
		//	33	SI	JoesServer	0	Joe	joe@epicgames.com	0.0.0.0:0	\moo\true\oink\false		//GameRules
		ProcessServer(	MatchID, ServerID,
						appAtoi(*LineParts(0)),							// Seconds
						*LineParts(2),	appAtoi(*LineParts(3)),			// ServerName, ServerRegion
						*LineParts(4),	*LineParts(5),					// AdminName, AdminEmail
						*LineParts(6),									// IP:Port
						*LineParts(7)	);								// GameRules
	}
	else
	if( LineParts(1) == TEXT("SG") && ColumnNumber == 2 )	//"StartGame"
	{
		// After NewGame, Connects, TeamChanges and NameChanges.
		// Getting this to have exact ingame times, e.g. 20 minute timelimits etc.
		//	47	SG
		ProcessStartGame(	MatchID,
							appAtoi(*LineParts(0)) );		// Seconds
	}
	else
	if( LineParts(1) == TEXT("EG") && ColumnNumber >= 3  )	//"EndGame"
	{
		// EG: teamscorelimit, serverquit, TimeLimit		//** might want to track these.
		//	0	1	2				3	4	5	6	7	8	9	10
		//	478	EG	teamscorelimit	0	5	2	7	3	6	4	1
		ProcessEndGameTime(	MatchID,
							appAtoi(*LineParts(0)) );		// Seconds
	
		// Take the scoreboard appart
		ProcessEndGamePlayers( MatchID, LineParts, ColumnNumber );

		// Cleanup - MatchLookup for this MatchID no longer used, remove it.
		SetMatchRemove( MatchID );	
	}
	else 
	{
		GWarn->Logf(TEXT("Warning: Log line not recognized or had wrong number of arguments..."));
		GWarn->Logf(TEXT("         ServerID=%d, MatchID=%d, ColumnNumber=%d"),	ServerID, MatchID, ColumnNumber );
		GWarn->Logf(TEXT("         (%s)"), *StatsLine );
	}

#if CAL_CALC_LOGLINE_TIMES
	// Remember the time for this logline type and addit to list
	SumLogLineTimes( LineParts(1), LOGLINE_STARTTIME );
#endif
	unguard;
}


#if CAL_CALC_LOGLINE_TIMES
void UProcessStatsCommandlet::SumLogLineTimes( FString& LogType, DOUBLE time )
{
	// Remember the time for this logline type and addit to list
	guard(UProcessStatsCommandlet::SumLogLineTimes);

	DOUBLE difftime = appSeconds() - time;

	// Summing times per log type
	DOUBLE* CurrentTime = LOGLINE_TIMELookup.Find( *LogType );
	if( !CurrentTime )
		LOGLINE_TIMELookup.Set( *LogType, difftime );					// New log type
	else
		LOGLINE_TIMELookup.Set( *LogType, (*CurrentTime + difftime ) );	// Add time to existing log time

	// Counting number of calls to log line type
	INT* CurrentCounter = LOGLINE_COUNTLookup.Find( *LogType );
	if( !CurrentCounter )
		LOGLINE_COUNTLookup.Set( *LogType, 1 );
	else
		LOGLINE_COUNTLookup.Set( *LogType, (*CurrentCounter + 1) );

	unguard;
}

void UProcessStatsCommandlet::DumpLogLineSummary( INT LineCount, DOUBLE overalltime )
{
	// Remember the time for this logline type and addit to list
	guard(UProcessStatsCommandlet::DumpLogLineSummary);

	// Showing summary data
	GWarn->Logf(TEXT("LogType  Time[s]  Counts     Av   "));

	DOUBLE stime=0.;
	INT scount=0;
	//TMap<FString, DOUBLE>	LOGLINE_TIMELookup;
	//TMap<FString, INT>	LOGLINE_COUNTLookup;
	for( TMap<FString, DOUBLE>::TIterator It(LOGLINE_TIMELookup); It; ++It )
	{
		FString	LogType		= It.Key();
		DOUBLE	time		= It.Value();
		INT*	countPtr	= LOGLINE_COUNTLookup.Find( LogType );
		INT count = *countPtr;
		stime += time;
		scount += count;
		GWarn->Logf(TEXT("%4s     %5.2f    %5d     %5.4f"), *LogType, time, count, time/count );
	}
	GWarn->Logf(TEXT("         %5.2f   %6d     %5.4f     Other time: %.3f  Rejected lines: %d"), 
		stime, scount, stime/scount, overalltime-stime, LineCount-scount );

	unguard;
}
#endif


/*-----------------------------------------------------------------------------
Processing rankteam calculations
-----------------------------------------------------------------------------*/

void  UProcessStatsCommandlet::ProcessRankTeam( INT MatchID, INT PlayerNumber, FLOAT Score, INT timeFrame )
{
	guard(UProcessStatsCommandlet::ProcessRankTeam);
	// Get the scorer's current teamid and rankteam value, for this timeFrame
	INT		teamid		= GetTeamID( MatchID, PlayerNumber );
	FLOAT	rankteam	= GetRankTeam( MatchID, PlayerNumber, timeFrame );

	// Getting the average rank of the opposing team, using this as virtual opponent rank.
	INT playercount = 0;									// Number of players on opposing team
	FLOAT virtualrank = CalcAvOppRank( MatchID, teamid, timeFrame, &playercount );

	// What are the dynamic factors?
	FLOAT k_dynamic_factor = FindELOFactors( timeFrame );	// Safe find

	//RANKTEAM 
	//if( Score < 0. ) GWarn->Logf(TEXT("Before: Sc(%4.1f) team(%5.2f) virt(%5.2f) f(%f)"),	Score, rankteam, virtualrank, k_dynamic_factor );
	INT rankmode = 3;										// Mode for rank team, does not change the incoming ranks!
	FLOAT diffrank = RankELO(	&rankteam,					// Rank of scorer
								&virtualrank,				// Rank of virtual opponent (the av. other teams rank)
								Score,						// Scorer's score
								0.,							// To nothing
								k_dynamic_factor,			// Dynamic factor
								rankmode );
	//if( Score < 0. )GWarn->Logf(TEXT("After:  Sc(%4.1f) team(%5.2f) virt(%5.2f) f(%f) diff(%f)"), Score, rankteam, virtualrank, k_dynamic_factor, diffrank );
	
	// New rank for scorer, and remember it in PlayerLookup
	FLOAT newrankteam = rankteam;
    if( playercount > 0 )									// There is at least one player on opposing team
	{
		if( Score > 0. )									// But you managed to "kill yourself", then reduce rank!
		{									
			newrankteam += diffrank;						// Scorers
			SetMatchRankTeam( MatchID, PlayerNumber, newrankteam, timeFrame );
		}
		else
		{
			newrankteam -= diffrank;						// Negative score events subtract score!
			if( newrankteam < 0. ) newrankteam = 0.;		// Lower edge in ranking
			SetMatchRankTeam( MatchID, PlayerNumber, newrankteam, timeFrame );
		}
	}
	else													// There is no player on opposing team
	{
		if( Score < 0. )									// But you managed to "kill yourself", then reduce rank!
		{
			newrankteam -= diffrank;						// Scorers - diffrank always positive!
			if( newrankteam < 0. ) newrankteam = 0.;		// Lower edge in ranking
			SetMatchRankTeam( MatchID, PlayerNumber, newrankteam, timeFrame );
		}
		// else... you "capped the flag", but there where no other players, so do nothing with rankteam!
	}

	// New (lessened) rank fraction for all opposing players, if no opposing players, skip
	UpdateAvOppRanks( MatchID, teamid, timeFrame, diffrank, playercount, Score );
	unguard;
}

void UProcessStatsCommandlet::UpdateAvOppRanks( INT MatchID, INT teamid, INT timeFrame, FLOAT diffrank, INT playercount, FLOAT Score )
{
	guard(UProcessStatsCommandlet::UpdateAvOppRanks);
	// Average diffrank
	FLOAT avdiffrank;
	if( playercount > 0 )
		avdiffrank = diffrank / (playercount*1.);
	else 
		return;											// No opposing team to reduce rank from.

	// Update the ranks for all players in opposing team
	FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);	

	for( TMap<INT, FPlayerData>::TIterator It(MatchLookupPtr->PlayerLookup); It; ++It )
	{
		INT PlayerNumber = It.Key();
		FPlayerData* PlayerLookupPtr = &It.Value();

		// Get teamid
		INT currentteamid = PlayerLookupPtr->teamid;

		// If player is from other team, then...
		if( currentteamid != teamid )
		{
			FLOAT rankteam = PlayerLookupPtr->rankteam[timeFrame];
			FLOAT newrankteam = rankteam;
			if( Score < 0. )
			{									
				newrankteam += avdiffrank;						// You negative scored, other team gets points!
				// PlayerLookupPtr->rankteam[timeFrame] = newrankteam;
				SetMatchRankTeam( MatchID, PlayerNumber, newrankteam, timeFrame );
			}
			else
			{
				newrankteam -= avdiffrank;						// You scored, other team looses points!
				if( newrankteam < 0. ) newrankteam = 0.;		// Lower edge in ranking
				SetMatchRankTeam( MatchID, PlayerNumber, newrankteam, timeFrame );
			}
		}
	}
	unguard;
}

FLOAT UProcessStatsCommandlet::CalcAvOppRank( INT MatchID, INT teamid, INT timeFrame, INT *playercount )
{
	guard(UProcessStatsCommandlet::CalcAvOppRank);
	// MatchLookup->PlayerLoop for players that are in other team
	FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);	
	INT counter = 0;
	FLOAT ranksum = 0.;

	for( TMap<INT, FPlayerData>::TIterator It(MatchLookupPtr->PlayerLookup); It; ++It )
	{
		//	INT PlayerNumber = It.Key();
		FPlayerData* PlayerLookupPtr = &It.Value();

		// Get teamid
		INT currentteamid = PlayerLookupPtr->teamid;

		// If player is from other team, then...
		if( currentteamid != teamid )
		{
			counter++;	
			ranksum += PlayerLookupPtr->rankteam[timeFrame];
		}
	}

    // Calculate average virual rank
	FLOAT avopprank;
	if( counter == 0)
		avopprank = 0.;				// Safe fallback... no opponents on other team!
	else 
		avopprank = ranksum / (counter*1.);

	// Remember the number of players on opposing team
	*playercount = counter;

	return avopprank;
	unguard;
}

void UProcessStatsCommandlet::MatchUpdateMutatorID( INT MatchID, INT pid )
{
	guard(UProcessStatsCommandlet::MatchUpdateMutatorID);
	// Remember the Update the ranks of ALL players in this Match to db
	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);	
	for( TMap<INT, INT>::TIterator It(MatchLookupPtr->mutidLookup); It; ++It )
	{
		// INT* mutidLookupPtr = &It.Value();
		//---Update the database with the new ranks!---
		INT mutid		= It.Key();
		INT pmutatorsid	= ISpmutatorsid( pid, mutid );
		if( pmutatorsid!=-1	)
			MySQL.DoSQL( "update pmutators set pmutators=pmutators+1 where pmutatorsid='%d' ", pmutatorsid );
	}
	unguard;
}

/*-----------------------------------------------------------------------------
Processing NameChange / TeamChange line
-----------------------------------------------------------------------------*/

void UProcessStatsCommandlet::ProcessNameChange(	INT MatchID, INT PlayerNumber, const TCHAR* NickName )
{
	// Update the player's nickname via PlayerNumber handle
	guard(UProcessStatsCommandlet::ProcessNameChange);

	// Getting playerid for this PlayerNum - Update nickname
	INT playerid = GetPlayerID( MatchID, PlayerNumber );
	// -1 = not found
	// 0 = NOUSER
	if( playerid>0 )
		MySQL.DoSQL( "update playerid set nickname='%s' where playerid='%d' ", MySQL.FormatSQL(NickName), playerid );
	unguard;
}

void UProcessStatsCommandlet::ProcessTeamChange( INT MatchID, INT PlayerNumber, INT TeamID )
{
	guard(UProcessStatsCommandlet::ProcessTeamChange);
	// Update the player's teamid number
	if( TeamID==0 || TeamID==1 )
	{
		SetMatchTeamID( MatchID, PlayerNumber, TeamID );
	}
	else
	{
		GWarn->Logf(TEXT("Warning: Changed to a TeamID that is not 0 or 1: (%d), forcing 0!"), TeamID );
		SetMatchTeamID( MatchID, PlayerNumber, 0 );
	}
	unguard;
}

/*-----------------------------------------------------------------------------
Processing Connect/Diconnect line
-----------------------------------------------------------------------------*/

void UProcessStatsCommandlet::ProcessConnect(	INT MatchID, INT Seconds, INT PlayerNumber,
												const TCHAR* StatsUsername, const TCHAR* StatsPassword )
{	
	guard(UProcessStatsCommandlet::ProcessConnect);

	// TABLE PLAYERID - Insert/Select data - Getting playerid
	INT playerid = ISplayerid( StatsUsername, StatsPassword );
	if( playerid==-1 ) return;								//%% Fallback not possible

	// Update nickname / last used - setting a default player name, the real nick gets set in NameChange.
	MySQL.DoSQL( "update playerid set nickname='(no user)', timeZone='%d', lastUsed=DATE_ADD('%s', INTERVAL %d SECOND) where playerid='%d' ",
					GetTimeZone(MatchID), MySQL.FormatSQL(GetDateTime(MatchID)), Seconds, playerid );

	// Add PlayerNumber vs playerid to mapped list for the Match
	// Also sets the teamid to 0, so if the game is a teamgame, and does NOT have any teamchange log lines, all will work.
	SetMatchPlayerID( MatchID, PlayerNumber, playerid );		

	// Remember the connect time for this player
	SetMatchConnectTime( MatchID, PlayerNumber, Seconds );

	// Get the current ids and maps data
	UBOOL	teamgame	= GetTeamGame( MatchID );
	INT		mid			= GetMID( MatchID );
	INT		svid		= GetSVID( MatchID );
	INT		mapid		= GetMapID( MatchID );
	FLOAT rankteam[4];		// Used for caching below!
	FLOAT rankelo[4];
	INT pid[4];

	// PLAYER TABLE - Update the connects counter...
	for( INT t=1; t<=3; t++ )
	{
		INT timeFrameNumber = GetTimeFrameNumber( MatchID, t );
		if( timeFrameNumber==-1 ) continue;

		// Insert/Select pid - TABLE PLAYER
			pid[t] = ISpid( playerid, t, timeFrameNumber, mid );
		if( pid[t]==-1 ) continue;								//%% Fallback not possible
		
		// Update playerConnects
		MySQL.DoSQL( "update player set playerConnects=playerConnects+1 where pid='%d' ", pid[t] );

		///---Player Ranking value updates---
		if( teamgame )
		{
			FString QueryText = FString::Printf(TEXT( "select rankTeam from player where pid='%d' "), pid[t] );
			rankteam[t] = FindIDF(*QueryText);
			SetMatchRankTeam( MatchID, PlayerNumber, rankteam[t], t );	// First value setting, from db
		}
		else
		{
			FString QueryText = FString::Printf(TEXT( "select rankELO from player where pid='%d' "), pid[t] );
			rankelo[t] = FindIDF(*QueryText);
			SetMatchRankELO( MatchID, PlayerNumber, rankelo[t], t );	// First value setting, from db
		}

		// Insert/Select pmutatorsid - TABLE PMUTATORS (also update pmutators for all mutids)
		MatchUpdateMutatorID( MatchID, pid[t] );

		// Insert/Select pmodsid - TABLE PMODS
		INT pmodsid = ISpmodsid( pid[t], mid );
		if( pmodsid!=-1 )
			MySQL.DoSQL( "update pmods set pmods=pmods+1 where pmodsid='%d' ", pmodsid );

		// Insert/Select pmapsid - TABLE PMAPS
		INT pmapsid = ISpmapsid( pid[t], mapid );
		if( pmapsid!=-1 )
			MySQL.DoSQL( "update pmaps set pmaps=pmaps+1 where pmapsid='%d' ", pmapsid );

		// SERVERS	
		INT serversid = ISserversid( svid, t, timeFrameNumber );
		if( serversid!=-1 )
			MySQL.DoSQL( "update servers set connects=connects+1 where serversid='%d' ", serversid );
	}

	//---Updating MATCH specific data---
	// First value setting, from db, Match special case t=0.
#if USE_MATCHRANKTEAM_TABLE
	if( teamgame )
		SetMatchRankTeam( MatchID, PlayerNumber, 0., 0 );
#endif
#if USE_MATCHRANKELO_TABLE
	if( !teamgame ) 
		SetMatchRankELO( MatchID, PlayerNumber, 0., 0 );	
#endif

	//---Remember the player's "global" rank before the match---
	INT T				= 3;											// timeFrame Alltime
	INT timeFrameNumber	= GetTimeFrameNumber( MatchID, T );		
	INT	mpid			= ISmpid( MatchID, playerid );	
	if( timeFrameNumber!=-1 && pid[T]!=-1 && mpid!=-1 )
	{
		FLOAT rankbeg = 0.;
		if( teamgame )
			rankbeg = rankteam[T];
		else
			rankbeg = rankelo[T];
		// Update update matchplayers table and remember the re-match rank
		MySQL.DoSQL( "update matchplayers set rankbeg='%f' where mpid='%d' ", rankbeg, mpid );
	}

	// TABLE MATCHES
	// Update playerconnects info
	INT matchesid = ISmatchesid( MatchID );
	if( matchesid!=-1 )
		MySQL.DoSQL( "update matches set playerconnects=playerconnects+1 where matchesid='%d' ", matchesid );

	// Update number of player connects counter
	INT	mapsid	= ISmapsid( svid, mid, mapid );
	if( mapsid!=-1 )
		MySQL.DoSQL( "update maps set connects=connects+1 where mapsid='%d' ", mapsid );

	unguard;
}

void UProcessStatsCommandlet::ProcessDisconnect( INT MatchID, INT Seconds, INT PlayerNumber )
{
	guard(UProcessStatsCommandlet::ProcessDisconnect);
	// Getting vars
	INT		ConnectTime	= GetConnectTime( MatchID, PlayerNumber );
	INT		playerid	= GetPlayerID( MatchID, PlayerNumber );
	INT		mid			= GetMID( MatchID );
	UBOOL	teamgame	= GetTeamGame( MatchID );
	INT pid[4];
	FLOAT Crankend[4];

	// Calc player minutes and update database
	for( INT t=1; t<=3; t++ )
	{
		INT	timeFrameNumber = GetTimeFrameNumber( MatchID, t );
		if( timeFrameNumber==-1 ) continue;

			pid[t] = ISpid( playerid, t, timeFrameNumber, mid );
		if( pid[t]!=-1 )
		{
			INT playerminutes = (Seconds - ConnectTime) / 60.;
			MySQL.DoSQL( "update player set playerMinutes='%d' where pid='%d' ", playerminutes, pid[t] );

			// Player disconnected, update rankteam/rankelo to DB?
			if( teamgame )
			{
				Crankend[t] = ProcessDumpRankTeamOne( MatchID, PlayerNumber, pid[t], t );	
			}
			else
			{
				Crankend[t] = ProcessDumpRankELOOne( MatchID, PlayerNumber, pid[t], t );	
			}

		}
	}

	//---MATCH specific updates---
	//---Remember the player's "global" rank when he leaves the match--- 
	INT T = 3;													// timeFrame Alltime
	INT	timeFrameNumber = GetTimeFrameNumber( MatchID, T );
	INT	mpid = ISmpid( MatchID, playerid );	
	if( timeFrameNumber!=-1 && pid[T]!=-1 && mpid!=-1 )
	{
		FLOAT playerminutes = (Seconds - ConnectTime) / 60.;	// Time player was in game in minutes
		FLOAT rankend = 0.;
		if( teamgame )
			rankend = Crankend[T];						// Cache lookup
		else
			rankend = Crankend[T];						// Cache lookup

		// Update update matchplayers table and remember the re-match rank
		MySQL.DoSQL( "update matchplayers set rankend='%f', playerminutes='%f' where mpid='%d' ", 
			rankend, playerminutes, mpid );

#if CAL_MATCHPLAYERS_SUMMING		
		ProcessDumpMPSumOne( mpid );
#endif
	}

	// If a player disconnects then remove him from PlayerLoopup to avoid ranking etc of "removed" players.
	FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);	
	FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find(PlayerNumber);

	// Find works, Ptr exists, delete PlayerLookup for PlayerNumber
	if( PlayerLookupPtr )
		MatchLookupPtr->PlayerLookup.Remove(PlayerNumber);		//!! hope this works
	unguard;
}


/*-----------------------------------------------------------------------------
Processing NewGame / EndGame / StartGame line, and Server info
-----------------------------------------------------------------------------*/

void UProcessStatsCommandlet::ProcessServer(	INT MatchID,				INT ServerID,			INT Seconds, 
												const TCHAR* ServerName,	INT ServerRegion, 
												const TCHAR* AdminName,		const TCHAR* AdminEmail, 
												const TCHAR* ipPort,		const TCHAR* GameRules )
{
	guard(UProcessStatsCommandlet::ProcessServer);

	// Remember svid - once per match
	INT svid = ISsvid( ServerID );
	if( svid==-1 ) return;									// All below needs a valid svid.
	SetMatchSVID( MatchID, svid );

	// SERVER
	// Find out if this firstConnect has been set before, if not set it once!
	FString QueryText = FString::Printf(TEXT( "select firstUse from server where svid='%d' "), svid );
	INT firstUse = FindID(*QueryText);
	if( firstUse==1 )										// Actually first time, serverid was seen					
	{
		// Set firstConnect date/time and reset the firstUse flag
		// firstUse, serverRegion, firstConnect -  serverName, adminName, adminEmail, ipPort, gameRules 
		MySQL.DoSQL( "update server set firstUse='0', serverRegion='%d', firstConnect=DATE_ADD('%s', INTERVAL %d SECOND), gameRules='%s' where svid='%d' ", 
			ServerRegion, MySQL.FormatSQL(GetDateTime(MatchID)), Seconds, MySQL.FormatSQL(GameRules), svid );
	}
	else
	{
		// gameRules
		MySQL.DoSQL( "update server set gameRules='%s' where svid='%d' ", 
			MySQL.FormatSQL(GameRules), svid );
	}
	// serverName, adminName, adminEmail, ipPort
	MySQL.DoSQL( "update server set serverName='%s', adminName='%s', adminEmail='%s', ipPort='%s' where svid='%d' ", 
		MySQL.FormatSQL(ServerName), MySQL.FormatSQL(AdminName), MySQL.FormatSQL(AdminEmail), MySQL.FormatSQL(ipPort), svid );

	// SERVERS
	for( INT t=1; t<=3; t++ )
	{
			INT timeFrameNumber = GetTimeFrameNumber( MatchID, t );
			if( timeFrameNumber==-1 ) continue;
			
			INT serversid = ISserversid( svid, t, timeFrameNumber );
			if( serversid==-1 ) continue;

			// maps
			MySQL.DoSQL( "update servers set maps=maps+1 where serversid='%d' ", serversid );
	}

	//---Updating MATCH specific data---
	// TABLE MATCHES
	// Update server/ mapid / mid / gamerules info
	INT	mapid		= GetMapID( MatchID );
	INT mid			= GetMID( MatchID );
	INT matchesid	= ISmatchesid( MatchID );
	if( matchesid!=-1 )
		MySQL.DoSQL( "update matches set svid='%d', mapid='%d', mid='%d', gameRules='%s' where matchesid='%d' ", 
		svid, mapid, mid, MySQL.FormatSQL(GameRules), matchesid );

	// Update the games counter - how often this map was played this server, mod
	INT	mapsid = ISmapsid( svid, mid, mapid );
	if( mapsid!=-1 ) 
		MySQL.DoSQL( "update maps set games=games+1 where mapsid='%d' ", mapsid );

	unguard;
}

void UProcessStatsCommandlet::ProcessStartGame( INT MatchID, INT Seconds )
{
	guard(UProcessStatsCommandlet::ProcessStartGame);
	// Getting the var
	INT matchesid = ISmatchesid( MatchID );

	// Update Start time in match, using current SG line!
	if( matchesid!=-1 )
		MySQL.DoSQL( "update matches set timeZone='%d', startmatch='%s', startgame=DATE_ADD('%s', INTERVAL %d SECOND) where matchesid='%d' ", 
			GetTimeZone(MatchID), MySQL.FormatSQL(GetDateTime(MatchID)), MySQL.FormatSQL(GetDateTime(MatchID)), Seconds, matchesid );
	unguard;
}

void UProcessStatsCommandlet::ProcessEndGamePlayers( INT MatchID, TArray<FString>& LineParts, INT ColumnNumber )
{
	guard(UProcessStatsCommandlet::ProcessEndGamePlayers);
	//	0	1	2				3	4	5	6	7	8	9	10
	//	478	EG	teamscorelimit	0	5	2	7	3	6	4	1

	// Misc match vars
	INT		mid			= GetMID( MatchID );
	UBOOL	teamgame	= GetTeamGame( MatchID );
	FLOAT	teamscore=0., teamscore2=0.;

	// MATCHES - endCause
	INT matchesid = ISmatchesid( MatchID );
	if( matchesid!=-1 )
		MySQL.DoSQL( "update matches set endCause='%s' where matchesid='%d' ", MySQL.FormatSQL(*LineParts(2)), matchesid );

	// PLAYER - place1st, place2nd, place3rd
	INT pMax = ColumnNumber - 3;								// Columnnumber is offset by 1 from above (values)

	//** Placement, if bots are in front e.g. p=0, is wrong fro players
	for( INT p=0; p<pMax; p++ )
	{
		INT PlayerNumber	= appAtoi(*LineParts( 3 + p ));		// 3,4,5,...
		INT playerid		= GetPlayerIDnoWarn( MatchID, PlayerNumber );	// Ignore bot spamming!
		if( playerid==-1 )	continue;

		INT teamid			= GetTeamID( MatchID, PlayerNumber );
		if( teamid==-1 )	continue;

		if( teamgame ) 
		{
			teamscore	= GetTeamScore( MatchID, teamid );
			teamscore2	= GetTeamScore( MatchID, !teamid );		// Other team
		}

		INT pid[4];												// More caching
		FLOAT Crankend[4];
		for( INT t=1; t<=3; t++ )
		{
			INT timeFrameNumber = GetTimeFrameNumber( MatchID, t );
			if( timeFrameNumber==-1 ) continue;

			// Insert/Select pid - TABLE PLAYER
				pid[t] = ISpid( playerid, t, timeFrameNumber, mid );
			if( pid[t]!=-1 )
			{
				// PLACEMENT
				FString Placement;
				if( p==0 )
					Placement = FString::Printf(TEXT("place1st=place1st+1"));	
				else if( p==1 )
					Placement = FString::Printf(TEXT("place2nd=place2nd+1"));	
				else if( p==2 )
					Placement = FString::Printf(TEXT("place3rd=place3rd+1"));	
				else
					Placement = FString::Printf(TEXT("placeRest=placeRest+1"));	

				// TEAMWINS / TEAMLOSSES
				FString TeamWinsLosses	= FString::Printf(TEXT( "" ));	// Default empty 
				if( teamgame )
				{ 
					// EG ... dump all rankteam infos from RAM to DB!
					Crankend[t] = ProcessDumpRankTeamOne( MatchID, PlayerNumber, pid[t], t );	
					if( teamscore > teamscore2 )
						TeamWinsLosses = FString::Printf(TEXT(",teamWins=teamWins+1"));	
					else
						TeamWinsLosses = FString::Printf(TEXT(",teamLosses=teamLosses+1"));	
				}
				else 
				{
					// EG ... dump all rankelo infos from RAM to DB!
					Crankend[t] = ProcessDumpRankELOOne( MatchID, PlayerNumber, pid[t], t );	
				}

				// Update: Placement, GAMESFINISHED, TeamWins/TeamLosses
				MySQL.DoSQL( "update player set %s, gamesFinished=gamesFinished+1 %s where pid='%d' ", 
					MySQL.FormatSQL(*Placement), MySQL.FormatSQL(*TeamWinsLosses), pid[t] );
			}
		}		// end t - timeFrame

		// MATCH specific
		//---Remember the player's "global" rank when the game ends---
		INT T				= 3;				// timeFrame Alltime
		INT timeFrameNumber = GetTimeFrameNumber( MatchID, T );
		INT	mpid			= ISmpid( MatchID, playerid );	
		if( timeFrameNumber!=-1 && pid[T]!=-1 && mpid!=-1 )
		{
			FLOAT rankend = 0.;
			if( teamgame )
				rankend = Crankend[T];			// Cache lookup
			else
				rankend = Crankend[T];

			// Update update matchplayers table and remember the re-match rank
			MySQL.DoSQL( "update matchplayers set rankend='%f' where mpid='%d' ", rankend, mpid );
		}
#if CAL_MATCHPLAYERS_SUMMING		
		if( mpid!=-1 )
			ProcessDumpMPSumOne( mpid );
#endif
	}			// end p - players

	unguard;
}

void UProcessStatsCommandlet::ProcessEndGameTime( INT MatchID, INT Seconds )
{
	guard(UProcessStatsCommandlet::ProcessEndGameTime);
	// Getting all the vars
	INT	svid		= GetSVID( MatchID );
	INT mid			= GetMID( MatchID );
	INT mapid		= GetMapID( MatchID );

	// PLAYERMINUTES in MYSQL,  
	// Update the time of ALL players in this Match to db
	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);	
	for( TMap<INT, FPlayerData>::TIterator It(MatchLookupPtr->PlayerLookup); It; ++It )
	{
		FPlayerData* PlayerLookupPtr = &It.Value();

		//---Update the database with the new times!---
		INT		playerid	= PlayerLookupPtr->playerid;
		INT		ConnectTime	= PlayerLookupPtr->ConnectTime;

		// PLAYER update
		for( INT t=1; t<=3; t++ )
		{
			INT timeFrameNumber = GetTimeFrameNumber( MatchID, t );
			if( timeFrameNumber==-1 ) continue;

			INT pid = ISpid( playerid, t, timeFrameNumber, mid );
			if( pid!=-1 )
			{
				INT playerminutes = (Seconds - ConnectTime) / 60.;
				MySQL.DoSQL( "update player set playerMinutes=playerMinutes+'%d' where pid='%d' ", playerminutes, pid );
			}
		}

		// MATCH update
		INT mpid = ISmpid( MatchID, playerid );
		if( mpid!=-1 )
		{
			FLOAT playerminutes = (Seconds - ConnectTime) / 60.;	// Time player was in game in minutes
			MySQL.DoSQL( "update matchplayers set playerminutes='%f' where mpid='%d' ", playerminutes, mpid );
		}
	}

	// Minutes
	INT minutes = Seconds / 60.;		

	// Updating the time the map was running / and when map was last played
	INT	mapsid = ISmapsid( svid, mid, mapid );
	if( mapsid!=-1 )
		MySQL.DoSQL( "update maps set uptime=uptime+'%d', timeZone='%d', lastPlayed=DATE_ADD('%s', INTERVAL %d SECOND) where mapsid='%d' ", 
			minutes, GetTimeZone(MatchID), MySQL.FormatSQL(GetDateTime(MatchID)), Seconds, mapsid );

	// Update Endgame time in match, using current SG line!
	INT matchesid = ISmatchesid( MatchID );
	if( matchesid!=-1 )
		MySQL.DoSQL( "update matches set endgame=DATE_ADD('%s', INTERVAL %d SECOND) where matchesid='%d' ", 
			MySQL.FormatSQL(GetDateTime(MatchID)), Seconds, matchesid );	// TimeZone only set for startgame!

	// SERVERS
	for( INT t=1; t<=3; t++ )
	{
		INT timeFrameNumber = GetTimeFrameNumber( MatchID, t );
		if( timeFrameNumber==-1 ) continue;
		
		INT serversid = ISserversid( svid, t, timeFrameNumber );
		if( serversid==-1 ) continue;

		// uptime, lastUpdate, timeZone
		MySQL.DoSQL( "update servers set uptime=uptime+'%d', timeZone='%d', lastUpdate=DATE_ADD('%s', INTERVAL %d SECOND) where serversid='%d' ", 
			minutes, GetTimeZone(MatchID), MySQL.FormatSQL(GetDateTime(MatchID)), Seconds, serversid );
	}
	unguard;
}

void UProcessStatsCommandlet::ProcessNewGame(	INT MatchID,
									const TCHAR* DateTime,  INT TimeZone,
									const TCHAR* MapName,	const TCHAR* MapTitle,	const TCHAR* MapAuthor,
									const TCHAR* GameClass, const TCHAR* GameName,	const TCHAR* Mutators )
{
	guard(UProcessStatsCommandlet::ProcessNewGame);

	// Generate / Filling struct for this MatchID
	FMatchData	CurrentFMatchData;
	CurrentFMatchData.DateTime		= FString(DateTime);
	CurrentFMatchData.TimeZone		= TimeZone;
//	CurrentFMatchData.MSDateTime	= FString(DateTime);	//**FIXME - real Master server date/time and TimeZone
//	CurrentFMatchData.MSTimeZone	= TimeZone;				//**FIXME
	CurrentFMatchData.MapName		= FString(MapName);
	CurrentFMatchData.MapTitle		= FString(MapTitle);
	CurrentFMatchData.MapAuthor		= FString(MapAuthor);
	CurrentFMatchData.GameName		= FString(GameName);	//$$ Actually not used
	CurrentFMatchData.Mutators		= FString(Mutators);

	// Gametype / Mutator checking
	// PURE? - If Mutators has a "|" in it's string then a mutator was activated -> unpure!
	UBOOL pure = 0;											// By default the gametype is not pure 	
	INT i = FString(Mutators).InStr( TEXT("|") );
	if( i==-1 )
		pure = 1;											// Only	standard mutator found, match mutator free
	CurrentFMatchData.pure = pure;

	// Remember mid
	// If not a pure game then prefix GameClass with "!"
	// If mod is unknown, e.g. custom, then assign "!XGame.xOther"
	FString tempGameClass = FString(GameClass);
	if( pure == 0 )
		tempGameClass = FString::Printf(TEXT("!%s"), GameClass );
	INT mid = FindModID( *tempGameClass );
	CurrentFMatchData.GameClass	= tempGameClass;			// Remember "fixed" GameClass
	CurrentFMatchData.mid		= mid;

	// Find out if this mod is a teamgame, e.g. for rankteam code.
	FString QueryText = FString::Printf(TEXT( "select teamgame from mod where mid='%d' "), mid );
	INT teamgame = FindID(*QueryText);						
	if( teamgame==-1 ) teamgame=0;							//%% Fallback, 0, no teamgame
	CurrentFMatchData.teamgame	= teamgame;					// Remember this per Match (UBOOL convertion should work)

	// Teamscore tracking, reset
	for( INT team=0; team<4; team++ )
		CurrentFMatchData.teamscore[team] = 0.;


	//---Generate MatchLookup for MatchID---
	MatchLookup.Set( MatchID, CurrentFMatchData );			// Set will overwrite a prev. SetMatchInitGet(MatchID)


	// Setting the timeFrameNumbers for this Match
	for( INT t=1; t<=3; t++ )
	{
		// Getting the currentTF for this timeframe
		INT timeFrameNumber = FindTimeFrameNumber( t );		// Current cached global TFNs
		if( timeFrameNumber==-1 ) return;					//%% No way to recover, TFN is vital!
		SetMatchTimeFrameNumber( MatchID, t, timeFrameNumber );
	}

	// PROCESSING AFTER MatchLookup and TFN have been set up!
	// What mutators where seen? Update db.
	ProcessMutators( MatchID, Mutators );

	// Update the MODS table counter
	ProcessMod( MatchID, mid );

	// Remember MapID
	INT mapid = FindMapID( MatchID, MapName );				// MatchID struct, must already exist! For auto-update!
	SetMatchMapID( MatchID, mapid );

	unguard;
}

void UProcessStatsCommandlet::ProcessMod( INT MatchID, INT mid )
{
	guard(FUProcessStatsCommandlet::ProcessMod);
	for( INT t=1; t<=3; t++ )
	{
		INT timeFrameNumber = GetTimeFrameNumber( MatchID, t );
		if( timeFrameNumber==-1 ) continue;

		// TABLE MODS (insert first, then update :)
		INT modsid = ISmodsid( t, timeFrameNumber, mid );
		if( modsid!=-1 )
			MySQL.DoSQL( "update mods set mods=mods+1 where modsid='%d' ", modsid );
	}
	unguard;
}

void UProcessStatsCommandlet::ProcessMutators(	INT MatchID, const TCHAR* Mutators )
{
	// Mutators:	game is pure is no "|" in line only default mutator "UnrealGame.DMMutator",
	//				count other mutators, remember if pure or not!
	//				into Match Tables
	guard(UProcessStatsCommandlet::ProcessMutators);

	// Working copy of Mutator list
	FString tempMutators = FString(Mutators);

	// Remove "Mutators=" from line
	INT i = tempMutators.InStr( TEXT("Mutators=") );
	if( i!=-1 )
		tempMutators = tempMutators.Mid( i+9 );		// 9 = appStrlen(TEXT("Mutators="))

	// Chop the Mutators up into bits on "|", e.g. UnrealGame.DMMutator|UnrealGame.MutBigHead|XGame.MutInstaGib
	TArray<FString> MutatorParts;
	SnipAppart( MutatorParts, tempMutators, TEXT("|") );

	// Update database with new mutator counts
	for( INT i=0; i<MutatorParts.Num(); i++ )
	{
		INT mutid = FindMutID( *MutatorParts(i) );
		SetMatchMutatorID( MatchID, mutid );		// remember in mutidLookup what mutators turned on for match
	
		for( INT t=1; t<=3; t++ )
		{
			INT timeFrameNumber = GetTimeFrameNumber( MatchID, t );
			if( timeFrameNumber==-1 ) continue;

			// TABLE MUTATORS
			INT mutatorsid = ISmutatorsid( t, timeFrameNumber, mutid );
			if( mutatorsid!=-1 )
				MySQL.DoSQL( "update mutators set mutators=mutators+1 where mutatorsid='%d' ", mutatorsid );
		}

		//---Updating MATCH specific data---
		// TABLE MATCHMUTATORS
		INT mmutid = ISmmutid( MatchID, mutid );	// mutators=1 is max, but heck :)
		if( mmutid!=-1 )
			MySQL.DoSQL( "update matchmutators set mutators=mutators+1 where mmutid='%d' ", mmutid );	
	}
	unguard;
}

/*-----------------------------------------------------------------------------
Processing KILL line - Normal Kills / Suicides
-----------------------------------------------------------------------------*/

void UProcessStatsCommandlet::ProcessKill( INT MatchID, INT KillerNumber,			const TCHAR* DamageType,
											 INT VictimNumber,			const TCHAR* VictimWeapon,
											 UBOOL TeamKill )
{
	guard(UProcessStatsCommandlet::ProcessKill);

	if( KillerNumber == -1 )
	{
		// An environment induced death (suicide), or might be a death caused by an actor that is no longer available
		if( VictimNumber == -1 )
		{
			// We are missing any player ID, Weapon infos... will need to discard this line
			GWarn->Logf(TEXT("Warning: KILL log line has no VictimNum (%d) (WeaponName (%s)) info - ignoring!"),
				VictimNumber, VictimWeapon );
			return;
		}
		else
		{
			// Upping the enviro suicide counters for Victim (typing is DT "Suicides")
			// Using Victim data, because this works for weapon suis and enviro suis.
			ProcessKillSuicide( MatchID, VictimNumber, DamageType );		// (0 Enviro Suicide)
		}

	}
	else
	if( KillerNumber == VictimNumber )
	{
		// This is a weapon related (suicide)
		// Using Victim data, because this works for weapon suis and enviro suis.
		ProcessKillSuicide( MatchID, VictimNumber, DamageType );			// (1 Weapon Suicide)
	}
	else
	{
		// This is a standard weapon related kill, killer's damagetype defines "weapon", also for TKs
		ProcessKillNormal( MatchID, KillerNumber, DamageType, VictimNumber, VictimWeapon, TeamKill );
	}

	unguard;
}

void UProcessStatsCommandlet::ProcessKillSuicide( INT MatchID, INT VictimNumber, const TCHAR* DamageType )
{
	guard(UProcessStatsCommandlet::ProcessKillSuicide);
	// Getting database relevant ids
	INT playerid		= GetPlayerID( MatchID, VictimNumber );
	INT mid				= GetMID( MatchID );
	INT	sid				= FindSuicideID( DamageType );	// Damagetype defines suicide type, does not matter if weapon or enviro
	INT mpid			= ISmpid( MatchID, playerid );	// TABLE MATCHPLAYERS
	UBOOL teamgame		= GetTeamGame( MatchID );
	INT rankmode		= 2;							// mode for suicides
	INT pid[4];
	INT timeFrameNumber[4];
	FLOAT k_dynamic_factor[4];

	// Caching TFN, and dynamic factors
	for( INT t=1; t<=3; t++ )
	{
		timeFrameNumber[t]	= GetTimeFrameNumber( MatchID, t );
		k_dynamic_factor[t]	= FindELOFactors( t );
	}

	// Updating the counters
	for( INT t=1; t<=3; t++ )
	{
		if( timeFrameNumber[t]==-1 ) continue;

		// Insert/Select pid - TABLE PLAYER
		pid[t] = ISpid( playerid, t, timeFrameNumber[t], mid );
		if( pid[t]==-1 ) continue;								//%% Fallback not possible

		// TABLE SUICIDES
		INT suicidesid = ISsuicidesid( pid[t], sid );
		if( suicidesid!=-1 )
			MySQL.DoSQL( "update suicides set suicides=suicides+1 where suicidesid='%d' ", suicidesid );

#if USE_VICTORS_TABLE
		// TABLE VICTORS
		INT svictorid = ISvictorid( pid[t], pid[t] );				// Suicide victor ID
		if( svictorid!=-1 )
			MySQL.DoSQL( "update victors set kills=kills-1 where victorid='%d' ", svictorid );
#endif	
	}	//end t - timeFrame


	//---Calculate the ELO rank--- only for non teamgames!
	if( teamgame==0 )
	{
		for( INT t=1; t<=3; t++ )
		{
			if( timeFrameNumber[t]==-1 || pid[t]==-1 ) continue;
			FLOAT s_rankELO, s_rankELO2;

			// Get the suicider's current rank in ELO
			s_rankELO = 0.;					//default rank for new player is 0.
			s_rankELO	= GetRankELO( MatchID, VictimNumber, t );
			s_rankELO2	= s_rankELO;	

			//SUICIDE
			RankELO(&s_rankELO,				//suicider's last rank, pointers return NEW values
					&s_rankELO2,			//dummy, just to be sure, but changes nothing.
					1.,						//suicider's score against himself
					0.,
					k_dynamic_factor[t],	//dynamic factors
					rankmode );

			// Update rank
			SetMatchRankELO( MatchID, VictimNumber, s_rankELO, t );	
		}	//end t - timeFrame

#if USE_MATCHRANKELO_TABLE
		// Match based ELO RANKING
		if( mpid!=-1 )	
		{
			INT t=1;						// Weekly dynamic factor used
			FLOAT s_rankELOmatch = 0.;
			// Get Match rankELO
			s_rankELOmatch = GetRankELO( MatchID, VictimNumber, 0 );	// Timeframe 0 - needs testing!

			// Calculate new rankELO
			RankELO( &s_rankELOmatch, &s_rankELOmatch,	1.,	0., k_dynamic_factor[t], rankmode );

			// Update rankELO in DB
			SetMatchRankELO( MatchID, VictimNumber, s_rankELOmatch, 0 );
		}
#endif
	}

	//---Updating MATCH specific counters ---
#if !CAL_MATCHPLAYERS_SUMMING
	if( mpid!=-1 )
		MySQL.DoSQL( "update matchplayers set suicides=suicides+1 where mpid='%d' ", mpid );
#endif

#if USE_MATCHVICTORS_TABLE
	//---MATCHVICTORS TABLE---
	INT smvictorid = ISmvictorid( mpid, mpid );				// Suicide victor ID
	if( smvictorid!=-1 )
		MySQL.DoSQL( "update matchvictors set kills=kills-1 where mvictorid='%d' ", smvictorid );
#endif

	// Now damagetype specific
	// TABLE MATCHSUICIDES
	// Update kills/deaths counters
	INT msid = ISmsid( mpid, sid );
	if( msid!=-1 )
		MySQL.DoSQL( "update matchsuicides set suicides=suicides+1 where msid='%d' ", msid );

	//---Updating MAPS data---
	// Update suicides counter
	INT svid	= GetSVID( MatchID );
	INT mapid	= GetMapID( MatchID );
	INT mapsid	= ISmapsid( svid, mid, mapid );
	if( mapsid!=-1 )
		MySQL.DoSQL( "update maps set suicides=suicides+1 where mapsid='%d' ", mapsid );

	unguard;
}


void UProcessStatsCommandlet::ProcessKillNormal(	INT MatchID, 
													INT KillerNumber, const TCHAR* DamageType,
													INT VictimNumber, const TCHAR* VictimWeapon,
													UBOOL TeamKill )
{
	guard(UProcessStatsCommandlet::ProcessKillNormal);
	// Getting database relevant playerids
	INT mid = GetMID(MatchID);
	INT wid[2];
	INT mpid[2];
	INT playerid[2];
	INT pid[2][4];												// Only elements [0,1] [t = 1,2,3] used
	INT timeFrameNumber[4];

	// Caching some data
	playerid[0] = GetPlayerID( MatchID, KillerNumber );
	playerid[1] = GetPlayerID( MatchID, VictimNumber );
	for( INT t=1; t<=3; t++  )
	{
		timeFrameNumber[t] = GetTimeFrameNumber( MatchID, t );
		for( INT p=0; p<2; p++ )								// 0 killer, 1 victim
		{
			if( t == 1 )										// Just once	
				mpid[p] = ISmpid( MatchID, playerid[p] );		// TABLE MATCHPLAYERS - ** check if needed here?!
			pid[p][t] = ISpid( playerid[p], t, timeFrameNumber[t], mid );
#if CAL_HK_PLAYERSSUM_ALT2
			pid[p][t] = ISpidHKwrap( playerid[p], t, timeFrameNumber[t], mid );
#else
			pid[p][t] = ISpid( playerid[p], t, timeFrameNumber[t], mid );
#endif
		}
	}

	// Main counter loop
	for( INT p=0; p<2; p++ )									// 0 killer, 1 victim
	{
		for( INT t=1; t<=3; t++ )								// Loop for the (3) timeFrames
		{
			if( timeFrameNumber[t]==-1 ) continue;
			if( pid[p][t]==-1 ) continue;

			// TABLE WEAPONS
			if( TeamKill==0  )									// Only for kills, not teamkills
			{
				// TABLE WEAPON - Find out what weapons where used in the kill
				FString empty = FString::Printf(TEXT(""));
				if( p == 0 )
					wid[p] = FindWeaponWid( *empty, DamageType );	// Killer damagetype used to find weapon!
				else
					wid[p] = FindWeaponWid( VictimWeapon, *empty );	// Victim does not have a damagetype, assuming primary!
				if( wid[p]==-1 ) continue;

				INT weaponsid = ISweaponsid( pid[p][t], wid[p] );
				if( weaponsid!=-1 )
				{
					if( p == 0 )								// Killer
						MySQL.DoSQL( "update weapons set kills=kills+1 where weaponsid='%d' ", weaponsid );
					else										// Victim
						MySQL.DoSQL( "update weapons set deaths=deaths+1 where weaponsid='%d' ", weaponsid );
				}
			}
			//---TEAMKILLS---
			else												// Only for teamkills, not kills
			{
				if( p == 0 )									// Killer
					MySQL.DoSQL( "update player set teamKills=teamKills+1 where pid='%d' ", pid[p][t] );
				else											// Victim
					MySQL.DoSQL( "update player set teamDeaths=teamDeaths+1 where pid='%d' ", pid[p][t] );
			}
		}	// end t - timeFrame
	}		// end p - playerIDs


	// Calculate the ELO rank	from pid[p][t] for 2 players and 3 timeFrames
	// ----------------------
	UBOOL teamgame = GetTeamGame(MatchID);

	//---DM ranking for non-teamgames only!---
	// Bit redundant, since Teamkills should never happen in DM
	FLOAT oldrank[2];
	if ( teamgame==0 && TeamKill==0 )	
	{
		FLOAT k_dynamic_factor[4];
		for( INT t=1; t<=3; t++ )									// Loop for the (3) timeFrames
		{
			FLOAT kv_rankELO[2];
			// Get k factors
			k_dynamic_factor[t] = FindELOFactors( t );				// Safe find

			// Get old ranks	
			for( INT p=0; p<2; p++ )
			{
				kv_rankELO[p] = 0.;									// Set safe defaults
				if( pid[p][t]==-1 ) continue;	
				kv_rankELO[p]	= GetRankELO( MatchID, p==0?KillerNumber:VictimNumber, t );
				oldrank[p] = kv_rankELO[p];							// Remember old rank
			}

			// KILL
			INT rankmode = 0;										//default mode for kills
			FLOAT rankdiff;
			rankdiff = RankELO(	&kv_rankELO[0],						//killer's last rank, pointers return NEW values
								&kv_rankELO[1],						//victim's last rank, pointers return NEW values
								1.,									//killer's score
								0.,									//victim's score
								k_dynamic_factor[t],				//dynamic factors
								rankmode );

			for( INT p=0; p<2; p++ )
			{
				if( pid[p][t]!=-1 )
				{
					SetMatchRankELO( MatchID, p==0?KillerNumber:VictimNumber, kv_rankELO[p], t );						
				}
			}
		}		//end t - timeFrame

#if USE_MATCHRANKELO_TABLE
		//---rankELO MATCH RANKING---
		FLOAT kv_rankELOmatch[2];
		for( INT p=0; p<2; p++ )
		{
			kv_rankELOmatch[p] = 0.;
			if( mpid[p]==-1 ) continue;	

			// Only get rankELO for player onceif this is not a teamgame! and do it only once!
			kv_rankELOmatch[p] = GetRankELO( MatchID, p==0?KillerNumber:VictimNumber, 0 );		//t=0!
			oldrank[p] = kv_rankELOmatch[p];	
		}	// end p

		// Calculated a kill in MATCH, only once!
		INT rankmode = 0;				//default mode for kills
		FLOAT rankdiff;
			rankdiff = RankELO( &kv_rankELOmatch[0], &kv_rankELOmatch[1], 1., 0., k_dynamic_factor[1], rankmode );

		for( INT p=0; p<2; p++ )
		{
			if( mpid[p]!=-1 )
			{
				SetMatchRankELO( MatchID, p==0?KillerNumber:VictimNumber, kv_rankELOmatch[p], 0 );						
			}
		}
#endif
	}


#if USE_VICTORS_TABLE
	//---Updating/Calculating the VICTORS TABLE---
	for( INT t=1; t<=3; t++ )
	{
		if( pid[0][t]==-1 || pid[1][t]==-1 ) continue;					//** Remove later, Killer pid or Victim vid

		//---TEAMKILLS---
		if( TeamKill )
		{
			INT svictorid = ISvictorid( pid[0][t], pid[0][t] );			// TeamKill = Suicide victor ID
			if( svictorid!=-1 )
				MySQL.DoSQL( "update victors set kills=kills-1 where victorid='%d' ", svictorid );
		}
		else															
		{
			INT victorid = ISvictorid( pid[0][t], pid[1][t] );			// Only for kills, not teamkills
			if( victorid!=-1 )
				MySQL.DoSQL( "update victors set kills=kills+1 where victorid='%d' ", victorid );
		}
	}	// end t - timeFrame
#endif

#if USE_MATCHVICTORS_TABLE
	//---Updating/Calculating the MATCHVICTORS TABLE---
	if( TeamKill )
	{
		if( mpid[0]!=-1 )												//** Remove later
		{
			INT smvictorid = ISmvictorid( mpid[0], mpid[0] );			// TeamKill = Suicide victor ID
			if( smvictorid!=-1 )
				MySQL.DoSQL( "update matchvictors set kills=kills-1 where mvictorid='%d' ", smvictorid );
		}
	}
	else															
	{
		if( mpid[0]!=-1 && mpid[1]!=-1 )
		{
			INT mvictorid = ISmvictorid( mpid[0], mpid[1] );			// Only for kills, not teamkills
			if( mvictorid!=-1 )
				MySQL.DoSQL( "update matchvictors set kills=kills+1 where mvictorid='%d' ", mvictorid );
		}
	}
#endif

	//---Updating MATCH specific data---
	for( INT p=0; p<2; p++ )
	{
		if( mpid[p]==-1 ) continue;

		// Update Match kills/deaths counters
		if( TeamKill )
		{
			if( p == 0 )	//Killer
				MySQL.DoSQL( "update matchplayers set teamkills=teamkills+1 where mpid='%d' ", mpid[p] );
			else			//Victim
				MySQL.DoSQL( "update matchplayers set teamdeaths=teamdeaths+1 where mpid='%d' ", mpid[p] );
		}
		else
		{
#if !CAL_MATCHPLAYERS_SUMMING
			if( p == 0 )	//Killer
				MySQL.DoSQL( "update matchplayers set kills=kills+1 where mpid='%d' ", mpid[p] );
			else			//Victim
				MySQL.DoSQL( "update matchplayers set deaths=deaths+1 where mpid='%d' ", mpid[p] );
#endif
			// TABLE MATCHWEAPONS - not happening for Teamkills!
			INT mwid = ISmwid( mpid[p], wid[p] );	
			if( mwid!=-1 )
			{
				// Update kills/deaths counters
				if( p == 0 )		//Killer
					MySQL.DoSQL( "update matchweapons set kills=kills+1 where mwid='%d' ", mwid );
				else				//Victim
					MySQL.DoSQL( "update matchweapons set deaths=deaths+1 where mwid='%d' ", mwid );
			}
		}

	}		//end p loop


	//---Updating MAPS data---
	// Update kills/deaths counters
	INT svid	= GetSVID( MatchID );
	INT mapid	= GetMapID( MatchID );
	INT mapsid	= ISmapsid( svid, mid, mapid );
	if( mapsid!=-1 )
	{
		if( TeamKill == 0 )
			MySQL.DoSQL( "update maps set kills=kills+1 where mapsid='%d' ", mapsid );
		else
			MySQL.DoSQL( "update maps set teamkills=teamkills+1 where mapsid='%d' ", mapsid );
	}
	unguard;
}


/*-----------------------------------------------------------------------------
Processing Score and TeamScore line
-----------------------------------------------------------------------------*/

void UProcessStatsCommandlet::ProcessScore( INT MatchID, INT PlayerNumber, FLOAT Score, const TCHAR* ScoreCode, INT Seconds )
{
	guard(UProcessStatsCommandlet::ProcessScore);

	// Getting database relevant ids
	INT playerid	= GetPlayerID( MatchID, PlayerNumber );
	INT mid			= GetMID( MatchID );
	INT	scid		= FindScoreID( ScoreCode );
	UBOOL teamgame	= GetTeamGame( MatchID );

	for( INT t=1; t<=3; t++ )									// Loop for the (3) timeFrames
	{
		INT timeFrameNumber = GetTimeFrameNumber( MatchID, t );
		if( timeFrameNumber==-1 ) continue;

		// TABLE SCORES
#if CAL_HK_PLAYERSSUM_ALT2
		INT pid = ISpidHKwrap( playerid, t, timeFrameNumber, mid );
#else
		INT pid = ISpid( playerid, t, timeFrameNumber, mid );	//%% Avoid -1 playerid in DB
#endif
		if( pid!=-1 )
		{
			INT scoresid = ISscoresid( pid, scid );
			if( scoresid!=-1 )
				MySQL.DoSQL( "update scores set scores=scores+1, scoresum=scoresum+'%f' where scoresid='%d' ", Score, scoresid );
		}

		//---Updating RANKTEAM data in RAM and DB---
		if( teamgame )
			ProcessRankTeam( MatchID, PlayerNumber, Score, t );
	}

	if( teamgame )
	{
#if USE_MATCHRANKTEAM_TABLE
		// TABLE MATCHPLAYERS
		//---Updating MATCH specific data---
		ProcessRankTeam( MatchID, PlayerNumber, Score, 0 );		// Match special case: t=0
#endif
	}

	// Update score counters
	INT mpid = ISmpid( MatchID, playerid );
#if !CAL_MATCHPLAYERS_SUMMING
	if( mpid!=-1 )
		MySQL.DoSQL( "update matchplayers set score=score+'%f' where mpid='%d' ", Score, mpid );
#endif

	// TABLE MATCHSCORES
	INT mscid = ISmscid( mpid, scid );
	if( mscid!=-1 )
		MySQL.DoSQL( "update matchscores set scores=scores+1, scoresums=scoresums+'%f' where mscid='%d' ", Score, mscid );

#if USE_MATCHSCORING_TABLE
	// TABLE MATCHSCORING
	INT mscoringid = ISmscoringid( mpid, Seconds );
	if( mscoringid!=-1 )
		MySQL.DoSQL( "update matchscoring set score=score+'%f' where mscoringid='%d' ",	Score, mscoringid );
#endif
	unguard;
}

void UProcessStatsCommandlet::ProcessTeamScore( INT MatchID, INT TeamID, FLOAT Score, const TCHAR* TeamScoreCode, INT Seconds )
{
	guard(UProcessStatsCommandlet::ProcessTeamScore);

	// Getting database relevant ids
	INT mid		= GetMID( MatchID );
	INT	tsid	= FindTeamScoreID( TeamScoreCode );

	// TABLE TEAMSCORES
	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);	
	for( TMap<INT, FPlayerData>::TIterator It(MatchLookupPtr->PlayerLookup); It; ++It )
	{
		//INT PlayerNumber = It.Key();
		FPlayerData* PlayerLookupPtr = &It.Value();

		// Get teamid
		INT playerid		= PlayerLookupPtr->playerid;
		INT currentteamid	= PlayerLookupPtr->teamid;

		// If player on team that just scored?
		if( currentteamid == TeamID )
		{
			// PLAYER update
			for( INT t=1; t<=3; t++ )
			{
				INT timeFrameNumber = GetTimeFrameNumber( MatchID, t );
				if( timeFrameNumber==-1 ) continue;

#if CAL_HK_PLAYERSSUM_ALT2
				INT pid = ISpidHKwrap(	playerid, t, timeFrameNumber, mid );
#else
				INT pid = ISpid(		playerid, t, timeFrameNumber, mid );
#endif
				if( pid!=-1 )
				{
					INT tscoresid = IStscoresid( pid, tsid );
					if( tscoresid!=-1 )
						MySQL.DoSQL( "update teamscores set teamscores=teamscores+1,teamscoresum=teamscoresum+'%f' where tscoresid='%d' ", 
							Score, tscoresid );
				}
			}
		}
	}

	// Update the match lookup infos
	SetMatchTeamScore( MatchID, TeamID, Score );

	// TABLE MATCHTEAMSCORES
	INT mtsid = ISmtsid( MatchID, TeamID, tsid );
	if( mtsid!=-1 )
		MySQL.DoSQL( "update matchteamscores set teamscores=teamscores+'%f' where mtsid='%d' ", Score, mtsid );

#if USE_MATCHTEAMSCORING_TABLE
	// TABLE MATCHTEAMSCORING
	INT mtscoringid = ISmtscoringid( MatchID, TeamID, Seconds );
	if( mtscoringid!=-1 )
		MySQL.DoSQL( "update matchteamscoring set teamscore=teamscore+'%f' where mtscoringid='%d' ", Score, mtscoringid );
#endif
	unguard;
}


/*-----------------------------------------------------------------------------
Processing GSpecial and PSpecial lines
-----------------------------------------------------------------------------*/

void UProcessStatsCommandlet::ProcessGSpecial( INT MatchID, const TCHAR* EventCode, INT PlayerNumber, INT TeamFlagID )
{
	guard(UProcessStatsCommandlet::ProcessGSpecial);
	// TeamFlagID not used... irrelevant (at least for CTF), since the we know in what team the player played!
	// In BR it is "255" since it never belongs to anyone

	// PlayerNumber can in certain cases become -1:
	// e.g. disconnect, flag_drop timing issue. 
	// or in where the game event can't be assigned to a player, e.g. auto flag return... Ignore line!
	if( PlayerNumber==-1 ) return;

	// Getting database relevant ids
	INT playerid	= GetPlayerID( MatchID, PlayerNumber );
	INT mid			= GetMID( MatchID );
	INT	eid			= FindEventID( EventCode );

	for( INT t=1; t<=3; t++ )
	{
		INT timeFrameNumber = GetTimeFrameNumber( MatchID, t );
		if( timeFrameNumber==-1 ) continue;

		// TABLE EVENTS
#if CAL_HK_PLAYERSSUM_ALT2
		INT pid = ISpidHKwrap( playerid, t, timeFrameNumber, mid );
#else
		INT pid = ISpid( playerid, t, timeFrameNumber, mid );
#endif
		if( pid!=-1 )
		{
			INT eventsid = ISeventsid( pid, eid );
			if( eventsid!=-1 )
				MySQL.DoSQL( "update events set events=events+1 where eventsid='%d' ", eventsid );
		}
	}

	//---Updating MATCH specific data---
	// TABLE MATCHPLAYERS
	INT mpid = ISmpid( MatchID, playerid );
#if !CAL_MATCHPLAYERS_SUMMING
	if( mpid!=-1 )
		MySQL.DoSQL( "update matchplayers set events=events+1 where mpid='%d' ", mpid );
#endif

	// TABLE MATCHEVENTS - event type specific
	INT meid = ISmeid( mpid, eid );
	if( meid!=-1 )
		MySQL.DoSQL( "update matchevents set events=events+1 where meid='%d' ", meid );

	//---Updating MAPS data---
	INT svid	= GetSVID( MatchID );
	INT mapid	= GetMapID( MatchID );
	INT mapsid	= ISmapsid( svid, mid, mapid );
	if( mapsid!=-1 )
		MySQL.DoSQL( "update maps set events=events+1 where mapsid='%d' ", mapsid );

	unguard;
}


/*-----------------------------------------------------------------------------
End Match processing - Update the rankteam at EG e.g.
-----------------------------------------------------------------------------*/

void UProcessStatsCommandlet::ProcessDumpMPSumOne( INT mpid )
{
	guard(UProcessStatsCommandlet::ProcessDumpMPSumOne);
	// Summing up data for matchplayers, at the end of the game, avoiding realtime counter spamming!
	FString QueryText;

	// "update matchplayers set kills=kills+1 where mpid='%d' ", mpid[p]
	// "update matchplayers set deaths=deaths+1 where mpid='%d' ", mpid[p]
	INT counts[2];			// 0 = kills, 1=deaths
	QueryText = FString::Printf(TEXT( "select sum(kills),sum(deaths) from matchweapons where mpid='%d' "), mpid );
	INT kills = FindIDMulti(*QueryText, counts, 2);
	INT deaths = counts[1];

	// "update matchplayers set suicides=suicides+1 where mpid='%d' ", mpid
	QueryText = FString::Printf(TEXT( "select sum(suicides) from matchsuicides where mpid='%d' "), mpid );
	INT suicides = FindID(*QueryText);

	// "update matchplayers set events=events+1 where mpid='%d' ", mpid 
	QueryText = FString::Printf(TEXT( "select sum(events) from matchevents where mpid='%d' "), mpid );
	INT events = FindID(*QueryText);

	// "update matchplayers set score=score+'%f' where mpid='%d' ", Score, mpid
	QueryText = FString::Printf(TEXT( "select sum(scoresums) from matchscores where mpid='%d' "), mpid );
	FLOAT score = FindIDF(*QueryText);

	// Updating the matchplayers table counters in one go
	MySQL.DoSQL( "update matchplayers set kills='%d', deaths='%d', suicides='%d', events='%d', score='%f' where mpid='%d' ",
		kills, deaths, suicides, events, score, mpid );

	unguard;
}

void UProcessStatsCommandlet::ProcessDumpMPSum( void )
{
	guard(UProcessStatsCommandlet::ProcessDumpMPSum);
	// Close unfinished games, and save back all matchplayer counter data!
	for( INT m=0; m<MatchIDs.Num(); m++ )	// Looking through global "MatchIDs"
	{
		INT MatchID = MatchIDs(m);						
		FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);	
		for( TMap<INT, FPlayerData>::TIterator It(MatchLookupPtr->PlayerLookup); It; ++It )
		{
			INT PlayerNumber = It.Key();
			//FPlayerData* PlayerLookupPtr = &It.Value();

			//---Update the database with the new ranks!---
			INT playerid	= GetPlayerID( MatchID, PlayerNumber );
			INT	mpid		= ISmpid( MatchID, playerid );	
			if( mpid!=-1 )
			{
				// Update the matchplayers counters
				ProcessDumpMPSumOne( mpid );
			}
		}		//end it - player loop
	}			//end m - MatchID loop
	unguard;
}

FLOAT UProcessStatsCommandlet::ProcessDumpRankELOOne( INT MatchID, INT PlayerNumber, INT pid, INT t )
{
	guard(UProcessStatsCommandlet::ProcessDumpRankELO);
	// Update the RankELO for this disconnecting player!
	FLOAT rankelo = 0.;	
	FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);	
	FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find(PlayerNumber);
	if( PlayerLookupPtr != NULL )                                                     
	{                                                                                             
		// Get the current other RankELO infos
		rankelo = PlayerLookupPtr->rankelo[t];		// Get RankELO if possible
		MySQL.DoSQL( "update player set rankELO='%f' where pid='%d' ", rankelo, pid );	// pid!=-1 ever!
	}                                                                                             
	return rankelo;
	unguard;
}

FLOAT UProcessStatsCommandlet::ProcessDumpRankTeamOne( INT MatchID, INT PlayerNumber, INT pid, INT t )
{
	guard(UProcessStatsCommandlet::ProcessDumpRankTeam);
	// Update the rankteam for this disconnecting player!
	FLOAT rankteam = 0.;	
	FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);	
	FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find(PlayerNumber);
	if( PlayerLookupPtr != NULL )                                                     
	{                                                                                             
		// Get the current other rankteam infos
		rankteam = PlayerLookupPtr->rankteam[t];		// Get rankteam if possible
		MySQL.DoSQL( "update player set rankTeam='%f' where pid='%d' ", rankteam, pid );	// pid!=-1 ever!
	}                                                                                             
	return rankteam;
	unguard;
}

void UProcessStatsCommandlet::ProcessDumpRankELO( void )
{
	guard(UProcessStatsCommandlet::ProcessDumpRankELO);
	// TEST CODE
	// Close unfinished games, and save back all the ranks!
	for( INT m=0; m<MatchIDs.Num(); m++ )				// Looking through global "MatchIDs"
	{
		INT MatchID = MatchIDs(m);						
		INT mid	= GetMID( MatchID );
		FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);	
		UBOOL teamgame	= GetTeamGame( MatchID );
		if( teamgame==0 )								// Only update appropriate ranks
		{
			for( INT t=1; t<=3; t++ )
			{
				INT timeFrameNumber = GetTimeFrameNumber( MatchID, t );
				if( timeFrameNumber==-1 ) continue;

				for( TMap<INT, FPlayerData>::TIterator It(MatchLookupPtr->PlayerLookup); It; ++It )
				{
					INT PlayerNumber = It.Key();
					FPlayerData* PlayerLookupPtr = &It.Value();

					//---Update the database with the new ranks!---
					INT playerid	= GetPlayerID( MatchID, PlayerNumber );

					// Insert/Select pid - TABLE PLAYER
					INT pid = ISpid( playerid, t, timeFrameNumber, mid );
					if( pid!=-1 )											// Making sure we are sending valid pid
					{
						// Get the current rankELO
						FLOAT rankelo = PlayerLookupPtr->rankelo[t];
						MySQL.DoSQL( "update player set rankELO='%f' where pid='%d' ", rankelo, pid );
					}
				}		//end it - player loop
			}			//end t - timeFrame
		}	
	}				//end m - MatchID loop
	unguard;
}

void UProcessStatsCommandlet::ProcessDumpRankTeam( void )
{
	guard(UProcessStatsCommandlet::ProcessDumpRankTeam);
	// TEST CODE
	// Close unfinished games, and save back all the ranks!
	for( INT m=0; m<MatchIDs.Num(); m++ )				// Looking through global "MatchIDs"
	{
		INT MatchID = MatchIDs(m);						
		INT mid	= GetMID( MatchID );
		FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);	
		UBOOL teamgame	= GetTeamGame( MatchID );
		if( teamgame==1 )								// Only update appropriate ranks
		{
			for( INT t=1; t<=3; t++ )
			{
				INT timeFrameNumber = GetTimeFrameNumber( MatchID, t );
				if( timeFrameNumber==-1 ) continue;

				for( TMap<INT, FPlayerData>::TIterator It(MatchLookupPtr->PlayerLookup); It; ++It )
				{
					INT PlayerNumber = It.Key();
					FPlayerData* PlayerLookupPtr = &It.Value();

					//---Update the database with the new ranks!---
					INT playerid	= GetPlayerID( MatchID, PlayerNumber );

					// Insert/Select pid - TABLE PLAYER
					INT pid = ISpid( playerid, t, timeFrameNumber, mid );
					if( pid!=-1 )											// Making sure we are sending valid pid
					{
						// Get the current rankteam
						FLOAT rankteam		= PlayerLookupPtr->rankteam[t];
						MySQL.DoSQL( "update player set rankTeam='%f' where pid='%d' ", rankteam, pid );
					}
				}		//end it - player loop
			}			//end t - timeFrame
		}	
	}				//end m - MatchID loop
	unguard;
}



/*-----------------------------------------------------------------------------
Lookup functions
-----------------------------------------------------------------------------*/

SQWORD UProcessStatsCommandlet::FindIDQ( const TCHAR* SelectQuery )
{
	// Define select query and get the requested ID back
	guard(UProcessStatsCommandlet::FindIDQ);
	SQWORD ID = -1;						// If failed return -1 !
	FQueryResult* IDQuery = MySQL.Query( appToAnsi(SelectQuery) );

	// Just getting first found entry, if there are more...  though :)
	FQueryField** IDRow;
	if( (IDRow = IDQuery->FetchNextRow()) != NULL )
		ID = IDRow[0]->AsBigInt();
	else
		GWarn->Logf(TEXT("Warning: MYSQL FindIDQ() (%s) failed! Returning -1."), SelectQuery );

	delete IDQuery;
	return ID;
	unguard;
}

FLOAT UProcessStatsCommandlet::FindIDF( const TCHAR* SelectQuery )
{
	// Define select query and get the requested IDF back
	// Only called by ranking code directly, so fallback and warning done here
	guard(UProcessStatsCommandlet::FindIDF);
	FLOAT IDF = 0.;						//%% Fallback
	FQueryResult* IDFQuery = MySQL.Query( appToAnsi(SelectQuery) );

	// Just getting first found entry, if there are more...  though :)
	FQueryField** IDFRow;
	if( (IDFRow = IDFQuery->FetchNextRow()) != NULL )
		IDF = IDFRow[0]->AsFloat();
	else
		GWarn->Logf(TEXT("Warning: MYSQL FindIDF() (%s) failed! Returning %f."), SelectQuery, IDF );

	delete IDFQuery;
	return IDF;
	unguard;
}

FLOAT UProcessStatsCommandlet::FindIDFnowarning( const TCHAR* SelectQuery )
{
	// Special version without warning, since this can and will fail
	// for certain calls. The Fallback is fine though!
	guard(UProcessStatsCommandlet::FindIDF);
	FLOAT IDF = 0.;						//%% Fallback
	FQueryResult* IDFQuery = MySQL.Query( appToAnsi(SelectQuery) );
	FQueryField** IDFRow;
	if( (IDFRow = IDFQuery->FetchNextRow()) != NULL )
		IDF = IDFRow[0]->AsFloat();
	delete IDFQuery;
	return IDF;
	unguard;
}

INT UProcessStatsCommandlet::FindIDUpdate( const TCHAR* SelectQuery, const TCHAR* Update )
{
	// Define select query and get the requested ID back
	guard(UProcessStatsCommandlet::FindIDUpdate);
	
	// "%s", 
	FQueryResult* IDQuery = MySQL.Query( appToAnsi(SelectQuery) );	//extra "%s", causes little or no spees penalty
	FQueryField** IDRow;

	while( (IDRow = IDQuery->FetchNextRow()) != NULL )
	{
		INT pid	= IDRow[0]->AsInt();
		INT sum	= IDRow[1]->AsInt();

		// Generate the update string and perform the update!
		FString RealUpdate = FString::Printf(TEXT("%s'%d' where pid='%d' "), Update, sum, pid );
		//GWarn->Logf(TEXT("Info: MYSQL (%s) (%s)"), SelectQuery, *RealUpdate );
		MySQL.DoSQL( appToAnsi(*RealUpdate) );
	}

	delete IDQuery;
	return 0;			// dummy not checked :)
	unguard;
}

INT UProcessStatsCommandlet::FindIDQUpdate( const TCHAR* SelectQuery, const TCHAR* Update )
{
	// Define select query and get the requested ID back
	guard(UProcessStatsCommandlet::FindIDQUpdate);
	FQueryResult* IDQuery = MySQL.Query( appToAnsi(SelectQuery) );
	FQueryField** IDRow;

	while( (IDRow = IDQuery->FetchNextRow()) != NULL )
	{
		INT		pid = IDRow[0]->AsInt();
		SQWORD	sum = IDRow[1]->AsBigInt();
		
		// Generate the update string and perform the update!
		FString RealUpdate = FString::Printf(TEXT("%s'%I64d' where pid='%d' "), Update, sum, pid );
		MySQL.DoSQL( appToAnsi(*RealUpdate) );
	}

	delete IDQuery;
	return 0;									// dummy not checked :)
	unguard;
}

INT UProcessStatsCommandlet::FindIDUpdateSum( const TCHAR* SelectQuery )
{
	// Define select query and get the requested ID back
	guard(UUProcessStatsCommandlet::FindIDUpdateSum);
	FQueryResult* IDQuery = MySQL.Query( appToAnsi(SelectQuery) );
	FQueryField** IDRow;

	// "select pid, pkills+akills as kills, pdeaths+adeaths as deaths, wsuicides+esuicides as suicides from players group by pid "
	while( (IDRow = IDQuery->FetchNextRow()) != NULL )
	{
		INT pid				= IDRow[0]->AsInt();
		INT kills			= IDRow[1]->AsInt();
		INT deaths			= IDRow[2]->AsInt();
		INT suicides		= IDRow[3]->AsInt();
		INT frags			= kills - suicides;
		INT fraction		= kills + deaths + suicides;		//** + teamkills
		if( fraction == 0 ) fraction = 1;						//Safe devision
		FLOAT efficiency	= kills / (fraction*1.);

		// Update the data for this player...
		MySQL.DoSQL( "update players_temp set kills='%d', deaths='%d', suicides='%d', frags='%d', efficiency='%f' where pid='%d' ",
			kills, deaths, suicides, frags, efficiency, pid );
	}

	delete IDQuery;
	return 0;			// dummy not checked :)
	unguard;
}

SQWORD UProcessStatsCommandlet::FindIDQMulti( const TCHAR* SelectQuery, SQWORD* values, INT columns )
{
	// Returning more than one SQWORD column on select.
	guard(UProcessStatsCommandlet::FindIDQMulti);
	SQWORD ID = -1;							// If failed return -1 !
	FQueryResult* IDQuery = MySQL.Query( appToAnsi(SelectQuery) );
	FQueryField** IDRow;
	if( (IDRow = IDQuery->FetchNextRow()) != NULL )
	{
		for( INT c=0; c<columns; c++ )		// Allow for more than 1 column.
			values[c] = IDRow[c]->AsBigInt();	
		ID = values[0];			
	}
	else
		GWarn->Logf(TEXT("Warning: MYSQL FindIDQMulti() (%s) failed! Returning %I64d."), SelectQuery, ID );

	delete IDQuery;
	return ID;
	unguard;
}

INT UProcessStatsCommandlet::FindIDMulti( const TCHAR* SelectQuery, INT* values, INT columns )
{
	// Returning more than one INT column on select.
	guard(UProcessStatsCommandlet::FindIDMulti);
	INT ID = -1;							// If failed return -1 !
	FQueryResult* IDQuery = MySQL.Query( appToAnsi(SelectQuery) );
	FQueryField** IDRow;
	if( (IDRow = IDQuery->FetchNextRow()) != NULL )
	{
		for( INT c=0; c<columns; c++ )		// Allow for more than 1 column.
			values[c] = IDRow[c]->AsInt();	
		ID = values[0];			
	}
	else
		GWarn->Logf(TEXT("Warning: MYSQL FindIDMulti() (%s) failed! Returning %d."), SelectQuery, ID );

	delete IDQuery;
	return ID;
	unguard;
}

INT UProcessStatsCommandlet::FindIDWildcard( const TCHAR* SelectQuery )
{
	// Define select query and get the requested ID back
	guard(UProcessStatsCommandlet::FindID);
	INT ID = -1;
	FQueryResult* IDQuery = MySQL.Query( "%s", appToAnsi(SelectQuery) );	// Allow for %% MYSQL wildcards!

	//** Just getting first found entry, if there are more...  though :)
	FQueryField** IDRow;
	if( (IDRow = IDQuery->FetchNextRow()) != NULL )
		ID = IDRow[0]->AsInt();			
	else
		GWarn->Logf(TEXT("Warning: MYSQL FindID() (%s) failed! Returning %d."), SelectQuery, ID );

	delete IDQuery;
	return ID;
	unguard;
}

INT UProcessStatsCommandlet::FindID( const TCHAR* SelectQuery )
{
	// Define select query and get the requested ID back
	guard(UProcessStatsCommandlet::FindID);
	INT ID = -1;						// If failed return -1 !
	FQueryResult* IDQuery = MySQL.Query( appToAnsi(SelectQuery) );	// Allow for %% MYSQL jokers!

	//** Just getting first found entry, if there are more...  though :)
	FQueryField** IDRow;
	if( (IDRow = IDQuery->FetchNextRow()) != NULL )
		ID = IDRow[0]->AsInt();			
	else
		GWarn->Logf(TEXT("Warning: MYSQL FindID() (%s) failed! Returning %d."), SelectQuery, ID );

	delete IDQuery;
	return ID;
	unguard;
}

INT UProcessStatsCommandlet::FindIDI( const TCHAR* SelectQuery )
{
	// Define select query and get the requested ID back
	// Special version of FindID() without warning!
	guard(UProcessStatsCommandlet::FindID);
	INT ID = -1;						// If failed return -1 !
	FQueryResult* IDQuery = MySQL.Query( appToAnsi(SelectQuery) );

	//** Just getting first found entry, if there are more...  though :)
	FQueryField** IDRow;
	if( (IDRow = IDQuery->FetchNextRow()) != NULL )
		ID = IDRow[0]->AsInt();			

	delete IDQuery;
	return ID;
	unguard;
}

INT UProcessStatsCommandlet::FindIDSafe( const TCHAR* InsertQuery, const TCHAR* SelectQuery)
{
	// Select first then insert... to avoid all the MYSQL warnings!
	guard(UProcessStatsCommandlet::FindIDSafe);

	// First, do select
	INT ID = FindIDI( SelectQuery );			// Special version without warning!

	// Second if select failed, generate field with insert
	if( ID == -1 )
	{
		MySQL.DoSQL( appToAnsi(InsertQuery) );
		ID = MySQL.GetInsertID();
		if( ID == 0 )							// Failed! Returning -1!, ID can never be a valid with 0!	
		{
			ID=-1;					
			// Select will fail often, but that is not the problem then, its the insert, if that fails warn!
			GWarn->Logf(TEXT("Warning: MYSQL FindIDSafe() (%s) failed! Returning %d."), InsertQuery, ID );
		}
	}

	return ID;
	unguard;
}

INT UProcessStatsCommandlet::FindMPID( INT matchid, INT playerid  )
{
	guard(UProcessStatsCommandlet::FindMPID);
	INT mpid = -1;										// Default failed!
	// Check if MatchIDLookup exits for MatchID, if so return it, else generate it
	FMatchIDData* MatchIDLookupPtr = MatchIDLookup.Find( matchid );

	if( !MatchIDLookupPtr ) return mpid;				// Failed, need to get info from DB			

	// Find succeeds, the matchid is known, now checking if the other data exists
	for( INT i=0; i<MatchIDLookupPtr->MatchIDInfoLookup.Num(); i++ )
	{
		INT tplayerid = MatchIDLookupPtr->MatchIDInfoLookup(i).playerid;
		if( tplayerid == playerid )
		{
			INT mpid = MatchIDLookupPtr->MatchIDInfoLookup(i).mpid;
			return mpid;								// mpid was found in cache
		}
	}

	return mpid;
	unguard;
}

INT UProcessStatsCommandlet::FindPID( INT playerid, INT timeFrame, INT timeFrameNumber, INT mid )
{
	guard(UProcessStatsCommandlet::FindPID);
	INT pid = -1;										// Default failed!
	// Check if PlayerIDLookup exits for playerid, if so return it, else generate it
	FPlayerIDData* PlayerIDLookupPtr = PlayerIDLookup.Find( playerid );

	if( !PlayerIDLookupPtr ) return pid;				// Failed, need to get info from DB			

	// Find succeeds, the playerid is known, now checking if the other data exists
	// Loop search: first for the mid
	for( INT i=0; i<PlayerIDLookupPtr->PlayerIDInfoLookup.Num(); i++ )
	{
		INT tmid = PlayerIDLookupPtr->PlayerIDInfoLookup(i).mid;
		if( tmid == mid )
		{
			INT ttimeFrame			= PlayerIDLookupPtr->PlayerIDInfoLookup(i).timeFrame;
			INT ttimeFrameNumber	= PlayerIDLookupPtr->PlayerIDInfoLookup(i).timeFrameNumber;
			if( ttimeFrame == timeFrame  &&  ttimeFrameNumber == timeFrameNumber )
			{
				INT pid = PlayerIDLookupPtr->PlayerIDInfoLookup(i).pid;
				return pid;								// pid was found in cache
			}
		}
	}

	return pid;
	unguard;
}

INT UProcessStatsCommandlet::FindWeaponWid( const TCHAR* WeaponCode, const TCHAR* DamageType )
{
	guard(UProcessStatsCommandlet::FindWeaponWid);
	// Case 0: Empty Weaponcode and Killer Damagetype used for weapon recognition (pri,sec)
	// Case 1: WeaponCode and empty Damagetype, Victim has no damagetype, getting primary weapon
	INT victimmode = 0;										// off by default

	if( appStrlen(DamageType) == 0 )						// Case 1 - VictimWeapon
		victimmode = 1;

	for( INT i=0; i<WeaponLookup.Num(); i++ )
	{
		if( victimmode == 0 )								
		{
			//WeaponCode is no longer used for KILLER, using unique damagetype!
			if( WeaponLookup(i).DamageTypeCode == FString(DamageType) )
				return WeaponLookup(i).wid;
		}
		else
		{
			//Default pri mode of weapon, used as victim weapon
			if( WeaponLookup(i).WeaponCode == FString(WeaponCode) &&
				WeaponLookup(i).mode == 1 )
				return WeaponLookup(i).wid;
		}
	}

	// Updating database with new data
	INT wid;
	if( victimmode==0 )
		wid = ISwid( DamageType, 0 );			// New damagetype, 0 = not official!
	else
		wid = ISwid( WeaponCode, 0 );			// We HAVE no damagetype for victim, using the new weapon name instead!
	
	// Add to the WeaponLookup array
	if( wid!=-1 )
	{
		// Updating database - weapon mode assuming primary = 1

		// Updating data in Lookup
		INT entry = WeaponLookup.AddZeroed();
		if( victimmode==0 )
		{
			// Weaponcode does not exist, using DT for killer!
			MySQL.DoSQL( "update weapon set weaponcode='%s', mode='%d' where wid='%d' ", 
				MySQL.FormatSQL(DamageType), 1, wid );
			WeaponLookup(entry).WeaponCode	= FString(DamageType);
		}
		else
		{
			MySQL.DoSQL( "update weapon set weaponcode='%s', mode='%d' where wid='%d' ", 
				 MySQL.FormatSQL(WeaponCode), 1, wid );
			WeaponLookup(entry).WeaponCode		= FString(WeaponCode);
		}
		WeaponLookup(entry).DamageTypeCode	= FString(DamageType);
		WeaponLookup(entry).wid				= wid;
		WeaponLookup(entry).mode			= 1;		//If new entry, default mode is 1 (primary)
	}

	return wid;
	unguard;
}

INT UProcessStatsCommandlet::FindSuicideID( const TCHAR* SuicideCode )
{
	guard(UProcessStatsCommandlet::FindSuicideID);

	INT* sidPtr = SuicideLookup.Find( SuicideCode );	// TMap<FString, ...> have a Find() which takes a TCHAR*

	if( !sidPtr )
	{
		// Suicide wasn't in the database!
		INT sid = ISsid( SuicideCode, 0 );				// official = 0

		// Add it to the Suicide.
		if( sid!=-1 )									//%% Avoid generating invalid data
			SuicideLookup.Set( SuicideCode, sid );
		return sid;
	}
	else
		return *sidPtr;

	unguard;
}

INT UProcessStatsCommandlet::FindEventID( const TCHAR* EventCode)
{
	guard(UProcessStatsCommandlet::FindEventID);

	INT* eidPtr = EventLookup.Find( EventCode );

	if( !eidPtr )
	{
		// Event wasn't in the database!
		// Add it to the Event.
		INT eid = ISeid( EventCode, 0 );				// official = 0
		if( eid!=-1 )									//%% Avoid generating invalid data
			EventLookup.Set( EventCode, eid );
		return eid;
	}
	else
		return *eidPtr;

	unguard;
}

INT UProcessStatsCommandlet::FindTeamScoreID( const TCHAR* TeamScoreCode )
{
	guard(UProcessStatsCommandlet::FindTeamScoreID);

	INT* tsidPtr = TeamScoreLookup.Find( TeamScoreCode );

	if( !tsidPtr )
	{
		// TeamScore wasn't in the database!
		// Add it to the TeamScore.
		INT tsid = IStsid( TeamScoreCode, 0 );			// official = 0
		if( tsid!=-1 )									//%% Avoid generating invalid data
			TeamScoreLookup.Set( TeamScoreCode, tsid );
		return tsid;
	}
	else
		return *tsidPtr;

	unguard;
}

INT UProcessStatsCommandlet::FindScoreID( const TCHAR* ScoreCode )
{
	guard(UProcessStatsCommandlet::FindScoreID);

	INT* scidPtr = ScoreLookup.Find( ScoreCode );

	if( !scidPtr )
	{
		// Score wasn't in the database!
		// Add it to the Score.
		INT scid = ISscid( ScoreCode, 0 );				// official = 0
		if( scid!=-1 )									//%% Avoid generating invalid data
			ScoreLookup.Set( ScoreCode, scid );
		return scid;
	}
	else
		return *scidPtr;

	unguard;
}

INT UProcessStatsCommandlet::FindMapID( INT MatchID, const TCHAR* MapName )
{
	guard(UProcessStatsCommandlet::FindMapID);

	// Looking up the mapid, if fails, add it in db.
	INT* mapidPtr = MapLookup.Find( MapName );

	if( !mapidPtr )
	{
		// Doing safe MatchLookup
		FMatchData* MatchLookupPtr = SetMatchInitGet( MatchID );	//MatchLookupPtr will exist, or quit code!
		FString MapTitle	= MatchLookupPtr->MapTitle;
		FString MapAuthor	= MatchLookupPtr->MapAuthor;

		// Map wasn't in the database!
		// Add it to the map.
		INT mapid = ISmapid( MapName, *MapTitle, *MapAuthor, 0 );		// official = 0;
		if( mapid!=-1 )													//%% Avoid generating invalid data
			MapLookup.Set( MapName, mapid );
		return mapid;
	}
	else
		return *mapidPtr;
	unguard;
}

INT UProcessStatsCommandlet::FindModID( const TCHAR* ModName )
{
	// The mod table is different, we will NOT be doing auto-updates
	// since this would put custom gametypes on the map. Custom mods
	// are all "!XGame.xOther".
	guard(UProcessStatsCommandlet::FindModID);

	// Looking up the mid
	INT* midPtr = ModLookup.Find( ModName );

	// Lookup failed. This is a custom mod, assign xOther
	if( !midPtr )
	{
		FString GameClass = TEXT("!XGame.xOther");
		midPtr = ModLookup.Find( *GameClass );
		if( !midPtr )									// Still not working -> exit
		{
			// This should never happen, but if it does, use mid 1 (DM) as Fallback.
			GWarn->Logf(TEXT("Error: ModLookupPtr (mid) for GameClass (%s) be found, returning mid=%d"),
				*GameClass, 1);
			*midPtr = 1;								//%% Fallback
		}
	}

	return *midPtr;
	unguard;
}

INT UProcessStatsCommandlet::FindMutID( const TCHAR* MutCode )
{
	// Mutator Lookup and auto-update
	guard(UProcessStatsCommandlet::FindMutID);

	// Looking up the mutid, if fails, add it in db.
	INT* mutidPtr = MutatorLookup.Find( MutCode );

	if( !mutidPtr )
	{
		// Mutator wasn't in the database!
		// Add it to the map.
		INT mutid = ISmutid( MutCode, 0 );				// official = 0
		if( mutid!=-1 )									//%% Avoid generating invalid data
			MapLookup.Set( MutCode, mutid );
		return mutid;
	}
	else
		return *mutidPtr;

	unguard;
}

FLOAT UProcessStatsCommandlet::FindELODecrease( INT timeFrame )
{
	// ELODecreaseLookup - no update
	guard(UProcessStatsCommandlet::FindELODecrease);
	FLOAT *decrease = ELODecreaseLookup.Find( timeFrame );
	if( !decrease )
	{
		FLOAT factor=8.;
		GWarn->Logf(TEXT("Warning: ELODecrease lookup failed, TF (%d) using default (%f)!"), timeFrame, factor );
		*decrease = factor;						//%% Safe fallback
	}
	return *decrease;
	unguard;
}

FLOAT UProcessStatsCommandlet::FindELOFactors( INT timeFrame )
{
	// ELOFactorsLookup - no update
	guard(UProcessStatsCommandlet::FindELOFactors);
	// Special Match rankteam case 0, will use TF=1 factor
	if( timeFrame == 0) timeFrame = 1;
	FLOAT *k_dynamic_factor = ELOFactorsLookup.Find( timeFrame );

	if( !k_dynamic_factor )
	{
		FLOAT factor=8.;
		GWarn->Logf(TEXT("Warning: ELOFactors lookup failed, TF (%d) using default (%f)!"), timeFrame, factor );
		*k_dynamic_factor = factor;						//%% Safe fallback
	}
	return *k_dynamic_factor;
	unguard;
}

INT UProcessStatsCommandlet::FindTimeFrameNumber( INT timeFrame )
{
	guard(UProcessStatsCommandlet::FindTimeFrameNumber);
	// TimeFrameLookup - no auto-update
	INT *timeFrameNumberPtr = TimeFrameLookup.Find( timeFrame );
	if( !timeFrameNumberPtr )
	{
		GWarn->Logf(TEXT("Warning: timeFrameNumber lookup failed, TF (%d) using default (%d)!"), timeFrame, 1 );
		*timeFrameNumberPtr = -1;						//%% Fallback not possible would overwrite data in DB
	}
	return *timeFrameNumberPtr;
	unguard;
}


/*-----------------------------------------------------------------------------
Remember Data for current MatchID in MatchLookup
-----------------------------------------------------------------------------*/

INT UProcessStatsCommandlet::SetMPID( INT matchid, INT playerid, INT mpid )
{
	guard(UProcessStatsCommandlet::SetMPID);

	FMatchIDData* CurrentFMatchIDData = MatchIDLookup.Find( matchid );
	if( !CurrentFMatchIDData )
		CurrentFMatchIDData = &MatchIDLookup.Set( matchid, FMatchIDData() );

	INT i = CurrentFMatchIDData->MatchIDInfoLookup.AddZeroed();
	CurrentFMatchIDData->MatchIDInfoLookup(i).playerid	= playerid;
	CurrentFMatchIDData->MatchIDInfoLookup(i).mpid		= mpid;

	return 0;
	unguard;
}

INT UProcessStatsCommandlet::SetPID( INT playerid, INT timeFrame, INT timeFrameNumber, INT mid, INT pid )
{
	guard(UProcessStatsCommandlet::SetPID);

	FPlayerIDData* CurrentFPlayerIDData = PlayerIDLookup.Find( playerid );
	if( !CurrentFPlayerIDData )
		CurrentFPlayerIDData = &PlayerIDLookup.Set( playerid, FPlayerIDData() );

	INT i = CurrentFPlayerIDData->PlayerIDInfoLookup.AddZeroed();
	CurrentFPlayerIDData->PlayerIDInfoLookup(i).timeFrame		= timeFrame;
	CurrentFPlayerIDData->PlayerIDInfoLookup(i).timeFrameNumber = timeFrameNumber;
	CurrentFPlayerIDData->PlayerIDInfoLookup(i).mid				= mid;
	CurrentFPlayerIDData->PlayerIDInfoLookup(i).pid				= pid;	

	return 0;
	unguard;
}

INT UProcessStatsCommandlet::SetMatchTimeFrameNumber(INT MatchID, INT timeFrame, INT timeFrameNumber )
{
	guard(UProcessStatsCommandlet::SetMatchSVID);
	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
	MatchLookupPtr->timeFrameNumber[timeFrame] = timeFrameNumber;
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::SetMatchLastUsed( INT MatchID )
{
	guard(UProcessStatsCommandlet::SetMatchSVID);
	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
	MatchLookupPtr->lastUsed = appSeconds();		// Current Master Server Time in seconds
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::SetMatchSVID( INT MatchID, INT svid )
{
	guard(UProcessStatsCommandlet::SetMatchSVID);
	// Update the svid
	if( svid!=-1 )
	{
		FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
		MatchLookupPtr->svid = svid;
	}
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::SetMatchConnectTime( INT MatchID, INT PlayerNumber, INT Seconds )
{
	guard(UProcessStatsCommandlet::SetMatchConnectTime);
	// Remember (cache) the ConnectTime for this playernumber and match
	if( PlayerNumber!=-1 )
	{
		FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);
		FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find(PlayerNumber);
		// Updating rankteam for player.
		if( PlayerLookupPtr != NULL )
		{
			PlayerLookupPtr->ConnectTime = Seconds;
			return 1;	//ok
		}
	}
	GWarn->Logf(TEXT("Warning: SetMatchConnectTime() could not set ConnectTime for MatchID %d, PNum %d in line: %d"), 
			MatchID, PlayerNumber, DEBUG_LOGLINENUMBER );
	return -1;
	unguard;
}

INT UProcessStatsCommandlet::SetMatchRankELO( INT MatchID, INT PlayerNumber, FLOAT newRankELO, INT timeFrame )
{
	guard(UProcessStatsCommandlet::SetMatchRankELO);
	// Remember (cache) the RankELO for this playernumber and match
	if( PlayerNumber!=-1 )
	{
		FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);
		FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find(PlayerNumber);
		// Updating RankELO for player.
		if( PlayerLookupPtr != NULL )
		{
			PlayerLookupPtr->rankelo[timeFrame]		= newRankELO;
			return 1;	//ok
		}
	}

	GWarn->Logf(TEXT("Warning: SetMatchRankELO() could not set rankelo for MatchID %d, PNum %d, TF %d in line: %d"), 
			MatchID, PlayerNumber, timeFrame, DEBUG_LOGLINENUMBER );
	return -1;
	unguard;
}

INT UProcessStatsCommandlet::SetMatchRankTeam(	INT MatchID, INT PlayerNumber, FLOAT newRankTeam, INT timeFrame )
{
	guard(UProcessStatsCommandlet::SetMatchRankTeam);
	// Remember (cache) the rankteam for this playernumber and match
	if( PlayerNumber!=-1 )
	{
		FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);
		FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find(PlayerNumber);
		// Updating rankteam for player.
		if( PlayerLookupPtr != NULL )
		{
			PlayerLookupPtr->rankteam[timeFrame] = newRankTeam;
			return 1;	//ok
		}
	}

	GWarn->Logf(TEXT("Warning: SetMatchRankTeam() could not set rankteam for MatchID %d, PNum %d, TF %d in line: %d"), 
			MatchID, PlayerNumber, timeFrame, DEBUG_LOGLINENUMBER );
	return -1;
	unguard;
}

INT UProcessStatsCommandlet::SetMatchTeamScore( INT MatchID, INT TeamID, FLOAT Score )
{
	guard(UProcessStatsCommandlet::SetMatchTeamScore);
	if( TeamID!=-1 )
	{
		// Remember teams teamscore in match lookup
		FMatchData* MatchLookupPtr			= SetMatchInitGet(MatchID);						// Doing safe MatchLookup
		MatchLookupPtr->teamscore[TeamID]	+= Score;
		return 1;	//ok
	}
	return -1;
	unguard;
}

INT UProcessStatsCommandlet::SetMatchTeamID( INT MatchID, INT PlayerNumber, INT TeamID )
{
	guard(UProcessStatsCommandlet::SetMatchTeamID);
	if( PlayerNumber!=-1 && TeamID!=-1 )
	{
		// Remember player's current teamid in db
		INT playerid	= GetPlayerID( MatchID, PlayerNumber );
		INT mpid		= ISmpid( MatchID, playerid );
		if( mpid!=-1 )
			MySQL.DoSQL( "update matchplayers set teamid='%d' where mpid='%d' ", TeamID, mpid );

		// Remember teamid in match lookup
		FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);
		FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find(PlayerNumber);
		// Updating the teamid
		if( PlayerLookupPtr != NULL )
		{
			PlayerLookupPtr->teamid = TeamID;
			return 1;	//ok
		}
	}
	GWarn->Logf(TEXT("Error: SetMatchTeamID() could not set teamid for MatchID %d, PNum %d in line: %d"), 
			MatchID, PlayerNumber, DEBUG_LOGLINENUMBER );
	return -1;
	unguard;
}

INT UProcessStatsCommandlet::SetMatchMapID( INT MatchID, INT mapid )
{
	guard(UProcessStatsCommandlet::SetMatchMapID);
	// Update the mapid
	if( mapid!=-1 )
	{
		FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);	//MatchLookupPtr will exist, or quit code!
		MatchLookupPtr->mapid = mapid;
	}
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::SetMatchMutatorID( INT MatchID, INT mutid )
{
	guard(UProcessStatsCommandlet::SetMatchMutatorID);
	// Remember if mutator used in this match
	if( mutid!=-1 )
	{
		FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
		MatchLookupPtr->mutidLookup.Set( mutid, 1 );
	}
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::SetMatchPlayerID( INT MatchID, INT PlayerNumber, INT playerid )
{
	guard(UProcessStatsCommandlet::SetMatchPlayerID);
	// Remember PlayerNumber/playerid lookup
	if( PlayerNumber!=-1 && playerid!=-1 )
	{
		FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
		FPlayerData NewPlayer;
		NewPlayer.playerid			= playerid;
		
		// Setting defaults
		NewPlayer.ConnectTime		= 0;
		// Setting default teamid to 0, this is needed for teamchange free game like DM.
		NewPlayer.teamid			= 0;	// Updated later, on a teamchange

		MatchLookupPtr->PlayerLookup.Set( PlayerNumber, NewPlayer );
	}
	return 0;
	unguard;
}

FMatchData* UProcessStatsCommandlet::SetMatchInitGet( INT MatchID )
{
	// Check if MatchLookup exits for MatchID, if so return it, else generate it
	FMatchData* MatchLookupPtr = MatchLookup.Find( MatchID );

	// Find fails - generate MatchID struct with default data
	if( !MatchLookupPtr )
	{
		// Setting some defaults
		FMatchData	InitFMatchData;

//		InitFMatchData.DateTime		= TEXT("Unknown");
//		InitFMatchData.TimeZone		= 0;
//		InitFMatchData.lastUsed;	= 0.;
		InitFMatchData.MapName		= TEXT("Unknown");
		InitFMatchData.MapTitle		= TEXT("Unknown");
		InitFMatchData.MapAuthor	= TEXT("Anonymous");
		InitFMatchData.GameClass	= TEXT("Unknown");
		InitFMatchData.GameName		= TEXT("Unknown");
		InitFMatchData.Mutators		= TEXT("Mutators=");
		InitFMatchData.mid			= 1;					// Default DM gametype
		InitFMatchData.mapid		= 1;					// First map in list - arbitrary.
		InitFMatchData.pure			= 0;					// Game not pure by default

		MatchLookup.Set(MatchID,InitFMatchData);			// Adding new lookup

		MatchLookupPtr = MatchLookup.Find( MatchID );		// Check again, must work!
		if( !MatchLookupPtr )								// Failed again, critical!
		{
			GWarn->Logf(TEXT("Error: MatchLookupPtr for MatchID (%d) could not be allocated!"),	MatchID );
			check( MatchLookupPtr != NULL );				// Exit
		}

		// Globally remember what MatchID are currently being passed to the Master Server
		MatchIDs.AddItem( MatchID );						// New MatchID, so adding it to list
	}

	return 	MatchLookupPtr;
}

void UProcessStatsCommandlet::SetMatchRemove( INT MatchID )
{
	// Removes all allocations for MatchLookup, for that MatchID
	// Also, removed sub allocs like: TMap<INT, FPlayerData> PlayerLookup;
	FMatchData* MatchLookupPtr = MatchLookup.Find( MatchID );

	// Find works, Ptr exists, delete MatchLookup for MatchID
	if( MatchLookupPtr )
		MatchLookup.Remove(MatchID);

	// This MatchID will never be tracked by Master Server again!
	// Remove it from list.
	for( INT m=0; m<MatchIDs.Num(); m++ )
	{
		// This MatchID timed out, remove data from RAM
		if( MatchID == MatchIDs(m) )
			MatchIDs.Remove(m);			//** Alternative MatchIDs.RemoveItem(MatchID);
	}
}


/*-----------------------------------------------------------------------------
Getting info from MatchID specific "storage"
-----------------------------------------------------------------------------*/

INT UProcessStatsCommandlet::GetTimeFrameNumber( INT MatchID, INT timeFrame )
{
	// Get this match's current TFN
	guard(UProcessStatsCommandlet::GetTimeFrameNumber);
	INT timeFrameNumber = 1;				// Default safe
	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
	timeFrameNumber = MatchLookupPtr->timeFrameNumber[timeFrame];
	return timeFrameNumber;
	unguard;
}

DOUBLE UProcessStatsCommandlet::GetLastUsed( INT MatchID )
{
	// Convert matchid to svid
	guard(UProcessStatsCommandlet::GetSVID);
	DOUBLE lastUsed = 0.;
	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
	lastUsed = MatchLookupPtr->lastUsed;
	return lastUsed;
	unguard;
}

INT UProcessStatsCommandlet::GetSVID( INT MatchID )
{
	// Convert matchid to svid
	guard(UProcessStatsCommandlet::GetSVID);
	INT svid = -1;
	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
	svid = MatchLookupPtr->svid;
	return svid;
	unguard;
}

UBOOL UProcessStatsCommandlet::GetTeamGame( INT MatchID )
{
	// Is this a team game or not?
	guard(UProcessStatsCommandlet::GetTeamGame);
	UBOOL teamgame = 0;					// Default safe
	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
	teamgame = MatchLookupPtr->teamgame;
	return teamgame;
	unguard;
}

INT UProcessStatsCommandlet::GetTimeZone( INT MatchID )
{
	// Retrieving this matches timezone info
	guard(UProcessStatsCommandlet::GetTimeZone);
	INT TimeZone = 0;					// Default safe
	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
	TimeZone = MatchLookupPtr->TimeZone;
	return TimeZone;
	unguard;
}

INT UProcessStatsCommandlet::GetConnectTime( INT MatchID, INT PlayerNumber )
{
	// Convert in-match player numbers into connect times
	guard(UProcessStatsCommandlet::GetConnectTime);
	INT ConnectTime = 0;				// Default safe
	if( PlayerNumber != -1 )			//PlayerNumber in Match
	{
		FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);
		FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find(PlayerNumber);
		if( PlayerLookupPtr != NULL )	// Else return safe 0.
			ConnectTime = PlayerLookupPtr->ConnectTime;
		else
			GWarn->Logf(TEXT("Warning: GetConnectTime() no ConnectTime for PlayerNum %d, using 0., in log line: %d"), 
				PlayerNumber, DEBUG_LOGLINENUMBER );
	}
	return ConnectTime;
	unguard;
}

FLOAT UProcessStatsCommandlet::GetTeamScore( INT MatchID, INT TeamID )
{
	// Look up the teamscore, for this match and team
	guard(UProcessStatsCommandlet::GetTeamScore);
	FLOAT teamscore = 0.;				// Default safe
	if( TeamID != -1 )
	{
		FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
		teamscore = MatchLookupPtr->teamscore[TeamID];
	}
	return teamscore;
	unguard;
}

FLOAT UProcessStatsCommandlet::GetRankELO( INT MatchID, INT PlayerNumber, INT timeFrame )
{
	// Convert in-match player numbers into RankELO values
	guard(UProcessStatsCommandlet::GetRankELO);
	FLOAT rankelo = 0.;					// Default safe
	if( PlayerNumber != -1 )			// PlayerNumber in Match
	{
		FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);
		FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find(PlayerNumber);
		if( PlayerLookupPtr != NULL )	// Else return safe 0.
			rankelo = PlayerLookupPtr->rankelo[timeFrame];
		else
			GWarn->Logf(TEXT("Warning: GetRankELO() no rankelo for PlayerNum %d and TF %d, using 0., in log line: %d"), 
				PlayerNumber, timeFrame, DEBUG_LOGLINENUMBER );
	}
	return rankelo;
	unguard;
}

FLOAT UProcessStatsCommandlet::GetRankTeam( INT MatchID, INT PlayerNumber, INT timeFrame )
{
	// Convert in-match player numbers into rankteam values
	guard(UProcessStatsCommandlet::GetRankTeam);
	FLOAT rankteam = 0.;				// Default safe
	if( PlayerNumber != -1 )			// PlayerNumber in Match
	{
		FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);
		FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find(PlayerNumber);
		if( PlayerLookupPtr != NULL )	// Else return safe 0.
			rankteam = PlayerLookupPtr->rankteam[timeFrame];
		else
			GWarn->Logf(TEXT("Warning: GetRankTeam() no rankteam for PlayerNum %d and TF %d, using 0., in log line: %d"), 
				PlayerNumber, timeFrame, DEBUG_LOGLINENUMBER );
	}
	return rankteam;
	unguard;
}

INT UProcessStatsCommandlet::GetTeamID( INT MatchID, INT PlayerNumber )
{
	// Convert in-match player numbers into teamids
	guard(UProcessStatsCommandlet::GetTeamID);
	INT teamid = 0;								// Default safe
	if( PlayerNumber != -1 )					// PlayerNumber in Match
	{
		FMatchData* MatchLookupPtr		= SetMatchInitGet(MatchID);
		FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find(PlayerNumber);
		if( PlayerLookupPtr != NULL )
			teamid = PlayerLookupPtr->teamid;	// Else return safe 0 team number
		else
			GWarn->Logf(TEXT("Warning: GetTeamID() no teamid for PlayerNum %d, using 0, in log line: %d"), 
				PlayerNumber, DEBUG_LOGLINENUMBER );
	}
	return teamid;
	unguard;
}


INT UProcessStatsCommandlet::GetPlayerIDnoWarn( INT MatchID, INT PlayerNumber )
{
	// Special no warning version of below, for EndGame
	// Convert in-match player numbers into playerids
	guard(UProcessStatsCommandlet::GetPlayerID);
	INT playerid = -1;				// Default missing, unsafe... needs to be checked for
	if( PlayerNumber != -1 )		// PlayerNumber in Match
	{
		FMatchData* MatchLookupPtr		= SetMatchInitGet( MatchID );
		FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find( PlayerNumber );

		if( PlayerLookupPtr != NULL )
			playerid = PlayerLookupPtr->playerid;
//		else
//			GWarn->Logf(TEXT("Warning: GetPlayerID() no playerid for PNum %d (-1), matchid %d, line: %d"),
//				PlayerNumber, MatchID, DEBUG_LOGLINENUMBER );
	}
	return playerid;
	unguard;
}


INT UProcessStatsCommandlet::GetPlayerID( INT MatchID, INT PlayerNumber )
{
	// Convert in-match player numbers into playerids
	guard(UProcessStatsCommandlet::GetPlayerID);
	INT playerid = -1;				// Default missing, unsafe... needs to be checked for
	if( PlayerNumber != -1 )		// PlayerNumber in Match
	{
		FMatchData* MatchLookupPtr		= SetMatchInitGet( MatchID );
		FPlayerData* PlayerLookupPtr	= MatchLookupPtr->PlayerLookup.Find( PlayerNumber );

		if( PlayerLookupPtr != NULL )
			playerid = PlayerLookupPtr->playerid;
		else
			GWarn->Logf(TEXT("Warning: GetPlayerID() no playerid for PNum %d (-1), matchid %d, line: %d"),
				PlayerNumber, MatchID, DEBUG_LOGLINENUMBER );
	}
	return playerid;
	unguard;
}

const TCHAR* UProcessStatsCommandlet::GetDateTime( INT MatchID )
{
	// Access the current time/date stamp
	guard(UProcessStatsCommandlet::GetDateTime);
	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
	return *MatchLookupPtr->DateTime;
	unguard;
}

INT UProcessStatsCommandlet::GetMapID( INT MatchID )
{
	// Access the current mapid, avoid crashes!
	guard(UProcessStatsCommandlet::GetMID);

	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
	INT mapid = MatchLookupPtr->mapid;

	return mapid;	
	unguard;
}

INT UProcessStatsCommandlet::GetMID( INT MatchID )
{
	// Access the current mid, avoid crashes!
	guard(UProcessStatsCommandlet::GetMID);

	FMatchData* MatchLookupPtr = SetMatchInitGet(MatchID);
	INT mid = MatchLookupPtr->mid;

	return mid;	
	unguard;
}


/*-----------------------------------------------------------------------------
Housekeeping functions
-----------------------------------------------------------------------------*/

void UProcessStatsCommandlet::StatsHousekeeping( void )
{
	// Housekeeping
	guard(UProcessStatsCommandlet::StatsHousekeeping);
	INT TIME_DETAILS = 1;		// 0 off

	// Keep housekeeping under control, do it ever 10 minutes or so!
	DOUBLE current_time = appSeconds();

#if CAL_NOTLOCAL_MYSQL_TESTING
	if( current_time - LAST_HOUSEKEEPING_TIME < TIME_BETWEEN_HOUSEKEEPINGS )
		return;
	else
#endif
		LAST_HOUSEKEEPING_TIME = current_time;		// Update timer

	DOUBLE starttime = appSeconds();
	GWarn->Logf(TEXT("Info: Starting stats housekeeping..."));

#if CAL_NOTLOCAL_MYSQL_TESTING
	// In timeout situation, remove all the MatchID lookup data
	CheckRemoveMatchIDs();
#endif

	// - Re-cache all db arrays
	CacheAll();

	// Calculate db summaries for players
		if( TIME_DETAILS ) 
			GWarn->Logf(TEXT("Info: Time (%f) seconds! - CheckRemoveMatchIDs, CacheAll done."), appSeconds()-current_time);
	if( THIS_MS_DOES_HOUSEKEEPING )
		CalcPlayersSummary();
		if( TIME_DETAILS ) 
			GWarn->Logf(TEXT("Info: Time (%f) seconds! - CalcPlayersSummary done."), appSeconds()-current_time);

	// Calculate global summaries from players data
	if( THIS_MS_DOES_HOUSEKEEPING )
		CalcGlobalSummary();
		if( TIME_DETAILS ) 
			GWarn->Logf(TEXT("Info: Time (%f) seconds! - CalcGlobalSummary done."), appSeconds()-current_time);

#if CAL_NOTLOCAL_MYSQL_TESTING
	// Regular Housekeeping
	// Daily:	Rank deterioration
	//			Per server only show latest matches, remove old ones!
	// Weekly:	Update of week timeFrameNumber counter
	// Monthly:	Update of month timeFrameNumber counter
	// Only done by one master server!
	if( THIS_MS_DOES_HOUSEKEEPING )
		NewTFNdayWeekMonth();
	if( TIME_DETAILS ) 
		GWarn->Logf(TEXT("Info: Time (%f) seconds! - NewTFNdayWeekMonth done."), appSeconds()-current_time);
#endif

	// - Purge unneeded data from database, e.g. players not seen for ages
	GWarn->Logf(TEXT("Info: Done with housekeeping..."));
	DOUBLE endtime = appSeconds();
	GWarn->Logf(TEXT("Info: Time needed (%f) seconds!"), endtime-starttime);

	unguard;
}

void UProcessStatsCommandlet::NewTFNdayWeekMonth( void )
{
	// Check if TFN for week / month need to be updated or not
	guard(UProcessStatsCommandlet::NewTFNdayWeekMonth);

	// Getting MYSQL's current date/time
	INT Year, Month, DayOfWeek, Day, Hour, Min, Sec;
	getMYSQLtime( Year, Month, DayOfWeek, Day, Hour, Min, Sec );

	//Info current MYSQL Date/Time: 2002-8-25 Weekday: 1=Sunday
	// GWarn->Logf(TEXT("Info current MYSQL Date/Time: %d-%d-%d %d:%d:%d  Weekday: %d"),
	//	Year, Month, Day, Hour, Min, Sec, DayOfWeek );

	// Checking when to update
	for( INT t=0; t<=2; t++ )		// Day, Week, Month
	{
		BOOL RightDay=0;
		if( t==0 )					// Day, every day is potentially the right day :)
			RightDay = 1;
		if( t==1 )					// Week, Monday, beginning of new week
			RightDay = (DayOfWeek==2);
		if( t==2 )					// Month, first day of new month
			RightDay = (Day==1);

		if( RightDay )				// Key DAY! - Up TFN day!
		{
			// Getting the last date when (day), week, month updates in TFN happened
			INT year, month, day; 
			getDateOfReset( t, year, month, day );
			// GWarn->Logf(TEXT("Info dateOfReset Date/Time: %d-%d-%d  for timeFrame %d"),	year, month, day, t );

			if( year!=Year || month!=Month || day!=Day )	// This is a new day. Never updated on before?
			{
				// Getting the current TF
				FString QueryText = FString::Printf(TEXT( 
					"select currentTF from timeframe where timeframe='%d' order by currentTF desc limit 1"), t );
				INT currentTF = FindID(*QueryText);
				INT tfid;

				if( t==0 )									// A new Day - do Rank deterioration / remove extrenious matches
				{
					ReduceEveryOnesRank();
					RemoveOldMatches();
					tfid = IStfid( t, currentTF );			// CurrentTF for Day not incremented, just one row!		
				}
				else										// Week, Month only require these updates
					tfid = IStfid( t, currentTF+1 );		

				// Generating next TFN
				MySQL.DoSQL( "update timeframe set timeZone='%d', dateOfReset=NOW() where tfid='%d' ", 0, tfid );
			}

		}		// end - RightDay
	}			// end - t loop
	unguard;
}

void UProcessStatsCommandlet::RemoveOldMatches( void )
{
	// Reducing all player ranks 
	guard(UProcessStatsCommandlet::RemoveOldMatches);
	TArray<INT>	MatchIDz;					// Local temporary array of MatchIDs, not the global "MatchIDs"
	TArray<INT>	mpids;

	// Getting list of serverID handles: svid
	FQueryResult* svidQuery = MySQL.Query( "select svid from server " );
	FQueryField** Row;
	while( (Row = svidQuery->FetchNextRow()) != NULL )
	{
		INT svid = Row[0]->AsInt();

		// Count number of matches for this server where thusfar buffered
		FString Query = FString::Printf(TEXT("select count(matchesid) from matches where svid='%d' "), svid );
		INT count = FindID(*Query);
		// GWarn->Logf( TEXT("Info: Counted (%d) matches!"), count );

		if( count > MAX_NUM_MATCHES_PER_SERVER )
		{
			INT limit = count - MAX_NUM_MATCHES_PER_SERVER;		// Number of matches to actually remove
			// GWarn->Logf( TEXT("Info: Removing limit (%d) matches!"), limit );

			// Find the matchIDz that are outdated, e.g. those with the lowest numbers!
			MatchIDz.Empty();										// Make sure this array is empty
			FQueryResult* matchIDQuery = MySQL.Query( 
				"select matchid from matches where svid='%d' order by matchid limit %d ", svid, limit );
			FQueryField** matchRow;
			while( (matchRow = matchIDQuery->FetchNextRow()) != NULL )
			{
				INT MatchID = matchRow[0]->AsInt();
				MatchIDz.AddItem( MatchID );	// Avoid read, delete conflict in matches! Pre-Cache MatchIDz.

				// Find the MatchIDz that are outdated, e.g. those with the lowest numbers!
				mpids.Empty();										// Make sure this array is empty
				FQueryResult* mpidQuery = MySQL.Query( 
					"select mpid from matchplayers where matchid='%d' ", MatchID );
				FQueryField** mpidRow;
				while( (mpidRow = mpidQuery->FetchNextRow()) != NULL )
				{
					INT mpid = mpidRow[0]->AsInt();
					mpids.AddItem( mpid );
				}
				delete mpidQuery;
			}
			delete matchIDQuery;

			// Deleting extrenious matches per MatchID
			for( INT m=0; m<MatchIDz.Num(); m++ )
			{
				INT MatchID = MatchIDz(m);
				// GWarn->Logf( TEXT("Info: Removing MatchID (%d)!"), MatchID );
				MySQL.DoSQL( "delete from matches where matchid='%d' ", MatchID );
				MySQL.DoSQL( "delete from matchplayers where matchid='%d' ", MatchID );
				MySQL.DoSQL( "delete from matchteamscores where matchid='%d' ", MatchID );
				MySQL.DoSQL( "delete from matchteamscoring where matchid='%d' ", MatchID );
				MySQL.DoSQL( "delete from matchmutators where matchid='%d' ", MatchID );
			}

			// Deleting extrenious matches per mpid
			for( INT m=0; m<mpids.Num(); m++ )
			{
				INT mpid = mpids(m);
				// GWarn->Logf( TEXT("Info: Removing mpid (%d)!"), mpid );
				MySQL.DoSQL( "delete from matchweapons where mpid='%d' ", mpid );
				MySQL.DoSQL( "delete from matchsuicides where mpid='%d' ", mpid );
				MySQL.DoSQL( "delete from matchevents where mpid='%d' ", mpid );
				MySQL.DoSQL( "delete from matchscores where mpid='%d' ", mpid );
				MySQL.DoSQL( "delete from matchscoring where mpid='%d' ", mpid );
				MySQL.DoSQL( "delete from matchvictors where mpid='%d' ", mpid );
			}

		}	// end - count if
	}		// end - svid loop
	delete svidQuery;
	unguard;
}

void UProcessStatsCommandlet::ReduceEveryOnesRank( void )
{
	// Reducing all player ranks 
	guard(UProcessStatsCommandlet::ProcessReduceEveryOnesRank);
	INT		pid,		timeFrame;
	FLOAT	decrease,	rankELO, rankTeam;

	// Getting current data from database
	FQueryResult* Query = MySQL.Query( "select pid, timeFrame, rankELO, rankTeam from player " );
	FQueryField** Row;
	while( (Row = Query->FetchNextRow()) != NULL )
	{
		pid			= Row[0]->AsInt();
		timeFrame	= Row[1]->AsInt();
		rankELO		= Row[2]->AsFloat();
		rankTeam	= Row[3]->AsFloat();

		if( rankELO>0. || rankTeam>0. )								// Only update if this player has some positive rank
		{
			decrease	=  FindELODecrease(timeFrame);
			rankELO		-= decrease;								// Rank reduction
			rankTeam	-= decrease;	
			if( rankELO	 <= 0. ) rankELO = 0.;						// Rank 0. is lower limit!
			if( rankTeam <= 0. ) rankTeam = 0.;
			MySQL.DoSQL( "update player set rankELO='%f', rankTeam='%f' where pid='%d' ", rankELO, rankTeam, pid );
		}
	}
	delete Query;
	unguard;
}

void UProcessStatsCommandlet::CheckRemoveMatchIDs( void )
{
	guard(UProcessStatsCommandlet::CheckRemoveMatchIDs);

	DOUBLE time = appSeconds();

	for( INT m=0; m<MatchIDs.Num(); m++ )
	{
		INT MatchID		= MatchIDs( m );
		DOUBLE lastUsed = GetLastUsed( MatchID );

		// This MatchID timed out, remove data from RAM
		// GWarn->Logf(TEXT("Info: Diff time %f for MatchIDs(%d) = %d from list!"), time - lastUsed, m, MatchID );
		if( time - lastUsed > TIME_MATCHID_TIMEOUT )	// 600 seconds
		{
			FMatchData* MatchLookupPtr = MatchLookup.Find( MatchID );

			// Find works, Ptr exists, delete MatchLookup for MatchID
			if( MatchLookupPtr )
				MatchLookup.Remove(MatchID);

			// This MatchID will never be tracked by Master Server again!
			// Remove it from list.
			MatchIDs.Remove(m);			//** Alternative MatchIDs.RemoveItem(MatchID);
			// GWarn->Logf(TEXT("Info: Removed MatchIDs(%d) = %d from list!"), m, MatchID );
		}
	}
	unguard;
}

void UProcessStatsCommandlet::CalcGlobalSummary( void )
{
	guard(UProcessStatsCommandlet::CalcGlobalSummary);

	// Getting database relevant ids
	FString QueryText;

	// Generate a players table with same pid number as player and fill fields with pids
	MySQL.DoSQL( "delete from global_temp" );						// Cleanup temp table, just in case
	MySQL.DoSQL( "insert into global_temp select * from global" );	// Need to remember older timeframes!

	// These are not differed by timeframe, so access them only once!
	//SERVERS
	QueryText = FString::Printf(TEXT(
		"select count(serverID) from server ") );			// Not differed by timeframe!!!
	INT servers = FindID(*QueryText);
	if( servers==-1 ) servers=0;

	//MAPS
	QueryText = FString::Printf(TEXT(
		"select count(mapid) from map ") );
	INT maps = FindID(*QueryText);
	if( maps==-1 ) maps=0;

	//MAPCHANGES
	QueryText = FString::Printf(TEXT(
		"select sum(games) from maps ") );
	INT mapChanges = FindID(*QueryText);
	if( mapChanges==-1 ) mapChanges=0;

	for( INT t=1; t<=3; t++ )								// Loop for the (3) timeFrames
	{
		// Getting the currentTF for this timeframe
		INT timeFrameNumber = FindTimeFrameNumber( t );
		if( timeFrameNumber==-1 ) continue;

		// TABLE GLOBAL_TEMP
		INT gid = ISgid( t, timeFrameNumber );
		if( gid==-1 ) continue;								//%% Fallback not possible, to much needs this gid

		// KILLS,SUICIDES,EVENTS,TOTALSCORE - all bigint - Merged calls to MYSQL
		SQWORD values[4];
		QueryText = FString::Printf(TEXT(
			"select sum(kills),sum(suicides),sum(events),sum(totalScore) from players, player where players.pid=player.pid and player.timeFrame='%d' and player.timeFrameNumber='%d' "), 
			t, timeFrameNumber );
		SQWORD kills		= FindIDQMulti(*QueryText, values, 4);
		SQWORD suicides		= values[1];
		SQWORD events		= values[2];
		SQWORD totalScore	= values[3];
		if( kills==-1 )		kills=0;
		if( suicides==-1 )	suicides=0;
		if( events==-1 )	events=0;
		MySQL.DoSQL( "update global_temp set kills='%I64d',suicides='%I64d',events='%I64d',totalScore='%I64d' where gid='%d' ", 
			kills, suicides, events, totalScore, gid );

		// TEAMKILLS, PLAYERS, PLAYERMINUTES, PLAYERCONNECTS, GAMESFINISHED - some bigint, some int
		SQWORD values2[5];
		QueryText = FString::Printf(TEXT(
			"select sum(teamKills),count(DISTINCT playerid),sum(playerMinutes),sum(playerConnects),sum(gamesFinished) from player where timeFrame='%d' and timeFrameNumber='%d' "), 
			t, timeFrameNumber );
		SQWORD teamkills	= FindIDQMulti(*QueryText, values2, 5);
		INT players			= values2[1];		// Down sampling
		SQWORD playerMinutes= values2[2];
		INT playerConnects	= values2[3];
		INT gamesFinished	= values2[4];
		if( teamkills==-1 )			teamkills=0;
		if( players==-1 )			players=0;
		if( playerMinutes==-1 )		playerMinutes=0;
		if( playerConnects==-1 )	playerConnects=0;
		if( gamesFinished==-1 )		gamesFinished=0;
		MySQL.DoSQL( "update global_temp set teamkills='%I64d',players='%d',playerMinutes='%I64d',playerConnects='%d',gamesFinished='%d' where gid='%d' ", 
			teamkills, players, playerMinutes, playerConnects, gamesFinished, gid );

		// GAMESPLAYED, SERVERCONNECTS, SERVERTIME - some int, some bigint 
		SQWORD values3[3];
		QueryText = FString::Printf(TEXT(
			"select sum(maps),sum(connects),sum(uptime) from servers where timeFrame='%d' and timeFrameNumber='%d' "), 
			t, timeFrameNumber );
		INT gamesPlayed		= FindIDQMulti(*QueryText, values3, 3);
		INT serverConnects	= values3[1];
		SQWORD serverTime	= values3[2];
		if( gamesPlayed<=0 )		gamesPlayed=1;
		if( serverConnects==-1 )	serverConnects=0;
		if( serverTime==-1 )		serverTime=0;
		MySQL.DoSQL( "update global_temp set gamesPlayed='%d',serverConnects='%d',serverTime='%I64d' where gid='%d' ", 
			gamesPlayed, serverConnects, serverTime, gid );

/*
		// Removing frags per hour, irrelevant stats!
		//FRAGSPERHOUR
		QueryText = FString::Printf(TEXT(
			"select (UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(dateOfReset))/3600 as hours from timeframe where timeFrame='%d' and currentTF='%d' "),
			t, timeFrameNumber );
		INT hours = FindID(*QueryText);
		if( hours<=0 ) hours=1;								//%% Fallback, good case!
		INT frags = kills - suicides - teamkills;
		FLOAT fragsPerHour = frags/gamesPlayed/hours;
		MySQL.DoSQL( "update global_temp set fragsPerHour='%f' where gid='%d' ", fragsPerHour, gid );
*/

		//EFFICIENCY
		INT fraction = ( kills + suicides + teamkills );	//kills = deaths //( 2*kills + suicides + 2*teamkills )
		if( fraction==0 ) fraction=1;						//Safe devision
		FLOAT efficiency = kills / (fraction*1.);

		//PLAYERSPERGAME
		if( gamesPlayed==0 ) gamesPlayed=1;					//Safe devision
		FLOAT playersPerGame = playerConnects / (gamesPlayed*1.);

#if USE_HKG_GAMESPERDAY
		//GAMESPERDAY
		QueryText = FString::Printf(TEXT(
			"select TO_DAYS(NOW())-TO_DAYS(dateOfReset) as days from timeframe where timeFrame='%d' and currentTF='%d' "),
			t, timeFrameNumber );
		INT days = FindID(*QueryText);
		if( days<=0 ) days=1;
		FLOAT gamesPerDay = gamesPlayed/days;
#else
		FLOAT gamesPerDay = 0;
#endif

		//MUTATORS
		QueryText = FString::Printf(TEXT(
			"select sum(mutators) from mutators where timeFrame='%d' and timeFrameNumber='%d' "), 
			t, timeFrameNumber );
		INT mutators = FindID(*QueryText);
		if( mutators==-1 ) mutators=0;

		//MODS
		QueryText = FString::Printf(TEXT(
			"select sum(mods) from mods where timeFrame='%d' and timeFrameNumber='%d' "), 
			t, timeFrameNumber );
		INT mods = FindID(*QueryText);
		if( mods==-1 ) mods=0;

		// One update
		MySQL.DoSQL( "update global_temp set efficiency='%f',playersPerGame='%f',gamesPerDay='%f',servers='%d',maps='%d',mapChanges='%d',mutators='%d',mods='%d' where gid='%d' ", 
			efficiency, playersPerGame, gamesPerDay, servers, maps,	mapChanges,	mutators, mods,	gid );

	}	//end t - timeFrame

	// Move data into real global table
	MySQL.DoSQL( "delete from global" );							// Cleanup last global data table
	MySQL.DoSQL( "insert into global select * from global_temp" );	// Copy data from temp to real table
	MySQL.DoSQL( "delete from global_temp" );						// Cleanup temp table

	unguard;
}

#if CAL_HK_PLAYERSSUM_ALT2
void UProcessStatsCommandlet::CalcPlayersSummary( void )
{
	guard(UProcessStatsCommandlet::CalcPlayersSummary);
	// Getting database relevant ids
	// GWarn->Logf(TEXT("Info: Doing CalcPlayersSummary..."));
	FString QueryText;
	FString Update;

	// Generate a players table with same pid number as player and fill fields with data and new pids
	MySQL.DoSQL( "delete from players_temp" );									// Cleanup temp table, just in case
	MySQL.DoSQL( "insert into players_temp select * from players" );			// Duplicate current table (might be empty)
	// All fields that have a pid in pidstatus will also generate data in players_temp,
	// the fields in pid, we do not care about.
	// MySQL.DoSQL( "insert into players_temp (pid) select pid from pidstatus" );
	// Alas the web might want to access these certain pids, thus they need to exist!
	MySQL.DoSQL( "insert into players_temp (pid) select pid from player" );		// Add/Insert any new pids

#if !USE_HKPl_APKILLS_PDEATHS
	// now getting all kills at once! and mode='1' 
	// KILLS,DEATHS
	QueryText	= FString::Printf(TEXT(
		"select weapons.pid,sum(kills),sum(deaths) from weapon,weapons,pidstatus where weapons.pid=pidstatus.pid and weapon.wid=weapons.wid group by weapons.pid "));
	// "update players_temp set kills='%d',deaths='%d' where pid='%d' ", kills, deaths, pid 
	FindIDUpdateMulti( *QueryText, 4 );		// mode = 4	-	kills,deaths
#else
	// PKILLS,PDEATHS - mode = 1 primary
	QueryText	= FString::Printf(TEXT(
		"select weapons.pid,sum(kills),sum(deaths) from weapon,weapons,pidstatus where weapons.pid=pidstatus.pid and weapon.wid=weapons.wid and mode='1' group by weapons.pid "));
	// "update players_temp set pkills='%d',pdeaths='%d',kills='%d',deaths='%d' where pid='%d' ",	kills, deaths, kills, deaths, pid );
	FindIDUpdateMulti( *QueryText, 0 );		// mode = 0	-	pkills, pdeaths

	// AKILLS	- mode > 1 secondary, combo, headshot
	QueryText	= FString::Printf(TEXT(
		"select weapons.pid, sum(kills) from weapon,weapons,pidstatus where weapons.pid=pidstatus.pid and weapon.wid=weapons.wid and mode>'1' group by weapons.pid "));
	// "update players_temp set akills='%d',kills=kills+'%d' where pid='%d' ", 				akills, akills, pid );
	FindIDUpdateMulti( *QueryText, 1 );		// mode = 1 -	akills
#endif

#if !USE_HKPl_WESUICIDES
	//SUICIDES
	QueryText	= FString::Printf(TEXT(
		"select suicides.pid, sum(suicides) from suicide,suicides,pidstatus where suicides.pid=pidstatus.pid and suicide.sid=suicides.sid group by suicides.pid "));
	// "update players_temp set suicides='%d' where pid='%d' ", suicides, pid
	FindIDUpdateMulti( *QueryText, 5 );		// mode = 5 -	suicides
#else
	//WSUICIDES	- mode = 0 enviro
	QueryText	= FString::Printf(TEXT(
		"select suicides.pid, sum(suicides) from suicide,suicides,pidstatus where suicides.pid=pidstatus.pid and suicide.sid=suicides.sid and mode='1' group by suicides.pid "));
	// "update players_temp set wsuicides='%d',suicides='%d' where pid='%d' ",	wsuicides, wsuicides, pid
	FindIDUpdateMulti( *QueryText, 2 );		// mode = 2 -	wsuicides

	//ESUICIDES	- mode = 1 weapons
	QueryText	= FString::Printf(TEXT(
		"select suicides.pid, sum(suicides) from suicide,suicides,pidstatus where suicides.pid=pidstatus.pid and suicide.sid=suicides.sid and mode='0' group by suicides.pid "));
	// "update players_temp set esuicides='%d',suicides=suicides+'%d' where pid='%d' ", esuicides, esuicides, pid  
	FindIDUpdateMulti( *QueryText, 3 );		// mode = 3 -	esuicides
#endif

#if USE_HKPl_FRAGS_EFFICIENCY
	// Frags and Efficiency are irrelevant!
	// KILLS, DEATHS, SUICIDES, FRAGS, EFFICIENCY - Updates
	/*
	// Done in FindIDUpdateSum()
	INT kills			= pkills + akills;
	INT deaths			= pdeaths + adeaths;
	INT suicides		= wsuicides + esuicides;	
	INT frags			= kills - suicides;
	INT fraction		= kills + deaths + suicides;		//** + teamkills
	if( fraction == 0 ) fraction=1;							//Safe devision
	FLOAT efficiency	= kills / (fraction*1.);
	*/
	QueryText	= FString::Printf(TEXT(
		"select players_temp.pid, kills, deaths, suicides from players_temp,pidstatus where players_temp.pid=pidstatus.pid "));
	FindIDUpdateSum2( *QueryText );
#endif

#if USE_HKPl_EVENTS
	//EVENTS
	QueryText	= FString::Printf(TEXT(
		"select events.pid, sum(events) from events,pidstatus where events.pid=pidstatus.pid group by events.pid "));
	Update		= FString::Printf(TEXT("update players_temp set events="));
	FindIDUpdate( *QueryText, *Update );
#endif

#if USE_HKPl_MKs_SPREES
	//MULTIKILLS
	QueryText	= FString::Printf(TEXT(
		"select events.pid, sum(events) from events,event,pidstatus where events.pid=pidstatus.pid and events.eid=event.eid and eventcode LIKE 'multikill_%%' group by events.pid "));
	Update		= FString::Printf(TEXT("update players_temp set multikills="));
	FindIDUpdateWildcard( *QueryText, *Update );

	//SPREES
	QueryText	= FString::Printf(TEXT(
		"select events.pid, sum(events) from events,event,pidstatus where events.pid=pidstatus.pid and events.eid=event.eid and eventcode LIKE 'spree_%%' group by events.pid "));
	Update		= FString::Printf(TEXT("update players_temp set sprees="));
	FindIDUpdateWildcard( *QueryText, *Update );
#endif

#if USE_HKPl_TOTALSCORE
	//TOTALSCORE - BIGINT
	QueryText	= FString::Printf(TEXT(
		"select scores.pid, sum(scoresum) from scores,pidstatus where scores.pid=pidstatus.pid group by scores.pid "));
	Update		= FString::Printf(TEXT("update players_temp set totalScore="));
	FindIDQUpdate( *QueryText, *Update );					//'%I64d' where pid='%d' added in FindIDQUpdate()
#endif

#if USE_HKPl_TEAMSCORE
	// check where this is use Not really relevant!
	//TEAMSCORE
	QueryText	= FString::Printf(TEXT(
		"select teamscores.pid, sum(teamscoresum) from teamscores,pidstatus where teamscores.pid=pidstatus.pid group by teamscores.pid "));
	Update		= FString::Printf(TEXT("update players_temp set teamScore="));
	FindIDUpdate( *QueryText, *Update );
#endif
	
	// Updated ALL pids, so reset them!
	MySQL.DoSQL( "delete from pidstatus" );								// All fields for this HK updated! Empty table!

	// Move data into real player table
	MySQL.DoSQL( "delete from players" );								// Cleanup last players data
	MySQL.DoSQL( "insert into players select * from players_temp" );	// Copy back new data from temp to real table
	MySQL.DoSQL( "delete from players_temp" );							// Cleanup temp table

	unguard;
}
#else
//============== ORIGINAL CODE ====================
void UProcessStatsCommandlet::CalcPlayersSummary( void )
{
	guard(UProcessStatsCommandlet::CalcPlayersSummary);
	// Getting database relevant ids
	// GWarn->Logf(TEXT("Info: Doing CalcPlayersSummary..."));
	FString QueryText;
	FString Update;
	//** Don't care about OFFICIAL or not

	// Generate a players table with same pid number as player and fill fields with pids
	MySQL.DoSQL( "delete from players_temp" );								// Cleanup temp table, just in case
	MySQL.DoSQL( "insert into players_temp (pid) select pid from player" );	// Copy known pids

	//PKILLS	- mode = 1 primary
	QueryText	= FString::Printf(TEXT(
		"select pid, sum(kills) from weapon,weapons where weapon.wid=weapons.wid and mode='1' group by pid "));
	Update		= FString::Printf(TEXT("update players_temp set pkills="));		//'%d' where pid='%d' is generated in FindIDUpdate
	FindIDUpdate( *QueryText, *Update );	

	//PDEATHS
	QueryText	= FString::Printf(TEXT(
		"select pid, sum(deaths) from weapon,weapons where weapon.wid=weapons.wid and mode='1' group by pid "));
	Update		= FString::Printf(TEXT("update players_temp set pdeaths="));
	FindIDUpdate( *QueryText, *Update );

	//AKILLS	- mode > 1 secondary, combo, headshot
	QueryText	= FString::Printf(TEXT(
		"select pid, sum(kills) from weapon,weapons where weapon.wid=weapons.wid and mode>'1' group by pid "));
	Update		= FString::Printf(TEXT("update players_temp set akills="));
	FindIDUpdate( *QueryText, *Update );

	//ADEATHS - NEVER AVAILABLE!
	//QueryText	= FString::Printf(TEXT(
	//	"select pid, sum(deaths) from weapon,weapons where weapon.wid=weapons.wid and mode>'1' group by pid "));
	//Update		= FString::Printf(TEXT("update players_temp set adeaths="));
	//FindIDUpdate( *QueryText, *Update );

	//WSUICIDES	- mode = 0 enviro
	QueryText	= FString::Printf(TEXT(
		"select pid, sum(suicides) from suicide,suicides where suicide.sid=suicides.sid and mode='1' group by pid "));
	Update		= FString::Printf(TEXT("update players_temp set wsuicides="));
	FindIDUpdate( *QueryText, *Update );

	//ESUICIDES	- mode = 1 weapons
	QueryText	= FString::Printf(TEXT(
		"select pid, sum(suicides) from suicide,suicides where suicide.sid=suicides.sid and mode='0' group by pid "));
	Update		= FString::Printf(TEXT("update players_temp set esuicides="));
	FindIDUpdate( *QueryText, *Update );

	// KILLS, DEATHS, SUICIDES, FRAGS, EFFICIENCY - Updates
	/*
	// Done in FindIDUpdateSum()
	INT kills			= pkills + akills;
	INT deaths			= pdeaths + adeaths;
	INT suicides		= wsuicides + esuicides;	
	INT frags			= kills - suicides;
	INT fraction		= kills + deaths + suicides;		//** + teamkills
	if( fraction == 0 ) fraction=1;							//Safe devision
	FLOAT efficiency	= kills / (fraction*1.);
	*/
	QueryText	= FString::Printf(TEXT(
		"select pid, pkills+akills as kills, pdeaths+adeaths as deaths, wsuicides+esuicides as suicides from players_temp group by pid "));
	FindIDUpdateSum( *QueryText );

	//EVENTS
	QueryText	= FString::Printf(TEXT(
		"select pid, sum(events) from events group by pid "));
	Update		= FString::Printf(TEXT("update players_temp set events="));
	FindIDUpdate( *QueryText, *Update );

	//MULTIKILLS
	QueryText	= FString::Printf(TEXT(
		"select pid, sum(events) from events,event where events.eid=event.eid and eventcode LIKE 'multikill_%%' group by pid "));
	Update		= FString::Printf(TEXT("update players_temp set multikills="));
	FindIDUpdate( *QueryText, *Update );

	//SPREES
	QueryText	= FString::Printf(TEXT(
		"select pid, sum(events) from events,event where events.eid=event.eid and eventcode LIKE 'spree_%%' group by pid "));
	Update		= FString::Printf(TEXT("update players_temp set sprees="));
	FindIDUpdate( *QueryText, *Update );

	//TOTALSCORE - BIGINT
	QueryText	= FString::Printf(TEXT(
		"select pid, sum(scoresum) from scores group by pid "));
	Update		= FString::Printf(TEXT("update players_temp set totalScore="));
	FindIDQUpdate( *QueryText, *Update );					//'%I64d' where pid='%d' added in FindIDQUpdate()

	//TEAMSCORE
	QueryText	= FString::Printf(TEXT(
		"select pid, sum(teamscoresum) from teamscores group by pid "));
	Update		= FString::Printf(TEXT("update players_temp set teamScore="));
	FindIDUpdate( *QueryText, *Update );

	// Move data into real player table
	MySQL.DoSQL( "delete from players" );								// Cleanup last players data
	MySQL.DoSQL( "insert into players select * from players_temp" );	// Copy data from temp to real table
	MySQL.DoSQL( "delete from players_temp" );							// Cleanup temp table

	unguard;
}
#endif

/*-----------------------------------------------------------------------------
Insert / Selects - needed to make sure the entry exists, prior to an update!
-----------------------------------------------------------------------------*/

INT UProcessStatsCommandlet::ISmvictorid( INT mpid, INT mvid )
{
	// TABLE MATCHVICTORS
	INT mvictorid=-1;
	FString Insert = FString::Printf(TEXT(
		"insert into matchvictors (mpid,mvid) values ('%d','%d') "), mpid, mvid );

	FString Select = FString::Printf(TEXT(
		"select mvictorid from matchvictors where mpid='%d' and mvid='%d' "), mpid, mvid );

	mvictorid = FindIDSafe( *Insert, *Select );
	return mvictorid;
}

INT UProcessStatsCommandlet::ISvictorid( INT pid, INT vid )
{
	// TABLE VICTORS
	INT victorid=-1;
	FString Insert = FString::Printf(TEXT(
		"insert into victors (pid,vid) values ('%d','%d') "), pid, vid );

	FString Select = FString::Printf(TEXT(
		"select victorid from victors where pid='%d' and vid='%d' "), pid, vid );

	victorid = FindIDSafe( *Insert, *Select );
	return victorid;
}

INT UProcessStatsCommandlet::IStfid( INT timeFrame, INT currentTF )
{
	// TABLE TIMEFRAME
	INT tfid=-1;
	FString Insert = FString::Printf(TEXT(
		"insert into timeframe (timeFrame,currentTF) values ('%d','%d') "), timeFrame, currentTF );

	FString Select = FString::Printf(TEXT(
		"select tfid from timeframe where timeFrame='%d' and currentTF='%d' "), timeFrame, currentTF );

	tfid = FindIDSafe( *Insert, *Select );
	return tfid;
}

INT UProcessStatsCommandlet::ISserversid( INT svid, INT t, INT timeFrameNumber )
{
	// TABLE SERVERS
	INT serversid=-1;
	if( svid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into servers (svid,timeFrame,timeFrameNumber) values ('%d','%d','%d') "), 
			svid, t, timeFrameNumber );

		FString Select = FString::Printf(TEXT(
			"select serversid from servers where svid='%d' and timeFrame='%d' and timeFrameNumber='%d' "), 
			svid, t, timeFrameNumber );

		serversid = FindIDSafe( *Insert, *Select );
	}
	return serversid;
}

INT UProcessStatsCommandlet::ISsvid( INT serverid )
{
	// TABLE SERVER
	INT svid=-1;
	if( serverid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into server (serverID) values ('%d') "), serverid );

		FString Select = FString::Printf(TEXT(
			"select svid from server where serverID='%d' "), serverid );

		svid = FindIDSafe( *Insert, *Select );
	}
	return svid;
}

INT UProcessStatsCommandlet::ISpmutatorsid( INT pid, INT mutid )
{
	// TABLE PMUTATORS
	INT pmutatorsid=-1;
	if( mutid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into pmutators (pid,mutid) values ('%d','%d') "), pid, mutid );

		FString Select = FString::Printf(TEXT(
			"select pmutatorsid from pmutators where pid='%d' and mutid='%d' "), pid, mutid );

		pmutatorsid = FindIDSafe( *Insert, *Select );
	}
	return pmutatorsid;
}

INT UProcessStatsCommandlet::ISpmapsid( INT pid, INT mapid )
{
	// TABLE PMAPS
	INT pmapsid=-1;
	if( mapid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into pmaps (pid,mapid) values ('%d','%d') "), pid, mapid );

		FString Select = FString::Printf(TEXT(
			"select pmapsid from pmaps where pid='%d' and mapid='%d' "), pid, mapid );

		pmapsid = FindIDSafe( *Insert, *Select );
	}
	return pmapsid;
}

INT UProcessStatsCommandlet::ISpmodsid( INT pid, INT mid )
{
	// TABLE PMODS
	INT pmodsid=-1;
	if( mid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into pmods (pid,mid) values ('%d','%d') "), pid, mid);

		FString Select = FString::Printf(TEXT(
			"select pmodsid from pmods where pid='%d' and mid='%d' "), pid, mid );

		pmodsid = FindIDSafe( *Insert, *Select );
	}
	return pmodsid;
}

INT UProcessStatsCommandlet::ISmatchesid( INT MatchID )
{
	// TABLE MATCHES
	FString Insert = FString::Printf(TEXT(
		"insert into matches (matchid) values ('%d') "), MatchID );

	FString Select = FString::Printf(TEXT(
		"select matchesid from matches where matchid='%d' "), MatchID );

	INT matchesid = FindIDSafe( *Insert, *Select );
	return matchesid;
}

INT UProcessStatsCommandlet::ISmwid( INT mpid, INT wid )
{
	// TABLE MATCHWEAPONS
	INT mwid=-1;
	if( mpid!=-1 && wid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into matchweapons (mpid, wid) values ('%d','%d') "), mpid, wid );

		FString Select = FString::Printf(TEXT(
			"select mwid from matchweapons where mpid='%d' and wid='%d' "), mpid, wid );

		mwid = FindIDSafe( *Insert, *Select );
	}
	return mwid;
}

INT UProcessStatsCommandlet::ISmsid( INT mpid, INT sid )
{
	// TABLE MATCHSUICIDES
	INT msid=-1;
	if( mpid!=-1 && sid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into matchsuicides (mpid, sid) values ('%d','%d') "), mpid, sid );

		FString Select = FString::Printf(TEXT(
			"select msid from matchsuicides where mpid='%d' and sid='%d' "), mpid, sid );

		msid = FindIDSafe( *Insert, *Select );
	}
	return msid;
}

INT UProcessStatsCommandlet::ISmeid( INT mpid, INT eid )
{
	// TABLE MATCHEVENTS
	INT meid=-1;
	if( mpid!=-1 && eid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into matchevents (mpid, eid) values ('%d','%d') "), mpid, eid );

		FString Select = FString::Printf(TEXT(
			"select meid from matchevents where mpid='%d' and eid='%d' "), mpid, eid );

		meid = FindIDSafe( *Insert, *Select );
	}
	return meid;
}

INT UProcessStatsCommandlet::ISmtscoringid( INT MatchID, INT TeamID, INT second )
{
	// TABLE MATCHTEAMSCORING
	INT mtscoringid=-1;
	if( second!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into matchteamscoring (matchid,teamid,second) values ('%d','%d','%d') "), MatchID, TeamID, second );

		FString Select = FString::Printf(TEXT(
			"select mtscoringid from matchteamscoring where matchid='%d' and teamid='%d' and second='%d' "), MatchID, TeamID, second );

		mtscoringid = FindIDSafe( *Insert, *Select );
	}
	return mtscoringid;
}

INT UProcessStatsCommandlet::ISmtsid( INT MatchID, INT TeamID, INT tsid )
{
	// TABLE MATCHTEAMSCORES
	INT mtsid=-1;
	if( tsid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into matchteamscores (matchid,teamid,tsid) values ('%d','%d','%d') "), MatchID, TeamID, tsid );

		FString Select = FString::Printf(TEXT(
			"select mtsid from matchteamscores where matchid='%d' and teamid='%d' and tsid='%d' "), MatchID, TeamID, tsid );

		mtsid = FindIDSafe( *Insert, *Select );
	}
	return mtsid;
}

INT UProcessStatsCommandlet::ISmscoringid( INT mpid, INT second )
{
	// TABLE MATCHSCORING
	INT mscoringid=-1;
	if( mpid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into matchscoring (mpid, second) values ('%d','%d') "), mpid, second );

		FString Select = FString::Printf(TEXT(
			"select mscoringid from matchscoring where mpid='%d' and second='%d' "), mpid, second );

		mscoringid = FindIDSafe( *Insert, *Select );
	}
	return mscoringid;
}

INT UProcessStatsCommandlet::ISmscid( INT mpid, INT scid )
{
	// TABLE MATCHSCORES
	INT mscid=-1;
	if( scid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into matchscores (mpid, scid) values ('%d','%d') "), mpid, scid );

		FString Select = FString::Printf(TEXT(
			"select mscid from matchscores where mpid='%d' and scid='%d' "), mpid, scid );

		mscid = FindIDSafe( *Insert, *Select );
	}
	return mscid;
}

INT UProcessStatsCommandlet::ISmmutid( INT MatchID, INT mutid )
{
	// TABLE MATCHMUTATORS
	INT mmutid=-1;
	if( mutid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into matchmutators (matchid, mutid) values ('%d','%d') "), MatchID, mutid );

		FString Select = FString::Printf(TEXT(
			"select mmutid from matchmutators where matchid='%d' and mutid='%d' "), MatchID, mutid );

		mmutid = FindIDSafe( *Insert, *Select );
	}
	return mmutid;
}

INT UProcessStatsCommandlet::ISplayerid( const TCHAR* StatsUsername, const TCHAR* StatsPassword )
{
	// TABLE PLAYERID
	FString Insert = FString::Printf(TEXT(
		"insert into playerid (statsusername, statspassword) values ('%s', '%s') "),
		MySQL.TCHARFormatSQL(StatsUsername), MySQL.TCHARFormatSQL(StatsPassword) );

	FString Select = FString::Printf(TEXT(
		"select playerid from playerid where statsusername='%s' and statspassword='%s' "),
		MySQL.TCHARFormatSQL(StatsUsername), MySQL.TCHARFormatSQL(StatsPassword) );

	INT playerid = FindIDSafe( *Insert, *Select );
	return playerid;
}

INT UProcessStatsCommandlet::ISmutid( const TCHAR* MutCode, INT official )
{
	// TABLE MUTATOR - official = 0 normally
	FString Insert = FString::Printf(TEXT(
		"insert into mutator (mutatorcode, official) values ('%s', '%d') "),	
		MySQL.TCHARFormatSQL(MutCode), official );

	FString Select = FString::Printf(TEXT(
		"select mutid from mutator where mutatorcode='%s' and official='%d' "), 
		MySQL.TCHARFormatSQL(MutCode), official );

	INT mutid = FindIDSafe( *Insert, *Select );
	return mutid;
}

INT UProcessStatsCommandlet::ISmapid( const TCHAR* MapName, const TCHAR* MapTitle, const TCHAR* MapAuthor, INT official )
{
	// TABLE MAP - official = 0 normally
	FString Insert = FString::Printf(TEXT(
		"insert into map (mapname, official, maptitle, author ) values ('%s', '%d', '%s', '%s') "),
		MySQL.TCHARFormatSQL(MapName), official, MySQL.TCHARFormatSQL(MapTitle), MySQL.TCHARFormatSQL(MapAuthor) );

	FString Select = FString::Printf(TEXT(
		"select mapid from map where mapname='%s' and official='%d' "), 
			MySQL.TCHARFormatSQL(MapName), official  );

	INT mapid = FindIDSafe( *Insert, *Select );
	return mapid;
}

INT UProcessStatsCommandlet::ISsid( const TCHAR* SuicideCode, INT official )
{
	// TABLE SUICIDE - official = 0 normally
	FString Insert = FString::Printf(TEXT(
		"insert into suicide (suicidecode, official) values ('%s', '%d') "), 
		MySQL.TCHARFormatSQL(SuicideCode), official );

	FString Select = FString::Printf(TEXT(
		"select sid from suicide where suicidecode='%s' and official='%d' "), 
		MySQL.TCHARFormatSQL(SuicideCode), official );

	INT sid = FindIDSafe( *Insert, *Select );
	return sid;
}

INT UProcessStatsCommandlet::ISeid( const TCHAR* EventCode, INT official )
{
	// TABLE EVENT - official = 0 normally
	FString Insert = FString::Printf(TEXT(
		"insert into event (eventcode, official) values ('%s', '%d') "), 
		MySQL.TCHARFormatSQL(EventCode), official );

	FString Select = FString::Printf(TEXT(
		"select eid from event where eventcode='%s' and official='%d' "), 
		MySQL.TCHARFormatSQL(EventCode), official );

	INT eid = FindIDSafe( *Insert, *Select );
	return eid;
}

INT UProcessStatsCommandlet::IStsid( const TCHAR* TeamScoreCode, INT official )
{
	// TABLE TeamScore - official = 0 normally
	FString Insert = FString::Printf(TEXT(
		"insert into teamscore (scorecode, official) values ('%s', '%d') "), 
		MySQL.TCHARFormatSQL(TeamScoreCode), official );

	FString Select = FString::Printf(TEXT(
		"select tsid from teamscore where scorecode='%s' and official='%d' "), 
		MySQL.TCHARFormatSQL(TeamScoreCode), official );

	INT tsid = FindIDSafe( *Insert, *Select );
	return tsid;
}

INT UProcessStatsCommandlet::ISscid( const TCHAR* ScoreCode, INT official )
{
	// TABLE SCORE - official = 0 normally
	FString Insert = FString::Printf(TEXT(
		"insert into score (scorecode, official) values ('%s', '%d') "), 
		MySQL.TCHARFormatSQL(ScoreCode), official );

	FString Select = FString::Printf(TEXT(
		"select scid from score where scorecode='%s' and official='%d' "), 
		MySQL.TCHARFormatSQL(ScoreCode), official );

	INT scid = FindIDSafe( *Insert, *Select );
	return scid;
}

INT UProcessStatsCommandlet::ISwid( const TCHAR* DamageType, INT official )
{
	// TABLE WEAPON
	FString Insert = FString::Printf(TEXT(
		"insert into weapon(damagetypecode,official) values ('%s','%d') "),
		 MySQL.TCHARFormatSQL(DamageType), official );

	FString Select = FString::Printf(TEXT(
		"select wid from weapon where damagetypecode='%s' and official='%d' "),
		MySQL.TCHARFormatSQL(DamageType), official );

	INT wid = FindIDSafe( *Insert, *Select );
	return wid;
}

INT UProcessStatsCommandlet::ISmapsid( INT svid, INT mid, INT mapid )
{
	// TABLE MAPS
	INT mapsid=-1;
	if( mapid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into maps (svid, mid, mapid) values ('%d', '%d', '%d') "), svid, mid, mapid );

		FString Select = FString::Printf(TEXT(
			"select mapsid from maps where svid='%d' and mid='%d' and mapid='%d' "), svid, mid, mapid );

		mapsid = FindIDSafe( *Insert, *Select );
	}	
	return mapsid;
}

INT UProcessStatsCommandlet::ISweaponsid( INT pid, INT wid )
{
	// TABLE WEAPONS
	INT weaponsid=-1;
	if( wid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into weapons (pid, wid) values (%d, %d) "), pid, wid);

		FString Select = FString::Printf(TEXT(
			"select weaponsid from weapons where pid='%d' and wid='%d' "), pid, wid	);

		weaponsid = FindIDSafe( *Insert, *Select );
	}	
	return weaponsid;
}

INT UProcessStatsCommandlet::ISsuicidesid( INT pid, INT sid )
{
	// TABLE SUICIDES
	INT suicidesid=-1;
	if( sid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into suicides (pid, sid) values (%d, %d) "), pid, sid );

		FString Select = FString::Printf(TEXT(
			"select suicidesid from suicides where pid='%d' and sid='%d' "), pid, sid);

		suicidesid = FindIDSafe( *Insert, *Select );
	}
	return suicidesid;
}

INT UProcessStatsCommandlet::ISeventsid( INT pid, INT eid )
{
	// TABLE EVENTS
	INT eventsid=-1;
	if( eid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into events (pid, eid) values (%d, %d) "), pid, eid );

		FString Select = FString::Printf(TEXT(
			"select eventsid from events where pid='%d' and eid='%d' "), pid, eid);

		eventsid = FindIDSafe( *Insert, *Select );
	}
	return eventsid;
}

INT UProcessStatsCommandlet::IStscoresid( INT pid, INT tsid )
{
	// TABLE TEAMSCORES
	INT tscoresid=-1;
	if( tsid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into teamscores (pid, tsid) values (%d, %d) "), pid, tsid );

		FString Select = FString::Printf(TEXT(
			"select tscoresid from teamscores where pid='%d' and tsid='%d' "), pid, tsid);

		tscoresid = FindIDSafe( *Insert, *Select );
	}	
	return tscoresid;
}

INT UProcessStatsCommandlet::ISscoresid( INT pid, INT scid )
{
	// TABLE SCORES
	INT scoresid=-1;
	if( scid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into scores (pid, scid) values (%d, %d) "), pid, scid );

		FString Select = FString::Printf(TEXT(
			"select scoresid from scores where pid='%d' and scid='%d' "), pid, scid);

		scoresid = FindIDSafe( *Insert, *Select );
	}	
	return scoresid;
}

INT UProcessStatsCommandlet::ISmutatorsid( INT t, INT timeFrameNumber, INT mutid )
{
	// TABLE MUTATORS
	INT mutatorsid=-1;
	if( mutid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into mutators (timeFrame, timeFrameNumber, mutid) values (%d, %d, %d) "),
			t, timeFrameNumber, mutid );

		FString Select = FString::Printf(TEXT(
			"select mutatorsid from mutators where timeFrame='%d' and timeFrameNumber='%d' and mutid='%d' "),
			t, timeFrameNumber, mutid );

		mutatorsid = FindIDSafe( *Insert, *Select );
	}	
	return mutatorsid;
}

INT UProcessStatsCommandlet::ISmodsid( INT t, INT timeFrameNumber, INT mid )
{
	// TABLE MODS
	INT modsid=-1;
	if( mid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into mods (timeFrame, timeFrameNumber, mid) values (%d, %d, %d) "), t, timeFrameNumber, mid );

		FString Select = FString::Printf(TEXT(
			"select modsid from mods where timeFrame='%d' and timeFrameNumber='%d' and mid='%d' "),
			t, timeFrameNumber, mid );

		modsid = FindIDSafe( *Insert, *Select );
	}
	return modsid;
}

INT UProcessStatsCommandlet::ISgid( INT timeFrame, INT timeFrameNumber )
{
	// TABLE GLOBAL
	FString Insert = FString::Printf(TEXT(
		"insert into global_temp (timeFrame, timeFrameNumber) values (%d, %d) "), timeFrame, timeFrameNumber );

	FString Select = FString::Printf(TEXT(
		"select gid from global_temp where timeFrame='%d' and timeFrameNumber='%d' "), timeFrame, timeFrameNumber );

	INT gid = FindIDSafe( *Insert, *Select );
	return gid;
}

INT UProcessStatsCommandlet::ISmpid( INT matchid, INT playerid )
{
	// TABLE MATCHPLAYERS
	INT mpid=-1;
	if( playerid!=-1 )
	{
		// Checking PlayerIDLookup cached in RAM to find the mpid
		mpid = FindMPID( matchid, playerid );					// returns -1, if mpid not cached
		// INT mpid2=mpid;
		// GWarn->Logf(TEXT("Warning: Cached is mpid=%d "), mpid);

		// Find in cache fails - need to lookup the data in database
		if( mpid == -1 )
		{
			FString Insert = FString::Printf(TEXT(
				"insert into matchplayers (matchid,playerid) values ('%d','%d') "), 
				matchid, playerid );

			FString Select = FString::Printf(TEXT(
				"select mpid from matchplayers where matchid='%d' and playerid='%d' "), 
				matchid, playerid );

			mpid = FindIDSafe( *Insert, *Select );
			if( mpid != -1)
				SetMPID( matchid, playerid, mpid );
		}
		//if( mpid != mpid2 && mpid2!=-1  )
		//	GWarn->Logf(TEXT("Warning: Cached mpid=%d, differs from database mpid2=%d"), mpid2, mpid );
	}
	return mpid;
}

#if CAL_HK_PLAYERSSUM_ALT2
INT UProcessStatsCommandlet::ISpidHKwrap( INT playerid, INT timeFrame, INT timeFrameNumber, INT mid )
{
	guard(UProcessStatsCommandlet::ISpidHKwrap);
	// Wrapper for updating the HKpidLookup table: done only for Score and Event calls to ISpid()

	// Lookup the pid, be it in RAM or in the database (and update if required)
	INT pid = ISpid( playerid, timeFrame, timeFrameNumber, mid );

	// Use above pid and check if this is already used in RAM
	//	- If the pid is already in HKpidLookup, do nothing
	//	- Else look select or insert it into the pidstatus database, 
	//	  also update the HKpidLookup list in RAM.
	ISpidstatusid( pid );			// pidstatusid is not actually used.

	return pid;
	unguard;
}

INT UProcessStatsCommandlet::ISpidstatusid( INT pid )
{
	guard(UProcessStatsCommandlet::ISpidstatusid);
	// TABLE PIDSTATUS
	INT pidstatusid=-1;							// We actually don't care about the return value
	if( pid!=-1 )
	{
		// Checking HKpidLookup cached in RAM to find the pid, already exists == set
		INT* HKpidPtr = HKpidLookup.Find( pid );
		
		// Find in cache fails - need to lookup ot set it in the database
		if( !HKpidPtr ) 
		{
			FString Insert = FString::Printf(TEXT(
				"insert into pidstatus (pid) values (%d) "), pid );

			FString Select = FString::Printf(TEXT(
				"select pidstatusid from pidstatus where pid='%d' "), pid);

				pidstatusid = FindIDSafe( *Insert, *Select );
			if( pidstatusid != -1 )
				HKpidLookup.Set( pid, 1 );		// We do not actually care what this is mapped to!
		}
	}
	return pidstatusid;
	unguard;
}
#endif

INT UProcessStatsCommandlet::ISpid( INT playerid, INT timeFrame, INT timeFrameNumber, INT mid )
{
	// TABLE PLAYER
	INT pid=-1;
	if( playerid!=-1 && mid!=-1 )
	{
		// Checking PlayerIDLookup cached in RAM to find the pid
		pid = FindPID( playerid, timeFrame, timeFrameNumber, mid );			// returns -1, if pid not cached
		// INT pid2=pid;
		// GWarn->Logf(TEXT("Warning: Cached is pid=%d "), pid);

		// Find in cache fails - need to lookup the data in database
		if( pid == -1 )
		{
			FString Insert = FString::Printf(TEXT(
				"insert into player (playerid, timeFrame, timeFrameNumber, mid) values (%d, %d, %d, %d) "),
				playerid, timeFrame, timeFrameNumber, mid );

			FString Select = FString::Printf(TEXT(
				"select pid from player where playerid='%d' and timeFrame='%d' and timeFrameNumber='%d' and mid='%d' "),
				playerid, timeFrame, timeFrameNumber, mid );

			pid = FindIDSafe( *Insert, *Select );
			if( pid != -1 )
				SetPID( playerid, timeFrame, timeFrameNumber, mid, pid );
		}
		//if( pid != pid2 && pid2!=-1 )
		//	GWarn->Logf(TEXT("Warning: Cached pid=%d, differs from database pid2=%d"), pid2, pid );
	}
	return pid;
}

INT UProcessStatsCommandlet::ISpsumid( INT pid )
{
	// TABLE PLAYERS
	INT psumid=-1;
	if( pid!=-1 )
	{
		FString Insert = FString::Printf(TEXT(
			"insert into players (pid) values ('%d') "), pid );

		FString Select = FString::Printf(TEXT(
			"select psumid from players where pid='%d' "), pid );

		psumid = FindIDSafe( *Insert, *Select );
	}	
	return psumid;
}


/*-----------------------------------------------------------------------------
Helpers
-----------------------------------------------------------------------------*/

void UProcessStatsCommandlet::getDateOfReset( INT timeFrame, INT& year, INT& month, INT& day )
{
	guard(UProcessStatsCommandlet::getDateOfReset);
	// Getting the year, month, day from timeframe
	FString SelectQuery = FString::Printf(TEXT( 
		"select tfid from timeframe where timeframe='%d' order by currentTF desc limit 1"), timeFrame );
	INT tfid = FindID(*SelectQuery);
    if( tfid==-1 ) return;

	SelectQuery = FString::Printf(TEXT( 
		"select YEAR(dateOfReset), MONTH(dateOfReset), DAYOFMONTH(dateOfReset) from timeframe where tfid='%d' "), tfid );

	// DB lookup
	FQueryResult* Query = MySQL.Query( appToAnsi(*SelectQuery) );
	FQueryField** Row;
	if( (Row = Query->FetchNextRow()) != NULL )
	{
		year		= Row[0]->AsInt();
		month		= Row[1]->AsInt();
		day			= Row[2]->AsInt();
	}
	else
		GWarn->Logf(TEXT("Warning: MYSQL getDateOfReset() (%s) failed!"), *SelectQuery );

	delete Query;
	unguard;
}

void UProcessStatsCommandlet::getMYSQLtime( INT& Year, INT& Month, INT& DayOfWeek, INT& Day, INT& Hour, INT& Min, INT& Sec )
{
	guard(UProcessStatsCommandlet::getMYSQLtime);
	// Getting the MYSQL current date/time
	FString SelectQuery = FString::Printf(TEXT( 
		"select YEAR(NOW()), MONTH(NOW()), DAYOFWEEK(NOW()), DAYOFMONTH(NOW()), HOUR(NOW()), MINUTE(NOW()), SECOND(NOW()) ") );

	// DB lookup
	FQueryResult* Query = MySQL.Query( appToAnsi(*SelectQuery) );
	FQueryField** Row;
	if( (Row = Query->FetchNextRow()) != NULL )
	{
		Year		= Row[0]->AsInt();
		Month		= Row[1]->AsInt();
		DayOfWeek	= Row[2]->AsInt();	// 1 Sunday, 2 Monday, ... 7 Sunday
		Day			= Row[3]->AsInt();
		Hour		= Row[4]->AsInt();
		Min			= Row[5]->AsInt();
		Sec			= Row[6]->AsInt();
	}
	else
		GWarn->Logf(TEXT("Warning: MYSQL getMYSQLtime() (%s) failed!"), *SelectQuery );

	delete Query;
	unguard;
}

/*-----------------------------------------------------------------------------
Cache functions - buffer database tables (Match independant, "static data")
-----------------------------------------------------------------------------*/

INT UProcessStatsCommandlet::CacheAll( void )
{
	// Wrapper - cache all
	CacheTimeFrame();
	CacheELOFactors();
	CacheWeapon();
	CacheSuicide();
	CacheMutator();
	CacheMap();
	CacheMod();
	CacheScore();
	CacheTeamScore();
	CacheEvent();
	CachePID();			// playerid, TF, TFN, mid -> pid
	CacheMPID();		// matchid, playerid -> mpid
#if CAL_HK_PLAYERSSUM_ALT2
	CacheHKpid();		// New version indeeds needs to be run here!
#endif
	return 0;
}

//------------------------------------------------------------------
#if CAL_HK_PLAYERSSUM_ALT2
INT UProcessStatsCommandlet::CacheHKpid( void )
{
	// Cache all the seen pids, for housekeeping players summary lookup, later used to fast pid use tracking
	guard(UProcessStatsCommandlet::CacheHKpid);
	HKpidLookup.Empty();							// Reset is always correct, since database is updated live!

	INT flagFullUpdate=0;

	// First case check for full update
	// SPECIAL FLAGGING in ut2003stats use: insert into pidstatus (pid) values(-2);
	FString QueryText = FString::Printf(TEXT( "select count(pid) from pidstatus where pid=-2 "));
	INT count = FindID(*QueryText);
	if( count == 1 )
	{ 
		GWarn->Logf(TEXT("Info: CacheHKpid - case (%s)"), *QueryText );
		flagFullUpdate=1;				// Special first time flag set, do full update!	
	}

	// Second special case
	if( flagFullUpdate==0 )							// Only check for second case if first case fails!
	{
		QueryText = FString::Printf(TEXT( "select count(pid) from players "));
		count = FindID(*QueryText);
		if( count == 0 ) flagFullUpdate=1;			// Players table empty, do full update!
		//	GWarn->Logf(TEXT("Info: CacheHKpid - case (%s)"), *QueryText );
	}

	// A full update needs to be done!
	if( flagFullUpdate )
	{
		//	GWarn->Logf(TEXT("Info: CacheHKpid() - pistatus full update requested!"));
		MySQL.DoSQL( "delete from pidstatus " );		// Full reset/cleanup of table since we will update ALL		
		MySQL.DoSQL( "insert into pidstatus (pid) select pid from player" );	
	}

	// Caching data in a HKpidLookup TMAP in RAM 
	FQueryResult* Query = MySQL.Query( "select pid from pidstatus " );
	FQueryField** Row;
	while( (Row = Query->FetchNextRow()) != NULL )
		HKpidLookup.Set( Row[0]->AsInt(), 1 );		// Setting dummy value 1, since it's not used anywhere
	delete Query;
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::FindIDUpdateMulti( const TCHAR* SelectQuery, INT mode )
{
	guard(UProcessStatsCommandlet::FindIDUpdateMulti);
	FString RealUpdate;

	FQueryResult* IDQuery = MySQL.Query( appToAnsi(SelectQuery) );	//extra "%s", causes little or no spees penalty
	FQueryField** IDRow;
	while( (IDRow = IDQuery->FetchNextRow()) != NULL )
	{
		INT pid		= IDRow[0]->AsInt();

		if( mode==4 )
		{
			INT kills	= IDRow[1]->AsInt();
			INT deaths	= IDRow[2]->AsInt();
			RealUpdate	= FString::Printf(TEXT(
				"update players_temp set kills='%d',deaths='%d' where pid='%d' "), kills, deaths, pid );
		} 
		else if( mode==5 )
		{
			INT suicides	= IDRow[1]->AsInt();
			RealUpdate		= FString::Printf(TEXT(
				"update players_temp set suicides='%d' where pid='%d' "), suicides, pid );
		} 
		else if( mode==0 )	//** REMOVE below LATER
		{
			INT pkills	= IDRow[1]->AsInt();
			INT pdeaths	= IDRow[2]->AsInt();
			RealUpdate	= FString::Printf(TEXT(
				"update players_temp set pkills='%d',pdeaths='%d',kills='%d',deaths='%d' where pid='%d' "), 
					pkills, pdeaths, pkills, pdeaths, pid );
		} 
		else if( mode==1 )
		{
			INT akills	= IDRow[1]->AsInt();
			RealUpdate	= FString::Printf(TEXT(
				"update players_temp set akills='%d',kills=kills+'%d' where pid='%d' "), 
					akills, akills, pid );
		}
		else if( mode==2 )
		{
			INT wsuicides	= IDRow[1]->AsInt();
			RealUpdate		= FString::Printf(TEXT(
				"update players_temp set wsuicides='%d',suicides='%d' where pid='%d' "), 
					wsuicides, wsuicides, pid );
		}
		else if( mode==3 )
		{
			INT esuicides	= IDRow[1]->AsInt();
			RealUpdate		= FString::Printf(TEXT(
				"update players_temp set esuicides='%d',suicides=suicides+'%d' where pid='%d' "), 
					esuicides, esuicides, pid );
		}

		//GWarn->Logf(TEXT("Info: MYSQL (%s) (%s)"), SelectQuery, *RealUpdate );
		MySQL.DoSQL( appToAnsi(*RealUpdate) );
	}

	delete IDQuery;
	return 0;			// dummy not checked :)
	unguard;
}

INT UProcessStatsCommandlet::FindIDUpdateSum2( const TCHAR* SelectQuery )
{
	// Define select query and get the requested ID back
	guard(UUProcessStatsCommandlet::FindIDUpdateSum);
	FQueryResult* IDQuery = MySQL.Query( appToAnsi(SelectQuery) );
	FQueryField** IDRow;

	//"select players_temp.pid, kills, deaths, suicides from players_temp,pidstatus where players_temp.pid=pidstatus.pid"
	while( (IDRow = IDQuery->FetchNextRow()) != NULL )
	{
		INT pid				= IDRow[0]->AsInt();
		INT kills			= IDRow[1]->AsInt();
		INT deaths			= IDRow[2]->AsInt();
		INT suicides		= IDRow[3]->AsInt();
		INT frags			= kills - suicides;
		INT fraction		= kills + deaths + suicides;		//** + teamkills
		if( fraction == 0 ) fraction = 1;						//Safe devision
		FLOAT efficiency	= kills / (fraction*1.);

		// Update the data for this player...
		MySQL.DoSQL( "update players_temp set frags='%d', efficiency='%f' where pid='%d' ", frags, efficiency, pid );
	}

	delete IDQuery;
	return 0;			// dummy not checked :)
	unguard;
}

INT UProcessStatsCommandlet::FindIDUpdateWildcard( const TCHAR* SelectQuery, const TCHAR* Update )
{
	// Define select query and get the requested ID back
	guard(UProcessStatsCommandlet::FindIDUpdate);

	FQueryResult* IDQuery = MySQL.Query( "%s", appToAnsi(SelectQuery) );	//extra "%s", causes little or no spees penalty
	FQueryField** IDRow;

	while( (IDRow = IDQuery->FetchNextRow()) != NULL )
	{
		INT pid	= IDRow[0]->AsInt();
		INT sum	= IDRow[1]->AsInt();

		// Generate the update string and perform the update!
		FString RealUpdate = FString::Printf(TEXT("%s'%d' where pid='%d' "), Update, sum, pid );
		//GWarn->Logf(TEXT("Info: MYSQL (%s) (%s)"), SelectQuery, *RealUpdate );
		MySQL.DoSQL( appToAnsi(*RealUpdate) );
	}

	delete IDQuery;
	return 0;			// dummy not checked :)
	unguard;
}
#endif
//------------------------------------------------------------------

INT UProcessStatsCommandlet::CachePID( void )
{
	// Cache PlayerID, TF, TFN and mid for pid lookups
	guard(UProcessStatsCommandlet::CachePID);

	PlayerIDLookup.Empty();	// Make sure that on full re-cache the array is empty, should work for TMAP

	FQueryResult* Query = MySQL.Query( "select timeFrame, timeFrameNumber, mid, pid, playerid from player " );
	FQueryField** Row;
	while( (Row = Query->FetchNextRow()) != NULL )
	{
		INT timeFrame		= Row[0]->AsInt();
		INT timeFrameNumber	= Row[1]->AsInt();
		INT mid				= Row[2]->AsInt();
		INT pid				= Row[3]->AsInt();
		INT playerid		= Row[4]->AsInt();

		SetPID( playerid, timeFrame, timeFrameNumber, mid, pid );
	}
	delete Query;
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::CacheMPID( void )
{
	// Cache MatchID, and PlayerID for mpid lookups
	guard(UProcessStatsCommandlet::CacheMPID);

	MatchIDLookup.Empty();	// Make sure that on full re-cache the array is empty, should work for TMAP

	FQueryResult* Query = MySQL.Query( "select playerid, mpid, matchid from matchplayers " );
	FQueryField** Row;
	while( (Row = Query->FetchNextRow()) != NULL )
	{
		INT playerid	= Row[0]->AsInt();
		INT mpid		= Row[1]->AsInt();
		INT matchid		= Row[2]->AsInt();

		SetMPID( matchid, playerid, mpid );
	}
	delete Query;
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::CacheEvent( void )
{
	// Cache Event types
	guard(UProcessStatsCommandlet::CacheEvent);
	EventLookup.Empty();
	FQueryResult* Query = MySQL.Query( "select eid, eventcode from event " );
	FQueryField** Row;
	while( (Row = Query->FetchNextRow()) != NULL )
		EventLookup.Set( *Row[1]->AsString(), Row[0]->AsInt() );
	delete Query;
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::CacheTeamScore( void )
{
	// Cache TeamScore types
	guard(UProcessStatsCommandlet::CacheTeamScore);
	TeamScoreLookup.Empty();
	FQueryResult* Query = MySQL.Query( "select tsid, scorecode from teamscore " );
	FQueryField** Row;
	while( (Row = Query->FetchNextRow()) != NULL )
		TeamScoreLookup.Set( *Row[1]->AsString(), Row[0]->AsInt() );
	delete Query;
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::CacheScore( void )
{
	// Cache Score types
	guard(UProcessStatsCommandlet::CacheScore);
	ScoreLookup.Empty();
	FQueryResult* Query = MySQL.Query( "select scid, scorecode from score " );
	FQueryField** Row;
	while( (Row = Query->FetchNextRow()) != NULL )
		ScoreLookup.Set( *Row[1]->AsString(), Row[0]->AsInt() );
	delete Query;
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::CacheWeapon( void )
{
	// Cache weapon names
	guard(UProcessStatsCommandlet::CacheWeapon);
	WeaponLookup.Empty();				//Make sure that on full re-cache the array is empty
	FQueryResult* Query = MySQL.Query( "select wid, weaponcode, damagetypecode, mode from weapon " );
	FQueryField** Row;
	while( (Row = Query->FetchNextRow()) != NULL )
	{
		INT i = WeaponLookup.AddZeroed();
		WeaponLookup(i).wid				= Row[0]->AsInt();
		WeaponLookup(i).WeaponCode		= Row[1]->AsString();
		WeaponLookup(i).DamageTypeCode	= Row[2]->AsString();
		WeaponLookup(i).mode			= Row[3]->AsInt();
	}
	delete Query;
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::CacheSuicide( void )
{
	// Cache suicide damagetypes
	guard(UProcessStatsCommandlet::CacheSuicide);
	SuicideLookup.Empty();
	FQueryResult* Query = MySQL.Query( "select sid, suicidecode from suicide " );
	FQueryField** Row;
	while( (Row = Query->FetchNextRow()) != NULL )
		SuicideLookup.Set( *Row[1]->AsString(), Row[0]->AsInt() );
	delete Query;
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::CacheMutator( void )
{
	// Cache Mutators
	guard(UProcessStatsCommandlet::CacheMutator);
	MutatorLookup.Empty();
	FQueryResult* Query = MySQL.Query( "select mutid, mutatorcode from mutator " );
	FQueryField** Row;
	while( (Row = Query->FetchNextRow()) != NULL )
		MutatorLookup.Set( *Row[1]->AsString(), Row[0]->AsInt() );
	delete Query;
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::CacheMap( void )
{
	// Cache map names
	guard(UProcessStatsCommandlet::CacheMap);
	MapLookup.Empty();
	FQueryResult* Query = MySQL.Query( "select mapid, mapname from map " );
	FQueryField** Row;
	while( (Row = Query->FetchNextRow()) != NULL )
		MapLookup.Set( *Row[1]->AsString(), Row[0]->AsInt() );
	delete Query;
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::CacheMod( void )
{
	// Cache mod names
	guard(UProcessStatsCommandlet::CacheMod);
	ModLookup.Empty();
	// modecode is prefixed with "!" for gametype OTHER
	FQueryResult* Query = MySQL.Query( "select mid, modcode from mod " );	
	FQueryField** Row;
	while( (Row = Query->FetchNextRow()) != NULL )
		ModLookup.Set( *Row[1]->AsString(), Row[0]->AsInt() );
	delete Query;
	return 0;
	unguard;
}

INT UProcessStatsCommandlet::CacheTimeFrame( void )
{
	// Cache mod names
	guard(UProcessStatsCommandlet::CacheTimeFrame);
	TimeFrameLookup.Empty();

	// Getting current timeframe number for timeframe 0 = day, 1 = week, 2 = month, 3 = all
	// e.g. for the given timeframe the biggest = latest currentTF
	for( INT t=0; t<=3; t++ )
	{
		FQueryResult* Query = MySQL.Query(
			"select timeframe, currentTF from timeframe where timeframe='%d' order by currentTF desc limit 1 ", t );
		FQueryField** Row = Query->FetchNextRow();;

		if( Row )
		{
			TimeFrameLookup.Set( Row[0]->AsInt(), Row[1]->AsInt() );
			delete Query;
		}
		else
		{
			GWarn->Logf(TEXT("Warning: MYSQL - table timeframe table, missing timeFrame=%d, set currentTF=1."), t );
			delete Query;
			// MYSQL's current time - No rows were returned, inserting default info, currentTF is always at least 1.
			MySQL.DoSQL( "INSERT INTO timeframe(timeFrame,currentTF,dateOfReset,timeZone) VALUES ('%d',1,NOW(),'%d') ",t,0 );
			TimeFrameLookup.Set( t, 1 );
		}		// end row
	}			// end t

	return 0;
	unguard;
}

INT UProcessStatsCommandlet::CacheELOFactors( void )
{
	// Cache the k-dynamic factors for ELO
	guard(UProcessStatsCommandlet::CacheELOFactors);
	ELOFactorsLookup.Empty();
	ELODecreaseLookup.Empty();

	FString Query;
	FLOAT factor, decrease; 

	for( INT t=1; t<=3; t++ )
	{
		Query = FString::Printf(TEXT( "select kdynamicfactor from elo where timeFrame='%d' "), t );
		factor = FindIDF(*Query);
		Query = FString::Printf(TEXT( "select decrease from elo where timeFrame='%d' "), t );
		decrease = FindIDF(*Query);

		if( factor>0. && decrease>0. )	
		{
			ELOFactorsLookup.Set(  t, factor );
			ELODecreaseLookup.Set( t, decrease );
		}
		else	// Entries do not exits	- there are cases where below might fail, but I am "admin" :)
		{
			// No rows were returned, inserting default info
			GWarn->Logf(TEXT("Warning: MYSQL - elo table has no entry for timeFrame=%d, setting k=8., d=8."), t);
			MySQL.DoSQL( "INSERT INTO elo(timeFrame,kdynamicfactor,decrease) VALUES ('%d','8.','8.') ", t );
			ELOFactorsLookup.Set(  t, 8. );
			ELODecreaseLookup.Set( t, 8. );
		}
	}
	return 0;
	unguard;
}


/*-----------------------------------------------------------------------------
Alternate Method of Ranking - Ladder System from Chess
-----------------------------------------------------------------------------*/

FLOAT UProcessStatsCommandlet::RankELO(	FLOAT *ranki,  FLOAT *rankj,
								FLOAT scorei,  FLOAT scorej,
								FLOAT Kfactor, INT mode	)
{
	guard(UProcessStatsCommandlet::RankELO);

	//---Calculate ELO ranking---
	//   INPUT:		*ranki, *rankj: the player's last ranks (pointers!)
	//				scorei, scorej:	frag scores for players i,j
	//				Kfactor - Factor that determins how dynamic the ranking is, default is 8.
	//   RETURN:	*ranki, *rankj: the player's new ranks, negative values are assigned 0.
	FLOAT Wij, fij, fji, Gi, Gj, Eij, K, rankshift;

	//---Calc Ranks for the 2 Players---
	K = Kfactor;
	Gi = *ranki;  Gj = *rankj;							//old ranks of the players i and j
	fij = scorei; fji = scorej;							//kill scores

	if( scorei<0. ){ fij -= scorei; fji -= scorei; }	//11:-5 -> 16:0 score shift
	if( scorej<0. ){ fij -= scorej; fji -= scorej; }	//-5:-3 -> 3:5  score shift

	Wij= 1. / (1. + pow(10., (-(Gi-Gj)/400.)) );		//probability that player i sacks player j

	if( (INT)(fij + fji) == 0 ) Eij = 0.5;				//players played 0:0
	else Eij = fij / (fij + fji);						//fij + fji >0 always

	//---Remember new Ranks---
	//   Note: K * (Eij - Wij) == -(K * (Eji - Wji)) save some in calculation
	rankshift = K * (Eij - Wij);						//i's possible gain matches j's loss in rank!

	if( mode == 0 )	// standard kills
	{
		*ranki = Gi + rankshift;
		*rankj = Gj - rankshift;
	}
	if( mode == 1 )	// teamkills - no longed used for DM
	{
		*ranki = Gi - fabs(rankshift);					//punish team killer
		*rankj = Gj;									//team victim's rank does not change
	}
	if( mode == 2 )	// suicides
	{
		*ranki = Gi - fabs(rankshift);					//punish suicider, *rankj == *ranki
	}
	if( mode == 3 )	// rankteam scores, will not change incoming ranks at all.
	{
		*ranki = Gi;
		*rankj = Gj;
	}

	//---No Rank below Zero possible---
	if( *ranki<0. ) *ranki = 0.;
	if( *rankj<0. ) *rankj = 0.;

	return fabs(rankshift);
	unguard;
}

IMPLEMENT_CLASS(UProcessStatsCommandlet);

/*-----------------------------------------------------------------------------
The End.
-----------------------------------------------------------------------------*/

