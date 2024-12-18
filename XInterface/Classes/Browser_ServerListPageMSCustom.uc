class Browser_ServerListPageMSCustom extends Browser_ServerListPageMS;

var GUISplitter		    GameTypeSplit;
var GUIComboBox			GameTypeCombo;
var GUISplitter		    MainSplit;

var bool				CustomInitialized;

var config string		LastGameType;

function ChangeGameType(GUIComponent Sender)
{
	Log( "New Game Type:"$GameTypeCombo.GetText()$" ("$GameTypeCombo.GetExtra()$")" );

	GameType = GameTypeCombo.GetExtra();

	LastGameType = GameType;
	SaveConfig();

	RefreshList();
}

// Beginning to think this should be somewhere else :)
function string MyParseDescStr(string DescStr, int index)
{
	local string temp;
	local int p,i;

	i = 0;

	while (DescStr!="")
	{
		p = instr(DescStr,"|");
		if (p<0)
		{
			Temp = DescStr;
			DescStr = "";
		}
		else
		{
			Temp = Left(DescStr,p);
			DescStr = Right(DescStr,Len(DescStr)-p-1);
		}
		if (i==Index)
			return Temp;

		i++;
	}
}

static function bool IsStandardGameType(string Desc)
{
	if( InStr(Desc, "xDeathMatch") >= 0 )
		return true;

	if( InStr(Desc, "xTeamGame") >= 0 )
		return true;

	if( InStr(Desc, "xCTFGame") >= 0 )
		return true;

	if( InStr(Desc, "xBombingRun") >= 0 )
		return true;

	if( InStr(Desc, "xDoubleDom") >= 0 )
		return true;

	// Bonus pack game types.. 'forward compatibility'
	if( InStr(Desc, "xMutantGame") >= 0 )
		return true;

	if( InStr(Desc, "xLastManStanding") >= 0 )
		return true;

	if( InStr(Desc, "Invasion") >= 0 )
		return true;

	return false;
}

// Find the friendly name for this game type class, by searching the dialog box
function string FindGameTypeName(string GameType)
{
	local int i;

	for(i=0; i<GameTypeCombo.List.ItemCount; i++)
	{
		if( GameType == GameTypeCombo.List.GetExtraAtIndex(i) )
			return GameTypeCombo.List.GetItemAtIndex(i);
	}

	Log("CustomTab: Could Not Find Game Type '"$GameType$"' In Combo");
	return "";
}

function PopulateGameTypes()
{
	local string Entry, Desc, MyGameType, TypeName;
	local int Index, PeriodPos;

	// Find other game types from .int files, and create tabs
	Index = 0;
	PlayerOwner().GetNextIntDesc("GameInfo",Index,Entry,Desc);
	while (Entry != "")
	{
		Desc = Entry$"|"$Desc;

		if ( !IsStandardGameType(Desc) )
		{
			MyGameType = MyParseDescStr(Desc, 0);

			// strip it down to just the stuff after the '.'
			PeriodPos = InStr(MyGameType, ".");
			if(PeriodPos != -1)
				MyGameType = Mid(MyGameType, PeriodPos+1);

			TypeName = MyParseDescStr(Desc, 2);

			// Add game type to combo box
			GameTypeCombo.AddItem(TypeName, None, MyGameType);
		}

		Index++;
		PlayerOwner().GetNextIntDesc("GameInfo", Index, Entry, Desc);
	}

	// Default to last used game type (if we find it).
	GameType = LastGameType;
	GameTypeCombo.SetText( FindGameTypeName(LastGameType) );
}

function InitComponent(GUIController C, GUIComponent O)
{
	Super.InitComponent(C, O);

	if(!CustomInitialized)
	{
		MainSplit = GUISplitter(Controls[0]);
		MainSplit.WinHeight = 1.0;

		// Set the main splitter as one half of this game type splitter
		GameTypeSplit.Controls[1] = MainSplit;

		// The the game type splitter as the main control on the page
		Controls[0] = GameTypeSplit;

		// (re)initialise the game type splitter as a child of this page
		GameTypeSplit.InitComponent(C, self);

		// Set up the game type combo box
		GameTypeCombo = GUIComboBox(GUIPanel(GameTypeSplit.Controls[0]).Controls[0]);
		PopulateGameTypes();
		GameTypeCombo.OnChange = ChangeGameType;

		// Done Init
		CustomInitialized = true;
	}

	GameTypeCombo.ReadOnly(true);
}

defaultproperties
{
	Begin Object class=GUIComboBox Name=MyGameTypeCombo
		WinWidth=0.365000
		WinHeight=24.000000
		WinLeft=0.615740
		WinTop=0.250000
	End Object

	Begin Object class=GUILabel Name=MyGameTypeLabel
		Caption="Game Type"
		TextALign=TXTA_Left
		TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.400000
		WinHeight=24.000000
		WinLeft=0.405117
		WinTop=0.283333
	End Object

	// Buddy panel
	Begin Object Class=GUIPanel Name=GameTypePanel
		Controls(0)=MyGameTypeCombo
		Controls(1)=MyGameTypeLabel
	End Object

	// Splitter to divide buddies and rules horizontally
	Begin Object Class=GUISplitter Name=MyGameTypeSplitter
		WinWidth=1.0
		WinHeight=0.9
		WinTop=0
		WinLeft=0
		Controls(0)=GameTypePanel
		SplitOrientation=SPLIT_Vertical
		SplitPosition=0.08
		Background=DefaultTexture
		bBoundToParent=true
		bScaleToParent=true
		bFixedSplitter=true
		bDrawSplitter=false
	End Object
	GameTypeSplit=MyGameTypeSplitter
}