// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class UT2K4UserMods extends ModsAndDemosTabs;

var automated GUISectionBackground sb_1, sb_2, sb_3;
var automated GUIListBox lb_ModList;
var automated GUIScrollTextBox lb_ModInfo;
var automated GUIImage i_ModLogo;

var localized string NoModsListText;
var localized string NoModsInfoText;

var array<class> ModClasses;

var bool bInitialized;


function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.Initcomponent(MyController, MyOwner);

    lb_ModList.List.Clear();
    LoadUserMods();
    bInitialized = true;
	ModListChange(none);

	sb_1.ManageComponent(lb_ModList);
	sb_2.Managecomponent(lb_ModInfo);
	sb_3.Managecomponent(i_ModLogo);

	MyPage.MyFooter.b_Activate.OnClick=LaunchClick;
	MyPage.MyFooter.b_Web.OnClick=WebClick;

	lb_ModInfo.MyScrollText.bClickText=true;
	lb_ModInfo.MyScrollText.OnDblClick=LaunchURL;
}

function bool LaunchURL(GUIComponent Sender)
{
    local string ClickString;

    ClickString = StripColorCodes(lb_ModInfo.MyScrollText.ClickedString);
   	Controller.LaunchURL(ClickString);
    return true;
}


function LoadUserMods()
{
	local array<string> ModDirs, ModTitles;
	local int i;

	GetModList(ModDirs, ModTitles);
	for (i=0;i<ModDirs.Length;i++)
	  	lb_ModList.List.Add(ModTitles[i],,ModDirs[i]);
}

function ModListChange(GUIComponent Sender)
{
	local Material M;
	lb_ModInfo.SetContent( GetModValue( lb_ModList.List.GetExtra(),"ModDesc" ));
	M = GetModLogo( lb_ModList.List.GetExtra() );

    if (!bInitialized)
    	return;

    if (m!=none)
    {
		i_ModLogo.Image = M;
        i_ModLogo.SetVisibility(true);

		sb_2.WinTop    = 0.300253;
		sb_2.WinHeight = 0.676279;
		sb_3.bVisible=true;

		lb_ModInfo.WinHeight=0.476758;
		lb_ModInfo.WinTop=0.376652;
    }
    else
	{
		sb_2.WinTop    = 0.012761;
		sb_2.WinHeight = 0.965263;
		sb_3.bVisible=false;

        i_ModLogo.SetVisibility(false);
		lb_ModInfo.WinHeight=0.750196;
		lb_ModInfo.WinTop=0.103215;
    }


}

function bool LaunchClick(GUIComponent Sender)
{
	local string CmdLine;

	if (lb_ModList.List.Index<0)
		return true;

	CmdLine = GetModValue( lb_ModList.List.GetExtra(), "ModCmdLine" );
    if (CmdLine!="")
		Console(Controller.Master.Console).DelayedConsoleCommand("relaunch"@CmdLine@"-mod="$lb_ModList.List.GetExtra()@"-newwindow");
	else
		Console(Controller.Master.Console).DelayedConsoleCommand("relaunch -mod="$lb_ModList.List.GetExtra()@"-newwindow");

    return true;
}

function bool WebClick(GUIComponent Sender)
{
	local string url;

	if (lb_ModList.List.Index<0)
		return true;

	url = GetModValue( lb_ModList.List.GetExtra(),"ModURL" );
	Console(Controller.Master.Console).DelayedConsoleCommand("open"@url);
    return true;
}

defaultproperties
{

	Begin Object class=GUISectionBackground Name=sb1
		WinWidth=0.408084
		WinHeight=0.960281
		WinLeft=0.012527
		WinTop=0.012761
        Caption="Mods"
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
        Caption="Description"
        RenderWeight=0.01
        bFillClient=true
	End Object
	sb_2=sb2



	Begin Object class=AltSectionBackground Name=sb3
		WinWidth=0.562541
		WinHeight=0.277682
		WinLeft=0.431054
		WinTop=0.012761
        Caption=""
        RenderWeight=0.01
        bFillClient=true
	End Object
	sb_3=sb3

	Begin Object class=GUIImage Name=iLogo
		WinWidth=0.583008
		WinHeight=0.255859
		WinLeft=0.377930
		WinTop=0.102865
		Image=Material'2K4Menus.Content.ModMenuLogo'
        DropShadow=Material'2K4Menus.Controls.shadow'
		ImageColor=(R=255,G=255,B=255,A=255);
		ImageRenderStyle=MSTY_Normal
		ImageStyle=ISTY_Scaled
        RenderWeight=0.4
        DropShadowX=0
        DropShadowY=0
        bVisible=false
	End Object
	i_ModLogo=iLogo

	// char stats
	Begin Object class=GUIListBox Name=lbModList
		WinWidth=0.333985
		WinHeight=0.749024
		WinLeft=0.030468
		WinTop=0.102865
        TabOrder=0
        bVisibleWhenEmpty=true
        OnChange=ModListChange
	End Object

    lb_ModList=lbModList

	Begin Object class=GUIScrollTextBox Name=lbModInfo
		WinWidth=0.582032
		WinHeight=0.750196
		WinLeft=0.378906
		WinTop=0.103215
        TabOrder=1
        bVisibleWhenEmpty=true
        bNoTeletype=true
	End Object

    lb_ModInfo=lbModInfo

    NoModsListText="No Mods Installed"
    NoModsInfoText="There are currently no mods or TC installed in this copy of UT2004.  Add message pimping cool places to get them here"
	PropagateVisibility=false
 	Tag=1
}
