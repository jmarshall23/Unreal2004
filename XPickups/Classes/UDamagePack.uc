//=============================================================================
// UDamagePack
//=============================================================================
class UDamagePack extends TournamentPickUp
	notplaceable;

#exec OBJ LOAD FILE=E_Pickups.usx
#exec OBJ LOAD FILE=XGameShaders.utx

static function StaticPrecache(LevelInfo L)
{
	L.AddPrecacheStaticMesh(StaticMesh'E_Pickups.Udamage');
	L.AddPrecacheMaterial(Material'XGameShaders.PlayerShaders.WeaponUDamageShader');
}

auto state Pickup
{
	function Touch( actor Other )
	{
        local Pawn P;

		if ( ValidTouch(Other) )
		{
            P = Pawn(Other);
            P.EnableUDamage(30);
			AnnouncePickup(P);
            SetRespawn();
		}
	}
}

defaultproperties
{
    PickupMessage="DOUBLE DAMAGE!"
    MaxDesireability=2.0
    RemoteRole=ROLE_DumbProxy
    AmbientGlow=254
    Mass=10.000000
    bPredictRespawns=true
    PickupForce="UDamagePickup"
    RespawnTime=90.000000
    Physics=PHYS_Rotating
	RotationRate=(Yaw=24000)
    DrawScale=0.9
    PickupSound=sound'PickupSounds.UDamagePickUp'
    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'E_Pickups.Udamage'
    CollisionRadius=32.0
    CollisionHeight=23.000000
    Style=STY_AlphaZ
    ScaleGlow=0.6
    TransientSoundRadius=600.0
}
