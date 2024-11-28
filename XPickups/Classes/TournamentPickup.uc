//=============================================================================
// TournamentPickup.
//=============================================================================
class TournamentPickup extends Pickup;

function RespawnEffect()
{
	spawn(class'PlayerSpawnEffect');
}

defaultproperties
{
	bAmbientGlow=true
	MessageClass=class'PickupMessagePlus'
}
