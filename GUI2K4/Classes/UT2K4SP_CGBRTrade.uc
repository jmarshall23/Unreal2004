//==============================================================================
// Single Player Challenge Bloodrite Trade page
// Message that you won a player and have to assign it to your team
//
// Written by Michiel Hendriks
// (c) 2003, 2004, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4SP_CGBRTrade extends UT2K4SP_CGBRUnTrade;

var automated GUICharacterListTeam clSelChar;
var automated GUIButton btnNextChar, btnPrevChar;

/** Warn the player that there's no empty spot */
var localized string FullTeamWarning;
/** Info about the player being fires */
var localized string FireInfo;
/** true if the player has afull team */
var bool bHasFullTeam;

var UT2K4GameProfile GP;

event InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	lblTitle.WinWidth=0.762501;
	lblTitle.WinHeight=0.058750;
	lblTitle.WinLeft=0.024063;
	lblTitle.WinTop=0.067846;

	imgPictureBg.WinWidth=0.121667;
	imgPictureBg.WinHeight=0.326250;
	imgPictureBg.WinLeft=0.044722;
	imgPictureBg.WinTop=0.149198;

	imgPicture.WinWidth=0.116667;
	imgPicture.WinHeight=0.312500;
	imgPicture.WinLeft=0.048472;
	imgPicture.WinTop=0.164198;

	btnOk.WinWidth=0.222222;
	btnOk.WinHeight=0.050694;
	btnOk.WinLeft=0.360070;
	btnOk.WinTop=0.821064;

	sbMessage.WinWidth=0.458332;
	sbMessage.WinHeight=0.306111;
	sbMessage.WinLeft=0.213542;
	sbMessage.WinTop=0.182976;

    super.InitComponent(MyController, MyOwner);
    GP = UT2K4GameProfile(PlayerOwner().Level.Game.CurrentGameProfile);
    bHasFullTeam = GP.HasFullTeam();
    clSelChar.PopulateList(GP.PlayerTeam);
	clSelChar.Index = 0;
    if (!bHasFullTeam)
	{
		btnNextChar.DisableMe();
		btnPrevChar.DisableMe();
		clSelChar.DisableMe();
	}
}

function SetDescription()
{
	local string tmp;
	if (bHasFullTeam)
	{
		tmp = Message$"||"$FullTeamWarning$"||"$FireInfo;

    	tmp = repl(tmp, "%player%", TargetPlayerName);
		tmp = repl(tmp, "%enemyteam%", ETI.default.TeamName);

		tmp = repl(tmp, "%fireplayer%", clSelChar.GetName());
		tmp = repl(tmp, "%refund%", GP.MoneyToString(GP.GetBotPrice(clSelChar.GetName())));

		sbMessage.SetContent(tmp);
	}
	else super.SetDescription();
}

function UpdateDetails(GUIComponent Sender)
{
	SetDescription();
}

function bool onSelectChar(GUIComponent Sender)
{
	if ( Sender == btnPrevChar )
	{
		clSelChar.ScrollLeft();
	}
	else if ( Sender == btnNextChar )
	{
		clSelChar.ScrollRight();
	}
	return true;
}

function OnClose(optional bool bCancel)
{
	if (bHasFullTeam) GP.ReleaseTeammate(clSelChar.GetName());
	GP.AddTeammate(TargetPlayerName);
}

DefaultProperties
{
	Begin Object Class=GUIButton Name=CGBbtnPrevChar
		Hint="Select the team mate you want to fire"
		StyleName="ArrowLeft"
		WinWidth=0.048750
		WinHeight=0.061250
		WinLeft=0.805001
		WinTop=0.806667
		bNeverFocus=true
		bRepeatClick=true
		OnClickSound=CS_Down
		OnClick=onSelectChar
		bBoundToParent=true
	End Object
	btnPrevChar=CGBbtnPrevChar

	Begin Object Class=GUIButton Name=CGBbtnNextChar
		Hint="Select the team mate you want to fire"
		StyleName="ArrowRight"
		WinWidth=0.048750
		WinHeight=0.061250
		WinLeft=0.898438
		WinTop=0.806667
		bNeverFocus=true
		bRepeatClick=true
		OnClickSound=CS_Up
		OnClick=onSelectChar
		bBoundToParent=true
	End Object
	btnNextChar=CGBbtnNextChar

	Begin Object class=GUICharacterListTeam Name=CGBclSelChar
		Hint="You will fire this team mate"
		WinWidth=0.124694
		WinHeight=0.325000
		WinLeft=0.807213
		WinTop=0.149584
		bCenterInBounds=true
		FixedItemsPerPage=1
		StyleName="CharButton"
		TabOrder=0
		OnChange=UpdateDetails
		bBoundToParent=true
	End Object
	clSelChar=CGBclSelChar

	DefaultWidth=0.8
	DefaultHeight=0.5
	DefaultLeft=0.1
	DefaultTop=0.25

	WinWidth=0.8
	WinHeight=0.5
	WinLeft=0.1
	WinTop=0.25

    Message="You have won the challenge against %enemyteam%. %player% will now become part of your team."
	FullTeamWarning="However your team is already at capacity. You will have to fire one of your team mates first. You will be refunded for the loss of the selected player."
	FireInfo="You have selected to fire %fireplayer%, the refund will be %refund%."
}
