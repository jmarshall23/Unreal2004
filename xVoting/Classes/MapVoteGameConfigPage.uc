// ====================================================================
//  Class:  xVoting.MapVoteGameConfigPage
//
//	this page allows modification of the xVotingHandler GameConfig
//  configuration variables.
//
//  Written by Bruce Bickar
//  (c) 2003, Epic Games, Inc.  All Rights Reserved
// ====================================================================
class MapVoteGameConfigPage extends GUICustomPropertyPage DependsOn(VotingHandler);

var automated GUISectionBackground sb_List, sb_List2,sb_3;
var automated GUIListBox lb_GameConfigList;
var automated moComboBox co_GameClass;
var automated moEditBox  ed_GameTitle;
var automated moEditBox  ed_Acronym;
var automated moEditBox  ed_Prefix;
var automated MultiSelectListBox lb_Mutator;
var automated moEditBox  ed_Parameter;
var automated GUIButton  b_New;
var automated GUIButton  b_Delete;
var automated moCheckBox ch_Default;

var array<CacheManager.GameRecord> GameTypes;
var array<CacheManager.MutatorRecord> Mutators;

// autosave
var int SaveIndex;
var bool bChanged;

// localization
var localized string lmsgNew;
var localized string lmsgAdd;

//------------------------------------------------------------------------------------------------
function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;
	//local array<string> CaptionList;

	Super.Initcomponent(MyController, MyOwner);

//	for ( i = 0; i < Controls.Length; i++ )
//	{
//		Controls[i].bBoundToParent = True;
//		Controls[i].bScaleToParent = True;
//	}

	// find the largest caption and set the SizingCaptions
//	CaptionList.Length = 3;
//	CaptionList[0]=b_New.Caption;
//	CaptionList[1]=b_Delete.Caption;
//	CaptionList[2]=b_Return.Caption;
//	for( i=1; i<CaptionList.Length; i++ )
//		if( len(CaptionList[i]) > len(CaptionList[x]) )
//			x = i;
//	b_New.SizingCaption = CaptionList[x];
//	b_Delete.SizingCaption = CaptionList[x];
//	b_Return.SizingCaption = CaptionList[x];

	// load existing configuration
	for(i=0; i<class'xVoting.xVotingHandler'.default.GameConfig.Length; i++)
		lb_GameConfigList.List.Add( class'xVoting.xVotingHandler'.default.GameConfig[i].GameName, none, string(i));

	// load game types
	class'CacheManager'.static.GetGameTypeList(GameTypes);
	for(i=0; i<GameTypes.Length; i++)
		co_GameClass.AddItem( GameTypes[i].GameName, none, GameTypes[i].ClassName);

	class'CacheManager'.static.GetMutatorList(Mutators);
	LoadMutators();

	sb_Main.SetPosition(0.484336,0.061979,0.451250,0.624218);
	lb_GameConfigList.List.AddLinkObject(co_GameClass);
	lb_GameConfigList.List.AddLinkObject(ed_GameTitle);
	lb_GameConfigList.List.AddLinkObject(ed_Acronym);
	lb_GameConfigList.List.AddLinkObject(ed_Prefix);
	lb_GameConfigList.List.AddLinkObject(ed_Parameter);
	lb_GameConfigList.List.AddLinkObject(lb_Mutator);
	lb_GameConfigList.List.AddLinkObject(ch_Default);
	lb_GameConfigList.List.AddLinkObject(b_Delete);

	lb_GameConfigList.OnChange=GameConfigList_Changed;
	bChanged = False;

	sb_Main.TopPadding=0.0.5;
	sb_Main.BottomPadding=0.4;

	sb_List.ManageComponent(lb_GameConfigList);

	sb_Main.ManageComponent(co_GameClass);
	sb_Main.ManageComponent(ed_GameTitle);
	sb_Main.ManageComponent(ed_Acronym);
	sb_Main.ManageComponent(ed_Prefix);

	sb_List2.ManageComponent(lb_Mutator);

	b_Cancel.SetVisibility(false);

}
/*
function bool MutListPreDraw(Canvas C)
{
 	sb_List2.WinTop = (sb_Main.WinTop + sb_Main.WinHeight) - SB_List2.WinHeight;
 	sb_List2.WinLeft = sb_Main.WinLeft;
 	sb_List2.WinWidth = sb_Main.WinWidth;
 	return false;
}
*/
//------------------------------------------------------------------------------------------------
function InternalOnOpen()
{
	lb_GameConfigList.List.SetIndex(0);
}
//------------------------------------------------------------------------------------------------
function LoadMutators()
{
	local int i;

	lb_Mutator.List.Clear();
	for(i=0; i<Mutators.Length; i++)
		lb_Mutator.List.Add( Mutators[i].FriendlyName, none, Mutators[i].ClassName);
}
//------------------------------------------------------------------------------------------------
function GameConfigList_Changed(GUIComponent Sender)
{
	local int s,i;
	local array<string> MutatorArray;

	if( bChanged )
    	SaveChange();

	s = int(lb_GameConfigList.List.GetExtra());
	SaveIndex = s;
	co_GameClass.Find(class'xVoting.xVotingHandler'.default.GameConfig[s].GameClass, true, True);
	ed_GameTitle.SetComponentValue(class'xVoting.xVotingHandler'.default.GameConfig[s].GameName, True);
	ed_Acronym.SetComponentValue(class'xVoting.xVotingHandler'.default.GameConfig[s].Acronym, True);
	ed_Prefix.SetComponentValue(class'xVoting.xVotingHandler'.default.GameConfig[s].Prefix, True);
	ed_Parameter.SetComponentValue(class'xVoting.xVotingHandler'.default.GameConfig[s].Options, True);
	ch_Default.SetComponentValue(string(class'xVoting.xVotingHandler'.default.DefaultGameConfig == s), True);

	LoadMutators();

	Split(class'xVoting.xVotingHandler'.default.GameConfig[s].Mutators, ",", MutatorArray);
	for(i=0; i<MutatorArray.Length; i++)
		lb_Mutator.List.Find(MutatorArray[i],False,True); // bExtra

	bChanged = False;
	EnableComponent(b_New);
}
//------------------------------------------------------------------------------------------------
function FieldChange(GUIComponent Sender)
{
	local string GameClass;
	local int i;

	bChanged=True;

	if(Sender == co_GameClass)
	{
		GameClass = co_GameClass.GetExtra();
		for(i=0; i<GameTypes.Length; i++)
			if(GameTypes[i].ClassName == GameClass)
				break;
		ed_GameTitle.SetComponentValue(GameTypes[i].GameName, True);
		ed_Acronym.SetComponentValue(GameTypes[i].GameAcronym, True);
		ed_Prefix.SetComponentValue(GameTypes[i].MapPrefix, True);
	}
}
//------------------------------------------------------------------------------------------------
function bool NewButtonClick(GUIComponent Sender)
{
	if( bChanged )
    	SaveChange();

	if( b_New.Caption == lmsgAdd )
		b_New.Caption = lmsgNew;
	else
	{
		b_New.Caption = lmsgAdd;
		lb_GameConfigList.List.SetIndex(-1);
		ed_GameTitle.SetComponentValue("", True);
		ed_Acronym.SetComponentValue("", True);
		ed_Prefix.SetComponentValue("", True);
		ed_Parameter.SetComponentValue("", True);
		ch_Default.SetComponentValue("False", True);
		LoadMutators();
		SaveIndex = -1;
		bChanged = False;

		EnableComponent(co_GameClass);
		EnableComponent(ed_GameTitle);
		EnableComponent(ed_Acronym);
		EnableComponent(ed_Prefix);
		EnableComponent(ed_Parameter);
		EnableComponent(lb_Mutator);
		EnableComponent(ch_Default);
	}
	return true;
}
//------------------------------------------------------------------------------------------------
function bool SaveChange()
{
	local int i;

	//log("SaveChange " $ SaveIndex);

	if( SaveIndex == -1 ) // Adding new record
	{
		i = class'xVoting.xVotingHandler'.default.GameConfig.Length;
		class'xVoting.xVotingHandler'.default.GameConfig.Length = i + 1;
		class'xVoting.xVotingHandler'.default.GameConfig[i].GameClass = co_GameClass.GetExtra();
		class'xVoting.xVotingHandler'.default.GameConfig[i].GameName = ed_GameTitle.GetComponentValue();
		class'xVoting.xVotingHandler'.default.GameConfig[i].Acronym = ed_Acronym.GetComponentValue();
		class'xVoting.xVotingHandler'.default.GameConfig[i].Prefix = ed_Prefix.GetComponentValue();
		class'xVoting.xVotingHandler'.default.GameConfig[i].Options = ed_Parameter.GetComponentValue();
		class'xVoting.xVotingHandler'.default.GameConfig[i].Mutators = lb_Mutator.List.GetExtra();
		if( bool(ch_Default.GetComponentValue()) )
			class'xVoting.xVotingHandler'.default.DefaultGameConfig = i;
		class'xVoting.xVotingHandler'.static.StaticSaveConfig();
		lb_GameConfigList.OnChange = none;
		lb_GameConfigList.List.Add( class'xVoting.xVotingHandler'.default.GameConfig[i].GameName, none, string(i));
		lb_GameConfigList.List.Find( class'xVoting.xVotingHandler'.default.GameConfig[i].GameName, False, False);
		lb_GameConfigList.OnChange=GameConfigList_Changed;
		SaveIndex = i;
	}
	else  // modification of existing record
	{
		i = SaveIndex;
		class'xVoting.xVotingHandler'.default.GameConfig[i].GameClass = co_GameClass.GetExtra();
		class'xVoting.xVotingHandler'.default.GameConfig[i].GameName = ed_GameTitle.GetComponentValue();
		class'xVoting.xVotingHandler'.default.GameConfig[i].Acronym = ed_Acronym.GetComponentValue();
		class'xVoting.xVotingHandler'.default.GameConfig[i].Prefix = ed_Prefix.GetComponentValue();
		class'xVoting.xVotingHandler'.default.GameConfig[i].Options = ed_Parameter.GetComponentValue();
		class'xVoting.xVotingHandler'.default.GameConfig[i].Mutators = lb_Mutator.List.GetExtra();
		if( bool(ch_Default.GetComponentValue()) )
			class'xVoting.xVotingHandler'.default.DefaultGameConfig = i;
		class'xVoting.xVotingHandler'.static.StaticSaveConfig();
		lb_GameConfigList.OnChange = none;
		lb_GameConfigList.List.Replace(i, class'xVoting.xVotingHandler'.default.GameConfig[i].GameName, none, string(i));
		lb_GameConfigList.OnChange=GameConfigList_Changed;
	}
	EnableComponent(b_New);
	EnableComponent(b_Delete);
	bChanged=False;
	return true;
}
//------------------------------------------------------------------------------------------------
function bool DeleteButtonClick(GUIComponent Sender)
{
	local int i;

	if( bChanged )
    	SaveChange();

	i = int(lb_GameConfigList.List.GetExtra());
	class'xVoting.xVotingHandler'.default.GameConfig.Remove(i,1);
	class'xVoting.xVotingHandler'.static.StaticSaveConfig();

	// reload list
	lb_GameConfigList.OnChange=none;
	lb_GameConfigList.List.Clear();
	for(i=0; i<class'xVoting.xVotingHandler'.default.GameConfig.Length; i++)
		lb_GameConfigList.List.Add( class'xVoting.xVotingHandler'.default.GameConfig[i].GameName, none, string(i));
	lb_GameConfigList.OnChange=GameConfigList_Changed;
	lb_GameConfigList.List.SetIndex(-1);
	ed_GameTitle.SetComponentValue("", True);
	ed_Acronym.SetComponentValue("", True);
	ed_Prefix.SetComponentValue("", True);
	ed_Parameter.SetComponentValue("", True);
	LoadMutators();

	EnableComponent(b_New);
	bChanged=False;
	return true;
}
//------------------------------------------------------------------------------------------------
function bool ReturnButtonClick(GUIComponent Sender)
{
	Controller.CloseMenu(true);
	return true;
}
//------------------------------------------------------------------------------------------------
function InternalOnClose(optional bool bCancelled)
{
	if( bChanged )
    	SaveChange();
}

defaultproperties
{

	OnOpen=InternalOnOpen
	OnClose=InternalOnClose

	Begin Object class=GUISectionBackground name=SBList
		Caption="GameTypes"
		WinWidth=0.377929
		WinHeight=0.573243
		WinLeft=0.042969
		WinTop=0.044272
	End Object
	sb_List=SBList

	Begin Object class=ALtSectionBackground name=SBList2
		Caption="Mutators"
		HeaderBase=material'2K4Menus.NewControls.Display2'
		WinWidth=0.451172
		WinHeight=0.295899
		WinLeft=0.484649
		WinTop=0.434896
		TopPadding=0.1
		BottomPadding=0.1
		LeftPadding=0.0
		RightPadding=0.0
		RenderWeight=0.49;
	End Object
	sb_List2=SBList2

	Begin Object class=ALtSectionBackground name=SB3
		WinWidth=0.861328
		WinHeight=0.115235
		WinLeft=0.037383
		WinTop=0.778647
	End Object
	sb_3=SB3

	Begin Object Class=GUIListBox Name=GameConfigListBox
		Hint="Select a game configuration to edit or delete."
		WinWidth=0.344087
		WinHeight=0.727759
		WinLeft=0.626758
		WinTop=0.160775
		TabOrder=0
		bVisibleWhenEmpty=true
	End Object
	lb_GameConfigList = GameConfigListBox

	Begin Object Class=moComboBox Name=GameClassComboBox
		Caption = "Game Class"
		Hint="Select a game type for the select game configuration."
		TabOrder=3
		WinWidth=0.592970
		WinHeight=0.076855
		WinLeft=0.028955
		WinTop=0.136135
		CaptionWidth=0.4
		ComponentWidth=0.6
		OnChange=FieldChange
        MenuState=MSAT_Disabled
	End Object
	co_GameClass = GameClassComboBox

	Begin Object Class=moEditBox Name=GameTitleEditBox
	    Caption = "Game Title"
	    Hint="Enter a custom game configuration title."
		TabOrder=4
		WinWidth=0.592970
		WinHeight=0.074249
		WinLeft=0.028955
		WinTop=0.223844
		CaptionWidth=0.4
		ComponentWidth=0.6
		OnChange=FieldChange
        MenuState=MSAT_Disabled
	End Object
	ed_GameTitle = GameTitleEditBox

	Begin Object Class=moEditBox Name=AcronymEditBox
	    //Caption = "Acronym"
	    Caption = "Abbreviation"
	    Hint="A short abbreviation, description, or acronym that identifies the game configuration. This will be appended to the map name in vote messages."
		TabOrder=5
		WinWidth=0.592970
		WinHeight=0.076855
		WinLeft=0.028955
		WinTop=0.306343
		CaptionWidth=0.4
		ComponentWidth=0.6
		OnChange=FieldChange
        MenuState=MSAT_Disabled
	End Object
	ed_Acronym = AcronymEditBox

	Begin Object Class=moEditBox Name=PrefixEditBox
	    Caption = "Map Prefixes"
	    Hint="List of map name prefixes. Separate each with commas."
		WinWidth=0.592970
		WinHeight=0.074249
		WinLeft=0.028955
		WinTop=0.393185
		TabOrder=6
		CaptionWidth=0.4
		ComponentWidth=0.6
		OnChange=FieldChange
        MenuState=MSAT_Disabled
	End Object
	ed_Prefix = PrefixEditBox

	Begin Object Class=MultiSelectListBox Name=MutatorListBox
        Hint="Select each mutator that should be loaded with this game configuration."
		WinWidth=0.396485
		WinHeight=0.315234
		WinLeft=0.224267
		WinTop=0.484369
		TabOrder=7
		bVisibleWhenEmpty=true
		OnChange=FieldChange
        MenuState=MSAT_Disabled
	End Object
	lb_Mutator = MutatorListBox

	Begin Object Class=moEditBox Name=ParameterEditBox
	    Caption = "Parameters"
	    Hint="(Advanced) List of game parameters with values. Separated each with a comma. (ex. GoalScore=4,MinPlayers=4)"
		WinWidth=0.490431
		WinHeight=0.030000
		WinLeft=0.077783
		WinTop=0.826949
		CaptionWidth=0.4
		TabOrder=8
		ComponentWidth=0.6
		OnChange=FieldChange
        MenuState=MSAT_Disabled
	End Object
	ed_Parameter = ParameterEditBox

	Begin Object class=moCheckbox Name=DefaultCheckBox
		Caption="Default"
		Hint="The selected game configuration will be the default if all the players leave the server"
		CaptionWidth=0.8
		ComponentWidth=0.2
		OnChange=FieldChange
		TabOrder=9
		WinWidth=0.194922
		WinHeight=0.030000
		WinLeft=0.659814
		WinTop=0.826949
        MenuState=MSAT_Disabled
	End Object
	ch_Default = DefaultCheckBox

	Begin Object class=GUIButton Name=NewButton
		Caption="New"
		Hint="Create a new game configuration."
		StyleName="SquareButton"
		TabOrder=1
		OnClick=NewButtonClick
		WinWidth=0.158281
		WinLeft=0.060047
		WinTop=0.704747
	End Object
	b_New=NewButton

	Begin Object class=GUIButton Name=DeleteButton
		Caption="Delete"
		Hint="Delete the selected game configuration."
		StyleName="SquareButton"
		TabOrder=2
		OnClick=DeleteButtonClick
		WinWidth=0.159531
		WinLeft=0.268403
		WinTop=0.704747
        MenuState=MSAT_Disabled
	End Object
	b_Delete=DeleteButton

	Background=None

	bAcceptsInput=false

	WinWidth=0.917187
	WinHeight=0.885075
	WinLeft=0.041015
	WinTop=0.031510

	DefaultWidth=0.917187
	DefaultHeight=0.885075
	DefaultLeft=0.041015
	DefaultTop=0.031510

 	WindowName="Map Voting Game Configuration"

 	lmsgNew="New"
 	lmsgAdd="Add"
}

