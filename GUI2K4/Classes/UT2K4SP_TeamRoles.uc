//==============================================================================
// Set the team roles for the next match
//
// Written by Michiel Hendriks
// (c) 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================

class UT2K4SP_TeamRoles extends LargeWindow;

#exec OBJ LOAD FILE=2K4Menus.utx

var UT2K4GameProfile GP;
var material PortraitBGMat;

var array<GUIImage> imgFaces;
var array<GUIComboBox> cbRoles;

var automated GUIButton btnOk, btnCancel;
var automated GUILabel lblTitle, lblInfo, lblNoMoney, lblBalance;
var automated GUIScrollTextBox sbCharInfo;

var localized string InactiveMsg, MinimalFeeCaption, InjuredCaption;
var localized string AggressionCaption, AccuracyCaption, AgilityCaption, TacticsCaption;

/** Used to check the number of roles assigned */
var int MaxTeamMates;
/** Currently assigned roles */
var int AssignedRoles;
/** used to check if the current team can be payed */
var array<int> TeamFee;
/** the fee for the currently selected team */
var int CurrentFee;
/** if true no balance check is done */
var bool bNoBalanceCheck;

/**	Will be called when the user clicks the "ok" button */
delegate OnOkClick();

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local MatchInfo mi;
	local CacheManager.MapRecord MR;
	bInit = true;

	Super.Initcomponent(MyController, MyOwner);
	GP = UT2K4GameProfile(PlayerOwner().Level.Game.CurrentGameProfile);
	if (GP == none)
	{
		Log("GP == none");
		Controller.CloseMenu(false);
		return;
	}

	MaxTeamMates = GP.GetNumTeammatesForMatch();
	lblInfo.Caption = repl(lblInfo.Caption, "%teammates%", string(MaxTeamMates));
	lblInfo.Caption = repl(lblInfo.Caption, "%gametype%", GP.GetMatchDescription());
	lblBalance.Caption = repl(lblBalance.Caption, "%balance%", GP.MoneyToString(GP.Balance));

	mi = GP.GetMatchInfo(GP.CurrentLadder, GP.CurrentMenuRung);
	if ( mi.MenuName != "" ) lblInfo.Caption = repl("%map%", mi.MenuName, lblInfo.Caption);
	else {
		MR = class'CacheManager'.static.GetMapRecord(mi.LevelName);
		if (MR.FriendlyName != "") lblInfo.Caption = repl(lblInfo.Caption, "%map%", MR.FriendlyName);
		else lblInfo.Caption = repl(lblInfo.Caption, "%map%", MR.MapName);
	}

}

event HandleParameters(string Param1, string Param2)
{
	bNoBalanceCheck = (Param1 != "");
	drawButtons();
	bInit = false;
}

/** draw team member faces and role selection controlls */
function drawButtons()
{
	local int i, j, health;
	local GUIImage img, img2;
	local GUIComboBox cb;
	local GUILabel lbl;
	local GUIButton btn;
	local float X, Y, BaseWidth, Spacing;
	local xUtil.PlayerRecord PR;

	BaseWidth = i_FrameBG.WinWidth/(1+GP.GetMaxTeamSize());
	Spacing = BaseWidth/(1+GP.GetMaxTeamSize());
	X = i_FrameBG.WinLeft+Spacing;
	Y = lblInfo.WinTop+lblInfo.WinHeight;
	AssignedRoles = 0;
	CurrentFee = 0;

	for (i = 0; i < GP.GetMaxTeamSize(); i++)
	{
		PR = class'xGame.xUtil'.static.FindPlayerRecord(GP.PlayerTeam[i]);
		j = GP.GetBotPosition(PR.DefaultName);
		if (j == -1)
		{
			TeamFee[i] = 0;
			health = 100;
		}
		else {
			TeamFee[i] = GP.BotStats[j].Price;
			health = GP.BotStats[j].Health;
		}

		img = new class'GUIImage';
		img.WinLeft = X;
		img.WinTop = Y;
		img.WinWidth = BaseWidth;
		img.WinHeight = img.WinWidth*2.7;
		img.Image = PortraitBGMat;
		img.ImageStyle = ISTY_Scaled;
		img.RenderWeight = 0.11;
		img.Hint = PR.DefaultName;
		img.bBoundToParent = true;
		AppendComponent(img, true);

		btn = new class'GUIButton';
		btn.WinLeft = img.WinLeft;
		btn.WinTop = img.WinTop;
		btn.WinWidth = img.WinWidth;
		btn.WinHeight = img.WinHeight;
		btn.RenderWeight = 0.5;
		btn.OnHover = ShowPlayerDetails;
		btn.StyleName = "NoBackground";
		btn.Tag = i;
		btn.bBoundToParent = true;
		AppendComponent(btn, true);

		img2 = new class'GUIImage';
		img2.WinLeft = img.WinLeft+0.00125;
		img2.WinTop = img.WinTop+0.008;
		img2.WinWidth = img.WinWidth-0.0025;
		img2.WinHeight = img.WinHeight-0.015;
		img2.Image = PR.Portrait;
		img2.ImageStyle = ISTY_Scaled;
		img2.RenderWeight = 0.12;
		img2.Hint = PR.DefaultName;
		img2.ImageRenderStyle = MSTY_Alpha;
		img2.ImageColor.A = 255;
		img2.bBoundToParent = true;
		if (Health < 100)
		{
			img2.ImageColor.G = Health*1.5;
			img2.ImageColor.B = Health*1.5;
		}
		AppendComponent(img2, true);
		imgFaces[i] = img2;

		lbl = new class'GUILabel';
		lbl.WinLeft = img.WinLeft;
		lbl.WinWidth = img.WinWidth;
		lbl.WinHeight = 0.09;
		lbl.WinTop = img.WinTop+0.01;
		lbl.RenderWeight = 0.13;
		lbl.StyleName = "TextLabel";
		lbl.bMultiLine = true;
		if (j > -1) lbl.Caption = GP.BotStats[j].Name;
		lbl.TextALign = TXTA_Center;
		lbl.ShadowOffsetX = 1.5;
		lbl.ShadowOffsetY = 1.5;
		lbl.bBoundToParent = true;
		AppendComponent(lbl, true);

		if (!bNoBalanceCheck)
		{
			lbl = new class'GUILabel';
			lbl.WinLeft = img.WinLeft-0.01;
			lbl.WinWidth = img.WinWidth;
			lbl.WinHeight = 0.09;
			lbl.WinTop = img.WinTop+(img.WinHeight*(WinWidth/WinHeight))-lbl.WinHeight;
			lbl.RenderWeight = 0.13;
			lbl.StyleName = "TextLabel";
			lbl.bMultiLine = true;
			lbl.Caption = repl(MinimalFeeCaption, "%fee%", GP.MoneyToString(round(float(TeamFee[i])*GP.LoserFee)));
			lbl.TextALign=TXTA_Right;
			lbl.ShadowOffsetX = 1.5;
			lbl.ShadowOffsetY = 1.5;
			lbl.bBoundToParent = true;
			AppendComponent(lbl, true);
		}

		if (Health < 100)
		{
			lbl = new class'GUILabel';
			lbl.WinLeft = img.WinLeft;
			lbl.WinWidth = img.WinWidth;
			lbl.WinHeight = 0.05;
			lbl.WinTop = img.WinTop+(img.WinHeight/3);
			lbl.RenderWeight = 0.13;
			lbl.TextColor.R = 255;
			lbl.TextColor.G = 0;
			lbl.TextColor.B = 0;
			lbl.Caption = InjuredCaption;
			lbl.TextALign=TXTA_Center;
			lbl.bBoundToParent = true;
			AppendComponent(lbl, true);
		}

		cb = new class'GUIComboBox';
		cb.WinLeft = X-0.005;
		cb.WinTop = img.WinTop+(img.WinHeight*(WinWidth/WinHeight));
		cb.WinWidth = BaseWidth+0.01;
		cb.WinHeight = 0.036;
		cb.bReadOnly = true;
		cb.bShowListOnFocus = true;
		cb.OnChange = onRoleChange;
		cb.tag = i;
		cb.TabOrder = 3+i;
		cb.bBoundToParent = true;
		cb.Edit.FontScale = FNS_Small;
		AppendComponent(cb);
		cbRoles[i] = cb;

		for (j = 0; j < GP.GetNumPositions(); j++)
		{
			cb.AddItem(GP.PositionName[j]);
		}
		cb.AddItem(InactiveMsg);
		cb.Find(InactiveMsg);
		// only set assigned role when there's still room
		if ((AssignedRoles < MaxTeamMates) && (img2.Image != none) && (Health >= 100))
		{
			for (j = 0; j < GP.LINEUP_SIZE; j++)
			{
				if (GP.PlayerLineup[j] == i)
				{
					cb.Find(GP.TextPositionDescription(GP.PlayerPositions[i]));
					if(!bNoBalanceCheck) CurrentFee += TeamFee[i];
					AssignedRoles++;
					break;
				}
			}
		}
		if ((cb.Get() ~= InactiveMsg) || (img2.Image == none) || (Health < 100))
		{
			// no free spot, so disable
			cb.DisableMe();
			img2.ImageColor.A = 128;
		}

		X += BaseWidth+Spacing;
	}
	// check to see if we need more team mates
	for (i = 0; i < GP.GetMaxTeamSize(); i++)
	{
		if ((cbRoles[i].Get() ~= InactiveMsg) && (imgFaces[i].Image != none)
			&& (imgFaces[i].ImageColor.G >= 255) && (AssignedRoles < MaxTeamMates)) // activate player
		{
			cbRoles[i].SetIndex(0);	// auto assign
			cbRoles[i].EnableMe();
			imgFaces[i].ImageColor.A = 255;
			AssignedRoles++;
			if(!bNoBalanceCheck) CurrentFee += TeamFee[i];
		}
	}
	setOkButtonState();
}

function bool InternalOnClick(GUIComponent Sender)
{
	local int i, cpos;
	local bool res;

	res = Controller.CloseMenu(Sender != btnOk);
	if (Sender == btnOk)
	{
		// reset old data
		for (i = 0; i < GP.LINEUP_SIZE; i++)
		{
			GP.PlayerLineup[i] = -1;
		}
		// set new data
		cpos = 0;
		for (i = 0; i < cbRoles.length; i++)
		{
			if (cbRoles[i].Get() != InactiveMsg)
			{
				GP.SetLineup(cpos, i);
				GP.SetPosition(cpos, cbRoles[i].Get());
				cpos++;
			}
		}
		OnOkClick();
	}
	return res;
}

/**
	A role has changed, check the constraints
*/
function onRoleChange(GUIComponent Sender)
{
	local int i;
	if (bInit) return;

	AssignedRoles = 0;
	CurrentFee = 0;
	if (cbRoles[Sender.Tag].Get() ~= InactiveMsg)	imgFaces[Sender.Tag].ImageColor.A = 128;
		else imgFaces[Sender.Tag].ImageColor.A = 255;
	for (i = 0; i < cbRoles.length; i++)
	{
		if (cbRoles[i].Get() != InactiveMsg)
		{
			AssignedRoles++;
			if(!bNoBalanceCheck) CurrentFee += TeamFee[i];
		}
	}
	// enable/disable assigned roles
	for (i = 0; i < cbRoles.length; i++)
	{
		if ((cbRoles[i].Get() ~= InactiveMsg) && (imgFaces[i].Image != none)
			&& (imgFaces[i].ImageColor.G >= 255))
		{
			if (AssignedRoles < MaxTeamMates)
			{
				cbRoles[i].EnableMe();
			}
			else {
				cbRoles[i].DisableMe();
			}
		}
	}
	// enable ok if all is assigned
	setOkButtonState();
}

function setOkButtonState()
{
	local bool enable;
	enable = (AssignedRoles == MaxTeamMates);
	if(!bNoBalanceCheck)
	{
		if ((float(CurrentFee)*GP.LoserFee) < GP.Balance)
		{ // has enough cache to pay out
			enable = enable && true;
			lblNoMoney.bVisible = false;
		}
		else {
			enable = false;
			lblNoMoney.bVisible = true;
		}
	}
	else lblNoMoney.bVisible = false;
	if (enable) btnOk.EnableMe();
		else btnOk.DisableMe();
}

function bool ShowPlayerDetails( GUIComponent Sender )
{
	local string PlayerDetails;
	local XUtil.PlayerRecord ActivePR;

	if (Sender.Tag < 0) return false;

	ActivePR = class'XUtil'.static.FindPlayerRecord(GP.PlayerTeam[Sender.Tag]);
	PlayerDetails = ActivePR.DefaultName;
	PlayerDetails $= "|"$AccuracyCaption@class'XUtil'.static.AccuracyRating(ActivePR)$"%";
	PlayerDetails $= "|"$AggressionCaption@class'XUtil'.static.AggressivenessRating(ActivePR)$"%";
	PlayerDetails $= "|"$AgilityCaption@class'XUtil'.static.AgilityRating(ActivePR)$"%";
	PlayerDetails $= "|"$TacticsCaption@class'XUtil'.static.TacticsRating(ActivePR)$"%";
	PlayerDetails $= "|"$class'xUtil'.static.GetFavoriteWeaponFor(ActivePR);
	sbCharInfo.SetContent(PlayerDetails);
	return true;
}

defaultproperties
{
	Begin Object Class=GUIButton Name=SPMbtnOk
		Caption="OK"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.764064
		WinTop=0.912292
		OnClick=InternalOnClick
		RenderWeight=0.2
		FontScale=FNS_Small
		TabOrder=1
		bBoundToParent=True
	End Object
	btnOk=SPMbtnOk

	Begin Object Class=GUIButton Name=SPTbtnCancel
		Caption="CANCEL"
		WinWidth=0.200000
		WinHeight=0.040000
		WinLeft=0.532813
		WinTop=0.912292
		OnClick=InternalOnClick
		RenderWeight=0.2
		FontScale=FNS_Small
		TabOrder=2
		bBoundToParent=True
	End Object
	btnCancel=SPTbtnCancel

	Begin Object Class=GUILabel Name=SPTlblTitle
		Caption="Team roster"
		WinWidth=0.806251
		WinHeight=0.065000
		WinLeft=0.089063
		WinTop=0.041666
		RenderWeight=0.2
		StyleName="TextLabel"
		FontScale=FNS_Large
		TextALign=TXTA_Center
		bBoundToParent=True
	End Object
	lblTitle=SPTlblTitle

	Begin Object Class=GUILabel Name=SPTlblInfo
		Caption="%gametype% in %map% with %teammates% team mates"
		WinWidth=0.931251
		WinHeight=0.058750
		WinLeft=0.032813
		WinTop=0.100000
		RenderWeight=0.2
		StyleName="TextLabel"
		bMultiLine=true
		TextALign=TXTA_Center
		bBoundToParent=True
	End Object
	lblInfo=SPTlblInfo

	Begin Object Class=GUILabel Name=SPTlblNoMoney
		Caption="You do not have enough money to pay this team's minimal fee"
		WinWidth=0.431250
		WinHeight=0.102500
		WinLeft=0.532813
		WinTop=0.758334
		RenderWeight=0.2
		TextColor=(R=228,G=0,B=0,A=255)
		bMultiLine=true
		TextALign=TXTA_Center
		//bVisible=false
		ShadowOffsetX=1
		ShadowOffsetY=1
		bBoundToParent=True
	End Object
	lblNoMoney=SPTlblNoMoney

	Begin Object Class=GUILabel Name=SPTlblBalance
		Caption="Balance: %balance%"
		WinWidth=0.415000
		WinHeight=0.040000
		WinLeft=0.556563
		WinTop=0.049999
		RenderWeight=0.2
		StyleName="TextLabel"
		bMultiLine=true
		TextALign=TXTA_Right
		bBoundToParent=True
	End Object
	lblBalance=SPTlblBalance

	Begin Object Class=GUIScrollTextBox Name=SPTsbCharInfo
		WinWidth=0.468750
		WinHeight=0.212500
		WinLeft=0.032500
		WinTop=0.726666
		bNoTeletype=true
		RenderWeight=0.2
		bVisibleWhenEmpty=true
		FontScale=FNS_Medium
		TabOrder=3
		bBoundToParent=True
	End Object
	sbCharInfo=SPTsbCharInfo

	PortraitBGMat=material'2K4Menus.Controls.sectionback'

	DefaultWidth=1
	DefaultHeight=0.88
	DefaultLeft=0.00
	DefaultTop=0.06

	WinWidth=1
	WinHeight=0.88
	WinLeft=0.00
	WinTop=0.06

	InactiveMsg="INACTIVE"
	MinimalFeeCaption="Minimal fee:|%fee%"
	InjuredCaption="INJURED"

	AggressionCaption="Aggressiveness:"
	AgilityCaption="Agility:"
	AccuracyCaption="Accuracy:"
	TacticsCaption="Tactics:"
}
