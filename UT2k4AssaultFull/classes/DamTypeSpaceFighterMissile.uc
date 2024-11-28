class DamTypeSpaceFighterMissile extends VehicleDamageType
	abstract;

static function ScoreKill(Controller Killer, Controller Killed)
{
	if (Killed != None && Killer != Killed && Vehicle(Killed.Pawn) != None && Vehicle(Killed.Pawn).bCanFly)
	{
		if ( ASVehicle_SpaceFighter(Killer.Pawn) == None )
			return;

		ASVehicle_SpaceFighter(Killer.Pawn).TopGunCount++;

		//Maybe add to game stats?
		if ( ASVehicle_SpaceFighter(Killer.Pawn).TopGunCount == 5 && PlayerController(Killer) != None )
		{
			ASVehicle_SpaceFighter(Killer.Pawn).TopGunCount = 0;
			PlayerController(Killer).ReceiveLocalizedMessage(class'Message_ASKillMessages', 0);
		}
	}
}

defaultproperties
{
    DeathString="%o couldn't avoid %k's missile."
    MaleSuicide="%o blasted himself out of space."
    FemaleSuicide="%o blasted herself out of space."

    VehicleDamageScaling=1.0
    VehicleMomentumScaling=1.0
    bDelayedDamage=true
    VehicleClass=class'ASVehicle_SpaceFighter_Human'
}
