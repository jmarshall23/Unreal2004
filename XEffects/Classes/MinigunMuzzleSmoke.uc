class MinigunMuzzleSmoke extends MuzzleSmoke;

#exec  TEXTURE IMPORT NAME=SmokeTex FILE=Textures\smoke.tga LODSET=1 NormalLOD=1 DXT=5 ALPHA=1 Mips=1

defaultproperties
{
    Skins(0)=Texture'SmokeTex'
    Style=STY_Alpha
    mSizeRange(0)=15.000000
    mSizeRange(1)=20.000000
}
