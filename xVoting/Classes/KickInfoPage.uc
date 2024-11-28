//====================================================================
//  xVoting.KickInfoPage
//  Player Information Page.
//
//  Written by Bruce Bickar
//  (c) 2003, Epic Games, Inc.  All Rights Reserved
// ====================================================================

class KickInfoPage extends LargeWindow;

var automated GUIButton        b_ReturnButton;
var automated GUIImage         i_PlayerPortrait;
var automated GUILabel         l_PlayerName;
var automated PlayerInfoMultiColumnListBox lb_PlayerInfoBox;

var localized string PlayerText, PingText, ScoreText, IDText, IPText, KillsText,
                     DeathsText, SuicidesText, MultiKillsText, SpreesText;
//------------------------------------------------------------------------------------------------
function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.Initcomponent(MyController, MyOwner);
	b_ReturnButton.OnClick=ReturnButtonOnClick;
}
//------------------------------------------------------------------------------------------------
function HandleParameters(string Param1, string Param2)
{
    LoadInfo(Param1);
}
//------------------------------------------------------------------------------------------------
function LoadInfo(string PlayerName)
{
	local int i, MultiKills, Sprees;
	local Material Portrait;
	local PlayerReplicationInfo PRI;

    if(PlayerName == "")
        return;

    if (!Controller.bCurMenuInitialized)
        return;

	for( i=0; i<PlayerOwner().GameReplicationInfo.PRIArray.Length; i++ )
	{
		if( PlayerOwner().GameReplicationInfo.PRIArray[i].PlayerName == PlayerName )
		{
			PRI = PlayerOwner().GameReplicationInfo.PRIArray[i];
			break;
		}
	}

    Portrait = PRI.GetPortrait();
    if(Portrait == None)
        Portrait = Material(DynamicLoadObject("PlayerPictures.cDefault", class'Material'));

    i_PlayerPortrait.Image = Portrait;
    l_PlayerName.Caption = PlayerName;

    lb_PlayerInfoBox.Add(PingText,string(PRI.Ping));
	lb_PlayerInfoBox.Add(ScoreText,string(PRI.Score));
	lb_PlayerInfoBox.Add(KillsText,string(PRI.Kills));
	lb_PlayerInfoBox.Add(DeathsText,string(PRI.Deaths));

	if( TeamPlayerReplicationInfo(PRI) != none )
	{
		lb_PlayerInfoBox.Add(SuicidesText,string(TeamPlayerReplicationInfo(PRI).Suicides));
		for (i = 0; i < 7; i++)
			MultiKills += TeamPlayerReplicationInfo(PRI).MultiKills[i];
		lb_PlayerInfoBox.Add(MultiKillsText,string(MultiKills));
		for (i = 0; i < 6; i++)
			Sprees += TeamPlayerReplicationInfo(PRI).Spree[i];
		lb_PlayerInfoBox.Add(SpreesText,string(Sprees));
	}
}
//------------------------------------------------------------------------------------------------
function bool ReturnButtonOnClick(GUIComponent Sender)
{
	Controller.CloseMenu(true);
	return true;
}
//------------------------------------------------------------------------------------------------
defaultproperties
{
    Begin Object class=GUIImage Name=KickImagePlayerPortrait
		WinWidth=0.155814
		WinHeight=0.358525
		WinLeft=0.206924
		WinTop=0.193199
		Image=Material'2K4Menus.Controls.buttonThick_b'
        ImageColor=(R=255,G=255,B=255,A=255);
        ImageRenderStyle=MSTY_Normal
        ImageStyle=ISTY_Scaled
    End Object
    i_PlayerPortrait = KickImagePlayerPortrait

	Begin Object class=GUIButton Name=ExitButton
		Caption="Close"
		StyleName="SquareButton"
		WinWidth=0.120000
		WinHeight=0.033203
		WinLeft=0.670934
		WinTop=0.531692
		TabOrder=2
		RenderWeight=1.0
	End Object
	b_ReturnButton = ExitButton

    Begin Object Class=PlayerInfoMultiColumnListBox Name=PlayerInfoBoxControl
		WinWidth=0.422477
		WinHeight=0.299483
		WinLeft=0.366960
		WinTop=0.234286
        bVisibleWhenEmpty=True
        bVisible=True
        StyleName="ServerBrowserGrid"
    End Object
    lb_PlayerInfoBox = PlayerInfoBoxControl

    Begin Object class=GUILabel Name=PlayerNameLabel
        Caption="PlayerName"
        TextAlign=TXTA_Center
        TextFont="UT2SmallHeaderFont"
        TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.425371
		WinHeight=0.038297
		WinLeft=0.365679
		WinTop=0.195429
        RenderWeight=0.3
    End Object
    l_PlayerName=PlayerNameLabel

	Background=None

	bAcceptsInput=false
    bRequire640x480=false
    bAllowedAsLast=true
	bRenderWorld=true
	bMoveAllowed=False
	//bPersistent=True

	WinWidth=0.622502
	WinHeight=0.440703
	WinLeft=0.188743
	WinTop=0.151276

    PingText="Ping"
    ScoreText="Score"
    IDText="Player ID"
    IPText="IP Address"
	KillsText="Kills"
    DeathsText="Deaths"
	SuicidesText="Suicides"
	MultiKillsText="MultiKills"
	SpreesText="Sprees"
}

