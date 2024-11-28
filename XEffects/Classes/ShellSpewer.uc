class ShellSpewer extends xEmitter;

#exec TEXTURE IMPORT NAME=ShellCasingTex FILE=Textures\ShellCasing.tga LODSET=2 DXT=5
#exec STATICMESH IMPORT NAME=ShellCasing FILE=Models\ShellCasing.lwo COLLISION=0

var() Sound ShellImpactSnd;

function CollisionSound()
{
    PlaySound(ShellImpactSnd);
}

defaultproperties
{
	bHighDetail=true
	mStartParticles=0
	mMaxParticles=150
	mLifeRange(0)=0.500000
	mLifeRange(1)=1.000000
	mRegenRange(0)=0.000000 //18
	mRegenRange(1)=0.000000 //18
	mSpeedRange(0)=200.000000 //250
	mSpeedRange(1)=250.000000 //300
	mMassRange(0)=2.000000
	mMassRange(1)=2.000000
	mDirDev=(X=0.500000,Y=0.200000,Z=0.600000)
	mSpinRange(0)=-100.0
	mSpinRange(1)=100.0
	mParticleType=PT_Mesh
	DrawScale=0.05000
    mCollision=false //TODO: fix collision
    mColMakeSound=true
	mMeshNodes(0)=StaticMesh'ShellCasing'
	Skins(0)=Texture'ShellCasingTex'
    ShellImpactSnd=Sound'WeaponSounds.P1Shell1'
    mAttenFunc=ATF_None
    RemoteRole=ROLE_None
    bNetTemporary=false
}
