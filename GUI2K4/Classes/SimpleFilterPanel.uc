//==============================================================================
//	This version of the filter page does not dynamically generate filter options.
//
//	Written by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class SimpleFilterPanel extends GUIPanel;

var automated GUISectionBackground sb_Checks, sb_Mutators;
var automated AltSectionBackground sb_Choices;

var automated moCheckbox		 ch_NoPassword, ch_NoFull, ch_NoEmpty, ch_NoBotServers, ch_Show2003;
var automated array<GUIComboBox> co_Mutator, co_MutatorMode;
var automated moComboBox		 co_StatsView, co_WeaponStay, co_Translocator, co_MapVoting, co_KickVoting;

var BrowserFilters FilterMaster;
var array<string> SelectedMutator;

var localized string	ViewStatsStrings[3];
var localized string	MutatorModeStrings[4];
var localized string    WeaponStayStrings[3];
var localized string    TranslocatorStrings[3];
var localized string    MapVotingStrings[3];
var localized string    KickVotingStrings[3];
var localized string	SaveString;

struct FilterItem
{
	var string 	FilterTag;
	var bool	bEnabled;
};

var array<CacheManager.MutatorRecord> MutatorRecords;

var int CurrentFilter;

function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	local int i, j;

	Super.InitComponent(MyController, MyOwner);

	// Set options for stats server viewing
	co_StatsView.AddItem(ViewStatsStrings[0],,"QT_Disabled");
	co_StatsView.AddItem(ViewStatsStrings[1],,"QT_Equals");
	co_StatsView.AddItem(ViewStatsStrings[2],,"QT_NotEquals");
	co_StatsView.ReadOnly(true);

	// Load mutators into combobox
	class'CacheManager'.static.GetMutatorList(MutatorRecords);

	for (i = 0; i < co_MutatorMode.Length; i++)
	{
		co_MutatorMode[i].AddItem(MutatorModeStrings[0],,"QT_Disabled");
		co_MutatorMode[i].AddItem(MutatorModeStrings[1],,"QT_Equals");
		co_MutatorMode[i].AddItem(MutatorModeStrings[2],,"QT_NotEquals");

		co_MutatorMode[i].ReadOnly(True);
		co_MutatorMode[i].Edit.bAlwaysNotify = True;
		co_MutatorMode[i].TabOrder = co_MutatorMode[0].TabOrder + (i * 2);
	}

	co_MutatorMode[0].AddItem(MutatorModeStrings[3],,"QT_Equals");

	for (i = 0; i < co_Mutator.Length; i++)
	{
		co_Mutator[i].List.bSorted = True;
		for (j = 0; j < MutatorRecords.Length; j++)
			co_Mutator[i].AddItem(MutatorRecords[j].FriendlyName, None, GetItemName(MutatorRecords[j].ClassName));

		co_Mutator[i].ReadOnly(True);
		co_Mutator[i].Edit.bAlwaysNotify = True;
		co_Mutator[i].TabOrder = co_MutatorMode[0].TabOrder + (i * 2);
	}

	// Weapon stay
	co_WeaponStay.AddItem(WeaponStayStrings[0],,"QT_Disabled");
	co_WeaponStay.AddItem(WeaponStayStrings[1],,"QT_Equals");
	co_WeaponStay.AddItem(WeaponStayStrings[2],,"QT_NotEquals");
	co_WeaponStay.ReadOnly(true);

	// Translocator
	co_Translocator.AddItem(TranslocatorStrings[0],,"QT_Disabled");
	co_Translocator.AddItem(TranslocatorStrings[1],,"QT_Equals");
	co_Translocator.AddItem(TranslocatorStrings[2],,"QT_NotEquals");
	co_Translocator.ReadOnly(true);

	// Map Voting
	co_MapVoting.AddItem(MapVotingStrings[0],,"QT_Disabled");
	co_MapVoting.AddItem(MapVotingStrings[1],,"QT_Equals");
	co_MapVoting.AddItem(MapVotingStrings[2],,"QT_NotEquals");
	co_MapVoting.ReadOnly(true);

	// Kick Voting
	co_KickVoting.AddItem(KickVotingStrings[0],,"QT_Disabled");
	co_KickVoting.AddItem(KickVotingStrings[1],,"QT_Equals");
	co_KickVoting.AddItem(KickVotingStrings[2],,"QT_NotEquals");
	co_KickVoting.ReadOnly(true);

	SelectedMutator.Length = co_Mutator.Length;

	sb_Checks.ManageComponent(ch_NoFull);
	sb_Checks.ManageComponent(ch_NoBotServers);
	sb_Checks.ManageComponent(ch_NoEmpty);
	sb_Checks.ManageComponent(ch_NoPassword);

	sb_Choices.ManageComponent(co_StatsView);
	sb_Choices.ManageComponent(co_WeaponStay);
	sb_Choices.ManageComponent(co_Translocator);
	sb_Choices.ManageComponent(co_MapVoting);
	sb_Choices.ManageComponent(co_KickVoting);

	for ( i = 0; i < co_MutatorMode.Length; i++ )
		sb_Mutators.ManageComponent(co_MutatorMode[i]);

	for ( i = 0; i < co_Mutator.Length; i++ )
		sb_Mutators.ManageComponent(co_Mutator[i]);
}

function FilterSelectionChanged(bool bValid)
{
	local int i;

	if (bValid)
	{
		for (i = 0; i < Components.Length; i++)
			EnableComponent(Components[i]);
	}

	else
	{
		ChangeNextMutatorState(0,False);
		for (i = 0; i < Components.Length; i++)
			DisableComponent(Components[i]);
	}
}

function Refresh(int NewFilterIndex)
{
	local array<CustomFilter.AFilterRule> Rules;
	local array<int> MutatorArray;
	local int i, j;
	local bool bTemp;

	bTemp = Controller.bCurMenuInitialized;
	Controller.bCurMenuInitialized = False;

	for (i = 0; i < Components.Length; i++)
	{
		if (GUIMenuOption(Components[i]) != None && moComboBox(Components[i]) == None )
			GUIMenuOption(Components[i]).ResetComponent();

		else if (GUIComboBox(Components[i]) != None)
			GUIComboBox(Components[i]).SetIndex(0);
	}

	ChangeNextMutatorState(0, False);
	for (i = 0; i < SelectedMutator.Length; i++)
		SelectedMutator[i] = "";

	if (FilterMaster != None && NewFilterIndex >= 0 && NewFilterIndex < FilterMaster.AllFilters.Length)
	{
		CurrentFilter = NewFilterIndex;
		Rules = FilterMaster.GetFilterRules(NewFilterIndex);

		// sort the mutators
		for (i = 0; i < Rules.Length; i++)
		{
			if (Rules[i].FilterItem.Key ~= "mutator")
			{
				if (Rules[i].ItemName ~= "none")
				{
					MutatorArray.Insert(0,1);
					MutatorArray[0] = i;
					continue;
				}

				for (j = 0; j < MutatorArray.Length; j++)
				{
					if (Rules[MutatorArray[j]].FilterItem.QueryType == QT_Disabled)
						break;
				}

				MutatorArray.Insert(j,1);
				MutatorArray[j] = i;
			}
		}

		for (i = 0; i < Rules.Length; i++)
		{
			if (Rules[i].FilterItem.Key ~= "mutator")
				continue;

			UpdateRule(Rules[i]);
		}

		for (i = 0; i < MutatorArray.Length; i++)
			UpdateRule(Rules[MutatorArray[i]]);
	}

	Controller.bCurMenuInitialized = bTemp;

	for ( i = 0; i < SelectedMutator.Length; i++ )
		if ( SelectedMutator[i] != "" )
			UpdateSelectedMutator(i, co_Mutator[i].GetExtra(), -1);
}

function UpdateRule(CustomFilter.AFilterRule Rule)
{
	local int i, idx;
	local bool bEnabled;


	bEnabled = Rule.FilterItem.QueryType != QT_Disabled;

	switch (Locs(Rule.FilterItem.Key))
	{
		case "weaponstay":
			switch(Rule.FilterItem.QueryType)
			{
				case QT_Equals:
					co_WeaponStay.SetIndex(1);
					break;

				case QT_NotEquals:
					co_WeaponStay.SetIndex(2);
					break;

				default:
					co_WeaponStay.SetIndex(0);
					break;
			}

			break;

		case "transloc":
			switch (Rule.FilterItem.QueryType)
			{
				case QT_Equals:
					co_Translocator.SetIndex(1);
					break;

				case QT_NotEquals:
					co_Translocator.SetIndex(2);
					break;

				default:
					co_Translocator.SetIndex(0);
					break;
			}

			break;

		case "stats":
			switch (Rule.FilterItem.QueryType)
			{
				case QT_Equals:
					co_StatsView.SetIndex(1);
					break;

				case QT_NotEquals:
					co_StatsView.SetIndex(2);

				default:
					co_StatsView.SetIndex(0);
					break;
			}

			break;

		case "mapvoting":
			switch (Rule.FilterItem.QueryType)
			{
				case QT_Equals:
					co_MapVoting.SetIndex(1);
					break;

				case QT_NotEquals:
					co_MapVoting.SetIndex(2);
					break;

				default:
					co_MapVoting.SetIndex(0);
					break;
			}
			break;

		case "kickvoting":
			switch (Rule.FilterItem.QueryType)
			{
				case QT_Equals:
					co_KickVoting.SetIndex(1);
					break;

				case QT_NotEquals:
					co_KickVoting.SetIndex(2);
					break;

				default:
					co_KickVoting.SetIndex(0);
					break;
			}
			break;

		case "password":
			ch_NoPassword.Checked(bEnabled);
			break;

		case "freespace":
			ch_NoFull.Checked(bEnabled);
			break;

		case "currentplayers":
			ch_NoEmpty.Checked(bEnabled);
			break;

		case "nobots":
			ch_NoBotServers.Checked(bEnabled);
			break;

		case "mutator":
			if (Rule.FilterItem.Value ~= "None")
				co_MutatorMode[0].SetIndex(3);

			else
			{
				while (i < SelectedMutator.Length)
				{
					if (SelectedMutator[i] == "")
					{
						if (co_MutatorMode[0].GetIndex() < 3 && Rule.FilterItem.QueryType != QT_Disabled)
						{
							idx = co_Mutator[i].FindIndex(Rule.FilterItem.Value, False, True);
							if (idx < 0)
								break;

							co_MutatorMode[i].SetIndex(co_MutatorMode[i].FindIndex(class'CustomFilter'.static.GetQueryString(Rule.FilterItem.QueryType),,True));
							co_Mutator[i].Show();
							SelectedMutator[i] = Rule.FilterItem.Value;

							co_Mutator[i].SetIndex(idx);
							if (i + 1 < co_MutatorMode.Length)
							{
								co_MutatorMode[i+1].SetIndex(0);
								co_MutatorMode[i+1].Show();
							}

						}
						break;
					}
					i++;
				}
			}

			break;

	}
}

function MyOnChange(GUIComponent Sender)
{
	local MasterServerClient.EQueryType QueryType;
	local int i, Index, idx;
	local string Str;
	local CustomFilter.AFilterRule Rule;

//	log("OnChange:"$Sender);
	if (!Controller.bCurMenuInitialized)
		return;

	if (moCheckBox(Sender) != None)
	{
		switch (Sender)
		{
			case ch_NoPassword:
				i = FilterMaster.AllFilters[CurrentFilter].FindRuleIndex("password");
				FilterMaster.SetRule(CurrentFilter, i, ch_NoPassword.Caption, "password", "false", "DT_Unique", "QT_Equals");
				break;

			case ch_NoFull:
				i = FilterMaster.AllFilters[CurrentFilter].FindRuleIndex("freespace");
				FilterMaster.SetRule(CurrentFilter, i, ch_NoFull.Caption, "freespace", "0", "DT_Unique", "QT_GreaterThan");
				break;

			case ch_NoEmpty:
				i = FilterMaster.AllFilters[CurrentFilter].FindRuleIndex("currentplayers");
				FilterMaster.SetRule(CurrentFilter, i, ch_NoEmpty.Caption, "currentplayers", "0", "DT_Unique", "QT_GreaterThan");
				break;

			case ch_NoBotServers:
				i = FilterMaster.AllFilters[CurrentFilter].FindRuleIndex("nobots");
				FilterMaster.SetRule(CurrentFilter, i, ch_NoBotServers.Caption, "nobots", "true", "DT_Unique", class'CustomFilter'.static.GetQueryString(QueryType));
				break;

		}
	}

	else if (GUIComboBox(Sender) != None)
	{
		for (i = 0; i < co_Mutator.Length; i++)
		{
			if (co_Mutator[i] == Sender)
			{
//				log("co_Mutator["$i$"] OnChange");
				if (SelectedMutator[i] == "")
					idx = -1;
				else idx = FilterMaster.AllFilters[CurrentFilter].FindRuleIndex("mutator",SelectedMutator[i]);

				if (UpdateSelectedMutator(i, co_Mutator[i].GetExtra(), co_Mutator[i].FindIndex(SelectedMutator[i],,True) ))
				{
					SelectedMutator[i] = co_Mutator[i].GetExtra();
					FilterMaster.SetRule(CurrentFilter, idx, co_Mutator[i].GetText(), "mutator", SelectedMutator[i], "DT_Multiple", co_MutatorMode[i].GetExtra());
				}
				return;
			}
		}

		for (i = 0; i < co_MutatorMode.Length; i++)
		{
			if (co_MutatorMode[i] == Sender)
			{
				Index = co_MutatorMode[i].GetIndex();
				if (SelectedMutator[i] == "")
					SelectedMutator[i] = co_Mutator[i].GetExtra();
				idx = FilterMaster.AllFilters[CurrentFilter].FindRuleIndex("mutator", SelectedMutator[i]);
//log("SelectedMutator["$i$"]:"$SelectedMutator[i]);
				switch (Index)
				{
					case 0:
						CheckForNone();
						FilterMaster.SetRule(CurrentFilter, idx, co_Mutator[i].GetText(), "mutator", SelectedMutator[i], "DT_Multiple", "QT_Disabled" );
//						co_Mutator[i].Hide();
						ChangeNextMutatorState(i, False);

						// For any additional mutators on the page, change their QueryType to QT_Disabled
						while (++i < co_MutatorMode.Length)
						{
							if (SelectedMutator[i] == "")
								break;

							idx = FilterMaster.AllFilters[CurrentFilter].FindRuleIndex("mutator", SelectedMutator[i]);
							if ( FilterMaster.AllFilters[CurrentFilter].GetRule(idx, Rule) )
								FilterMaster.AllFilters[CurrentFilter].ChangeRule(idx, Rule.ItemName, Rule.FilterItem.Value, QT_Disabled);
						}

						break;

					case 1:
					case 2:
						CheckForNone();

						FilterMaster.SetRule(CurrentFilter, idx, co_Mutator[i].GetText(), "mutator", SelectedMutator[i], "DT_Multiple", co_MutatorMode[i].GetExtra() );
						co_Mutator[i].Show();
						ChangeNextMutatorState(i+1, True);
						break;

					case 3:
						FilterMaster.AllFilters[CurrentFilter].RemoveRule("mutator");
						FilterMaster.AllFilters[CurrentFilter].AddRule("none", "mutator", "", QT_Equals, DT_Multiple);
						NoMutatorsMode();
						break;
				}

				return;
			}
		}
	}

	else if (moComboBox(Sender) != None)
	{
		switch (Sender)
		{
			case co_WeaponStay:		Str = "weaponstay"; 	break;
			case co_Translocator:	Str = "transloc";		break;
			case co_StatsView:		Str = "stats";			break;
			case co_MapVoting:		Str = "mapvoting";		break;
			case co_KickVoting:		Str = "kickvoting";		break;
		}

		i = FilterMaster.AllFilters[CurrentFilter].FindRuleIndex(Str);
		FilterMaster.SetRule(CurrentFilter, i, moComboBox(Sender).GetText(), Str, "true", "DT_Unique", moComboBox(Sender).GetExtra());
	}
}

function bool UpdateSelectedMutator(int Index, string NewValue, int OldValue)
{
	local int i, idx;

	for (i = 0; i < co_Mutator.Length; i++)
	{
		if (i == Index) continue;

		idx = co_Mutator[i].FindIndex(NewValue,,True);
		if (idx >= 0)
		{
			if (idx == co_Mutator[i].GetIndex() && co_Mutator[i].bVisible)
				return false;

			co_Mutator[i].List.RemoveSilent(idx);
		}

		if (OldValue != -1)
		{
			idx = co_Mutator[i].FindIndex(co_Mutator[Index].List.GetExtraAtIndex(OldValue),,True);
			if (idx < 0)
				co_Mutator[i].AddItem(co_Mutator[Index].GetItem(OldValue), co_Mutator[Index].GetItemObject(OldValue), co_Mutator[Index].List.GetExtraAtIndex(OldValue));
		}
	}

	return true;
}

// We've selected a mutator, so we need to remove the special mutator entry for 'No mutators'
protected function CheckForNone()
{
	local int i, idx;
	local array<CustomFilter.CurrentFilter> Mutes;

	Mutes = FilterMaster.AllFilters[CurrentFilter].GetRuleSet("mutator");
	for (i = 0; i < Mutes.Length; i++)
	{
		if (Mutes[i].Item.ItemName ~= "none")
		{
			idx = FilterMaster.AllFilters[CurrentFilter].FindItemIndex("mutator",i);
			if (idx >= 0)
				FilterMaster.AllFilters[CurrentFilter].RemoveRuleAt(idx);

			break;
		}
	}
}

// turns off all mutator combo boxes
protected function NoMutatorsMode()
{
	if (co_MutatorMode.Length > 0 && co_Mutator.Length > 0)
		co_MutatorMode[0].Show();

	ChangeNextMutatorState(0,False);
}

// This function handles mutator combobox visiblity
// Pass current index if hiding, pass current index + 1 if showing
protected function ChangeNextMutatorState(int Index, bool bShow)
{
	local int i;
	local CustomFilter.AFilterRule Rule;

	if (Index < 0 || Index >= co_MutatorMode.Length)
		return;

	if (bShow)
	{

		co_MutatorMode[Index].Show();
		if (SelectedMutator[Index] != "")
		{
			// Correct this mutator's QueryType if this mutator was previously hidden
			i = FilterMaster.AllFilters[CurrentFilter].FindRuleIndex("mutator", SelectedMutator[Index]);
			if ( FilterMaster.AllFilters[CurrentFilter].GetRule(i, Rule) &&
			class'CustomFilter'.static.GetQueryString(Rule.FilterItem.QueryType) != co_MutatorMode[Index].GetExtra() )
				FilterMaster.AllFilters[CurrentFilter].ChangeRule(i, Rule.ItemName, Rule.FilterItem.Value, class'CustomFilter'.static.GetQueryType(co_MutatorMode[Index].GetExtra()));

			co_Mutator[Index].Show();
			ChangeNextMutatorState(Index + 1, bShow);
		}
	}

	else
	{
		if (co_Mutator[Index].bVisible)
			co_Mutator[Index].Hide();

		if (Index + 1 < co_MutatorMode.Length && co_MutatorMode[Index + 1].bVisible)
			co_MutatorMode[Index + 1].Hide();

		ChangeNextMutatorState(Index + 1, bShow);
	}
}

defaultproperties
{
	Begin Object Class=GUISectionBackground Name=MainOptions
        Caption="Game"
        LeftPadding=0.580000
        RightPadding=0.020000
        TopPadding=0.050000
        ImageOffset(3)=0.000000
		WinWidth=1.635429
		WinHeight=0.500044
		WinLeft=0.005237
		WinTop=0.022619
	End Object
	sb_Checks=MainOptions

	Begin Object Class=AltSectionBackground Name=MultiOptions
        ImageOffset(3)=26.000000
        WinTop=0.049848
        WinLeft=0.034899
        WinWidth=0.895876
        WinHeight=0.467421
        RenderWeight=0.091
	End Object
	sb_Choices=MultiOptions

	Begin Object Class=GUISectionBackground Name=MutatorOptions
        bRemapStack=False
        Caption="Mutators"
        ColPadding=0.020000
        LeftPadding=0.009000
        RightPadding=0.009000
        TopPadding=0.020000
        ImageOffset(3)=10.000000
        NumColumns=2
		WinWidth=0.989162
		WinHeight=0.456234
		WinLeft=0.003492
		WinTop=0.539586
    End Object
	sb_Mutators=MutatorOptions

	Begin Object class=moCheckBox Name=NoPasswdCheckBox
		WinWidth=0.45
		WinHeight=0.040000
		WinLeft=0.500000
		WinTop=0.023333
		Caption="No Passworded"
		Hint="Do not display passworded servers in the server browser"
		OnChange=MyOnChange
		TabOrder=4
	End Object

	Begin Object class=moCheckBox Name=NoFullCheckBox
		WinWidth=0.45
		WinHeight=0.040000
		WinLeft=0.020000
		WinTop=0.085453
		Caption="No Full Servers"
		Hint="Do not display full servers in the server browser"
		OnChange=MyOnChange
		TabOrder=0
	End Object

	Begin Object class=moCheckBox Name=NoEmptyCheckBox
		WinWidth=0.45
		WinHeight=0.040000
		WinLeft=0.500000
		WinTop=0.085453
		Caption="No Empty Servers"
		Hint="Do not display empty servers in the server browser"
		OnChange=MyOnChange
		TabOrder=2
	End Object

	Begin Object class=moCheckBox Name=NoBotServersCheckBox
		WinWidth=0.447070
		WinHeight=0.040000
		WinLeft=0.023333
		WinTop=0.147573
		Caption="No Servers With Bots"
		Hint="Do not display servers that allow bots in the server browser"
		OnChange=MyOnChange
		TabOrder=1
	End Object

	Begin Object class=moComboBox Name=StatsViewCombo
		WinWidth=0.946110
		WinHeight=0.04
		WinLeft=0.017500
		WinTop=0.271813
		Caption="Stats Servers"
		Hint="Only display servers with stats enabled in the server browser"
		CaptionWidth=0.4
		OnChange=MyOnChange
		TabOrder=4
		IniDefault="QT_Disabled"
	End Object

	Begin Object class=moComboBox Name=WeaponStayCombo
		WinWidth=0.946110
		WinHeight=0.04
		WinLeft=0.017500
		WinTop=0.333933
		Caption="WeaponStay"
		Hint="Only display servers with weapons stay enabled in the server browser"
		CaptionWidth=0.4
		OnChange=MyOnChange
		TabOrder=5
		IniDefault="QT_Disabled"
	End Object

	Begin Object class=moComboBox Name=TranslocatorCombo
		WinWidth=0.946110
		WinHeight=0.04
		WinLeft=0.017500
		WinTop=0.396053
		Caption="Translocator"
		Hint="Only display servers with translocator enabled in the server browser"
		CaptionWidth=0.4
		OnChange=MyOnChange
		TabOrder=6
		IniDefault="QT_Disabled"
	End Object

	Begin Object class=moComboBox Name=MapVotingCombo
		WinWidth=0.946110
		WinHeight=0.04
		WinLeft=0.017500
		WinTop=0.458173
		Caption="Map Voting"
		Hint="Only display servers with map voting enabled in the server browser"
		CaptionWidth=0.4
		OnChange=MyOnChange
		TabOrder=7
		IniDefault="QT_Disabled"
	End Object

	Begin Object class=moComboBox Name=KickVotingCombo
		WinWidth=0.946110
		WinHeight=0.04
		WinLeft=0.017500
		WinTop=0.520293
		Caption="Kick Voting"
		Hint="Only display servers with kick voting enabled in the server browser"
		CaptionWidth=0.4
		OnChange=MyOnChange
		TabOrder=8
		IniDefault="QT_Disabled"
	End Object

	Begin Object class=GUIComboBox Name=MutatorModeCombo
		WinWidth=0.618840
		WinHeight=0.04
		WinLeft=0.017500
		WinTop=0.632558
		OnChange=MyOnChange
		TabOrder=9
		IniDefault="QT_Disabled"
		Hint="Select the type of mutator filter"
	End Object

	Begin Object class=GUIComboBox Name=MutatorCombo
		WinWidth=0.308750
		WinHeight=0.04
		WinLeft=0.657571
		WinTop=0.632401
		OnChange=MyOnChange
		TabOrder=10
		bVisible=False
		IniDefault="Choose Mutator"
		Hint="Select the mutator that should be filtered"
	End Object

	ch_NoPassword=NoPasswdCheckBox
	ch_NoFull=NoFullCheckBox
	ch_NoEmpty=NoEmptyCheckBox
	ch_NoBotServers=NoBotServersCheckBox
	co_StatsView=StatsViewCombo

	co_Mutator(0)=MutatorCombo
	co_Mutator(1)=MutatorCombo
	co_Mutator(2)=MutatorCombo
	co_Mutator(3)=MutatorCombo
	co_Mutator(4)=MutatorCombo
	co_MutatorMode(0)=MutatorModeCombo
	co_MutatorMode(1)=MutatorModeCombo
	co_MutatorMode(2)=MutatorModeCombo
	co_MutatorMode(3)=MutatorModeCombo
	co_MutatorMode(4)=MutatorModeCombo

	co_WeaponStay=WeaponStayCombo
	co_Translocator=TranslocatorCombo
	co_MapVoting=MapVotingCombo
	co_KickVoting=KickVotingCombo

	ViewStatsStrings(0)="Any Servers"
	ViewStatsStrings(1)="Only Stats Servers"
	ViewStatsStrings(2)="No Stats Servers"
	WeaponStayStrings(0)="Any Servers"
	WeaponStayStrings(1)="Only Weapon Stay Servers"
	WeaponStayStrings(2)="No Weapon Stay Servers"
	TranslocatorStrings(0)="Any Servers"
	TranslocatorStrings(1)="Only Translocator Servers"
	TranslocatorStrings(2)="No Translocator Servers"
	MutatorModeStrings(0)="Any Mutators"
	MutatorModeStrings(1)="This Mutator"
	MutatorModeStrings(2)="Not This Mutator"
	MutatorModeStrings(3)="No Mutators"
	MapVotingStrings(0)="Any Servers"
	MapVotingStrings(1)="Only Map Vote Servers"
	MapVotingStrings(2)="No Map Vote Servers"
	KickVotingStrings(0)="Any Servers"
	KickVotingStrings(1)="Only Kick Vote Servers"
	KickVotingStrings(2)="No Kick Vote Servers"

	SaveString="Setting saved successfully!"

	PropagateVisibility=False
}
