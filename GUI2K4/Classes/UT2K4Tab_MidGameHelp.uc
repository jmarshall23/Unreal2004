// ====================================================================
// Tab for login/midgame menu that shows game description and helpful hints
//
// Written by Matt Oelfke
// (C) 2003, Epic Games, Inc. All Rights Reserved
// ====================================================================

class UT2K4Tab_MidGameHelp extends MidGamePanel;

var bool bReceivedGameClass;

var automated GUISectionBackground sb_GameDesc, sb_Hints;

var automated GUIScrollTextBox GameDescriptionBox, HintsBox;
var automated GUILabel HintCountLabel;
var automated GUIButton PrevHintButton, NextHintButton;
var class<GameInfo> GameClass;
var array<string> AllGameHints;
var int CurrentHintIndex;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.Initcomponent(MyController, MyOwner);
	sb_GameDesc.ManageComponent(GameDescriptionBox);
	sb_Hints.ManageComponent(HintsBox);

	PrevHintButton.bBoundToParent=false;  PrevHintButton.bScaleToParent=false;
	NextHintButton.bBoundToParent=false;  NextHintButton.bScaleToParent=false;
	HintCountLabel.bBoundToParent=false;  HintCountLabel.bScaleToParent=false;

}

function ShowPanel(bool bShow)
{
	Super.ShowPanel(bShow);

	if (bShow && !bReceivedGameClass)
	{
		SetTimer(1.0, true);
		Timer();
	}
}

function Timer()
{
	local PlayerController PC;
	local int i;

	PC = PlayerOwner();
	if (PC != None && PC.GameReplicationInfo != None && PC.GameReplicationInfo.GameClass != "")
	{
		GameClass = class<GameInfo>(DynamicLoadObject(PC.GameReplicationInfo.GameClass, class'Class'));
		if (GameClass != None)
		{
			//get game description and hints from game class
			GameDescriptionBox.SetContent(GameClass.default.Description);
			AllGameHints = GameClass.static.GetAllLoadHints();
			if (AllGameHints.length > 0)
			{
				for (i = 0; i < AllGameHints.length; i++)
				{
					AllGameHints[i] = GameClass.static.ParseLoadingHint(AllGameHints[i], PC, HintsBox.Style.FontColors[HintsBox.MenuState]);
					if (AllGameHints[i] == "")
					{
						AllGameHints.Remove(i, 1);
						i--;
					}
				}
				HintsBox.SetContent(AllGameHints[CurrentHintIndex]);
				HintCountLabel.Caption = string(CurrentHintIndex + 1) @ "/" @ string(AllGameHints.length);
				EnableComponent(PrevHintButton);
				EnableComponent(NextHintButton);
			}

			KillTimer();
			bReceivedGameClass = true;
		}
	}
}

function bool ButtonClicked(GUIComponent Sender)
{
	if (Sender == PrevHintButton)
	{
		CurrentHintIndex--;
		if (CurrentHintIndex < 0)
			CurrentHintIndex = AllGameHints.length - 1;
	}
	else if (Sender == NextHintButton)
	{
		CurrentHintIndex++;
		if (CurrentHintIndex >= AllGameHints.length)
			CurrentHintIndex = 0;
	}

	HintsBox.SetContent(AllGameHints[CurrentHintIndex]);
	HintCountLabel.Caption = string(CurrentHintIndex + 1) @ "/" @ string(AllGameHints.length);

	return true;
}

function bool FixUp(Canvas C)
{
	local float t,h,l,w,xl;

    h = 20;
    t = sb_Hints.ActualTop() + sb_Hints.ActualHeight() -  27;

	PrevHintButton.WinLeft = sb_Hints.ActualLeft() + 40;
	PrevHintButton.WinTop = t;
	PrevHintButton.WinHeight=h;

	NextHintButton.WinLeft = sb_Hints.ActualLeft() + sb_Hints.ActualWidth() - 40 - NextHintButton.ActualWidth();
	NextHintButton.WinTop = t;
	NextHintButton.WinHeight=h;

	l = PrevHintButton.ActualLeft() + PrevHintButton.ActualWidth();
	w = NextHintButton.ActualLeft() - L;

	XL = HintCountLabel.ActualWidth();
	l = l + (w/2) - (xl/2);
	HintCountLabel.WinLeft=l;
	HintCountLabel.WinTop=t;
	HintCountLabel.WinWidth = xl;
	HintCountLabel.WinHeight=h;

	return false;
}

defaultproperties
{
	OnPreDraw=Fixup;

	Begin Object class=AltSectionBackground name=sbGameDesc
		WinWidth=0.944875
		WinHeight=0.455783
		WinLeft=0.023625
		WinTop=0.020438
		bBoundToParent=true
		bScaleToParent=true
		bFillClient=true
		Caption="Game Description"
	End Object
	sb_GameDesc=sbGameDesc;

	Begin Object Class=GUIScrollTextBox Name=InfoText
		WinWidth=1.000000
		WinHeight=0.316016
		WinLeft=0.000000
		WinTop=0.143750
		CharDelay=0.0025
		EOLDelay=0
		StyleName="NoBackground"
		bNoTeletype=true
		bNeverFocus=true
		TextAlign=TXTA_Center
		bBoundToParent=true
		bScaleToParent=true
	End Object
	GameDescriptionBox=InfoText


	Begin Object class=AltSectionBackground name=sbHints
		WinWidth=0.881875
		WinHeight=0.436125
		WinLeft=0.055125
		WinTop=0.482921
		bBoundToParent=true
		bScaleToParent=true
		bFillClient=true
		Caption="Helpful Hints"
	End Object
	sb_Hints=sbHints;

	Begin Object Class=GUIScrollTextBox Name=HintText
		WinWidth=1.000000
		WinHeight=0.266016
		WinLeft=0.000000
		WinTop=0.593750
		CharDelay=0.0025
		EOLDelay=0
		StyleName="NoBackground"
		bNoTeletype=true
		bNeverFocus=true
		TextAlign=TXTA_Center
		bBoundToParent=true
		bScaleToParent=true
	End Object
	HintsBox=HintText

	Begin Object Class=GUIButton Name=PrevHint
		Caption="Previous Hint"
		StyleName="SquareButton"
		OnClick=ButtonClicked
		WinWidth=0.226801
		WinHeight=0.042125
		WinLeft=0.131500
		WinTop=0.900000
		bAutoSize=True
		TabOrder=0
		MenuState=MSAT_Disabled
	End Object
	PrevHintButton=PrevHint

	Begin Object Class=GUIButton Name=NextHint
		Caption="Next Hint"
		StyleName="SquareButton"
		OnClick=ButtonClicked
		WinWidth=0.159469
		WinHeight=0.042125
		WinLeft=0.698425
		WinTop=0.900000
		bAutoSize=True
		TabOrder=1
		MenuState=MSAT_Disabled
	End Object
	NextHintButton=NextHint

	Begin Object class=GUILabel Name=HintCount
		TextALign=TXTA_Center
		TextColor=(R=255,G=255,B=255,A=255)
		WinWidth=0.400000
		WinHeight=32.000000
		WinLeft=0.300000
		WinTop=0.900000
	End Object
	HintCountLabel=HintCount
}
