//-----------------------------------------------------------
//
//-----------------------------------------------------------
class UT2K4ModFooter extends ButtonFooter;

var automated GUIButton b_Activate, b_Web, b_Download, b_Dump, b_Watch, b_Back;
var UT2K4ModsAndDemos MyPage;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController,MyOwner);
    MyPage = UT2K4ModsAndDemos(MyOwner);
}

function bool BackClick(GUIComponent Sender)
{
	Controller.CloseMenu(false);
	return true;
}

function TabChange(int NewTag)
{
	local int i;
	local GUIButton B;
	for (i=0;i<Components.Length;i++)
	{
		B = GUIButton(Components[i]);
		if ( b!=None && b.Tag>0 )
		{
			if (b.Tag==NewTag)
				b.SetVisibility(true);
			else
				b.SetVisibility(false);
		}
	}

	SetupButtons("true");
}

defaultproperties
{
    Begin Object Class=GUIButton Name=BB1
        Caption="Activate"
        Hint="Activates the selected mod"
		WinWidth=0.12
        WinLeft=0.885352
		WinHeight=0.036482
		WinTop=0.085678
        RenderWeight=2
        TabOrder=0
        bBoundToParent=True
        StyleName="FooterButton"
		Tag=1
		bVisible=false
    End Object
    b_Activate=BB1

    Begin Object Class=GUIButton Name=BB2
        Caption="Visit Web Site"
        Hint="Visit the web site of the selected mod"
		WinWidth=0.12
        WinLeft=0.885352
		WinHeight=0.036482
		WinTop=0.085678
        RenderWeight=2.0001
        TabOrder=1
        bBoundToParent=True
        StyleName="FooterButton"
		bVisible=false
		Tag=1
    End Object
    b_Web=BB2

    Begin Object Class=GUIButton Name=BB3
        Caption="Download"
        Hint="Download the selected Ownage map"
        WinLeft=0.885352
		WinHeight=0.036482
		WinWidth=0.12
		WinTop=0.085678
        RenderWeight=2.0001
        TabOrder=2
        bBoundToParent=True
        StyleName="FooterButton"
		bVisible=false
		Tag=2
    End Object
    b_Download=BB3

    Begin Object Class=GUIButton Name=BB5
        Caption="Create AVI"
        Hint="Convert the selected demo to a DIVX AVI"
		WinWidth=0.12
		WinHeight=0.036482
		WinTop=0.085678
        RenderWeight=2.0001
        TabOrder=3
        bBoundToParent=True
        StyleName="FooterButton"
		bVisible=false
		Tag=3
    End Object
	b_Dump=BB5

    Begin Object Class=GUIButton Name=BB4
        Caption="Watch Demo"
        Hint="Watch the selected demo"
		WinHeight=0.036482
		WinWidth=0.12
		WinTop=0.085678
        RenderWeight=2.0001
        TabOrder=4
        bBoundToParent=True
        StyleName="FooterButton"
		bVisible=false
		Tag=3
    End Object
	b_Watch=BB4


    Begin Object Class=GUIButton Name=BackB
        Caption="BACK"
        Hint="Return to the previous menu"
		WinHeight=0.036482
		WinWidth=0.12
		WinTop=0.085678
        RenderWeight=2.0002
        TabOrder=3
        bBoundToParent=True
        StyleName="FooterButton"
		OnClick=BackClick
		Tag=0
    End Object
	b_Back=BackB
	Margin=0.01
	Padding=0.3
	Spacer=0.01

//	bFixedWidth=false;

}
