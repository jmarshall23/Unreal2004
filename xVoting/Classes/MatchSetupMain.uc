//==============================================================================
//  Created on: 01/02/2004
//  Configure general match setup options
//
//  Written by Ron Prestenback
//  © 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class MatchSetupMain extends MatchSetupPanelBase;

var automated moComboBox co_GameType;
var automated moEditBox ed_Params, ed_DemoRec;
var automated moCheckbox ch_DemoRec, ch_Tournament;

var bool bDemoRec, bTournament;
var string GameClass, DemoFilename, Params;

function InitPanel()
{
	Super.InitPanel();

	Group = class'VotingReplicationInfo'.default.GeneralID;
}

function bool HandleResponse(string Type, string Info, string Data)
{
	local string InfoStr, SubType;

	if ( Type ~= Group )
	{
		log("MAIN HandleResponse Info '"$Info$"'  Data '"$Data$"'",'MapVoteDebug');
		if ( !Divide(Info, Chr(27), InfoStr, SubType) )
		{
			log("received unknown general token");
			return true;
		}

		if ( InfoStr ~= class'VotingReplicationInfo'.default.AddID )
		{
			switch ( SubType )
			{
			case class'VotingReplicationInfo'.default.TournamentID:
				bTournament = bool(Data);
				ch_Tournament.SetComponentValue(bTournament,True);
				break;

			case class'VotingReplicationInfo'.default.DemoRecID:
				bDemoRec = bool(Data);
				ch_DemoRec.SetComponentValue(bDemoRec,true);
				if ( bDemoRec )
					EnableComponent(ed_DemoRec);
				else DisableComponent(ed_DemoRec);
				break;

			case class'VotingReplicationInfo'.default.GameTypeID:
				GameClass = Data;
				if ( GameClass != "" )
				{
					co_GameType.MyComboBox.List.bNotify = false;
					co_GameType.Find(Data, ,true);
					co_Gametype.MyComboBox.List.bNotify = true;
				}
				break;

			case class'VotingReplicationInfo'.default.URLID:
				Params = Data;
				ed_Params.SetComponentValue(Params,true);
				break;
			}
		}

		else if ( InfoStr ~= class'VotingReplicationInfo'.default.UpdateID )
		{
			// this setting was changed by someone else
			switch ( SubType )
			{
			case class'VotingReplicationInfo'.default.TournamentID:
				bTournament = bool(Data);
				ch_Tournament.SetComponentValue(bTournament,True);
				break;

			case class'VotingReplicationInfo'.default.DemoRecID:
				bDemoRec = bool(Data);
				ch_DemoRec.SetComponentValue(bDemoRec,true);
				if ( bDemoRec )
					EnableComponent(ed_DemoRec);
				else DisableComponent(ed_DemoRec);
				break;

			case class'VotingReplicationInfo'.default.GameTypeID:
				GameClass = Data;
				if ( GameClass != "" )
				{
					co_GameType.MyComboBox.List.bNotify = false;
					co_GameType.Find(Data, ,true);
					co_Gametype.MyComboBox.List.bNotify = true;
				}
				break;

			case class'VotingReplicationInfo'.default.URLID:
				Params = Data;
				ed_Params.SetComponentValue(Params,true);
				break;
			}
		}

		return true;
	}

	return false;
}

function InternalOnChange( GUIComponent Sender )
{
	if ( Sender == ch_DemoRec )
	{
		if ( ch_DemoRec.IsChecked() )
			EnableComponent(ed_DemoRec);
		else DisableComponent(ed_DemoRec);
	}

	Super.InternalOnChange(Sender);
}

function SubmitChanges()
{
	if ( GameClass != co_GameType.GetExtra() )
		SendCommand( GetCommandString(co_GameType) );

	if ( bTournament != ch_Tournament.IsChecked() )
		SendCommand( GetCommandString(ch_Tournament) );

	if ( bDemoRec != ch_DemoRec.IsChecked() )
		SendCommand( GetCommandString(ch_DemoRec) );

	if ( Params != ed_Params.GetText() )
		SendCommand( GetCommandString(ed_Params) );

	Super.SubmitChanges();
}

function string GetCommandString( GUIComponent Comp )
{
	local string str;

	if ( Comp == None )
		return "";

	str = Group;

	switch ( Comp )
	{
	case co_Gametype:
		str $= ":" $ class'VotingReplicationInfo'.default.GametypeID $ ";" $ co_GameType.GetExtra();
		break;

	case ch_Tournament:
		str $= ":" $ class'VotingReplicationInfo'.default.TournamentID $ ";" $ ch_Tournament.IsChecked();
		break;

	case ch_DemoRec:
		str $= ":" $ class'VotingReplicationInfo'.default.DemoRecID;
		if ( ch_DemoRec.IsChecked() )
			str $= ";" $ ed_DemoRec.GetText();
		break;

	case ed_Params:
		str $= ":" $ class'VotingReplicationInfo'.default.URLID;
		if ( ed_Params.GetText() != "" )
			str $= ";" $ ed_Params.GetText();
		break;
	}

	return str;
}

DefaultProperties
{
	PanelCaption="General"

	Begin Object Class=moComboBox Name=GameTypeCombo
		Caption="Game Type"
		Hint="Select the gametype to use in the current match"
		WinWidth=0.622588
		WinHeight=0.1
		WinLeft=0.014282
		WinTop=0.132839
		TabOrder=0
	End Object
	co_GameType=GameTypeCombo

	Begin Object Class=moCheckbox Name=TournamentCheckbox
		Caption="Tournament Mode"
		Hint="All players must be connected to the server before the match can start"
		WinWidth=0.353296
		WinHeight=0.100000
		WinLeft=0.012272
		WinTop=0.295934
		TabOrder=1
	End Object
	ch_Tournament=TournamentCheckBox

	Begin Object Class=moEditBox Name=CommandLineParamsBox
		Caption="Additional Command Line Parameters"
		Hint="Specify any additional command line parameters (optional)"
		bVerticalLayout=True
		LabelJustification=TXTA_Center
		WinWidth=0.986084
		WinHeight=0.2
		WinLeft=0.008252
		WinTop=0.734349
		TabOrder=2
	End Object
	ed_Params=CommandLineParamsBox

	Begin Object Class=moCheckBox Name=DemoRecCheckbox
		Caption="Record Demo"
		Hint="Record a server-side demo of this match"
		WinWidth=0.353046
		WinHeight=0.100000
		WinLeft=0.011267
		WinTop=0.459699
		OnChange=InternalOnChange
		TabOrder=3
	End Object
	ch_DemoRec=DemoRecCheckBox

	Begin Object Class=moEditBox Name=DemoRecBox
		Caption="Filename"
		CaptionWidth=0.1
		Hint="Enter the name of the demo you'd like to record for this match"
		WinWidth=0.591943
		WinHeight=0.100000
		WinLeft=0.391845
		WinTop=0.457450
		MenuState=MSAT_Disabled
		TabOrder=4
	End Object
	ed_DemoRec=DemoRecBox

}
