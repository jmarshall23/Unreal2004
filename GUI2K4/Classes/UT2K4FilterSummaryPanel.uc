//==============================================================================
//	Lists the active rules for the current filter.
//
//	Created by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4FilterSummaryPanel extends GUIFilterPanel;

var automated UT2K4FilterSummaryListBox	lb_FilterRules;
var UT2K4FilterSummaryList				li_FilterRules;

var(Menu) config array<float>			HeaderColumnPerc;

function Refresh()
{
	UpdateRules();
}

function UpdateRules()
{
	local int i;

	ClearRules();
	FilterRules = FilterMaster.GetFilterRules(p_Anchor.Index);
	for (i = 0; i < FilterRules.Length; i++)
		AddFilterRule(FilterRules[i]);

log("FilterSummaryPanel.LoadRules()---------------------");
	for (i = 0; i < li_FilterRules.Rules.Length; i++)
		log("Rules["$i$"]:"$li_FilterRules.Rules[i].ItemName);

	Super.UpdateRules();
}

function ClearRules()
{
	li_FilterRules.Clear();
	Super.ClearRules();
}

function AddFilterRule(CustomFilter.AFilterRule NewRule)
{
	li_FilterRules.AddFilterRule(NewRule);
}

function ListOnCreateComponent(GUIComponent NewComp, GUIComponent Sender)
{
	if (UT2K4FilterSummaryListBox(Sender) != None)
	{
		if (UT2K4FilterSummaryList(NewComp) != None)
		{
			li_FilterRules = UT2K4FilterSummaryList(NewComp);
			li_FilterRules.INIOption = "@Internal";
			li_FilterRules.p_Anchor = p_Anchor;
			li_FilterRules.ExpandLastColumn = True;
		}

		UT2K4FilterSummaryListBox(Sender).InternalOnCreateComponent(NewComp,Sender);
	}
}

function string InternalOnSaveINI(GUIComponent Sender)
{
	HeaderColumnPerc = lb_FilterRules.HeaderColumnPerc;
	SaveConfig();

	return Super.OnSaveINI(Sender);
}

DefaultProperties
{
	OnCreateComponent=InternalOnCreateComponent
	OnSaveINI=InternalOnSaveINI

	HeaderColumnPerc(0)=0.40
	HeaderColumnPerc(1)=0.40
	HeaderColumnPerc(2)=0.20

	Begin Object Class=GUIMultiOptionListBox Name=RulesLB
		bVisibleWhenEmpty=True
	End Object

	lb_Rules=RulesLB

	Begin Object Class=UT2K4FilterSummaryListBox Name=RulesBox
		bVisibleWhenEmpty=True
		WinWidth=1.0
		WinHeight=1.0
		WinLeft=0.0
		OnCreateComponent=ListOnCreateComponent
	End Object

	lb_FilterRules=RulesBox
}
