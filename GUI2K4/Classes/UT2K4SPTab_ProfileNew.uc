//==============================================================================
// Create a new single player profile
//
// Written by Michiel Hendriks
// (c) 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4SPTab_ProfileNew extends UT2K4SPTab_Base;

var automated GUISectionBackground sbEditBg, sbPortraitBack, sbSponsorBack;
var automated GUIButton btnPrevSkin, btnNextSkin, btnPrevSponsor, btnNextSponsor;
var automated moEditBox edName, edTeam;
var automated moComboBox cbDifficulty;
var automated GUICharacterListTeam clPlayerSkins;
var automated GUIImageList ilSponsor;

var	localized string DefaultTeamName;
var string DefaultCharacter, TeamSponsorPrefix;
var int DefaultDifficulty, DefaultTeamSponsor;

/** more agents to add to the free agent pool */
var array<string> InitialFreeAgents;

/**
	The URL to visit after a profile has been created, the profile name will be appended to the end
*/
var string GameIntroURL;

var localized string ErrorProfileExists, ErrorCantCreateProfile;
var() localized string    DifficultyLevels[8];

function InitComponent(GUIController pMyController, GUIComponent MyOwner)
{
	local int i;
	Super.Initcomponent(pMyController, MyOwner);
	edName.MyEditBox.MaxWidth=16;  // as per polge, check UT2K4Tab_PlayerSettings if you change this
	edName.MyEditBox.AllowedCharSet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_";

	clPlayerSkins.InitListInclusive("SP");

	for(i = 0; i < 8; i++)
		cbDifficulty.AddItem(DifficultyLevels[i]);

	LoadSponsorSymbols();
	ResetValues();
}

function ShowPanel(bool bShow)
{
	Super.ShowPanel(bShow);
	if (bShow)
	{
		MainWindow.btnPlay.Caption = CaptionCreate;
		MainWindow.btnBack.Caption = CaptionCancel;

		btnPlayEnabled(true);
	}
	else {
		MainWindow.btnPlay.Caption = CaptionPlay;
		MainWindow.btnBack.Caption = CaptionBack;
	}
}

/**
	Create the profile and load it
*/
function bool onPlayClick()
{
	local array<string> ExistingProfiles;
	local string ProfileName;
	local int i;
	local GUIQuestionPage QPage;

	ProfileName = MainWindow.ProfilePrefix$edName.GetText();
	// test for existing record
	Controller.GetProfileList(MainWindow.ProfilePrefix, ExistingProfiles);
	for (i = 0; i < ExistingProfiles.length; i++)
	{
		if (ExistingProfiles[i] ~= ProfileName)
		{
			if (Controller.OpenMenu(Controller.QuestionMenuClass))
			{
				QPage = GUIQuestionPage(Controller.ActivePage);
				QPage.SetupQuestion(QPage.Replace(ErrorProfileExists, "profile", edName.GetText()), QBTN_Ok, QBTN_Ok); // no prefix
			}
			SetFocus(edName);
			return true;
		}
	}
	GP = PlayerOwner().Level.Game.CreateDataObject(ProfileClass, "GameProfile", ProfileName);
	if ( GP != none )
	{
		GP.CreateProfile(PlayerOwner());
		GP.PlayerName = edName.GetText();
		GP.TeamName = edTeam.GetText();
		GP.TeamSymbolName = String(ilSponsor.Image);
		GP.PlayerCharacter = clPlayerSkins.GetName();
		AddDefaultBots(GP);
		AddDefaultTeams(GP);
		GP.BaseDifficulty = cbDifficulty.GetIndex();
		// set difficulty variables
		GP.InjuryChance = GP.InjuryChance+(GP.BaseDifficulty/30);
		GP.ChallengeChance = GP.ChallengeChance+(GP.BaseDifficulty/30);
		GP.FeeIncrease = GP.FeeIncrease+(GP.BaseDifficulty/300);
		GP.TeamPercentage = GP.TeamPercentage+(GP.BaseDifficulty/30);
		GP.MatchBonus = GP.MatchBonus+(GP.TeamPercentage/10);
		GP.InjuryTreatment = GP.InjuryTreatment+(GP.BaseDifficulty / 50);
		GP.MapChallengeCost = GP.MapChallengeCost+(GP.BaseDifficulty / 100);
		GP.AltPath = rand(MaxInt);
		PlayerOwner().Level.Game.CurrentGameProfile = GP;
		GP.Initialize(PlayerOwner().Level.Game, ProfileName);
		GP.SpecialEvent = "WELCOME";
		PlayerOwner().Level.Game.CurrentGameProfile.bInLadderGame = true;  // so it'll reload into SP menus
		if (!PlayerOwner().Level.Game.SavePackage(ProfileName))
		{
			Warn("Couldn't save profile package");
		}

		// launch the game introduction
		PlayerOwner().ConsoleCommand("START"@GameIntroUrl$profilename);
		if (MainWindow != none) MainWindow.bAllowedAsLast = true; // workaround, or else the main window will pop up
		Controller.CloseAll(false,true);
	}
	else
	{
		Warn("Error creating profile");
		if (Controller.OpenMenu(Controller.QuestionMenuClass))
		{
			QPage = GUIQuestionPage(Controller.ActivePage);
			QPage.SetupQuestion(ErrorCantCreateProfile, QBTN_Ok, QBTN_Ok);
		}
		// restore old tab
		MainWindow.c_Tabs.ReplaceTab(MyButton, MainWindow.PanelCaption[0], MainWindow.PanelClass[0], , MainWindow.PanelHint[0], true);
		MainWindow.UpdateTabs();
		return true;
	}

	return true;
}

/**
	Go back to the main menu when there are not proflies
*/
function bool onBackClick()
{
	local array<string> profilenames;
	MainWindow.btnBack.Caption = CaptionBack;
	Controller.GetProfileList(MainWindow.ProfilePrefix, profilenames);
	if ( profilenames.Length == 0 )	Controller.CloseMenu();
	else MainWindow.c_Tabs.ReplaceTab(MyButton, MainWindow.PanelCaption[0], MainWindow.PanelClass[0], , MainWindow.PanelHint[0], true);
	return true;
}

/**
	Clear the input box when it has the default name
*/
function OnNameActivate()
{
	if (edName.GetText() == clPlayerSkins.GetName()) edName.SetText("");
}

/**
	Clear the input box when it has the default team name
*/
function OnTeamActivate()
{
	if (edTeam.GetText() == DefaultTeamName) edTeam.SetText("");
}

/**
	Check to see if we have an empty name
*/
function OnNameDeActivate()
{
	if ( edName.GetText() == "") edName.SetText(repl(clPlayerSkins.GetName(), ".", "_"));
	if ( edTeam.GetText() == "") edTeam.SetText(DefaultTeamName);
}

/**
	Disable "create" button when names are empty
*/
function onNameChange( GUIComponent Sender)
{
	btnPlayEnabled((edName.GetText() != "") && (edTeam.GetText() != ""));
}

/**
	Change the selected skin
*/
function bool onSelectSkin(GUIComponent Sender)
{
	local string prev;
	prev = repl(clPlayerSkins.GetName(), ".", "_");
	if ( Sender == btnPrevSkin )
	{
		clPlayerSkins.ScrollLeft();
	}
	else if ( Sender == btnNextSkin )
	{
		clPlayerSkins.ScrollRight();
	}
	if (edName.GetText() == prev) edName.SetText(repl(clPlayerSkins.GetName(), ".", "_"));
	return Super.OnClick(Sender);
}

/**
	Change the selected sponsor
*/
function bool onSelectSponsor(GUIComponent Sender)
{
	if ( Sender == btnPrevSponsor )
	{
		ilSponsor.PrevImage();
	}
	else if ( Sender == btnNextSponsor )
	{
		ilSponsor.NextImage();
	}
	return Super.OnClick(Sender);
}

/**
	Will reset all the settings to their initial value
*/
function ResetValues()
{
	local int i;
	edTeam.SetText(DefaultTeamName);
	cbDifficulty.SetIndex(DefaultDifficulty);
	clPlayerSkins.Find(DefaultCharacter);
	edName.SetText(clPlayerSkins.GetName());
	for (i = 0; i < ilSponsor.MatNames.Length; i++)
	{
		if (ilSponsor.MatNames[i] ~= "TeamSymbols_UT2004.Design1")
		{
			ilSponsor.SetIndex(i);
			break;
		}
	}
}

/**
	Add the default bots to the free agent list
*/
function AddDefaultBots(UT2K4GameProfile GP)
{
	local array<xUtil.PlayerRecord> FreeAgents;
	local int i;

	GP.BotStats.length = 0;
	class'xUtil'.static.GetPlayerList(FreeAgents);
	for (i = 0; i < FreeAgents.Length; i++)
	{
		if ((FreeAgents[i].Menu ~= "SP") &&
				((FreeAgents[i].species == class'xGame.SPECIES_Egypt') || (FreeAgents[i].species == class'xGame.SPECIES_Merc')))
		{
			if (FreeAgents[i].DefaultName ~= GP.PlayerCharacter) continue;
			GP.GetBotPosition(FreeAgents[i].DefaultName, true);
		}
	}
	for (i = 0; i < InitialFreeAgents.length; i++)
	{
		GP.GetBotPosition(InitialFreeAgents[i], true);
	}
}

/**
	Add the default teams to the list (the Phantom teams)
*/
function AddDefaultTeams(UT2K4GameProfile GP)
{
	local class<UT2K4RosterGroup> RGclass;
	local int i;

	RGclass = class<UT2K4RosterGroup>(DynamicLoadObject("xGame.UT2K4TeamRosterPhantom", class'Class'));
	for (i = 0; i < RGclass.default.Rosters.length; i++)
	{
		GP.GetTeamPosition(RGclass.default.Rosters[i], true);
	}
}

/**
	Loads all Sponsor symbols
*/
function LoadSponsorSymbols()
{
	local int i;
	local Material M;
	local array<string> SymbolNames;

	Controller.GetTeamSymbolList(SymbolNames, true);  // get all usable team symbols

	for (i = 0; i < SymbolNames.Length; i++)
	{
		M = Material(DynamicLoadObject(SymbolNames[i], class'Material'));
		ilSponsor.AddMaterial(SymbolNames[i], M);
	}
}


defaultproperties
{
	Begin Object class=GUICharacterListTeam Name=SPNclPlayerSkins
		Hint="Your character's appearance, use arrow keys to change"
		WinWidth=0.199566
		WinHeight=0.500000
		WinLeft=0.048207
		WinTop=0.191921
		bCenterInBounds=true
		FixedItemsPerPage=1
		StyleName="CharButton"
		TabOrder=0
		bBoundToParent=true
	End Object
	clPlayerSkins=SPNclPlayerSkins

	Begin Object class=GUISectionBackground Name=SPNimgEditBg
		WinWidth=0.425171
		WinHeight=0.770000
		WinLeft=0.536263
		WinTop=0.101463
		Caption="Profile"
		bBoundToParent=true
    End Object
    sbEditBg=SPNimgEditBg

	Begin Object class=moEditBox Name=SPNmeName
		Caption="Player Name: "
		Hint="Your character's name"
		LabelJustification=TXTA_Left
		WinHeight=0.122500
		WinWidth=0.345000
		WinLeft=0.572258
		WinTop=0.286087
		bVerticalLayout=true
		OnChange=onNameChange
		OnActivate=OnNameActivate
		OnDeActivate=OnNameDeActivate
		TabOrder=1
		bBoundToParent=true
	End Object
	edName=SPNmeName

	Begin Object class=moEditBox Name=SPNmeTeam
		Caption="Team Name: "
		Hint="The name of your team"
		LabelJustification=TXTA_Left
		WinHeight=0.122500
		WinWidth=0.345000
		WinLeft=0.572258
		WinTop=0.428007
		bVerticalLayout=true
		OnChange=onNameChange
		OnActivate=OnTeamActivate
		OnDeActivate=OnNameDeActivate
		TabOrder=2
		bBoundToParent=true
	End Object
	edTeam=SPNmeTeam

	Begin Object class=moComboBox Name=SPNmcDifficulty
		Caption="Difficulty: "
		Hint="Customize your challenge"
		LabelJustification=TXTA_Left
		WinWidth=0.345000
		WinHeight=0.068311
		WinLeft=0.572258
		WinTop=0.568803
		bVerticalLayout=true
		bReadOnly=true
		TabOrder=3
		bBoundToParent=true
	End Object
	cbDifficulty=SPNmcDifficulty

	Begin Object class=GUIImageList Name=SPNilSponsor
		Hint="Your team's sponsor, use arrow keys to change"
		WinWidth=0.185561
		WinHeight=0.506378
		WinLeft=0.304401
		WinTop=0.189296
		ImageRenderStyle=MSTY_Normal
		ImageStyle=ISTY_Justified
		ImageAlign=IMGA_Center
		bWrap=true
		TabOrder=7
		bBoundToParent=true
	End Object
	ilSponsor=SPNilSponsor

	Begin Object class=GUISectionBackground Name=SPNimgPortraitBack
		WinWidth=0.234133
		WinHeight=0.770000
		WinLeft=0.030428
		WinTop=0.101463
		Caption="Portrait"
		bBoundToParent=true
    End Object
    sbPortraitBack=SPNimgPortraitBack

	Begin Object Class=GUIButton Name=SPNbtnPrevSkin
		Hint="Selects a new appearance for your character"
		StyleName="ArrowLeft"
		WinWidth=0.048750
		WinHeight=0.080000
		WinLeft=0.060867
		WinTop=0.733722
		bNeverFocus=true
		bRepeatClick=true
		OnClickSound=CS_Down
		OnClick=onSelectSkin
		TabOrder=5
		bBoundToParent=true
	End Object
	btnPrevSkin=SPNbtnPrevSkin

	Begin Object Class=GUIButton Name=SPNbtnNextSkin
		Hint="Selects a new appearance for your character"
		StyleName="ArrowRight"
		WinWidth=0.048750
		WinHeight=0.080000
		WinLeft=0.178054
		WinTop=0.733722
		bNeverFocus=true
		bRepeatClick=true
		OnClickSound=CS_Up
		OnClick=onSelectSkin
		TabOrder=6
		bBoundToParent=true
	End Object
	btnNextSkin=SPNbtnNextSkin

	Begin Object class=GUISectionBackground Name=SPNimgSponsorBack
		WinWidth=0.253265
		WinHeight=0.770000
		WinLeft=0.273865
		WinTop=0.101463
		Caption="Team symbol"
		bBoundToParent=true
    End Object
    sbSponsorBack=SPNimgSponsorBack

	Begin Object Class=GUIButton Name=SPNbtnPrevSponsor
		Hint="Selects a new symbol for your team"
		StyleName="ArrowLeft"
		WinWidth=0.048750
		WinHeight=0.080000
		WinLeft=0.311505
		WinTop=0.733722
		bNeverFocus=true
		bRepeatClick=true
		OnClickSound=CS_Down
		OnClick=onSelectSponsor
		TabOrder=8
		bBoundToParent=true
	End Object
	btnPrevSponsor=SPNbtnPrevSponsor

	Begin Object Class=GUIButton Name=SPNbtnNextSponsor
		Hint="Selects a new symbol for your team"
		StyleName="ArrowRight"
		WinWidth=0.055000
		WinHeight=0.080000
		WinLeft=0.433380
		WinTop=0.733722
		bNeverFocus=true
		bRepeatClick=true
		OnClickSound=CS_Up
		OnClick=onSelectSponsor
		TabOrder=9
		bBoundToParent=true
	End Object
	btnNextSponsor=SPNbtnNextSponsor

	PanelCaption="New profile"

	DefaultTeamName="Team"
	DefaultTeamSponsor=1
	DefaultDifficulty=2
	TeamSponsorPrefix="TeamSymbols_UT2003.sym"
	DefaultCharacter="Jakob"
	ErrorProfileExists="Profile with name '%profile%' already exists!"
	ErrorCantCreateProfile="Profile creation failed."
	GameIntroURL="MOV-UT2004-Intro?quickstart=true?TeamScreen=False?savegame="

    DifficultyLevels(0)="Novice"
    DifficultyLevels(1)="Average"
    DifficultyLevels(2)="Experienced"
    DifficultyLevels(3)="Skilled"
    DifficultyLevels(4)="Adept"
    DifficultyLevels(5)="Masterful"
    DifficultyLevels(6)="Inhuman"
    DifficultyLevels(7)="Godlike"

	InitialFreeAgents(0)="Hathor"
	InitialFreeAgents(1)="Huntress"
	InitialFreeAgents(2)="Sunspear"
	InitialFreeAgents(3)="Cipher"
	InitialFreeAgents(4)="Medusa"
	InitialFreeAgents(5)="Jackhammer"
	InitialFreeAgents(6)="Avarice"
	InitialFreeAgents(7)="Darkling"
}
