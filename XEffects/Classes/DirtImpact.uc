//=============================================================================
// DirtImpact.
//=============================================================================
class DirtImpact extends BloodSpurt;

#exec TEXTURE IMPORT NAME=pcl_Dirta FILE=TEXTURES\Dirta.tga GROUP=Skins Alpha=1  DXT=5

simulated function PostNetBeginPlay() 
{
	if ( Level.NetMode == NM_DedicatedServer )
		LifeSpan = 0.2;
}

defaultproperties  
{ 
    BloodDecalClass=None
     mLifeRange(0)=0.90000 
     mLifeRange(1)=1.700000
     mSpeedRange(0)=35.000000 
     mSpeedRange(1)=150.000000 
     mSizeRange(0)=5.500000 
     mSizeRange(1)=13.500000
     mMassRange(0)=0.400000
     mMassRange(1)=0.700000
     mMaxParticles=30
     mStartParticles=30
     Texture=Texture'pcl_Dirta'
     Skins(0)=Texture'pcl_Dirta'
     CullDistance=+7000.0
}
