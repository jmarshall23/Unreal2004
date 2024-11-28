// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class UT2K4Ownage extends ModsAndDemosTabs;

var automated GUISectionBackground sb_1, sb_2;

var automated GUIListBox lb_MapList;
var automated GUIScrollTextBox lb_MapInfo;
var automated GUIImage i_Background;


var int    								OwnageLevel;
var array<GUIController.eOwnageMap> 	OwnageMaps;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i,j;
    local array<int> RLevel;
    local array<string> MName;
    local array<string> MDesc;
    local array<string> MURL;
    Super.InitComponent(MyController, MyOwner);

	Controller.GetOwnageList(RLevel, MName, MDesc, MURL);

	for (i=0;i<RLevel.Length;i++)
    {
		j = OwnageMaps.Length;
        OwnageMaps.Length = OwnageMaps.Length+1;
        OwnageMaps[j].RLevel  = RLevel[i];
        OwnageMaps[j].MapName = MName[i];
        OwnageMaps[j].MapDesc = MDesc[i];
        OwnageMaps[j].MapURL  = MURL[i];

		if (OwnageMaps[j].RLevel > OwnageLevel)
        	OwnageLevel = OwnageMaps[j].RLevel;

    }

    sb_1.ManageComponent(lb_MapList);
    sb_2.ManageComponent(lb_MapInfo);

	PrimeMapList();
    lb_MapList.List.SetIndex(0);
    ListOnChange(lb_MapList);

    MyPage.MyFooter.b_Download.OnClick=DownloadClick;

}

function PrimeMapList()
{
	local int i;
    lb_MapList.List.Clear();
    for (i=0;i<OwnageMaps.Length;i++)
    	lb_MapList.List.Add(OwnageMaps[i].MapName,,string(i));
}

function bool DownloadClick(GUIComponent Sender)
{
	local int index;
	local string url;

    Index = int(lb_MapList.List.GetExtra() );
	url = OwnageMaps[Index].MapURL;

    if (url!="")
    	Controller.LaunchURL(Url);

    return true;
}

Function ListOnChange(GUIComponent Sender)
{
	local int i;

    i = int(lb_MapList.List.GetExtra());
    lb_MapInfo.SetContent(OwnageMaps[i].MapDesc,"|");
}

function AddMap(int Level, string mName, string mDesc, string mURL)
{
	local int i,Index;

    Index = -1;
    for (i=0;i<OwnageMaps.Length;i++)
    	if (OwnageMaps[i].RLevel == Level)
        	Index = i;

	if (Index==-1)
    {
    	Index = OwnageMaps.Length;
        OwnageMaps.Length = OwnageMaps.Length+1;
        OwnageMaps[Index].RLevel = Level;
    }

	if (mName!="")
    	OwnageMaps[Index].MapName = mName;

    if (mDesc!="")
    	OwnageMaps[Index].MapDesc = OwnageMaps[Index].MapDesc$mDesc;

    if (mURL!="")
    	OwnageMaps[Index].MapURL = mUrl;

	Controller.SaveOwnageList(OwnageMaps);
    PrimeMapList();
}

defaultproperties
{

	Begin Object class=GUISectionBackground Name=sb1
		WinWidth=0.408084
		WinHeight=0.960281
		WinLeft=0.012527
		WinTop=0.012761
        Caption="Ownage Maps"
        RenderWeight=0.01
        BottomPadding=0.2
        bFillClient=true
	End Object
	sb_1=sb1


	Begin Object class=AltSectionBackground Name=sb2
		WinWidth=0.562541
		WinHeight=0.965263
		WinLeft=0.431054
		WinTop=0.012761
        Caption="Map Details"
        RenderWeight=0.01
        bFillClient=true
	End Object
	sb_2=sb2

	// char stats
	Begin Object class=GUIListBox Name=lbMapList
		WinWidth=0.265626
		WinHeight=0.735548
		WinLeft=0.030468
		WinTop=0.109375
        TabOrder=0
        bVisibleWhenEmpty=true
        OnChange=ListOnChange
	End Object

    lb_MapList=lbMapList

	Begin Object class=GUIScrollTextBox Name=lbMapInfo
		WinWidth=0.655274
		WinHeight=0.735548
		WinLeft=0.305664
		WinTop=0.109725
        TabOrder=1
        bVisibleWhenEmpty=true
        bNoTeletype=true
	End Object

    lb_MapInfo=lbMapInfo

	Tag=2


}
