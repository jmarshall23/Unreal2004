// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class UT2K4Demos extends ModsAndDemosTabs;

var automated GUIListBox lb_DemoList,lb_DemoInfo;
var automated GUIScrollTextBox lb_ReqPacks;
var automated GUILabel lbl_Game, l_NoPreview;
var automated GUIImage i_MapShot;
var automated GUISectionBackground sb_1, sb_2, sb_3, sb_4;

var localized string ltScoreLimit, ltTimeLimit, UnknownText, CorruptDemText,ltSelectMsg;
var localized string ltClientSide, ltServerSide, ltRecordedBy,ltGoodMsg, ltBadMsg;
var array<CacheManager.MapRecord> Maps;
var array<CacheManager.GameRecord> Games;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local array<string> Demos;
    local int i;

	Super.InitComponent(MyController,MyOwner);

	class'CacheManager'.static.GetMapList(Maps);
	class'CacheManager'.static.GetGameTypeList(Games);

    MyController.GetDEMList(Demos);
    for (i=0;i<Demos.Length;i++)
    	lb_DemoList.List.Add(TrimName(Demos[i]));

	bInit = False;
    if (lb_DemoList.ItemCount()>0)
	    DemoListClick(none);

	MyPage.MyFooter.b_Watch.OnClick=WatchClick;
	MyPage.MyFooter.b_Dump.OnClick=DumpClick;

	sb_1.ManageComponent(lb_DemoList);
	sb_2.Managecomponent(i_MapShot);
	sb_3.ManageComponent(lb_ReqPacks);
}

function string TrimName(string s)
{
	local int p;
	p = InStr(Caps(s),".DEMO4");
	if (p>=0)
		return Left(s,p);

	return s;
}

function bool DumpClick(GUIComponent Sender)
{
	Controller.OpenMenu("GUI2K4.UT2K4Demo2AVI",lb_DemoList.List.Get());
    return true;
}

function bool WatchClick(GUIComponent Sender)
{
	Console(Controller.Master.Console).DelayedConsoleCommand("demoplay"@lb_DemoList.List.Get());
    return true;
}


function DemoListClick(GUIComponent Sender)
{
	local string MapName, GameType, RecordedBy, TimeStamp,ReqPackages;
    local int	 i, ScoreLimit, TimeLimit, ClientSide;
    local Material Screenie;

	if ( bInit )
		return;

	lb_DemoInfo.List.Clear();
	if ( LB_DemoList.List.ItemCount<=0 )
	{
		lbl_Game.Caption = "";
		sb_2.Caption = "";
		lb_ReqPacks.SetContent("");
		i_MapShot.SetVisibility( false);
		l_NoPreview.SetVisibility( true );
		return;
	}

	if ( Controller.GetDEMHeader(lb_DemoList.List.Get( True )$".DEMO4",MapName,GameType,ScoreLimit,TimeLimit,ClientSide,RecordedBy,TimeStamp,ReqPackages) )
    {
    	lbl_Game.Caption=Caps(MapName);
    	i = GetGameIndex(GameType);

    	if ( i != -1 )
    		sb_2.Caption = Games[i].GameName;
		else sb_2.Caption = UnknownText;

		lb_DemoInfo.List.Add(ltScoreLimit@ScoreLimit@"  "@ltTimeLimit@TimeLimit);

        if (ClientSide!=0)
	        lb_DemoInfo.List.Add(ltClientSide);
    	else
	    	lb_DemoInfo.List.Add(ltServerSide);

       	lb_DemoInfo.List.Add(ltRecordedBy@RecordedBy);

        if (ReqPackages!="")
        	lb_ReqPacks.SetContent(ltBadMsg$"||"$ReqPackages);
        else
        	lb_ReqPacks.SetContent(ltGoodMsg);

		i = GetMapIndex(MapName);
		if ( i != -1 )
			Screenie = Material(DynamicLoadObject(Maps[i].ScreenshotRef, class'Material'));

    }
    else
    {
    	sb_2.Caption="";
        lbl_Game.Caption=UnknownText;
    	lb_DemoInfo.List.Clear();
    	lb_ReqPacks.SetContent(CorruptDemText);
    }

    i_MapShot.Image = Screenie;
	i_MapShot.SetVisibility( Screenie != None );
	l_NoPreview.SetVisibility( Screenie == None );

    lb_ReqPacks.Restart();
    lb_ReqPacks.Stop();

    return;
}

function SetVisibility( bool bIsVisible )
{
	Super.SetVisibility(bIsVisible);

	i_MapShot.SetVisibility( i_MapShot.Image != none );
	l_NoPreview.SetVisibility( i_MapShot.Image == None );
}

function int GetMapIndex( string MapName )
{
	local int i;

	for ( i = 0; i < Maps.Length; i++ )
		if ( Maps[i].MapName ~= MapName )
			return i;

	return -1;
}

function int GetGameIndex( string GameClass )
{
	local int i;

	for ( i = 0; i < Games.Length; i++ )
		if ( Games[i].ClassName ~= GameClass || GameClass ~= ("class " $ Games[i].ClassName ) )
			return i;

	return -1;
}

function InfoClick(GUIComponent Sender)
{
	lb_DemoInfo.List.Index=0;
}

defaultproperties
{
	Begin Object class=GUIImage Name=iMapShot
		WinWidth=0.326562
		WinHeight=0.288124
		WinLeft=0.619686
		WinTop=0.199687
        ImageColor=(R=255,G=255,B=255,A=255)
        ImageStyle=ISTY_Scaled
        ImageRenderStyle=MSTY_Normal
        RenderWeight=1
        DropShadowX=8
        DropShadowY=8
	End Object
	i_MapShot=iMapShot

    Begin Object Class=GUILabel Name=NoPreview
		WinWidth=0.318399
		WinHeight=0.226862
		WinLeft=0.517749
		WinTop=0.286842
        TextFont="UT2HeaderFont"
        TextAlign=TXTA_Center
        VertAlign=TXTA_Center
        bMultiline=True
        bTransparent=False
        TextColor=(R=247,G=255,B=0,A=255)
        Caption="No Preview Available"
        RenderWeight=1
    End Object
    l_NoPreview=NoPreview

	Begin Object class=AltSectionBackground name=iInfoBk
		WinWidth=0.637278
		WinHeight=0.629445
		WinLeft=0.354323
		WinTop=0.011296
		HeaderBase=material'2K4Menus.NewControls.Display2'
		Caption="... Information ..."
        RenderWeight=0.2
	End Object
	sb_4=iInfoBk

	// char stats
	Begin Object class=GUIListBox Name=lbDemoList
		WinWidth=0.265626
		WinHeight=0.735548
		WinLeft=0.030468
		WinTop=0.109375
        OnChange=DemoListClick;
        OutlineStyleName="NoBackground"
        SectionStyleName="NoBackground"
        SelectedStyleName="NoBackground"
        TabOrder=0
        bVisibleWhenEmpty=true
	End Object

    lb_DemoList=lbDemoList

	Begin Object class=GUIListBox Name=lbDemoInfo
		WinWidth=0.276054
		WinHeight=0.078998
		WinLeft=0.529180
		WinTop=0.155622
        bAcceptsInput=false
        bVisibleWhenEmpty=true
        OnChange=InfoClick
	End Object

    lb_DemoInfo=lbDemoInfo


	Begin Object class=GUIScrollTextBox Name=lbReqPacks
		WinWidth=0.650391
		WinHeight=0.193555
		WinLeft=0.310547
		WinTop=0.650090
        TabOrder=2
        bVisibleWhenEmpty=true
        bNoTeletype=true
	End Object

    lb_ReqPacks=lbReqPacks

	Begin Object class=GUILabel Name=lblGame
		Caption=""
		TextColor=(R=230,G=200,B=0,A=255)
		TextALign=TXTA_Center
		TextFont="UT2LargeFont"
		WinWidth=0.634467
		WinHeight=0.061558
		WinLeft=0.355370
		WinTop=0.103929
        RenderWeight=0.6
	End Object
    lbl_Game=lblGame

	Begin Object class=GUISectionBackground Name=sb1
		WinWidth=0.328364
		WinHeight=0.962274
		WinLeft=0.012527
		WinTop=0.012761
        Caption="Demos"
        BottomPadding=0.2
        RenderWeight=0.5
        bFillClient=true
	End Object
	sb_1=sb1

	Begin Object class=AltSectionBackground Name=sb2
		WinWidth=0.368224
		WinHeight=0.346441
		WinLeft=0.492837
		WinTop=0.228215
        Caption=""
        RenderWeight=0.5
        bFillClient=true
	End Object
	sb_2=sb2

   	Begin Object class=GUISectionBackground Name=sb3
		WinWidth=0.637278
		WinHeight=0.318539
		WinLeft=0.354323
		WinTop=0.656193
        Caption="Required Packages"
        RenderWeight=0.5
        BottomPadding=0.2
        bFillClient=true
	End Object
	sb_3=sb3


	CorruptDemText="Corrupted or missing .DEMO4 file !"
	UnknownText="Unknown"
    ltScoreLimit="Score Limit:"
    ltTimeLimit="Time Limit:"
	ltClientSide="Client Side Demo"
	ltServerSide="Server Side/Single Player Demo"
	ltGoodMsg="All of the packages required for this demo are installed"
	ltBadMsg="In order to be played, this demo requires the packages listed below.  If you are connected to the Internet, they will be autodownloaded when the demo is played||::Required Packages::"
	ltRecordedBy="Recorded By:"
	ltSelectMsg="Please select a demo from the list to the left."
	Tag=3
}
