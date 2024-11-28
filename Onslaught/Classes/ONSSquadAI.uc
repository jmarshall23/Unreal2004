class ONSSquadAI extends SquadAI;

var bool bDefendingSquad;
var float LastFailedNodeTeleportTime;
var float MaxObjectiveGetOutDist; //cached highest ObjectiveGetOutDist of all the vehicles available on this level

function Reset()
{
	Super.Reset();
	bDefendingSquad = false;
}

function name GetOrders()
{
	local name NewOrders;

	if ( PlayerController(SquadLeader) != None )
		NewOrders = 'Human';
	else if ( bFreelance && !bFreelanceAttack && !bFreelanceDefend )
		NewOrders = 'Freelance';
	else if ( bDefendingSquad || bFreelanceDefend || (SquadObjective != None && SquadObjective.DefenseSquad == self) )
		NewOrders = 'Defend';
	else
		NewOrders = 'Attack';
	if ( NewOrders != CurrentOrders )
		CurrentOrders = NewOrders;
	return CurrentOrders;
}

function byte PriorityObjective(Bot B)
{
	local ONSPowerCore Core;

	if (GetOrders() == 'Defend')
	{
		Core = ONSPowerCore(SquadObjective);
		if (Core != None && (Core.DefenderTeamIndex == Team.TeamIndex) && Core.bUnderAttack)
			return 1;
	}
	else if (CurrentOrders == 'Attack')
	{
		Core = ONSPowerCore(SquadObjective);
		if (Core != None)
		{
			if (Core.bFinalCore)
			{
				if (B.Enemy != None && Core.BotNearObjective(B))
					return 1;
			}
			else if (VSize(B.Pawn.Location - Core.Location) < 1000)
				return 1;
		}
	}
	return 0;
}

function SetDefenseScriptFor(Bot B)
{
	local ONSPowerCore Core;

	//don't look for defense scripts when heading for neutral node
	Core = ONSPowerCore(SquadObjective);
	if (Core == None || (Core.DefenderTeamIndex == Team.TeamIndex && (Core.CoreStage == 2 || Core.CoreStage == 0)) )
	{
		Super.SetDefenseScriptFor(B);
		return;
	}

	if (B.GoalScript != None)
		B.FreeScript();
}

function float MaxVehicleDist(Pawn P)
{
	if ( GetOrders() != 'Attack' || SquadObjective == None )
		return 3000;
	return FMin(3000, VSize(P.Location - SquadObjective.Location));
}

/* FindPathToObjective()
Returns path a bot should use moving toward a base
*/
function bool FindPathToObjective(Bot B, Actor O)
{
	local vehicle OldVehicle;

	if ( B.Pawn.bStationary )
		return false;

	if ( O == None )
	{
		O = SquadObjective;
		if ( O == None )
		{
			B.GoalString = "No SquadObjective";
			return false;
		}
	}

	if ( ONSPowerNode(O) != None && (ONSPowerNode(O).CoreStage != 0 || ONSPowerCore(O).DefenderTeamIndex == Team.TeamIndex) )
	{
		B.MoveTarget = None;
		if ( (Vehicle(B.Pawn) != None) && (B.Pawn.Location.Z - O.Location.Z < 500)
		     && (VSize(B.Pawn.Location - O.Location) < Vehicle(B.Pawn).ObjectiveGetOutDist) && B.LineOfSightTo(O) )
		{
			OldVehicle = Vehicle(B.Pawn);
			Vehicle(B.Pawn).KDriverLeave(false);
			if ( (Vehicle(B.Pawn) == None) && (B.Pawn.Physics == PHYS_Falling) && B.DoWaitForLanding() )
			{
				B.Pawn.Velocity.Z = 0;
				return true;
			}
		}

		if ( B.ActorReachable(O) )
		{
			if ( (Vehicle(B.Pawn) != None) && (B.Pawn.Location.Z - O.Location.Z < 500) )
				Vehicle(B.Pawn).KDriverLeave(false);
			if ( B.Pawn.ReachedDestination(O) )
			{
				//log(B.GetHumanReadableName()$" Force touch for reached objective");
				O.Touch(B.Pawn);
				return false;
			}
			if ( OldVehicle != None )
				OldVehicle.TeamUseTime = Level.TimeSeconds + 6;
			B.RouteGoal = O;
			B.RouteCache[0] = None;
			B.GoalString = "almost at "$O;
			B.MoveTarget = O;
			B.SetAttractionState();
			return true;
		}
		if ( OldVehicle != None )
			OldVehicle.UsedBy(B.Pawn);
	}

	if ( Super.FindPathToObjective(B,O) )
		return true;

	if ( Vehicle(B.Pawn) != None && !Vehicle(B.Pawn).bKeyVehicle && (B.Enemy == None || !B.EnemyVisible()) )
	{
		if (B.Pawn.HasWeapon() && Vehicle(B.Pawn).MaxDesireability > 0.5)
			Vehicle(B.Pawn).bDefensive = true; //have bots use it as a turret instead
		else
			Vehicle(B.Pawn).VehicleLostTime = Level.TimeSeconds + 20;
		//log(B.PlayerReplicationInfo.PlayerName$" Abandoning "$Vehicle(B.Pawn)$" because can't reach "$O);
		Vehicle(B.Pawn).KDriverLeave(false);
	}
	return false;
}

function bool CheckVehicle(Bot B)
{
	local ONSPowerCore Nearest, Best;
	local GameObjective O;
	local float NewRating, BestRating;
	local byte SourceDist;

	if ( Vehicle(B.Pawn) != None && GetOrders() == 'Defend' && !Vehicle(B.Pawn).bDefensive && SquadObjective != None
	     && VSize(B.Pawn.Location - SquadObjective.Location) < 2000 && !Vehicle(B.Pawn).bKeyVehicle && (B.Enemy == None || !B.EnemyVisible()) )
	{
	     	Vehicle(B.Pawn).KDriverLeave(false);
	     	return false;
	}

	if (Vehicle(B.Pawn) == None && ONSPowerCore(SquadObjective) != None)
	{
		if (ONSPowerCore(SquadObjective).CoreStage == 0)
		{
			if ( GetOrders() == 'Defend' && (B.Enemy == None || (!B.EnemyVisible() && Level.TimeSeconds - B.LastSeenTime > 3))
			     && VSize(B.Pawn.Location - SquadObjective.Location) < GetMaxObjectiveGetOutDist()
			     && ONSPowerCore(SquadObjective).Health < ONSPowerCore(SquadObjective).DamageCapacity
			     && ((B.Pawn.Weapon != None && B.Pawn.Weapon.CanHeal(SquadObjective)) || (B.Pawn.PendingWeapon != None && B.Pawn.PendingWeapon.CanHeal(SquadObjective))) )
				return false;
		}
		if (ONSPowerCore(SquadObjective).CoreStage == 2)
		{
			if ( (B.Enemy == None || !B.EnemyVisible()) && VSize(B.Pawn.Location - SquadObjective.Location) < GetMaxObjectiveGetOutDist() )
				return false;
		}
		if ((ONSPowerCore(SquadObjective).CoreStage == 4 || ONSPowerCore(SquadObjective).CoreStage == 1) && VSize(B.Pawn.Location - SquadObjective.Location) < GetMaxObjectiveGetOutDist())
			return false;
	}

	if (Super.CheckVehicle(B))
		return true;
	if ( Vehicle(B.Pawn) != None || (B.Enemy != None && B.EnemyVisible()) || LastFailedNodeTeleportTime > Level.TimeSeconds - 20 || ONSPowerCore(SquadObjective) == None
	     || ONSPlayerReplicationInfo(B.PlayerReplicationInfo) == None || ONSPowerCore(SquadObjective).HasUsefulVehicles(B) || B.Skill + B.Tactics < 2 + FRand() )
		return false;

	//no vehicles around
	if (VSize(B.Pawn.Location - SquadObjective.Location) > 5000 && !B.LineOfSightTo(SquadObjective))
	{
		//really want a vehicle to get to SquadObjective, so teleport to a different node to find one
		if (ONSPowerCore(B.RouteGoal) != None && ONSPlayerReplicationInfo(B.PlayerReplicationInfo).GetCurrentNode() == B.RouteGoal)
		{
			SourceDist = ONSPowerCore(B.RouteGoal).FinalCoreDistance[Abs(1 - Team.TeamIndex)];
			for (O = Team.AI.Objectives; O != None; O = O.NextObjective)
				if (O != B.RouteGoal && ONSOnslaughtGame(Level.Game).ValidSpawnPoint(ONSPowerCore(O), Team.TeamIndex))
				{
					NewRating = ONSPowerCore(O).TeleportRating(B, Team.TeamIndex, SourceDist);
					if (NewRating > BestRating || (NewRating == BestRating && FRand() < 0.5))
					{
						Best = ONSPowerCore(O);
						BestRating = NewRating;
					}
				}

			if (Best != None)
				ONSPlayerReplicationInfo(B.PlayerReplicationInfo).TeleportTo(Best);
			else
				LastFailedNodeTeleportTime = Level.TimeSeconds;

			return false;
		}

		Nearest = ONSPowerCore(SquadObjective).ClosestTo(B.Pawn);
		if ( Nearest == None || Nearest.CoreStage != 0 || Nearest.DefenderTeamIndex != Team.TeamIndex
		     || VSize(Nearest.Location - B.Pawn.Location) > 2000 )
			return false;

		B.MoveTarget = B.FindPathToward(Nearest, false);
		if (B.MoveTarget != None)
		{
			B.RouteGoal = Nearest;
			B.GoalString = "Node teleport from "$B.RouteGoal;
			B.SetAttractionState();
			return true;
		}
	}

	return false;
}

//return a value indicating how useful this vehicle is to the bot
function float VehicleDesireability(Vehicle V, Bot B)
{
	local float Rating;

	if (CurrentOrders == 'Defend')
	{
		if ((SquadObjective == None || VSize(SquadObjective.Location - B.Pawn.Location) < 2000) && Super.VehicleDesireability(V, B) <= 0)
			return 0;
		if (V.Health < V.HealthMax * 0.125 && B.Enemy != None && B.EnemyVisible())
			return 0;
		Rating = V.BotDesireability(self, Team.TeamIndex, SquadObjective);
		if (Rating <= 0)
			return 0;

		if (V.bDefensive)
		{
			if (ONSPowerCore(SquadObjective) != None)
			{
				//turret can't hit priority enemy
				if ( V.bStationary && B.Enemy != None && ONSPowerCore(SquadObjective).LastDamagedBy == B.Enemy.PlayerReplicationInfo
				     && !FastTrace(B.Enemy.Location + B.Enemy.CollisionHeight * vect(0,0,1), V.Location) )
					return 0;
				if (ONSPowerCore(SquadObjective).ClosestTo(V) != SquadObjective)
					return 0;
			}
			if (ONSStationaryWeaponPawn(V) != None && !ONSStationaryWeaponPawn(V).bPowered)
				return 0;
		}

		return Rating;
	}

	return Super.VehicleDesireability(V, B);
}

function bool CheckSpecialVehicleObjectives(Bot B)
{
	local ONSPowerCore Core;
	local SquadAI S;

	if (Size > 1)
		return Super.CheckSpecialVehicleObjectives(B);

	//if bot is the only bot headed to a neutral (unclaimed) node, that's more important, so don't head to any SpecialVehicleObjectives
	Core = ONSPowerCore(SquadObjective);
	if (Core == None || Core.CoreStage != 4 || !Core.PoweredBy(Team.TeamIndex))
		return Super.CheckSpecialVehicleObjectives(B);

	for (S = Team.AI.Squads; S != None; S = S.NextSquad)
		if (S != self && S.SquadObjective == Core)
			return Super.CheckSpecialVehicleObjectives(B);

	return false;
}

function float GetMaxObjectiveGetOutDist()
{
	local SVehicleFactory F;

	if (MaxObjectiveGetOutDist == 0.0)
		foreach DynamicActors(class'SVehicleFactory', F)
			if (F.VehicleClass != None)
				MaxObjectiveGetOutDist = FMax(MaxObjectiveGetOutDist, F.VehicleClass.default.ObjectiveGetOutDist);

	return MaxObjectiveGetOutDist;
}

function bool CheckSquadObjectives(Bot B)
{
	local bool bResult;

	bResult = Super.CheckSquadObjectives(B);

	if (!bResult && CurrentOrders == 'Freelance' && B.Enemy == None && ONSPowerCore(SquadObjective) != None)
	{
		if (ONSPowerCore(SquadObjective).PoweredBy(Team.TeamIndex))
		{
			B.GoalString = "Disable Objective "$SquadObjective;
			return SquadObjective.TellBotHowToDisable(B);
		}
		else if (!B.LineOfSightTo(SquadObjective))
		{
			B.GoalString = "Harass enemy at "$SquadObjective;
			return FindPathToObjective(B, SquadObjective);
		}
	}

	return bResult;
}

function float ModifyThreat(float current, Pawn NewThreat, bool bThreatVisible, Bot B)
{
	if ( NewThreat.PlayerReplicationInfo != None && ONSPowerCore(SquadObjective) != None
	     && ONSPowerCore(SquadObjective).LastDamagedBy == NewThreat.PlayerReplicationInfo
	     && ONSPowerCore(SquadObjective).bUnderAttack )
	{
		if (!bThreatVisible)
			return current + 0.5;
		if ( (VSize(B.Pawn.Location - NewThreat.Location) < 2000) || B.Pawn.IsA('Vehicle') || B.Pawn.Weapon.bSniping
			|| ONSPowerCore(SquadObjective).Health < ONSPowerCore(SquadObjective).DamageCapacity * 0.5 )
			return current + 6;
		else
			return current + 1.5;
	}
	else
		return current;
}

function bool MustKeepEnemy(Pawn E)
{
	return ( E.PlayerReplicationInfo != None && ONSPowerCore(SquadObjective) != None
		 && ONSPowerCore(SquadObjective).LastDamagedBy == E.PlayerReplicationInfo
		 && ONSPowerCore(SquadObjective).bUnderAttack );
}

//don't actually merge squads, because they could be two defending squads from different teams going to same neutral powernode
function MergeWith(SquadAI S)
{
	SquadObjective = S.SquadObjective;
}

defaultproperties
{
	GatherThreshold=+0.0
	MaxSquadSize=3
	bAddTransientCosts=true
}
