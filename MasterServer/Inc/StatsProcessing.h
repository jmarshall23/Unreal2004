/*=============================================================================
StatsProcessing.h: Stats processing
Copyright 1997-2002 Epic Games, Inc. All Rights Reserved.

Revision history:
* Created by Jack Porter (Framework) and Christoph A. Loewe (Meat)
=============================================================================*/

/*-----------------------------------------------------------------------------
Source Control
-----------------------------------------------------------------------------*/

// Live vs. local stats... (probably redundant now, always live!)
#define CAL_NOTLOCAL_MYSQL_TESTING	1	// 1 Live Stats  0 Local Housekeeping testing
#define CAL_LIVECODE				1	// 1 Live Stats  0 Local testing (wait times, no delete of log lines, 30K+ lines not 10K)

#define CAL_HK_TEST					0	// 1 directly call playersummary() and globalsummary() code, 0 back to normal code

// Performance Stats
#define CAL_CALC_LOGLINE_TIMES		0	// 1 Testing of times for log lines,  0 Testing removed

// Optimization
#define CAL_MATCHPLAYERS_SUMMING	1	// 1 matchplayers counters are not updated in real time, at EG!,  0 Testing removed

// new code...
#define CAL_HK_PLAYERSSUM_ALT2		1	// 1 housekeeping alternate code for players summary code,  0 original code

// Housekeeping - CalcGlobalSummary / CalcPlayersSummary
#define	USE_HKG_GAMESPERDAY			0	// 0 do not sum, 1 
#define	USE_HKPl_APKILLS_PDEATHS	0	// 0 only sums kills, 1 differ a/pkills again!
#define USE_HKPl_WESUICIDES			0	// 0 only sums suicides, 1 differ w/esuicides again!
#define USE_HKPl_FRAGS_EFFICIENCY	0	// 0 remove, 1 calculate (only useful if kills and wesuicides on!

#define USE_HKPl_MKs_SPREES			0	// (Multikills/Sprees)
#define USE_HKPl_TOTALSCORE			1	//**ON for now
#define USE_HKPl_TEAMSCORE			1
#define USE_HKPl_EVENTS				1	//**ON for now	

// Feature reduction
#define USE_MATCHSCORING_TABLE		0	// 0 no matchscoring updates, 1 turns this on		
#define USE_MATCHTEAMSCORING_TABLE	0	// 0 no matchscoring updates, 1 turns this on		
#define USE_MATCHRANKTEAM_TABLE		0	// 0 no RANKTEAM in matchplayers calculated, 1 turns this on		
#define USE_MATCHRANKELO_TABLE		0	// 0 no RANKELO in matchplayers calculated, 1 turns this on		
#define USE_VICTORS_TABLE			0	// 0 no VICTORS table usage, 1 turns this on		
#define USE_MATCHVICTORS_TABLE		0	// 0 no MATCHVICTORS table usage, 1 turns this on		


/*-----------------------------------------------------------------------------
Structs
-----------------------------------------------------------------------------*/

struct FStatsLine
{
	INT MatchID;
	INT ServerID;
	FString Line;
};

struct FWeaponInfo
{
	INT wid;
	FString WeaponCode;
	FString DamageTypeCode;
	INT	mode;
};

struct FPlayerData
{
	INT ConnectTime;		// In seconds relative to current Match DateTime	
	INT playerid;			// playerid associated to PlayerNumber
	INT teamid;				// If player joined team, which one is he on.
	FLOAT rankelo[4];		//** Number of timeFrames! - caching ranks in RAM
	FLOAT rankteam[4];		//** Number of timeFrames!, 
							// [1-3] Player's rank as a teamplayer, per current TF
							// [0] remembers the players rank during this MATCH.
};

struct FMatchData
{
	DOUBLE	lastUsed;		// remember when this MatchID was last use, used to free RAM in timeout situation.

	FString DateTime;		// match started at this date/time, format: 2002.6.12@17:37:56
	INT		TimeZone;		// the game server's timezone, offset to above LT to GMT
	FString MapName;		// needed by FindMapID for DB updates
	FString MapTitle;
	FString MapAuthor;
	FString GameClass;
	FString GameName;
	FString Mutators;
	FString GameRules;

	INT mid;				// this mod's id (gametype)
	INT mapid;				// this map's id
	INT svid;				// the svid for this match...		//**use it?

	UBOOL teamgame;			// flag (0,1) that says, if thie gametype is a teamgame or not, used by rankteam code.
	UBOOL pure;				// if server is running mutators with a known official gametype, then its unpure 0, pure 1

	FLOAT teamscore[4];		//** Number of teams!, using 4 just to be sure :), using [0-1]
	FLOAT timeFrameNumber[4]; //** Number of timeFrames!, using [1-3], remembering THIS match's TFN, set once then fixed!

	TMap<INT, INT>			mutidLookup;		// sets a flag if a mutid is in this map or not (1).
	TMap<INT, FPlayerData>	PlayerLookup;		// converts a PlayerNumber into a FPlayerData struct (where the playerid is).
};

struct FPlayerIDInfo
{
	INT timeFrame;
	INT timeFrameNumber;
	INT mid;
	INT pid;
};

struct FPlayerIDData
{
	TArray<FPlayerIDInfo>	PlayerIDInfoLookup;	// timeFrame, timeFrameNumber, mid -> pid
};

struct FMatchIDInfo
{
	INT playerid;
	INT mpid;
};

struct FMatchIDData
{
	TArray<FMatchIDInfo>	MatchIDInfoLookup;	// playerid -> mpid
};


/*-----------------------------------------------------------------------------
UProcessStatsCommandlet - Stats processing commandlet.
-----------------------------------------------------------------------------*/

class UProcessStatsCommandlet : public UCommandlet
{
	DECLARE_CLASS(UProcessStatsCommandlet,UCommandlet,CLASS_Transient,MasterServer);

	FMySQL MySQL;								// connection to the database

	// Remember what MatchIDs the Master Server has been dealing with, needed for Housekeeping
	TArray<INT> MatchIDs;

	INT THIS_MS_DOES_HOUSEKEEPING;				//!! FIXME Jack	- Determines what server does Housekeeping DB updates	
	INT CURRENT_SERVERID;						//!! FIXME Jack - The Serverid for this Match

	INT MAX_NUM_MATCHES_PER_SERVER;				// Maximum number of matches tracked per server at one time.
	DOUBLE LAST_HOUSEKEEPING_TIME;				// Last time we did a Housekeeping
	INT TIME_BETWEEN_HOUSEKEEPINGS;				// Time between each Housekeeping, e.g. 600 = 10 Minutes
	INT TIME_MATCHID_TIMEOUT;					// When a MatchID times out, 600 = 10 Minutes


#if CAL_CALC_LOGLINE_TIMES
	// TMaps used by CAL_CALC_LOGLINE_TIMES 
	TMap<FString, DOUBLE>		LOGLINE_TIMELookup;	 // Logline type, e.g. "SI" to a sum of times...
	TMap<FString, INT>			LOGLINE_COUNTLookup; // Logline type, e.g. "SI" to a counter how often the type was seen...
#endif

	// Debug code to help find problems in log files
	INT DEBUG_LOGLINENUMBER;

	// Cached data
	TArray<FWeaponInfo>			WeaponLookup;		// list of FWeaponInfos
	TMap<FString, INT>			SuicideLookup;		// converts a suicidecode to a sid
	TMap<FString, INT>			ScoreLookup;		// converts a scorecode to a scid
	TMap<FString, INT>			TeamScoreLookup;	// converts a teamscorecode to a tsid
	TMap<FString, INT>			EventLookup;		// converts a eventcode to a eid
	TMap<FString, INT>			ModLookup;			// converts a modcode to a mid
	TMap<FString, INT>			MapLookup;			// converts a mapname to a mapid
	TMap<FString, INT>			MutatorLookup;		// converts a mutatorcode to a mutid
	TMap<INT, INT>				TimeFrameLookup;	// converts a timeframe number to currentTF number
	TMap<INT, FLOAT>			ELOFactorsLookup;	// converts a timeframe number to ELO k dynamic factor
	TMap<INT, FLOAT>			ELODecreaseLookup;	// converts a timeframe number to ELO decrease value
	
	TMap<INT, FMatchData>		MatchLookup;		// converts a MatchID to an FMatchData.
	TMap<INT, FPlayerIDData>	PlayerIDLookup;		// converts a PlayerID to an struct FPlayerIDData.
	TMap<INT, FMatchIDData>		MatchIDLookup;		// converts a MatchID to an struct FMatchIDData.

#if CAL_HK_PLAYERSSUM_ALT2
	TMap<INT, INT>				HKpidLookup;		// converts a pid to flag, if set flag is 1
#endif

	// UCommandlet interface
	void StaticConstructor();
	INT Main( const TCHAR* Parms );

	// Called from external code, not from this thread.
	void InsertNewStatsLine( FMySQL* CDKeyMySQL, FString ServerAddr, INT InMatchID, INT InServerID, INT RemoteVersion, const TCHAR* InLine );

private:
#if CAL_CALC_LOGLINE_TIMES
	void SumLogLineTimes( FString& LogType, DOUBLE time );
	void DumpLogLineSummary( INT LineCount, DOUBLE overalltime );
#endif

	// Housekeeping
	void StatsHousekeeping(		void );
	void CalcGlobalSummary(		void );
	void CalcPlayersSummary(	void );
	void CheckRemoveMatchIDs(	void );
	void NewTFNdayWeekMonth(	void );
	void ReduceEveryOnesRank(	void );
	void RemoveOldMatches(		void );

	// Ranking
	FLOAT	RankELO(			FLOAT *ranki, FLOAT *rankj, FLOAT scorei, FLOAT scorej, FLOAT Kfactor, INT mode );
	FLOAT	CalcAvOppRank(		INT MatchID, INT teamid, INT timeFrame, INT *playercount );
	void	UpdateAvOppRanks(	INT MatchID, INT teamid, INT timeFrame, FLOAT diffrank, INT playercount, FLOAT Score );

	// Helpers
	void getMYSQLtime(			INT& Year, INT& Month, INT& DayOfWeek, INT& Day, INT& Hour, INT& Min, INT& Sec );
	void getDateOfReset(		INT timeFrame, INT& year, INT& month, INT& day );

	// Stats processing
	void ProcessStatsLine(		INT ServerID, INT MatchID, FString& StatsLine );

	void ProcessKill(			INT MatchID, INT KillerNumber, const TCHAR* DamageType, INT VictimNumber, const TCHAR* VictimWeapon, UBOOL TeamKill );
	void ProcessKillNormal(		INT MatchID, INT KillerNumber, const TCHAR* DamageType, INT VictimNumber, const TCHAR* VictimWeapon, UBOOL TeamKill );
	void ProcessKillSuicide(	INT MatchID, INT VictimNumber, const TCHAR* DamageType );

	void ProcessServer(			INT MatchID, INT ServerID, INT Seconds, const TCHAR* ServerName, INT ServerRegion, const TCHAR* AdminName, const TCHAR* AdminEmail, const TCHAR* ipPort, const TCHAR* GameRules );
	void ProcessStartGame(		INT MatchID, INT Seconds );
	void ProcessNewGame(		INT MatchID, const TCHAR* DateTime, INT TimeZone, const TCHAR* MapName,	const TCHAR* MapTitle,	const TCHAR* MapAuthor,	const TCHAR* GameClass, const TCHAR* GameName,	const TCHAR* Mutators );
	void ProcessEndGameTime(	INT MatchID, INT Seconds );
	void ProcessEndGamePlayers( INT MatchID, TArray<FString>& LineParts, INT ColumnNumber );

	void ProcessConnect(		INT MatchID, INT Seconds, INT PlayerNumber, const TCHAR* StatsUsername, const TCHAR* StatsPassword );
	void ProcessDisconnect(		INT MatchID, INT Seconds, INT PlayerNumber );

	void ProcessMutators(		INT MatchID, const TCHAR* Mutators );
	void ProcessMod(			INT MatchID, INT mid );

	void ProcessScore(			INT MatchID, INT PlayerID, FLOAT Score, const TCHAR* ScoreCode, INT Seconds );
	void ProcessTeamScore(		INT MatchID, INT TeamID, FLOAT Score, const TCHAR* TeamScoreCode, INT Seconds );

	void ProcessGSpecial(		INT MatchID, const TCHAR* EventCode, INT PlayerNumber, INT TeamFlagID );
	void ProcessTeamChange(		INT MatchID, INT PlayerNumber, INT TeamID );
	void ProcessNameChange(		INT MatchID, INT PlayerNumber, const TCHAR* NickName );

	void ProcessRankTeam(		INT MatchID, INT PlayerNumber, FLOAT Score, INT timeFrame );
	void MatchUpdateMutatorID(	INT MatchID, INT pid );
	void ProcessDumpRankTeam(	void );
	void ProcessDumpRankELO(	void );
	void ProcessDumpMPSum(		void );
	FLOAT ProcessDumpRankTeamOne(INT MatchID, INT PlayerNumber, INT pid, INT t );
	FLOAT ProcessDumpRankELOOne(INT MatchID, INT PlayerNumber, INT pid, INT t );
	void ProcessDumpMPSumOne( INT mpid );

	// Common Insert / Select wrappers
	INT ISmpid(			INT MatchID, INT PlayerID );
	INT ISmmutid(		INT MatchID, INT mutid );
	INT ISmatchesid(	INT MatchID );
	INT ISmwid(			INT mpid, INT wid );
	INT ISmsid(			INT mpid, INT sid );
	INT ISmscid(		INT mpid, INT scid );
	INT ISmtsid(		INT MatchID, INT TeamID, INT tsid );
	INT ISmeid(			INT mpid, INT eid );
	INT ISmscoringid(	INT mpid, INT second );
	INT ISmtscoringid(	INT MatchID, INT TeamID, INT second );
	INT ISmvictorid(	INT mpid, INT mvid );

	INT ISgid(			INT t, INT timeFrameNumber );
	INT ISpid(			INT playerid, INT t, INT timeFrameNumber, INT mid );
	INT ISmodsid(		INT t, INT timeFrameNumber, INT mid );
	INT ISmutatorsid(	INT t, INT timeFrameNumber, INT mutid );
	INT ISsuicidesid(	INT pid, INT sid );
	INT ISscoresid(		INT pid, INT scid );
	INT ISeventsid(		INT pid, INT eid );
	INT IStscoresid(	INT pid, INT tsid );
	INT ISsvid(			INT serverid );
	INT ISserversid(	INT svid, INT t, INT timeFrameNumber );
	INT ISvictorid(		INT pid, INT vid );

	INT ISpmutatorsid(	INT pid, INT mutid );
	INT ISpmodsid(		INT pid, INT mid );
	INT ISpmapsid(		INT pid, INT mapid );

	INT ISpsumid(		INT pid );
	INT ISweaponsid(	INT pid, INT wid );
	INT ISwid(			const TCHAR* DamageType, INT official );
	INT ISsid(			const TCHAR* SuicideCode, INT official );
	INT ISscid(			const TCHAR* ScoreCode, INT official );
	INT IStsid(			const TCHAR* TeamScoreCode, INT official );
	INT ISeid(			const TCHAR* EventCode, INT official );
	INT ISmapid(		const TCHAR* MapName, const TCHAR* MapTitle, const TCHAR* MapAuthor, INT official );
	INT ISmutid(		const TCHAR* MutCode, INT official );
	INT ISplayerid(		const TCHAR* StatsUsername, const TCHAR* StatsPassword );
	INT ISmapsid(		INT svid, INT mid, INT mapid );
	INT IStfid(			INT timeFrame, INT currentTF );
#if CAL_HK_PLAYERSSUM_ALT2	
	INT ISpidstatusid(	INT pid );
	INT ISpidHKwrap( INT playerid, INT timeFrame, INT timeFrameNumber, INT mid );
#endif

	// Finding data
	FLOAT	FindELOFactors(		INT timeFrame );
	FLOAT	FindELODecrease(	INT timeFrame );

	INT		FindIDUpdate(			const TCHAR* SelectQuery, const TCHAR* Update );
	INT		FindIDQUpdate(			const TCHAR* SelectQuery, const TCHAR* Update );
	INT		FindIDUpdateSum(		const TCHAR* SelectQuery );
#if CAL_HK_PLAYERSSUM_ALT2
	INT		FindIDUpdateMulti(		const TCHAR* SelectQuery, INT mode );
	INT		FindIDUpdateSum2(		const TCHAR* SelectQuery );
	INT		FindIDUpdateWildcard(	const TCHAR* SelectQuery, const TCHAR* Update );
#endif

	INT		FindID(					const TCHAR* SelectQuery );	// Returns INT, direct calles, shows Warning.
	INT		FindIDWildcard(			const TCHAR* SelectQuery );	// Special wildcard version
	INT		FindIDI(				const TCHAR* SelectQuery );	// Version of FindID, without warning.
	INT		FindIDMulti(			const TCHAR* QueryText, INT* values, INT columns );	// Return mutiple columns...
	SQWORD	FindIDQMulti(			const TCHAR* SelectQuery, SQWORD* values, INT columns );
	FLOAT	FindIDF(				const TCHAR* SelectQuery );
    FLOAT	FindIDFnowarning(		const TCHAR* SelectQuery );	// Version of FindIDF, without warning.
	SQWORD	FindIDQ(				const TCHAR* SelectQuery );
	INT		FindIDSafe(				const TCHAR* InsertQuery, const TCHAR* SelectQuery );
	INT		FindTimeFrameNumber(	INT timeFrame );
	INT		FindWeaponWid(			const TCHAR* WeaponCode, const TCHAR* DamageType );
	INT		FindSuicideID(			const TCHAR* SuicideCode );
	INT		FindScoreID(			const TCHAR* ScoreCode );
	INT		FindTeamScoreID(		const TCHAR* TeamScoreCode );
	INT		FindEventID(			const TCHAR* EventCode );
	INT		FindMapID(				INT MatchID, const TCHAR* MapName );
	INT		FindModID(				const TCHAR* ModName );
	INT		FindMutID(				const TCHAR* MutCode );
	INT		FindPID(				INT playerid, INT timeFrame, INT timeFrameNumber, INT mid );
	INT		FindMPID(				INT matchid, INT playerid  );


	// Getting data from somewhere, might not be db
	INT		GetTimeFrameNumber(	INT MatchID, INT timeFrame );
	FLOAT	GetTeamScore(		INT MatchID, INT TeamID );
	FLOAT	GetRankTeam(		INT MatchID, INT PlayerNumber, INT timeFrame );
	FLOAT	GetRankELO(			INT MatchID, INT PlayerNumber, INT timeFrame );
	INT		GetConnectTime(		INT MatchID, INT PlayerNumber );
	UBOOL	GetTeamGame(		INT MatchID );
	INT		GetPlayerID(		INT MatchID, INT PlayerNumber );
	INT		GetPlayerIDnoWarn(	INT MatchID, INT PlayerNumber );	// used by EndGame processing only
	INT		GetTeamID(			INT MatchID, INT PlayerNumber );
	INT		GetMID(				INT MatchID );
	INT		GetMapID(			INT MatchID );
	const TCHAR* GetDateTime(	INT MatchID );
	INT		GetTimeZone(		INT MatchID );
	INT		GetSVID(			INT MatchID );
	DOUBLE	GetLastUsed(		INT MatchID );

	// Remembering data for the current MatchID
	INT SetMatchPlayerID(		INT MatchID, INT PlayerNumber, INT playerid );
	INT SetMatchMapID(			INT MatchID, INT mapid );
	INT SetMatchMutatorID(		INT MatchID, INT mutid );
	INT SetMatchTeamID(			INT MatchID, INT PlayerNumber, INT TeamID );
	INT SetMatchRankTeam(		INT MatchID, INT PlayerNumber, FLOAT newRankTeam, INT timeFrame );
	INT SetMatchRankELO(		INT MatchID, INT PlayerNumber, FLOAT newRankELO, INT timeFrame );
	INT SetMatchConnectTime(	INT MatchID, INT PlayerNumber, INT Seconds );
	INT SetMatchTeamScore(		INT MatchID, INT TeamID, FLOAT Score );
	INT SetMatchSVID(			INT MatchID, INT svid );
	INT SetMatchLastUsed(		INT MatchID );	
	INT SetMatchTimeFrameNumber(INT MatchID, INT timeFrame, INT timeFrameNumber );
	INT SetPID(					INT playerid, INT timeFrame, INT timeFrameNumber, INT mid, INT pid );
	INT	SetMPID(				INT matchid, INT playerid, INT mpid );

	// MatchID struct housekeeping
	FMatchData* SetMatchInitGet( INT MatchID );
	void SetMatchRemove( INT MatchID );

	// Caching data from database tables
	INT CacheAll( void );
	INT CacheTimeFrame( void );
	INT CacheELOFactors( void );
	INT CacheWeapon( void );
	INT CacheSuicide( void );
	INT CacheScore( void );
	INT CacheTeamScore( void );
	INT CacheEvent( void );
	INT CacheMap( void );
	INT CacheMod( void );
	INT CacheMutator( void );
	INT CachePID( void );
	INT CacheMPID( void );
#if CAL_HK_PLAYERSSUM_ALT2
	INT CacheHKpid( void );
#endif

};


/*-----------------------------------------------------------------------------
The End.
-----------------------------------------------------------------------------*/
