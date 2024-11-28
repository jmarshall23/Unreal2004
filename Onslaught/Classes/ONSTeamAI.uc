class ONSTeamAI extends TeamAI;

var ONSPowerCore FinalCore; //this team's main powercore

function SetObjectiveLists()
{
	local GameObjective O;
	local ONSPowerCore Core;

	Super.SetObjectiveLists();

	for (O = Objectives; O != None; O = O.NextObjective)
	{
		Core = ONSPowerCore(O);
		if (Core != None && Core.bFinalCore && Core.CoreStage == 0 && Core.DefenderTeamIndex == Team.TeamIndex)
		{
			FinalCore = Core;
			break;
		}
	}

	if (FinalCore == None)
		warn("Couldn't find PowerCore!");
}

function ReAssessStrategy()
{
	local GameObjective O;

	if (FreelanceSquad == None)
		return;

	if (FinalCore == None || ONSTeamAI(EnemyTeam.AI).FinalCore == None)
	{
		Super.ReAssessStrategy();
		return;
	}

	if ( FinalCore.PoweredBy(EnemyTeam.TeamIndex) || ONSTeamAI(EnemyTeam.AI).FinalCore.PoweredBy(Team.TeamIndex)
	     || (Level.Game.bOverTime && FinalCore.Health < ONSTeamAI(EnemyTeam.AI).FinalCore.Health) )
	{
		FreelanceSquad.bFreelanceAttack = true;
		FreelanceSquad.bFreelanceDefend = false;
		O = GetPriorityAttackObjectiveFor(FreelanceSquad);
	}
	else if (Level.Game.bOverTime && FinalCore.Health > ONSTeamAI(EnemyTeam.AI).FinalCore.Health)
	{
		FreelanceSquad.bFreelanceAttack = false;
		FreelanceSquad.bFreelanceDefend = true;
		O = GetLeastDefendedObjective();
	}
	else
	{
		FreelanceSquad.bFreelanceAttack = false;
		FreelanceSquad.bFreelanceDefend = false;
		O = GetPriorityFreelanceObjective();
	}

	if ( (O != None) && (O != FreelanceSquad.SquadObjective) )
		FreelanceSquad.SetObjective(O,true);
}

function CriticalObjectiveWarning(GameObjective AttackedObjective, Pawn EventInstigator)
{
	local SquadAI S;
	local Bot M;
	local bool bFoundDefense;

	for (S = Squads; S != None; S = S.NextSquad)
		if (S.SquadObjective == AttackedObjective)
		{
			S.CriticalObjectiveWarning(EventInstigator);
			bFoundDefense = true;
			for (M = S.SquadMembers; M != None; M = M.NextSquadMember)
				if ( (M.Enemy == None || M.Enemy == EventInstigator) && Vehicle(M.Pawn) != None && M.Pawn.bStationary
				     && !FastTrace(EventInstigator.Location + EventInstigator.CollisionHeight * vect(0,0,1), M.Pawn.Location) )
				{
					Vehicle(M.Pawn).KDriverLeave(false);
					M.WhatToDoNext(67);
				}
		}

	if (!bFoundDefense)
	{
		for (S = Squads; S != None; S = S.NextSquad)
			if ( (S.GetOrders() == 'Defend' || S.bFreelanceDefend)
			     && (ONSPowerCore(S.SquadObjective) == None || (!ONSPowerCore(S.SquadObjective).bUnderAttack && ONSPowerCore(S.SquadObjective).CoreStage != 4)) )
			{
				S.SetObjective(AttackedObjective, true);
				S.CriticalObjectiveWarning(EventInstigator);
				return;
			}
	}
}

function FindNewObjectives(GameObjective DisabledObjective)
{
	local SquadAI S;
	local Bot M;
	local ONSPowerCore Core, DisabledCore;
	local GameObjective O;
	local int i;
	local bool bHasVulnerableNode;

	for (O = Team.AI.Objectives; O != None; O = O.NextObjective)
	{
		Core = ONSPowerCore(O);
		if (Core != None && Core.CoreStage == 0 && Core.DefenderTeamIndex == Team.TeamIndex && Core.PoweredBy(EnemyTeam.TeamIndex))
		{
			bHasVulnerableNode = true;
			break;
		}
	}

	for (S = Squads; S != None; S = S.NextSquad)
	{
		S.GetOrders();
		Core = ONSPowerCore(S.SquadObjective);
		if (S.CurrentOrders == 'Attack' || S.bFreelanceAttack || (!bHasVulnerableNode && (S.CurrentOrders == 'Defend' || S.bFreelanceDefend)))
		{
			if ( Core == None || Core.bDisabled
	     		     || (Core.DefenderTeamIndex == Team.TeamIndex && Core.CoreStage == 0)
			     || (!Core.PoweredBy(Team.TeamIndex) && (bHasVulnerableNode || !Core.LinkedToCoreConstructingFor(Team.TeamIndex))) )
				FindNewObjectiveFor(S, false);
			else if (Core.CoreStage == 1)
				for (M = S.SquadMembers; M != None; M = M.NextSquadMember)
					if (M.IsInState('RangedAttack')) //just finished destroying node, now take it over
						M.WhatToDoNext(66);
		}
		else if (S.CurrentOrders == 'Defend' || S.bFreelanceDefend)
		{
			if ( Core == None || Core.bDisabled
	     		     || (Core.CoreStage != 4 && Core.CoreStage != 1 && (Core.DefenderTeamIndex != Team.TeamIndex || !Core.PoweredBy(EnemyTeam.TeamIndex))) )
				FindNewObjectiveFor(S, false);
			else
			{
				//check if objective with higher defensepriority is now vulnerable
				DisabledCore = ONSPowerCore(DisabledObjective);
				if (DisabledCore != None && DisabledCore.CoreStage == 0)
					for (i = 0; i < DisabledCore.PowerLinks.Length; i++)
						if ( DisabledCore.PowerLinks[i].DefensePriority > Core.DefensePriority
						     && (DisabledCore.PowerLinks[i].DefenseSquad == None || DisabledCore.PowerLinks[i].DefenseSquad.Team != Team) )
						{
							S.SetObjective(DisabledCore, true);
							break;
						}
			}
		}
		else
			FindNewObjectiveFor(S, false);
	}
}

//ObjectiveCovered() returns true if the given objective is a SquadObjective for a squad on this team
function bool ObjectiveCovered(GameObjective O)
{
	local SquadAI S;

	for (S = Squads; S != None; S = S.NextSquad)
		if (S.SquadObjective == O)
			return true;

	return false;
}

function GameObjective GetPriorityAttackObjectiveFor(SquadAI AttackSquad)
{
	local GameObjective O, NextPickedObjective;
	local ONSPowerCore Core;
	local array<ONSPowerCore> NextCores;
	local int i, j;
	local bool bAlreadyInList, bPickedObjectiveCovered, bTestObjectiveCovered;

	PickedObjective = None;
	for (O = Objectives; O != None; O = O.NextObjective)
	{
		Core = ONSPowerCore(O);
		if ( Core != None && !Core.bDisabled && (Core.DefenderTeamIndex != Team.TeamIndex || Core.CoreStage != 0)
			&& Core.PoweredBy(Team.TeamIndex) )
		{
			//keep track of neutral nodes that will be obtainable when the currently obtainable ones have been built
			//but only if this one is neutral or owned by this team because don't want bots waiting on "future" nodes
			//while a single team futilely attacks the enemy controlled node!
			if (Core.DefenderTeamIndex == Team.TeamIndex || (Core.CoreStage != 0 && Core.CoreStage != 2))
				for (i = 0; i < Core.PowerLinks.Length; i++)
					if (Core.PowerLinks[i].CoreStage == 4 || Core.PowerLinks[i].CoreStage == 2)
					{
						bAlreadyInList = false;
						for (j = 0; j < NextCores.length; j++)
						{
							if (NextCores[j] == Core.PowerLinks[i])
							{
								bAlreadyInList = true;
								break;
							}
						}
						if (!bAlreadyInList)
							NextCores[NextCores.length] = Core.PowerLinks[i];
					}

			bTestObjectiveCovered = ObjectiveCovered(Core);
			if ( PickedObjective == None || (!bTestObjectiveCovered && (bPickedObjectiveCovered || PickedObjective.DefensePriority < Core.DefensePriority))
			     || (PickedObjective.DefensePriority == Core.DefensePriority && (bPickedObjectiveCovered == bTestObjectiveCovered) && FRand() < 0.5) )
			{
				PickedObjective = Core;
				bPickedObjectiveCovered = bTestObjectiveCovered;
			}
		}
	}

	if (PickedObjective != None && ObjectiveCovered(PickedObjective))
	{
		for (i = 0; i < NextCores.length; i++)
			if ( !ObjectiveCovered(NextCores[i]) && ( NextPickedObjective == None || NextPickedObjective.DefensePriority < NextCores[i].DefensePriority
								  || (NextPickedObjective.DefensePriority == NextCores[i].DefensePriority && FRand() < 0.5) ) )
				NextPickedObjective = NextCores[i];

		if (NextPickedObjective != None)
			PickedObjective = NextPickedObjective;
	}

	return PickedObjective;
}

function GameObjective GetLeastDefendedObjective()
{
	local GameObjective O, Best;

	for ( O=Objectives; O!=None; O=O.NextObjective )
	{
		if ( ONSPowerCore(O) != None && ONSPowerCore(O).CoreStage == 0 && (O.DefenderTeamIndex == Team.TeamIndex)
			&& ONSPowerCore(O).PoweredBy(EnemyTeam.TeamIndex)
			&& ((Best == None) || (Best.DefensePriority	< O.DefensePriority)
				|| ((Best.DefensePriority == O.DefensePriority) && (Best.GetNumDefenders() > O.GetNumDefenders()))) )
			Best = O;
	}
	if (Best == None)
		Best = GetPriorityAttackObjectiveFor(None); //nothing needs defending, so head to neutral node

	return Best;
}

function GameObjective GetMostDefendedObjective()
{
	local GameObjective O, Best;

	for ( O=Objectives; O!=None; O=O.NextObjective )
	{
		if ( ONSPowerCore(O) != None && ONSPowerCore(O).CoreStage == 0 && (O.DefenderTeamIndex == Team.TeamIndex)
			&& ONSPowerCore(O).PoweredBy(EnemyTeam.TeamIndex)
			&& ((Best == None) || (Best.DefensePriority	< O.DefensePriority)
				|| ((Best.DefensePriority == O.DefensePriority) && (Best.GetNumDefenders() < O.GetNumDefenders()))) )
			Best = O;
	}
	if (Best == None)
		Best = GetPriorityAttackObjectiveFor(None); //nothing needs defending, so head to neutral node

	return Best;
}

function bool PutOnDefense(Bot B)
{
	local GameObjective O;

	O = GetLeastDefendedObjective();
	if ( O != None )
	{
		//we need this because in Onslaught, unlike other gametypes, two defending squads (possibly from different teams!)
		//could be headed to the same objective
		if ( O.DefenseSquad == None || O.DefenseSquad.Team != Team )
		{
			O.DefenseSquad = AddSquadWithLeader(B, O);
			ONSSquadAI(O.DefenseSquad).bDefendingSquad = true;
		}
		else
			O.DefenseSquad.AddBot(B);
		return true;
	}
	return false;
}

function GameObjective GetPriorityFreelanceObjective()
{
	local GameObjective O, Best;
	local ONSPowerCore Core;

	//find a node that the enemy wants to get (regardless of whether this team can get it)
	for (O = Objectives; O != None; O = O.NextObjective)
	{
		Core = ONSPowerCore(O);
		if ( Core != None && (Core.CoreStage == 4 || Core.CoreStage == 2 || Core.CoreStage == 5) && Core.DefenderTeamIndex != Team.TeamIndex
		     && Core.PoweredBy(EnemyTeam.TeamIndex) && (Best == None || Best.DefensePriority < Core.DefensePriority) )
			Best = O;
	}
	if (Best == None)
		Best = GetPriorityAttackObjectiveFor(None);

	return Best;
}

defaultproperties
{
	OrderList(0)=ATTACK
	OrderList(1)=ATTACK
	OrderList(2)=DEFEND
	OrderList(3)=ATTACK
	OrderList(4)=DEFEND
	OrderList(5)=FREELANCE
	OrderList(6)=ATTACK
	OrderList(7)=ATTACK
	SquadType=class'Onslaught.ONSSquadAI'
}
