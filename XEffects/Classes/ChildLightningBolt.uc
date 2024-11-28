//=============================================================================
// ChildLightningBolt.
//=============================================================================
class ChildLightningBolt extends xEmitter;


simulated function bool CheckMaxEffectDistance(PlayerController P, vector SpawnLocation)
{
	return !P.BeyondViewDistance(SpawnLocation,5000);
}

simulated function PostBeginPlay()
{
    Super.PostBeginPlay();
    // pass in +vect(0,0,2) to EffectIsRelevant() because this actor just spawned too (not valid to check if its been rendered)
	if( EffectIsRelevant(Location+vect(0,0,2),false) )
        Spawn(class'ChildBlueSparks',,,Location,Rotation);
}

defaultproperties
{
    LifeSpan=0.5
	mStartParticles=10
	mMaxParticles=10
	mLifeRange(0)=0.500000
	mLifeRange(1)=0.500000
	mSizeRange(0)=15.000000
	mSizeRange(1)=15.000000
	mPosDev=(X=15.000000,Y=15.000000,Z=15.000000)
	mParticleType=PT_Branch
	mRegen=false
	Skins(0)=Texture'LightningBoltT'
	Style=STY_Additive
    mAtLeastOneFrame=true

    Physics=PHYS_None
    bUnlit=True
    bGameRelevant=true
    CollisionRadius=+0.00000
    CollisionHeight=+0.00000

    mSpawnVecB=(X=20.0,Y=0.0,Z=10.0)

    RemoteRole=ROLE_DumbProxy
	bNetTemporary=true
    blockOnNet=true
    bReplicateMovement=false
    bSkipActorPropertyReplication=true
}