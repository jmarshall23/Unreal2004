class FlakGlow extends ScaledSprite;

#exec  TEXTURE IMPORT NAME=GoldGlow FILE=Textures\GoldMarker.TGA LODSET=3 DXT=5

function PostBeginPlay()
{
	if ( Owner != None )
		SetBase(Owner);
}

singular function BaseChange();

defaultproperties
{
	texture=texture'GoldGlow'
	bHardAttach=true
    bHidden=false
    DrawType=DT_Sprite
    Style=STY_Translucent
    bStatic=false
    DrawScale=0.25
	RemoteRole=ROLE_None
	bUnlit=true
    bStasis=false
    bShouldBaseAtStartup=false
    Mass=0.0
    bCollideActors=false
}
