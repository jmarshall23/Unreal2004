class MasterServerUplink extends MasterServerLink
    config
	native;

cpptext
{
	// AActor interface
	void Destroy();
	void PostScriptDestroyed();
	// AServerQueryLink interface
	UBOOL Poll( INT WaitTime );
}

enum EServerToMaster
{
	STM_ClientResponse,
	STM_GameState,
	STM_Stats,
	STM_ClientDisconnectFailed,
	STM_MD5Version,
	STM_CheckOptionReply,
};

enum EMasterToServer
{
	MTS_ClientChallenge,
	MTS_ClientAuthFailed,
	MTS_Shutdown,
	MTS_MatchID,
	MTS_MD5Update,
	MTS_UpdateOption,
	MTS_CheckOption,
    MTS_ClientMD5Fail,
    MTS_ClientBanned,
    MTS_ClientDupKey,
};

enum EHeartbeatType
{
	HB_QueryInterface,
	HB_GamePort,
	HB_GamespyQueryPort,
};

// MD5 data coming from the master server.
struct native export MD5UpdateData
{
	var string Guid;
	var string MD5;
	var INT Revision;
};

var GameInfo.ServerResponseLine ServerState;
var MasterServerGameStats GameStats;
var UdpLink	GamespyQueryLink;
var const int MatchID;
var float ReconnectTime;
var bool bReconnectPending;

// config
var globalconfig bool DoUplink;
var globalconfig bool UplinkToGamespy;
var globalconfig bool SendStats;
var globalconfig bool ServerBehindNAT;
var globalconfig bool DoLANBroadcast;

const MSUPROPNUM = 2;
var localized string MSUPropText[MSUPROPNUM];
var localized string MSUPropDesc[MSUPROPNUM];

// sorry, no code for you!
native function Reconnect();

event BeginPlay()
{
	local class<UdpLink> LinkClass;

	if( DoUplink )
	{
		// if we're uplinking to gamespy, also spawn the gamespy actors.
		if( UplinkToGamespy )
		{
			LinkClass = class<UdpLink>(DynamicLoadObject("IpDrv.UdpGamespyQuery", class'Class'));
			if ( LinkClass != None )
				GamespyQueryLink = Spawn( LinkClass );

			// FMasterServerUplink needs this for NAT.
			LinkClass = class<UdpLink>(DynamicLoadObject("IpDrv.UdpGamespyUplink", class'Class'));
			if ( LinkClass != None )
				Spawn( LinkClass );
		}

		// If we're sending stats,
		if( SendStats )
		{
			foreach AllActors(class'MasterServerGameStats', GameStats )
			{
				if( GameStats.Uplink == None )
					GameStats.Uplink = Self;
				else
					GameStats = None;
				break;
			}
			if( GameStats == None )
				Log("MasterServerUplink: MasterServerGameStats not found - stats uploading disabled.");
		}
	}

	Reconnect();
}

// Called when the connection to the master server fails or doesn't connect.
event ConnectionFailed( bool bShouldReconnect )
{
	Log("Master server connection failed");
	bReconnectPending = bShouldReconnect;
	ReconnectTime = 0;
}

// Called when we should refresh the game state
event Refresh()
{
	Level.Game.GetServerInfo(ServerState);
	Level.Game.GetServerDetails(ServerState);
	Level.Game.GetServerPlayers(ServerState);
}

// Call to log a stat line
native event bool LogStatLine( string StatLine );

// Handle disconnection.
simulated function Tick( float Delta )
{
	Super.Tick(Delta);
	ReconnectTime = ReconnectTime + Delta;
	if( bReconnectPending )
	{
		if( ReconnectTime > 10.0 )
		{
			Log("Attempting to reconnect to master server");
			bReconnectPending = False;
			Reconnect();
		}
	}
}

static function FillPlayInfo(PlayInfo PlayInfo)
{
	Super.FillPlayInfo(PlayInfo);

	PlayInfo.AddSetting(default.ServerGroup, "DoUplink", 	default.MSUPropText[0], 	255, 1, "Check",,,True);
}

static event string GetDescriptionText(string PropName)
{
	switch (PropName)
	{
		case "DoUplink":		return default.MSUPropDesc[0];
		case "SendStats":		return default.MSUPropDesc[1];
	}

	return Super.GetDescriptionText(PropName);
}

defaultproperties
{
	DoUplink=True
	UplinkToGamespy=True
	SendStats=True
	MatchID=0

	MSUPropText(0)="Advertise Server"
	MSUPropText(1)="Process Stats"
	MSUPropDesc(0)="if true, your server is advertised on the internet server browser."
	MSUPropDesc(1)="Publishes player stats from your server on the UT2004 stats website."
}
