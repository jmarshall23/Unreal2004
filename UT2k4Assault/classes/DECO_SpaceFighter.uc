//=============================================================================
// DECO_SpaceFighter
//=============================================================================

class DECO_SpaceFighter extends Decoration;

#exec OBJ LOAD FILE=..\StaticMeshes\AS_Vehicles_SM.usx

function Landed(vector HitNormal);
function HitWall (vector HitNormal, actor Wall);
function TakeDamage( int NDamage, Pawn instigatedBy, Vector hitlocation, 
					Vector momentum, class<DamageType> damageType);
singular function PhysicsVolumeChange( PhysicsVolume NewVolume );
function Trigger( actor Other, pawn EventInstigator );
singular function BaseChange();
function Bump( actor Other );


//=============================================================================
// defaultproperties
//=============================================================================

defaultproperties
{
	bMovable=false
	bDamageable=false
	bCanBeDamaged=false
	bShouldBaseAtStartup=false

	bEdShouldSnap=true
    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'AS_Vehicles_SM.Vehicles.SpaceFighter_Human'
    DrawScale=3.0
    AmbientGlow=48
    bUnlit=false
	
	bUseCylinderCollision=false
    bCollideActors=true
    bCollideWorld=false
    bBlockActors=true
	bBlockKarma=true

	RemoteRole=Role_None
}