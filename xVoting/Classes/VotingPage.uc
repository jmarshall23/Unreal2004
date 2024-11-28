//====================================================================
//  xVoting.VotingPage
//  Voting page is the base for MapVoting and KickVoting pages.
//
//  Written by Bruce Bickar
//  (c) 2003, Epic Games, Inc.  All Rights Reserved
// ====================================================================

class VotingPage extends LargeWindow DependsOn(VotingHandler);

var automated MapVoteFooter f_Chat;
var() editconst noexport VotingReplicationInfo MVRI;

//------------------------------------------------------------------------------------------------
function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local PlayerController PC;

	Super.Initcomponent(MyController, MyOwner);

	PC = PlayerOwner();
	MVRI = VotingReplicationInfo(PC.VoteReplicationInfo);

	// Turn pause off if currently paused (stops replication)
	if(PlayerOwner() != None && PlayerOwner().Level.Pauser != None)
		PlayerOwner().SetPause(false);
}
//------------------------------------------------------------------------------------------------
function Free()
{
	MVRI = None;
    Super.Free();
}
//------------------------------------------------------------------------------------------------
defaultproperties
{
	Background=None

	bAcceptsInput=false
	bPauseIfPossible=false

    WinLeft=0.1
    WinTop=0.1
    WinWidth=0.8
    WinHeight=0.8

	Begin Object Class=MapVoteFooter Name=MatchSetupFooter
		WinWidth=0.962109
		WinHeight=0.291406
		WinLeft=0.019921
		WinTop=0.686457
		TabOrder=10
		RenderWeight=0.5
		bBoundToParent=True
		bScaleToParent=True
	End Object
	f_Chat=MatchSetupFooter

	bRenderWorld=true
    bRequire640x480=false
    bAllowedAsLast=true
	bMoveAllowed=False
	//bPersistent=True     -- Note: this causes problems because free is called when closed
}

