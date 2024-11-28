//==============================================================================
//	Created on: 10/10/2003
//	Base class for non-fullscreen menus
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class PopupPageBase extends UT2K4GUIPage;

var automated FloatingImage    i_FrameBG;
var bool                       bFading, bClosing;
var(Fade) config float         FadeTime;
var(Fade) float                CurFadeTime;
var(Fade) byte                 CurFade, DesiredFade;

delegate FadedIn();
delegate FadedOut();

event Opened(GUIComponent Sender)
{
	if ( bCaptureInput )
		FadeIn();

	Super.Opened(Sender);
}

function bool InternalOnPreDraw( Canvas C )
{
	if ( !bFading )
		return false;

	if (CurFadeTime >= 0.0)
	{
		CurFade += float(DesiredFade - CurFade) * (Controller.RenderDelta / CurFadeTime);
		InactiveFadeColor = class'Canvas'.static.MakeColor(CurFade, CurFade, CurFade);
		CurFadeTime -= Controller.RenderDelta;

		if ( CurFadeTime < 0 )
		{
			CurFade = DesiredFade;
			InactiveFadeColor = class'Canvas'.static.MakeColor(CurFade, CurFade, CurFade);
			bFading = False;
			if ( bClosing )
			{
				bClosing = False;
				FadedOut();
			}
			else
				FadedIn();
		}
	}

    return false;
}

function FadeIn()
{
	if ( Controller.bModulateStackedMenus )
	{
		bClosing = False;
		bFading = True;
		CurFadeTime = FadeTime;
	}
	else FadedIn();
}

function FadeOut()
{
	if ( Controller.bModulateStackedMenus )
	{
		bFading = True;
		bClosing = True;
		CurFadeTime = FadeTime;
		DesiredFade = default.CurFade;
	}
	else FadedOut();
}

defaultproperties
{

	Begin Object Class=FloatingImage Name=FloatingFrameBackground
		Image=Material'2K4Menus.NewControls.Display1'
		ImageRenderStyle=MSTY_Normal
		ImageStyle=ISTY_Stretched
		ImageColor=(R=255,G=255,B=255,A=255)
		DropShadow=None
		WinWidth=1
		WinHeight=0.98
		WinLeft=0
		WinTop=0.02
		RenderWeight=0.000003
		bBoundToParent=True
		bScaleToParent=True
	End Object
	i_FrameBG=FloatingFrameBackground

	bRequire640x480=False
	bRenderWorld=True
	bCaptureInput=True

	DesiredFade=80
	CurFade=200
	FadeTime=0.35

	OnPreDraw=InternalOnPreDraw
	BackgroundColor=(R=255,G=255,B=255,A=255)
	BackgroundRStyle=MSTY_Modulated
}
