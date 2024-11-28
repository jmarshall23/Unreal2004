class ShieldEffect extends Actor;

#exec OBJ LOAD FILE=XEffectMat.utx

var float Brightness, DesiredBrightness;

function Flash(int Drain)
{
    Brightness = FMin(Brightness + Drain / 2, 250.0);
    Skins[0] = Skins[1];
    SetTimer(0.2, false);
}

function Timer()
{
    Skins[0] = default.Skins[0];
}

function SetBrightness(int b)
{
    DesiredBrightness = FMin(50+b*2, 250.0);
}

defaultproperties
{
    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'WeaponStaticMesh.Shield'
    Skins(0)=XEffectMat.Shield3rdFB
    Skins(1)=FinalBlend'XEffectMat.ShieldRip3rdFB'
    DrawScale=1.8
    RemoteRole=ROLE_None
    bUnlit=true
    bHidden=true
    bOnlyOwnerSee=true
    Brightness=250.0
    DesiredBrightness=250.0
    AmbientGlow=250
}
