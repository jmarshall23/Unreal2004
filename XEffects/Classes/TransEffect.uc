class TransEffect extends xEmitter;

#exec OBJ LOAD FILE=XGameShaders.utx
#exec STATICMESH IMPORT NAME=TeleRing FILE=Models\TeleRing.lwo COLLISION=0

simulated event PostBeginPlay()
{
	SetTimer(0.7,true);
    SetRotation(rot(0,0,0));
    PlaySound(Sound'WeaponSounds.P1WeaponSpawn1',SLOT_None);
    Super.PostBeginPlay();
}

simulated event timer()
{
	mRegen = false;
}

defaultproperties
{
	mStartParticles=0
	mLifeRange(0)=0.600000
	mLifeRange(1)=0.600000
	mRegenRange(0)=30.000000
	mRegenRange(1)=30.000000
	mSpeedRange(0)=0.000000
	mSpeedRange(1)=0.000000
	mSizeRange(0)=1.1
	mSizeRange(1)=0.5
	mGrowthRate=-0.5
	mMeshNodes(0)=StaticMesh'XEffects.TeleRing'
	mPosDev=(Z=30.000000)
	mParticleType=PT_Mesh
	Tag=xEmitter
	Skins(0)=Material'XGameShaders.Trans.TransRing'

    Physics=PHYS_Rotating
    bRotateToDesired=false
    bFixedRotationDir=true
    RemoteRole=ROLE_SimulatedProxy
    bNetTemporary=true
}