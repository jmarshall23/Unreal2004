class MasterServerLink extends Info
	native
	transient;

cpptext
{
	virtual UBOOL Poll( INT WaitTime ) { return 0; }
}

struct native tMasterServerEntry
{
    var string 	Address;
    var int		Port;
};

var native const pointer LinkPtr;
var globalconfig int LANPort;
var globalconfig int LANServerPort;

var globalconfig array<tMasterServerEntry> MasterServerList;

native function bool Poll( int WaitTime );

// Cheap and easy load balancing coming up here.

event GetMasterServer( out string OutAddress, out int OutPort )
{
	local int Index;
	Index      = rand(MasterServerList.Length);
	OutAddress = MasterServerList[Index].Address;
	OutPort    = MasterServerList[Index].Port;
}

simulated function Tick( float Delta )
{
	Poll(0);
}

defaultproperties
{
	bAlwaysTick=True
	LANPort=11777
	LANServerPort=10777
	MasterServerList(0)=(Address="ut2004master1.epicgames.com",Port=28902)
	MasterServerList(1)=(Address="ut2004master2.epicgames.com",Port=28902)

}
