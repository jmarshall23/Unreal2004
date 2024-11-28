//====================================================================
//  xVoting.MapInfoPage
//  Map Information Page
//
//  Written by Bruce Bickar
//  (c) 2003, Epic Games, Inc.  All Rights Reserved
// ====================================================================
class MapInfoPage extends LargeWindow;

var automated GUIButton        b_ReturnButton;
var automated GUIScrollTextBox lb_MapDesc;
var automated GUIImage         i_MapImage;
var automated GUILabel         l_MapName, l_MapAuthor, l_MapPlayers, l_NoPreview;

var array<CacheManager.MapRecord> Maps;

var localized string MessageNoInfo, AuthorText, PlayerText, lmsgLevelPreviewUnavailable;
//------------------------------------------------------------------------------------------------
function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.Initcomponent(MyController, MyOwner);

	class'CacheManager'.static.GetMapList( Maps );
	b_ReturnButton.OnClick=ReturnButtonOnClick;
}
//------------------------------------------------------------------------------------------------
function HandleParameters(string Param1, string Param2)
{
    ReadMapInfo(Param1);
}
//------------------------------------------------------------------------------------------------
function ReadMapInfo(string MapName)
{
    local DecoText DText;
    local string mDesc;
    local int Index, i;
    local Material Screenie;
    local string Package, Item;

    if(MapName == "")
        return;

    if (!Controller.bCurMenuInitialized)
        return;

	MapName = StripMapName(MapName);
    Index = FindCacheRecordIndex(MapName);

    if (Maps[Index].FriendlyName != "")
        l_MapName.Caption = Maps[Index].FriendlyName;
    else
		l_MapName.Caption = MapName;

	if ( Maps[Index].ScreenshotRef != "" )
	    Screenie = Material(DynamicLoadObject(Maps[Index].ScreenshotRef, class'Material'));
    i_MapImage.Image = Screenie;

    l_NoPreview.SetVisibility( Screenie == None );
    i_MapImage.SetVisibility( Screenie != None );

    l_MapPlayers.Caption = Maps[Index].PlayerCountMin@"-"@Maps[Index].PlayerCountMax@PlayerText;

	if ( class'CacheManager'.static.Is2003Content(Maps[Index].MapName) )
	{
		if ( Maps[i].TextName != "" )
		{
			if ( !Divide(Maps[Index].TextName, ".", Package, Item) )
			{
				Package = "XMaps";
				Item = Maps[Index].TextName;
			}
			DText = class'xUtil'.static.LoadDecoText( Package, Item );
		}
	}

    if (DText != None)
    {
        for (i = 0; i < DText.Rows.Length; i++)
        {
            if (mDesc != "")
                mDesc $= "|";
            mDesc $= DText.Rows[i];
        }
    }

    else mDesc = Maps[Index].Description;

    if (mDesc == "")
        mDesc = MessageNoInfo;

	lb_MapDesc.SetContent( mDesc );
    if (Maps[Index].Author != "")
        l_MapAuthor.Caption = AuthorText$":"@Maps[Index].Author;
    else l_MapAuthor.Caption = "";
}
//------------------------------------------------------------------------------------------------
// Remove any additional text from the map's name
// Used for getting just the mapname
function string StripMapName( string FullMapName )
{
	local int pos;

	pos = InStr(FullMapName, " ");
	if ( pos != -1 )
		FullMapName = Left(FullMapName, pos);

	return FullMapName;
}
//------------------------------------------------------------------------------------------------
function int FindCacheRecordIndex(string MapName)
{
    local int i;

    for (i = 0; i < Maps.Length; i++)
        if (Maps[i].MapName == MapName)
            return i;

    return -1;
}
//------------------------------------------------------------------------------------------------
function bool ReturnButtonOnClick(GUIComponent Sender)
{
	Controller.CloseMenu(true);
	return true;
}
//------------------------------------------------------------------------------------------------
function SetVisibility(bool bIsVisible)
{
	Super.SetVisibility(bIsVisible);

    l_NoPreview.SetVisibility( i_MapImage.Image == None );
    i_MapImage.SetVisibility( i_MapImage.Image != None );
}

defaultproperties
{
    Begin Object class=GUILabel Name=MapNameLabel
        Caption="MapName"
        TextAlign=TXTA_Center
        TextFont="UT2HeaderFont"
        TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.911979
		WinHeight=0.062348
		WinLeft=0.283496
		WinTop=0.120773
        RenderWeight=0.3
		bScaleToParent=True
    End Object
    l_MapName=MapNameLabel

    Begin Object Class=GUIImage Name=MapImage
		WinWidth=0.917083
		WinHeight=0.451979
		WinLeft=0.281885
		WinTop=0.173177
        ImageColor=(R=255,G=255,B=255,A=255)
        ImageStyle=ISTY_Scaled
        ImageRenderStyle=MSTY_Normal
        RenderWeight=0.2
		bScaleToParent=True
    End Object
    i_MapImage=MapImage

    Begin Object Class=GUILabel Name=NoPreview
		WinWidth=0.917083
		WinHeight=0.451979
		WinLeft=0.281885
		WinTop=0.173177
        TextFont="UT2HeaderFont"
        TextAlign=TXTA_Center
        VertAlign=TXTA_Center
        bMultiline=True
        bTransparent=False
        TextColor=(R=247,G=255,B=0,A=255)
        Caption="No Preview Available"
		bScaleToParent=True
    End Object
    l_NoPreview=NoPreview

    Begin Object class=GUILabel Name=MapAuthorLabel
        Caption="MapAuthor"
        TextAlign=TXTA_Center
        TextFont="UT2SmallHeaderFont"
        TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.915313
		WinHeight=0.049359
		WinLeft=0.282387
		WinTop=0.536739
        RenderWeight=0.3
		bScaleToParent=True
    End Object
    l_MapAuthor=MapAuthorLabel

    Begin Object class=GUILabel Name=MapPlayersLabel
        Caption="Players"
        TextAlign=TXTA_Center
        TextFont="UT2SmallHeaderFont"
        TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.915313
		WinHeight=0.049359
		WinLeft=0.282387
		WinTop=0.576252
        RenderWeight=0.3
		bScaleToParent=True
    End Object
    l_MapPlayers=MapPlayersLabel

	Begin Object Class=GUIScrollTextBox Name=MapInfoList
		WinWidth=0.918322
		WinHeight=0.207500
		WinLeft=0.284888
		WinTop=0.620235
        CharDelay=0.0025
        EOLDelay=0.5
        bNeverFocus=true
        StyleName="NoBackground"
        bTabStop=False
        bBoundToParent=false
		bVisibleWhenEmpty=true
        bNoTeletype=false
		bStripColors=false
		bScaleToParent=True
	End Object
	lb_MapDesc = MapInfoList

	Begin Object class=GUIButton Name=ExitButton
		Caption="Return"
		StyleName="SquareButton"
		WinWidth=0.283333
		WinHeight=0.056536
		WinLeft=0.433766
		WinTop=0.791113
		TabOrder=2
		RenderWeight=1.0
		bScaleToParent=True
	End Object
	b_ReturnButton = ExitButton

	Background=None

	bAcceptsInput=false

	WinWidth=0.468750
	WinHeight=0.801954
	WinLeft=0.264063
	WinTop=0.077213

	bRenderWorld=true
    bRequire640x480=false
    bAllowedAsLast=true
	bMoveAllowed=False
	//bPersistent=True

    MessageNoInfo="No information available!"
    AuthorText="Author"
    PlayerText="players"
    lmsgLevelPreviewUnavailable="Level Preview Unavailable"
}

