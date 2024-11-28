//==============================================================================
// Overview of the enemy teams played against
//
// Written by Michiel Hendriks
// (c) 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4SPTab_DetailEnemies extends UT2K4SPTab_Base;

var automated GUISectionBackground sbgFoesBG, sbdDetails;
var automated GUI2K4MultiColumnListBox mclFoes;
var automated GUIImage imgPicture;
var automated GUIButton btnOk;
var automated GUIScrollTextBox cbDescription;
var localized array<string> StrengthLabels;

/** local copy, filtered */
var array<UT2K4GameProfile.TeamStatsRecord> TeamStats;
var array< class<UT2K4TeamRoster> > TeamRosters;

var string TeamDetailPage;

var localized string ColumnHeadings[5], msgMembers;

var GUIStyles SelStyle;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i;
	Super.Initcomponent(MyController, MyOwner);
	mclFoes.List.OnDrawItem = OnDrawFoesList;
	mclFoes.List.ExpandLastColumn = true;
	mclFoes.List.ColumnHeadings[0] = ColumnHeadings[0];
	mclFoes.List.ColumnHeadings[1] = ColumnHeadings[1];
	mclFoes.List.ColumnHeadings[2] = ColumnHeadings[2];
	mclFoes.List.ColumnHeadings[3] = ColumnHeadings[3];
	mclFoes.List.ColumnHeadings[4] = ColumnHeadings[4];
	mclFoes.List.InitColumnPerc[0]=0.30;
	mclFoes.List.InitColumnPerc[1]=0.15;
	mclFoes.List.InitColumnPerc[2]=0.15;
	mclFoes.List.InitColumnPerc[3]=0.20;
	mclFoes.List.InitColumnPerc[4]=0.20;
	if (GP != none)
	{
		for (i = 0; i < GP.TeamStats.Length; i++)
		{
			if ((GP.TeamStats[i].Matches > 0) && (GP.TeamStats[i].Name != ""))
			{
				TeamStats.Length = TeamStats.Length+1;
				TeamStats[TeamStats.length - 1] = GP.TeamStats[i];
				TeamStats[TeamStats.length - 1].Level = (TeamStats[TeamStats.length - 1].Level + StrengthLabels.length) % StrengthLabels.length; // fix level

				TeamRosters.Length = TeamStats.Length;
				TeamRosters[TeamStats.length - 1] = class<UT2K4TeamRoster>(DynamicLoadObject(GP.TeamStats[i].Name, class'Class'));
				if (TeamRosters[TeamStats.length - 1] == none)
				{
					Warn("Failed spawning UT2K4TeamRoster:"@GP.TeamStats[i].Name);
				}

				mclFoes.List.AddedItem();
			}
		}
	}
	mclFoes.List.SortList();
	mclFoes.List.OnChange = OnTeamSelect;
	SelStyle = Controller.GetStyle(mclFoes.SelectedStyleName, mclFoes.FontScale);
}

function OnDrawFoesList(Canvas Canvas, int i, float X, float Y, float W, float H, bool bSelected, bool bPending)
{
	local float CellLeft, CellWidth;
	local float XL, YL;
	local GUIStyles curstyle;

	if( bSelected )
	{
		SelStyle.Draw(Canvas,MSAT_Pressed, X, Y-2, W, H+2 );
		curstyle = SelStyle;
	}
	else curstyle = mclFoes.List.Style;

	i = mclFoes.List.SortData[i].SortItem;

	if (TeamRosters[i] == none) return;

	mclFoes.List.GetCellLeftWidth( 0, CellLeft, CellWidth );
	curstyle.DrawText( Canvas, mclFoes.List.MenuState, CellLeft, Y, CellWidth, H, TXTA_Left, TeamRosters[i].default.TeamName, mclFoes.List.FontScale );

	mclFoes.List.GetCellLeftWidth( 1, CellLeft, CellWidth );
	curstyle.TextSize(Canvas, mclFoes.List.MenuState, string(TeamStats[i].Matches), XL, YL, mclFoes.List.FontScale);
	if (CellLeft + XL <= mclFoes.List.ActualLeft() + mclFoes.List.ActualWidth())
		curstyle.DrawText( Canvas, mclFoes.List.MenuState, CellLeft, Y, CellWidth, H, TXTA_Left, string(TeamStats[i].Matches), mclFoes.List.FontScale );

	mclFoes.List.GetCellLeftWidth( 2, CellLeft, CellWidth );
	curstyle.TextSize(Canvas, mclFoes.List.MenuState, string(TeamStats[i].Won), XL, YL, mclFoes.List.FontScale);
	if (CellLeft + XL <= mclFoes.List.ActualLeft() + mclFoes.List.ActualWidth())
		curstyle.DrawText( Canvas, mclFoes.List.MenuState, CellLeft, Y, CellWidth, H, TXTA_Left, string(TeamStats[i].Won), mclFoes.List.FontScale );

	mclFoes.List.GetCellLeftWidth( 3, CellLeft, CellWidth );
	curstyle.TextSize(Canvas, mclFoes.List.MenuState, string(TeamStats[i].Rating), XL, YL, mclFoes.List.FontScale);
	if (CellLeft + XL <= mclFoes.List.ActualLeft() + mclFoes.List.ActualWidth())
		curstyle.DrawText( Canvas, mclFoes.List.MenuState, CellLeft, Y, CellWidth, H, TXTA_Left, string(TeamStats[i].Rating), mclFoes.List.FontScale );

	mclFoes.List.GetCellLeftWidth( 4, CellLeft, CellWidth );
	curstyle.TextSize(Canvas, mclFoes.List.MenuState, StrengthLabels[TeamStats[i].Level], XL, YL, mclFoes.List.FontScale);
	if (CellLeft + XL <= mclFoes.List.ActualLeft() + mclFoes.List.ActualWidth())
		curstyle.DrawText( Canvas, mclFoes.List.MenuState, CellLeft, Y, CellWidth, H, TXTA_Left, StrengthLabels[TeamStats[i].Level], mclFoes.List.FontScale );

}

function onCmFoesClick(GUIContextMenu Sender, int ClickIndex)
{
	local int i;
	if (ClickIndex == 0)
	{
		i = mclFoes.List.SortData[mclFoes.List.Index].SortItem;
		if (TeamStats.length > 0) Controller.OpenMenu(TeamDetailPage, TeamStats[i].Name);
	}
}

function string mclFoesOnGetSortString(GUIComponent Sender, int item, int column)
{
	local string s;
	switch (column)
	{
		case 0: s = TeamRosters[item].default.TeamName; break;
		case 1: s = string(TeamStats[item].Matches);
						PadLeft(S, 5, "0");
						break;
		case 2: s = string(TeamStats[item].Won);
						PadLeft(S, 5, "0");
						break;
		case 3: s = string(TeamStats[item].Rating*1000+100000);
						break;
		case 4: s = string(TeamStats[item].Level);
						PadLeft(S, 5, "0");
						break;
	}
	return s;
}

function OnTeamSelect(GUIComponent Sender)
{
	local int i;
	i = mclFoes.List.SortData[mclFoes.List.Index].SortItem;
	if (i < TeamStats.length && i > -1)
		ShowTeamDetails(TeamStats[i].Name);
}

/** Param1 is the fully qualified name for the team */
function ShowTeamDetails(string TeamName)
{
	local class<UT2K4TeamRoster> ETI;
	local array<string> TeamMembers;
	local string TeamDesc;

	ETI = class<UT2K4TeamRoster>(DynamicLoadObject(TeamName, class'Class'));
	if (ETI == none)
	{
		Warn(TeamName@"is not a valid subclass of UT2K4TeamRoster");
		return;
	}
	sbdDetails.Caption = ETI.default.TeamName;
	if (ETI.default.TeamSymbolName != "") imgPicture.Image = Material(DynamicLoadObject(ETI.default.TeamSymbolName, class'Material', true));
	if (!GP.GetAltTeamRoster(TeamName, TeamMembers)) TeamMembers = ETI.default.RosterNames;
	TeamDesc = ETI.default.TeamDescription;
	if (TeamMembers.length != 0) TeamDesc @= "||"$msgMembers@Joinarray(TeamMembers, ", ", true);
	cbDescription.SetContent(TeamDesc);
	if (ETI.default.VoiceOver != none) PlayerOwner().PlayOwnedSound(ETI.default.VoiceOver, SLOT_Interface, 1.0);
	else if (ETI.default.TeamNameSound != none) PlayerOwner().PlayOwnedSound(ETI.default.TeamNameSound, SLOT_Interface, 1.0);
}

defaultproperties
{
	Begin Object class=GUIContextMenu Name=SPDEcmFoes
		ContextItems(0)="Team details"
		OnSelect=onCmFoesClick
	End Object

	Begin Object Class=GUI2K4MultiColumnListBox Name=SPDEmclFoes
		WinWidth=0.791837
		WinHeight=0.267856
		WinLeft=0.097832
		WinTop=0.675411
		bVisibleWhenEmpty=True
		RenderWeight=0.2
		StyleName="ServerBrowserGrid"
		SelectedStyleName="ListSelection"
		OnGetSortString=mclFoesOnGetSortString
		OnChange=OnTeamSelect
		bBoundToParent=true
	End Object
	mclFoes=SPDEmclFoes
	ContextMenu=SPDEcmFoes

	Begin Object class=GUISectionBackground Name=SPPsbgFoesBG
		WinWidth=0.827500
		WinHeight=0.356249
		WinLeft=0.080000
		WinTop=0.616587
		Caption="Enemy Teams"
		bBoundToParent=true
    End Object
    sbgFoesBG=SPPsbgFoesBG

    Begin Object class=AltSectionBackground Name=SPPsbdDetails
		WinWidth=0.820751
		WinHeight=0.552000
		WinLeft=0.082750
		WinTop=0.036500
		bBoundToParent=true
    End Object
    sbdDetails=SPPsbdDetails

	Begin Object class=GUIImage name=SPDTimgPicture
		WinWidth=0.318878
		WinHeight=0.417091
		WinLeft=0.101276
		WinTop=0.105163
		ImageColor=(R=255,G=255,B=255,A=255);
		ImageRenderStyle=MSTY_Alpha
		ImageAlign=IMGA_Center
		ImageStyle=ISTY_Justified
		RenderWeight=0.3
		bBoundToParent=true
	End Object
	imgPicture=SPDTimgPicture

	Begin Object Class=GUIScrollTextBox Name=SPDTcbDescription
		WinWidth=0.456249
		WinHeight=0.412500
		WinLeft=0.431123
		WinTop=0.108998
		bNoTeletype=true
		FontScale=FNS_Medium
		bVisibleWhenEmpty=true
		bBoundToParent=true
		StyleName="NoBackground"
	End Object
	cbDescription=SPDTcbDescription

	PanelCaption="Opponent Teams"
	TeamDetailPage="GUI2K4.UT2K4SP_DetailsTeam"

	StrengthLabels(0)="Weak"
	StrengthLabels(1)="Tough"
	StrengthLabels(2)="Strong"
	StrengthLabels(3)="Godlike"
	StrengthLabels(4)="Phantom"

	ColumnHeadings[0]="Name"
	ColumnHeadings[1]="Matches"
	ColumnHeadings[2]="They won"
	ColumnHeadings[3]="Rating"
	ColumnHeadings[4]="Strength"

	msgMembers="Members:"
}
