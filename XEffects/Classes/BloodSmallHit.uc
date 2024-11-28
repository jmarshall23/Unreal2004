//=============================================================================
// BloodSmallHit.
//=============================================================================
class BloodSmallHit extends BloodSpurt;

#exec OBJ LOAD File=XGameShadersB.utx

simulated function PostNetBeginPlay()
{
	if ( (Role < ROLE_Authority) && class'GameInfo'.Static.UseLowGore() )
	{
		splats[0] = Material'xbiosplat'; 
		splats[1] = Material'xbiosplat';
		splats[2] = Material'xbiosplat';
		BloodDecalClass = class'BioDecal';
		Skins[0] = Material'BloodPuffGreen';
	}
	Super.PostNetBeginPlay();
}

defaultproperties  
{
    Style=STY_Alpha
    mSizeRange(0)=10.00000 
    mSizeRange(1)=15.00000
    mDirDev=(X=0.7,Y=0.7,Z=0.7) 
    mPosDev=(X=5.00000,Y=5.00000,Z=5.00000)
    mLifeRange(0)=0.50000
    mLifeRange(1)=0.900000
    mSpeedRange(0)=20.000000
    mSpeedRange(1)=70.000000 
    mMassRange(0)=0.100000
    mMassRange(1)=0.200000
    mDelayRange(0)=0.0
    mDelayRange(1)=0.1
    mNumTileColumns=1
    mNumTileRows=1
    Skins(0)=XGameShadersB.Blood.BloodPuffA
}