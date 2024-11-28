class RocketCorona extends Effects;

#exec TEXTURE IMPORT NAME=RocketFlare FILE=TEXTURES\RocketFlare.tga DXT=5

auto state Start
{
    simulated function Tick(float dt)
    {
        SetDrawScale(FMin(DrawScale + dt*12.0, 1.5));
        if (DrawScale >= 1.5)
        {
            GotoState('End');
        }
    }
}

state End
{
    simulated function Tick(float dt)
    {
        SetDrawScale(FMax(DrawScale - dt*12.0, 0.9));
        if (DrawScale <= 0.9)
        {
            GotoState('');
        }
    }
}

defaultproperties
{
    RemoteRole=ROLE_None 
    Physics=PHYS_Trailer
    DrawType=DT_Sprite
    Style=STY_Translucent
    Texture=Texture'RocketFlare'
    Skins(0)=Texture'RocketFlare'
    DrawScale=+1.2
    DrawScale3D=(X=0.7,Y=0.35,Z=0.35)
    bTrailerSameRotation=true
    bUnlit=true
    Mass=13.0
}
