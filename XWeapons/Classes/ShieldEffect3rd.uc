class ShieldEffect3rd extends AimedAttachment;

#exec OBJ LOAD FILE=XEffectMat.utx

var float Brightness, DesiredBrightness;
var int HitCount, OldHitCount;
var ShieldSparks Sparks;

replication
{
    unreliable if (Role == ROLE_Authority && !bHidden)
        HitCount; 
}

simulated function Destroyed()
{
    if (Sparks != None)
        Sparks.Destroy();
}

simulated function Flash(int Drain)
{
    if (Sparks == None)
    {
	    Sparks = Spawn(class'ShieldSparks');
    }

    if (Instigator != None && Instigator.IsFirstPerson())
    {
        Sparks.SetLocation(Location+Vect(0,0,20)+VRand()*12.0);
        Sparks.SetRotation(Rotation);
        Sparks.mStartParticles = 16;
    }
    else if ( EffectIsRelevant(Location,false) )
    {
        Sparks.SetLocation(Location+VRand()*8.0);
        Sparks.SetRotation(Rotation);
        Sparks.mStartParticles = 16;
    }
    Brightness = FMin(Brightness + Drain / 2, 250.0);
    Skins[0] = Skins[1];
    SetTimer(0.2, false);
}

simulated function Timer()
{
    Skins[0] = default.Skins[0];
}

function SetBrightness(int b, bool hit) // server only please
{
    DesiredBrightness = FMin(50+b*2, 250.0);
    if (hit)
    {
        HitCount++;
        Flash(50);
    }
}

simulated function PostNetReceive()
{
    if (OldHitCount == -1)
    {
        OldHitCount = HitCount;
    }
    else if (HitCount != OldHitCount)
    {
        Flash(50);
        OldHitCount = HitCount;
    }
}

defaultproperties
{
    RemoteRole=ROLE_SimulatedProxy
    bNetNotify=true
    bReplicateInstigator=true
    Physics=PHYS_Trailer
    DrawType=DT_StaticMesh
    StaticMesh=StaticMesh'WeaponStaticMesh.Shield'
    Skins(0)=XEffectMat.Shield3rdFB
    Skins(1)=FinalBlend'XEffectMat.ShieldRip3rdFB'
    DrawScale=3.2
    bUnlit=true
    bHidden=true
    bOwnerNoSee=true

    BaseOffset=(Z=10.0)
    AimedOffset=(X=28.0f,Y=28.0f,Z=34.0)
    DownwardBias=16.0

    Brightness=250.0
    DesiredBrightness=250.0
    AmbientGlow=250
    OldHitCount=-1
}
