//==============================================================================
// Notification of a team mate injury
//
// Written by Michiel Hendriks
// (c) 2003, 2004, Epic Games, Inc.  All Rights Reserved
//==============================================================================

class UT2K4SP_Injury extends LargeWindow;

var automated GUIImage	imgPictureBg, imgPicture;
var automated GUIButton btnCancel, btnTreat;
var automated GUILabel lblTitle, lblMessage;

var UT2K4GameProfile GP;
/** price to heal the injured player */
var int TreatmentCost;
/** the offset in the GameProfile's botstats array, set in HandleParameters */
var int InjuredPlayer;

event HandleParameters(string Param1, string Param2)
{
	local XUtil.PlayerRecord PR;
	InjuredPlayer = int(Param1);
	GP = UT2K4GameProfile(PlayerOwner().Level.Game.CurrentGameProfile);
	if ((GP == none) || (InjuredPlayer < 0) || (InjuredPlayer >= GP.BotStats.length))
	{
		Warn("Invalid injury menu request:"@(GP == none)@(InjuredPlayer < 0)@(InjuredPlayer >= GP.BotStats.length));
		Controller.CloseMenu(true);
		return;
	}
	if (GP.BotStats[InjuredPlayer].Health >= 100)
	{
		Controller.CloseMenu(true);
		return;
	}
	lblMessage.Caption = repl(lblMessage.Caption, "%player%", GP.BotStats[InjuredPlayer].Name);
	lblMessage.Caption = repl(lblMessage.Caption, "%health%", GP.BotStats[InjuredPlayer].Health);
	TreatmentCost = round((100-GP.BotStats[InjuredPlayer].Health)*GP.BotStats[InjuredPlayer].Price/100*GP.InjuryTreatment);
	lblMessage.Caption = repl(lblMessage.Caption, "%treatment%", GP.MoneyToString(TreatmentCost));
	lblMessage.Caption = repl(lblMessage.Caption, "%balance%", GP.MoneyToString(GP.Balance));
	PR = class'xGame.xUtil'.static.FindPlayerRecord(GP.BotStats[InjuredPlayer].Name);
	imgPicture.Image = PR.Portrait;
	if (TreatmentCost > GP.Balance-GP.MinBalance) btnTreat.DisableMe(); // cant afford it, but still report
}

function bool btnCancelOnClick(GUIComponent Sender)
{
	Controller.CloseMenu(true);
	return true;
}

function bool btnTreatOnClick(GUIComponent Sender)
{
	GP.BotStats[InjuredPlayer].Health = 100;
	GP.Balance -= TreatmentCost;
	Controller.CloseMenu(false);
	return true;
}

defaultproperties
{
	Begin Object Class=GUIButton Name=SPMbtnCancel
		Caption="CANCEL"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.051563
		WinTop=0.857292
		OnClick=btnCancelOnClick
		FontScale=FNS_Small
		bBoundToParent=true
	End Object
	btnCancel=SPMbtnCancel

	Begin Object Class=GUIButton Name=SPMbtnTreat
		Caption="TREAT NOW"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.664063
		WinTop=0.857292
		OnClick=btnTreatOnClick
		FontScale=FNS_Small
		bBoundToParent=true
	End Object
	btnTreat=SPMbtnTreat

	Begin Object class=GUIImage name=SPMimgPicture
		WinWidth=0.174500
		WinHeight=0.452000
		WinLeft=0.059000
		WinTop=0.146500
		Image=Material'2K4Menus.Controls.thinpipe_b'
		ImageColor=(R=255,G=255,B=255,A=255);
		ImageRenderStyle=MSTY_Normal
		ImageStyle=ISTY_Scaled
		RenderWeight=0.3
		bBoundToParent=true
	End Object
	imgPicture=SPMimgPicture

	Begin Object class=GUIImage name=SPMimgPictureBg
		WinWidth=0.177000
		WinHeight=0.464500
		WinLeft=0.057750
		WinTop=0.136499
		ImageRenderStyle=MSTY_Normal
		Image=Material'2K4Menus.Controls.sectionback'
		ImageStyle=ISTY_Scaled
		RenderWeight=0.2
		bBoundToParent=true
	End Object
	imgPictureBg=SPMimgPictureBg

	Begin Object Class=GUILabel Name=SPLlblTitle
		WinWidth=0.661250
		WinHeight=0.072500
		WinLeft=0.028750
		WinTop=0.051667
		Caption="Injury"
		StyleName="TextLabel"
		FontScale=FNS_Large
		TextAlign=TXTA_Center
		RenderWeight=0.4
		bBoundToParent=true
	End Object
	lblTitle=SPLlblTitle

	Begin Object Class=GUILabel Name=SPLlblMessage
		WinWidth=0.373749
		WinHeight=0.391250
		WinLeft=0.372500
		WinTop=0.176667
		Caption="%player% got injured in the last match.|%player%'s health is down to %health%%, treatment of the injuries costs %treatment%.|Balance: %balance%."
		StyleName="TextLabel"
		RenderWeight=0.4
		bMultiline=true
		bBoundToParent=true
	End Object
	lblMessage=SPLlblMessage

	DefaultWidth=0.7
	DefaultHeight=0.7
	DefaultLeft=0.15
	DefaultTop=0.15

	WinWidth=0.7
	WinHeight=0.7
	WinLeft=0.15
	WinTop=0.15
}
