//=============================================================================
// LightningBolt.
//=============================================================================
class LightningBolt extends xEmitter;

#exec TEXTURE  IMPORT NAME=LightningBoltT FILE=textures\LightningBolt.tga GROUP="Skins" DXT=5
#exec OBJ LOAD FILE=..\Sounds\WeaponSounds.uax

simulated function PostBeginPlay()
{
    Super.PostBeginPlay();
    MakeNoise(0.5);
	PlaySound(Sound'WeaponSounds.LightningGun.LightningGunImpact', SLOT_Misc,,,,,false);
	if( Level.NetMode != NM_DedicatedServer )
        Spawn(class'BlueSparks',,,Location,Rotation);
}

defaultproperties
{
	mStartParticles=30
	mMaxParticles=30
	mLifeRange(0)=0.500000
	mLifeRange(1)=0.500000
	mSizeRange(0)=30.000000
	mSizeRange(1)=30.000000
	mPosDev=(X=5.000000,Y=5.000000,Z=5.000000)
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

    mSpawnVecB=(X=40.0,Y=40.0,Z=10.0)

    RemoteRole=ROLE_DumbProxy
	bNetTemporary=true
    blockOnNet=true
    bReplicateMovement=false
    bSkipActorPropertyReplication=true
    bReplicateInstigator=true
	NetPriority=3
}