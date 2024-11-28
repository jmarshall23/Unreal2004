//==============================================================================
// Team Management window, also used during the draft
//
// Written by Michiel Hendriks
// (c) 2003, Epic Games, Inc. All Rights Reserved
//==============================================================================

class UT2K4SPTab_TeamManagement extends UT2K4SPTab_Base;

var automated GUISectionBackground sbgTeamBg, sbgSelectionBg, sbgFreeAgentBg, sbgOverviewBg;
var automated GUIImage imgSelectionFaceBg, imgSelectionFace;
var automated GUIScrollTextBox sbDescription, sbDetails;
var automated GUICharacterListTeam clFreeAgents;
var automated GUIButton btnHire, btnFire, btnLeft, btnRight, btnTreat, btnTreatAll, btnAutoMake;
var automated GUILabel lblTeam;
var automated moEditBox edBalance;

var array<GUIImage> TeamFaces;
var array<GUIProgressBar> TeamHealth;

var localized string HealthHint, NameTitle;
var localized string AggressionCaption, AccuracyCaption, AgilityCaption, TacticsCaption, FeeCaption, HealthCaption;
var localized string msgFirstDraftYourTeam;

/**	Initial team draft */
var bool bInitialDraft;

/** Used for empty slots */
var Material EmptySlotImage;

/**	The PR of the currentl selected player */
var xUtil.PlayerRecord ActivePR;
/** Position of the currently selected bot in the bot stats list*/
var int ActiveBotPos;
/** cost to heal selected player(s) */
var int ActiveTreatmentCost, TotalTreatmentCost;

/** true when the team is full, only used with bInitialDraft */
var bool bHasFullTeam;

/** hire/fire options are locked */
var bool NoTeamEditing;

/** names in this list will be refunded when fired */
var array<string> NewTeam;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.Initcomponent(MyController, MyOwner);
	CreateTeamButtons(2, 3, 0.04);
}

function ShowPanel(bool bShow)
{
	Super.ShowPanel(bShow);
	if (bShow)
	{
		UpdateTeamList();
		FillFreeAgentsList(clFreeAgents);
		if (bInitialDraft)
		{
			btnPlayEnabled(bHasFullTeam); // "DONE" button only available during the team draft
			MainWindow.btnPlay.Caption = CaptionDone;
      		MainWindow.btnBack.Caption = CaptionCancel;
		}
		else {
			btnPlayEnabled(MainWindow.LastLadderPage != none);
			MainWindow.btnPlay.Caption = CaptionPlay;
		}
	}
}

/**
	Create the buttons for the team mates
*/
function CreateTeamButtons(int cols, int rows, optional float topOffset)
{
	local int i, n;
	local GUIImage img, img2;
	local GUIButton btn;
	local GUIProgressBar pb;
	local float colWidth, rowHeight, cellWidth, cellHeight, spacingHeight, spacingWidth, X, Y;

	rowHeight = sbgTeamBg.WinHeight / rows * 0.87;
	colWidth = sbgTeamBg.WinWidth / cols;

	cellWidth = colWidth*0.9;
	cellHeight = rowHeight*0.8;

	// calculate aspect ratio 2:5
	X = FMin(cellWidth*2.55, cellHeight);
	cellWidth = X/2.55;
	cellHeight = X;
	// find centers
	spacingHeight = (rowHeight-cellHeight)/2.0 + 0.005;
	spacingWidth = (colWidth-cellWidth)/2.0 - 0.005;

	TeamFaces.length = GP.GetMaxTeamSize();
	TeamHealth.Length = TeamFaces.length;

	for (i = 0; i < GP.GetMaxTeamSize(); i++)
	{
		X = (i % cols) * colWidth + spacingWidth + (sbgTeamBg.WinLeft/2);
		Y = (i / cols) * rowHeight + spacingHeight + topOffset + sbgTeamBg.WinTop;

		if (((i/cols)+1 == rows) && (rows*cols > GP.GetMaxTeamSize()))
		{
			// center last row
			n = rows*cols-GP.GetMaxTeamSize();
			X = X+(colWidth*n)/2.0;
		}

		// border
		img = new class'GUIImage';
		img.WinLeft = X-0.00125;
		img.WinTop = Y-0.005;
		img.WinWidth = cellWidth+0.00250; // result is 1/3 for the portrait
		img.WinHeight = cellHeight+0.009375;
		img.Image = imgSelectionFaceBg.Image;
		img.ImageStyle = ISTY_Scaled;
		img.bNeverFocus = true;
		img.bAcceptsInput = false;
		img.RenderWeight = 0.11;
		img.bBoundToParent = true;
		AppendComponent(img, true);

		// image
		img2 = new class'GUIImage';
		img2.WinLeft = X;
		img2.WinTop = Y;
		img2.WinWidth = cellWidth;
		img2.WinHeight = cellHeight;
		img2.Image = EmptySlotImage;
		img2.ImageStyle = ISTY_Scaled;
		img2.bNeverFocus = true;
		img2.bAcceptsInput = false;
		img2.RenderWeight = 0.12;
		img2.ImageRenderStyle = MSTY_Alpha;
		img2.ImageColor.A = 255;
		img2.bBoundToParent = true;
		TeamFaces[i] = img2;
		AppendComponent(img2, true);

		// button
		btn = new class'GUIButton';
		btn.WinLeft = X;
		btn.WinTop = Y;
		btn.WinWidth = cellWidth;
		btn.WinHeight = cellHeight;
		btn.RenderWeight = 0.3;
		btn.StyleName = "NoBackground";
		btn.Tag = i;
		btn.OnClick = onTeamOnClick;
		btn.TabOrder = Controls.length+1;
		btn.bDropSource = true;
		btn.bDropTarget = true;
		btn.OnDragDrop = TeamMemberOnDragDrop;
		btn.bBoundToParent = true;
		AppendComponent(btn, true);

		// health bar
		pb = new class'GUIProgressBar';
		pb.WinLeft = btn.WinLeft+btn.WinWidth;
		pb.WinTop = Y;
		pb.WinWidth = 0.015;
		pb.WinHeight = cellHeight;
		pb.Low = 0;
		pb.High = 100;
		pb.Caption = "";
		pb.bShowLow = false;
		pb.bShowHigh = false;
		pb.bShowValue = false;
		pb.CaptionWidth = 0;
		pb.ValueRightWidth = 0;
		pb.RenderWeight = 0.2;
		pb.BarDirection = DRD_BottomToTop;
		pb.bBoundToParent = true;
		TeamHealth[i] = pb;
		AppendComponent(pb, false);
	}
}

/**
	Update team listing
*/
function UpdateTeamList()
{
	local int i, bp, j;
	local xUtil.PlayerRecord PR;
	TotalTreatmentCost = 0;

	for (i = 0; i < GP.GetMaxTeamSize(); i++)
	{
		if (GP.PlayerTeam[i] != "")
		{
			PR = class'xGame.xUtil'.static.FindPlayerRecord(GP.PlayerTeam[i]);
			TeamFaces[i].Image = PR.Portrait;
			bp = GP.GetBotPosition(PR.DefaultName, true);
			TotalTreatmentCost += round((100-GP.BotStats[bp].Health)*GP.BotStats[bp].Price/100*GP.InjuryTreatment);
			TeamHealth[i].Value = GP.BotStats[bp].Health;
			TeamHealth[i].BarColor.R = 255*(1.0-float(GP.BotStats[bp].Health)/100.0);
			TeamHealth[i].BarColor.G = 255*(float(GP.BotStats[bp].Health)/100.0);
			TeamHealth[i].BarColor.B = 0;
			TeamHealth[i].Hint = Repl(Repl(HealthHint, "%player%", GP.BotStats[bp].Name), "%health%", string(GP.BotStats[bp].Health));
			TeamFaces[i].ImageColor.A = 255;
			for (j = 0; j < NewTeam.length; j++)
			{
				if (NewTeam[j] ~= PR.DefaultName)
				{
					TeamFaces[i].ImageColor.A = 127;
					break;
				}
			}
		}
		else {
			TeamFaces[i].ImageColor.A = 127;
			TeamFaces[i].Image = EmptySlotImage;
			TeamHealth[i].Value = 0;
			TeamHealth[i].Hint = "";
		}
	}
	edBalance.SetText(GP.MoneyToString(GP.Balance));
	if ((TotalTreatmentCost < GP.Balance-GP.MinBalance) && (TotalTreatmentCost > 0)) btnTreatAll.EnableMe();
		else btnTreatAll.DisableMe();
	if (TotalTreatmentCost > 0) btnTreatAll.Hint = GP.MoneyToString(TotalTreatmentCost);
		else btnTreatAll.Hint = "";
}

function bool onTeamOnClick(GUIComponent Sender)
{
	if (GP.PlayerTeam[Sender.Tag] != "")
	{
		btnHire.MenuStateChange(MSAT_Disabled);
		if (!NoTeamEditing) btnFire.EnableMe();
		ActivePR = class'xGame.xUtil'.static.FindPlayerRecord(GP.PlayerTeam[Sender.Tag]);
		showPlayerDetails(true);
	}
	/* don't reset the data
	else {
		ActivePR.DefaultName = "";
		imgSelectionFace.Image = EmptySlotImage;
		sbDescription.SetContent("");
		sbDetails.SetContent("");
	}
	*/
	return true;
}

function onFreeAgentChange(GUIComponent Sender)
{
	local int i;
	ActivePR = GUICharacterListTeam(Sender).GetRecord();
	showPlayerDetails();
	if (!NoTeamEditing) btnFire.DisableMe();
	if (GP.Balance-GP.MinBalance < (GP.GetBotPrice(ActivePR.DefaultName)))
	{
		if (!NoTeamEditing) btnHire.DisableMe();
		return;
	}
	for (i = 0; i < GP.GetMaxTeamSize(); i++)
	{
		if (GP.PlayerTeam[i] == "") // he've got a free space so enable the button
		{
			if (!NoTeamEditing) btnHire.EnableMe();
			break;
		}
	}
	return;
}

/**
	Scroll the free agent list
*/
function bool onFreeAgentScroll(GUIComponent Sender)
{
	if (clFreeAgents.Index == -1) clFreeAgents.Index = 0;
	if (Sender == btnLeft)
	{
		clFreeAgents.PgUp();
	}
	else if (Sender == btnRight)
	{
		clFreeAgents.PgDown();
	}
	return true;
}

/**
	Display details about the selected player, the "details" contain the players
	stats and price/fee
*/
function showPlayerDetails(optional bool bIsTeamMate)
{
	local string PlayerDetails, tmp;

	if (ActivePR.DefaultName == "") return;
	imgSelectionFace.Image = ActivePR.Portrait;

	PlayerDetails = Controller.LoadDecoText("", ActivePR.TextName);
	tmp = Left(PlayerDetails, InStr(PlayerDetails, "|"));
	tmp = Mid(tmp, InStr(tmp, ":")+1);
	while (Left(tmp, 1) == " ") tmp = Mid(tmp, 1);
	sbDescription.SetContent(repl(PlayerDetails, tmp, ActivePR.DefaultName)); // replace name with real name
	ActiveBotPos = GP.GetBotPosition(ActivePR.DefaultName, true);

	ActiveTreatmentCost = round((100-GP.BotStats[ActiveBotPos].Health)*GP.BotStats[ActiveBotPos].Price/100*GP.InjuryTreatment);
	if ((ActiveTreatmentCost < GP.Balance-GP.MinBalance) && (ActiveTreatmentCost > 0)) btnTreat.EnableMe();
		else btnTreat.DisableMe();

	PlayerDetails = FeeCaption@GP.MoneyToString(GP.BotStats[ActiveBotPos].Price);
	if (ActiveTreatmentCost > 0) PlayerDetails $= "|"$HealthCaption@GP.BotStats[ActiveBotPos].Health$"%|     ("$GP.MoneyToString(ActiveTreatmentCost)$")";
		else PlayerDetails $= "|";
	PlayerDetails $= "|"$AccuracyCaption@class'XUtil'.static.AccuracyRating(ActivePR)$"%";
	PlayerDetails $= "|"$AggressionCaption@class'XUtil'.static.AggressivenessRating(ActivePR)$"%";
	PlayerDetails $= "|"$AgilityCaption@class'XUtil'.static.AgilityRating(ActivePR)$"%";
	//PlayerDetails $= "|"$"Combat Style:"@class'XUtil'.static.AccuracyRating(ActivePR)$"%";
	PlayerDetails $= "|"$TacticsCaption@class'XUtil'.static.TacticsRating(ActivePR)$"%";
	//PlayerDetails $= "|"$"Reaction Time:"@class'XUtil'.static.AccuracyRating(ActivePR)$"%";
	//PlayerDetails $= "|"$"Jumpiness:"@class'XUtil'.static.AccuracyRating(ActivePR)$"%";
	PlayerDetails $= "|"$class'xUtil'.static.GetFavoriteWeaponFor(ActivePR);
	sbDetails.SetContent(PlayerDetails);
}

function bool onHireClick(GUIComponent Sender)
{
	if (NoTeamEditing) return false;
	if (HireBot(ActivePR.DefaultName, true))
	{
		btnFire.EnableMe();
		btnHire.DisableMe();
	}
	return true;
}

/** hire a bot by name and update the balance */
function bool HireBot(string botname, optional bool bUpdateList)
{
	local int i;
	if (NoTeamEditing) return false;
	if (GP.AddTeammate(botname))
	{
		for (i = 0; i < NewTeam.length; i++)
		{
			if (NewTeam[i] ~= botname) break;
		}
		NewTeam[i] = botname;
		GP.Balance -= GP.GetBotPrice(botname);
		if (bInitialDraft) // check if a full team has been selected
		{
			for (i = 0; i < GP.GetMaxTeamSize(); i++)
			{
				if (GP.PlayerTeam[i] == "") break;
			}
			bHasFullTeam = (i == GP.GetMaxTeamSize());
			btnPlayEnabled(bHasFullTeam);
		}
		if (bUpdateList)
		{
			UpdateTeamList();
			FillFreeAgentsList(clFreeAgents);
		}
		return true;
	}
	return false;
}

function bool onFireClick(GUIComponent Sender)
{
	if (NoTeamEditing) return false;
	if (FireBot(ActivePR.DefaultName, true))
	{
		btnFire.DisableMe();
		if (GP.Balance-GP.MinBalance > (GP.GetBotPrice(ActivePR.DefaultName))) btnHire.EnableMe();
			else btnHire.DisableMe();
	}
	return true;
}

/**
	fire a bot by name
	refund the cash when the player is in the dirty list
*/
function bool FireBot(string botname, optional bool bUpdateList)
{
	local int i;
	if (NoTeamEditing) return false;
	if (GP.ReleaseTeammate(botname))
	{
		for (i = 0; i < NewTeam.length; i++)
		{
			if (NewTeam[i] ~= botname)
			{
				GP.Balance += GP.GetBotPrice(botname);
				break;
			}
		}
		i = GP.GetBotPosition(botname);
		if (i > -1) GP.BotStats[i].Health = 100; // heal this bot

		if (bInitialDraft) // check if a full team has been selected
		{
			for (i = 0; i < GP.GetMaxTeamSize(); i++)
			{
				if (GP.PlayerTeam[i] == "") break;
			}
			bHasFullTeam = (i == GP.GetMaxTeamSize());
			btnPlayEnabled(bHasFullTeam);
		}
		if (bUpdateList)
		{
			UpdateTeamList();
			FillFreeAgentsList(clFreeAgents);
		}
		return true;
	}
	return false;
}

/** Play the selected match */
function bool onPlayClick()
{
	if (bInitialDraft) // show Qualification page again
	{
		MainWindow.tpQualification = UT2K4SPTab_Qualification(MainWindow.c_Tabs.ReplaceTab(MyButton, MainWindow.PanelCaption[3], MainWindow.PanelClass[3], , MainWindow.PanelHint[3], true));
		MainWindow.tpQualification.onPlayClick();	// start the match
		return true;
	}
	else if (MainWindow.LastLadderPage != none) {
		return MainWindow.LastLadderPage.onPlayClick();
	}
	return false;
}

/*
function bool CanClose(optional Bool bCancelled)
{
	local GUIQuestionPage QPage;

	if (!bInitialDraft) return true;
	if (bHasFullTeam) return true;

	if (MainWindow.c_Tabs.ActiveTab != MyButton)
		MainWindow.c_Tabs.ActivateTab(MyButton, true);
	if (Controller.OpenMenu(Controller.QuestionMenuClass))
	{
		QPage=GUIQuestionPage(Controller.TopPage());
		QPage.SetupQuestion(msgFirstDraftYourTeam, QBTN_Ok, QBTN_Ok);
	}
	return false;
}
*/

/**
	Fills the character list with all available free agents
*/
function FillFreeAgentsList(GUICharacterListTeam cl)
{
	local array<xUtil.PlayerRecord> FreeAgents;
	local int i, pos;

	class'xUtil'.static.GetPlayerList(FreeAgents);

	for (i = FreeAgents.length-1; i >= 0; i--)
	{
		// remove ourself
		if (FreeAgents[i].DefaultName ~= GP.PlayerCharacter)
		{
			FreeAgents.remove(i, 1);
			continue;
		}
		// then filter team mates
		pos = GP.GetBotPosition(FreeAgents[i].DefaultName);
		if (pos != -1)
		{
			if (GP.BotStats[pos].FreeAgent) continue;
		}
		FreeAgents.remove(i, 1); // not free, or not in the bot stats
	}
	cl.InitList();
	cl.ResetList(FreeAgents, FreeAgents.length);
}

/** heal the active player */
function bool onTreatClick(GUIComponent Sender)
{
	GP.BotStats[ActiveBotPos].Health = 100;
	GP.Balance -= ActiveTreatmentCost;
	UpdateTeamList();
	btnTreat.DisableMe();
	return true;
}

function bool onTreatAllClick(GUIComponent Sender)
{
	local int i;
	for (i = 0; i < GP.BotStats.length; i++)
	{
		GP.BotStats[i].Health = 100;
	}
	GP.Balance -= TotalTreatmentCost;
	UpdateTeamList();
	btnTreat.DisableMe();
	return true;
}

function bool TeamMemberOnDragDrop(GUIComponent Target)
{
	local string tmp;
	if (NoTeamEditing) return false;
	// no internal switching
	if ((GUIButton(Controller.DropSource) != None) && (GUIButton(Target) != None)) return false;
	// hire
	if ((GUIButton(Target) != None) && (GUICharacterListTeam(Controller.DropSource) != None))
	{
		if (GP.Balance-GP.MinBalance < (GP.GetBotPrice(GUICharacterListTeam(Controller.DropSource).GetName()))) return false; // not enough cash
		FireBot(GP.PlayerTeam[Target.Tag], false);
		HireBot(GUICharacterListTeam(Controller.DropSource).GetName(), true);
		GUICharacterListTeam(Controller.DropSource).SetOutlineAlpha(255);
		Target.OnClick(Target);
		return true;
	}
	// fire
	else if ((GUICharacterListTeam(Target) != None) && (GUIButton(Controller.DropSource) != None))
	{
		tmp = GP.PlayerTeam[Controller.DropSource.Tag];
		FireBot(tmp, true);
		GUICharacterListTeam(Target).Find(tmp);
		Target.OnChange(Target);
		return true;
	}
	else if (GUICharacterList(Target) != None) return true;
	return false;
}

function bool FreeAgentOnBeginDrag(GUIComponent Sender)
{
	if (NoTeamEditing) return false;
	if (GP.Balance-GP.MinBalance < (GP.GetBotPrice(GUICharacterListTeam(Sender).GetName()))) return false; // not enough cash
	else return GUICharacterListTeam(Sender).InternalOnBeginDrag(Sender);
}

function LockManagementTools()
{
	NoTeamEditing = true;
	btnFire.DisableMe();
	btnHire.DisableMe();
	btnAutoMake.DisableMe();
	clFreeAgents.DisableMe();
}

function bool onAutoMakeClick(GUIComponent Sender)
{
	local int i;
	local string botname;
	for (i = 0; i < GP.GetMaxTeamSize(); i++)
	{
		if (GP.PlayerTeam[i] == "")
		{
			botname = clFreeAgents.GetNameAt(i % clFreeAgents.ItemCount);
			if (GP.Balance-GP.MinBalance > GP.GetBotPrice(botname)) HireBot(botname);
			else break;
		}
	}
	UpdateTeamList();
	FillFreeAgentsList(clFreeAgents);
	return true;
}

/**
	Go back to the main menu when there are not proflies
*/
function bool onBackClick()
{
	local int i;
	if ( !bInitialDraft ) Controller.CloseMenu();
	else {
		for (i = GP.GetMaxTeamSize()-1; i >= 0; i--)
		{
			if (GP.PlayerTeam[i] != "")
			{
				FireBot(GP.PlayerTeam[i]);
			}
		}
		MainWindow.btnBack.Caption = CaptionBack;
		MainWindow.tpQualification = UT2K4SPTab_Qualification(MainWindow.replaceTab(3, MainWindow.tpTeamManagement, true));
		MainWindow.c_Tabs.ActivateTab(MainWindow.tpQualification.MyButton, true);
	}
	return true;
}

defaultproperties
{
	// Current team
	Begin Object class=AltSectionBackground Name=SPLsbgTeamBg
		WinWidth=0.318750
		WinHeight=0.968750
		WinLeft=0.006173
		WinTop=0.016667
		Caption="Your team"
		bBoundToParent=true
    End Object
    sbgTeamBg=SPLsbgTeamBg

	// Selection details
	Begin Object class=GUISectionBackground Name=SPLsbgSelectionBg
		WinWidth=0.425000
		WinHeight=0.662500
		WinLeft=0.562500
		WinTop=0.016667
		Caption="Selection"
		bBoundToParent=true
    End Object
    sbgSelectionBg=SPLsbgSelectionBg

	Begin Object Class=GUIImage Name=SPTMimgSelectionFaceBg
		Image=Material'2K4Menus.Controls.sectionback'
		WinWidth=0.095000
		WinHeight=0.300000
		WinLeft=0.589082
		WinTop=0.081284
		ImageStyle=ISTY_Scaled
		RenderWeight=0.11
		bBoundToParent=true
	End Object
	imgSelectionFaceBg=SPTMimgSelectionFaceBg

	Begin Object Class=GUIImage Name=SPTMimgSelectionFace
		Image=Material'PlayerPictures.cDefault'
		WinWidth=0.092500
		WinHeight=0.290000
		WinLeft=0.590332
		WinTop=0.088182
		ImageStyle=ISTY_Scaled
		RenderWeight=0.12
		bBoundToParent=true
	End Object
	imgSelectionFace=SPTMimgSelectionFace

	Begin Object Class=GUIScrollTextBox Name=SPTMsbDescription
		WinWidth=0.382041
		WinHeight=0.274745
		WinLeft=0.585256
		WinTop=0.380130
		CharDelay=0.0025
		EOLDelay=0.5
		RenderWeight=0.2
		bVisibleWhenEmpty=true
		FontScale=FNS_Medium
		TabOrder=4
		StyleName="NoBackground"
		bBoundToParent=true
	End Object
	sbDescription=SPTMsbDescription

	Begin Object Class=GUIScrollTextBox Name=SPTMsbDetails
		WinWidth=0.278316
		WinHeight=0.280867
		WinLeft=0.689005
		WinTop=0.081284
		bNoTeletype=true
		RenderWeight=0.2
		bVisibleWhenEmpty=true
		FontScale=FNS_Medium
		TabOrder=3
		StyleName="NoBackground"
		bBoundToParent=true
	End Object
	sbDetails=SPTMsbDetails

	// Free agent pool
	Begin Object class=GUISectionBackground Name=SPLsbgFreeAgentBg
		WinWidth=0.662755
		WinHeight=0.287500
		WinLeft=0.334872
		WinTop=0.700000
		Caption="Free agents"
		bBoundToParent=true
    End Object
    sbgFreeAgentBg=SPLsbgFreeAgentBg

	Begin Object class=GUICharacterListTeam Name=SPNclFreeAgents
		WinWidth=0.561505
		WinHeight=0.204490
		WinLeft=0.386122
		WinTop=0.762721
		bCenterInBounds=true
		StyleName="CharButton"
		RenderWeight=0.2
		OnChange=onFreeAgentChange
		TabOrder=4
		bDropSource=true
		bDropTarget=true
		OnDragDrop=TeamMemberOnDragDrop
		OnBeginDrag=FreeAgentOnBeginDrag
		bMultiSelect=false
		bBoundToParent=true
	End Object
	clFreeAgents=SPNclFreeAgents

	Begin Object class=GUIButton Name=SPNbtnLeft
		WinWidth=0.050000
		WinHeight=0.090000
		WinLeft=0.343750
		WinTop=0.806154
		bNeverFocus=true
		bRepeatClick=true
		OnClickSound=CS_Down
		StyleName="ArrowLeft"
		OnClick=onFreeAgentScroll
		TabOrder=5
		bBoundToParent=true
	End Object
	btnLeft=SPNbtnLeft

	Begin Object class=GUIButton Name=SPNbtnRight
		WinWidth=0.050000
		WinHeight=0.090000
		WinLeft=0.938750
		WinTop=0.806154
		StyleName="ArrowRight"
		bNeverFocus=true
		bRepeatClick=true
		OnClickSound=CS_Up
		OnClick=onFreeAgentScroll
		TabOrder=6
		bBoundToParent=true
	End Object
	btnRight=SPNbtnRight

	// Controlls
	Begin Object Class=GUIButton Name=SPPbtnHire
		Caption="HIRE"
		Hint="Hire the selected player"
		WinWidth=0.206250
		WinHeight=0.050000
		WinLeft=0.341199
		WinTop=0.033333
		RenderWeight=0.2
		MenuState=MSAT_Disabled
		OnClick=onHireClick
		FontScale=FNS_Small
		TabOrder=1
		bBoundToParent=true
	End Object
	btnHire=SPPbtnHire

	Begin Object Class=GUIButton Name=SPPbtnFire
		Caption="RELEASE"
		Hint="Fire this team mate"
		WinWidth=0.206250
		WinHeight=0.050000
		WinLeft=0.341199
		WinTop=0.091667
		RenderWeight=0.2
		MenuState=MSAT_Disabled
		OnClick=onFireClick
		FontScale=FNS_Small
		TabOrder=2
		bBoundToParent=true
	End Object
	btnFire=SPPbtnFire

	Begin Object Class=GUIButton Name=SPPbtnAutoMake
		Caption="AUTO FILL"
		Hint="Automatically fill your team"
		WinWidth=0.206250
		WinHeight=0.050000
		WinLeft=0.341199
		WinTop=0.150000
		RenderWeight=0.2
		OnClick=onAutoMakeClick
		FontScale=FNS_Small
		TabOrder=3
		bBoundToParent=true
	End Object
	btnAutoMake=SPPbtnAutoMake

	Begin Object class=AltSectionBackground Name=SPLsbgOverviewBg
		WinWidth=0.212628
		WinHeight=0.468750
		WinLeft=0.337372
		WinTop=0.208333
		bBoundToParent=true
    End Object
    sbgOverviewBg=SPLsbgOverviewBg

	Begin Object class=moEditBox Name=SPTMedBalance
		Caption="Balance:"
		Hint=""
		WinWidth=0.179694
		WinHeight=0.034156
		WinLeft=0.355102
		WinTop=0.271283
		bVerticalLayout=true
		TabOrder=4
		bReadOnly=true
		bBoundToParent=true
	End Object
	edBalance=SPTMedBalance

	Begin Object Class=GUIButton Name=SPPbtnTreat
		Caption="TREAT INJURIES"
		Hint="Treat the injuries of the current player"
		WinWidth=0.184821
		WinHeight=0.050000
		WinLeft=0.352550
		WinTop=0.521536
		RenderWeight=0.2
		MenuState=MSAT_Disabled
		OnClick=onTreatClick
		FontScale=FNS_Small
		TabOrder=5
		bBoundToParent=true
	End Object
	btnTreat=SPPbtnTreat

	Begin Object Class=GUIButton Name=SPPbtnTreatAll
		Caption="TREAT ALL"
		Hint="Treat the injuries of all your team mates"
		WinWidth=0.184821
		WinHeight=0.050000
		WinLeft=0.352550
		WinTop=0.570382
		RenderWeight=0.2
		FontScale=FNS_Small
		OnClick=onTreatAllClick
		TabOrder=6
		bBoundToParent=true
	End Object
	btnTreatAll=SPPbtnTreatAll

	EmptySlotImage=Material'PlayerPictures.cDefault'
	bInitialDraft=false
	PanelCaption="Team Management"
	AggressionCaption="Aggressiveness:"
	NameTitle="Name:"
	AgilityCaption="Agility:"
	AccuracyCaption="Accuracy:"
	TacticsCaption="Tactics:"
	FeeCaption="Fee:"
	HealthCaption="Health:"
	HealthHint="%player%'s health is at %health%%"
	msgFirstDraftYourTeam="First draft your team."
}
