class Behemoth extends Brute;

function RangedAttack(Actor A)
{
	if ( bShotAnim )
		return;
	if ( Controller.InLatentExecution(Controller.LATENT_MOVETOWARD) )
	{
		SetAnimAction('WalkFire');
		bShotAnim = true;
		return;
	}
	Super.RangedAttack(A);
}

defaultproperties
{
	Health=260
    bCanStrafe=true
	skins(0)=jBrute2

	ScoringValue=6
}