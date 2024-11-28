#exec OBJ LOAD FILE=..\textures\EpicParticles.utx

class BulldogHeadlightCorona extends Effects;


defaultproperties
{
	Texture=Material'EpicParticles.flashflare1'
	bUnlit=True
	DrawType=DT_Sprite
	Style=STY_Translucent
	bHidden=False
	DrawScale=0.4
	bHardAttach=True
	bCollideActors=False
	bCorona=False
	bBlockActors=False
	bDynamicLight=False
	LightType=LT_None
    LightBrightness=0
    LightHue=255
    LightSaturation=255
	Physics=PHYS_None
}