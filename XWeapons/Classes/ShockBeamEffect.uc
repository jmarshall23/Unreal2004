class ShockBeamEffect extends xEmitter;

var Vector HitNormal;
var class<ShockBeamCoil> CoilClass;
var class<ShockMuzFlash> MuzFlashClass;
var class<ShockMuzFlash3rd> MuzFlash3Class;

replication
{
    reliable if (bNetInitial && Role == ROLE_Authority)
        HitNormal;
}

function AimAt(Vector hl, Vector hn)
{
    HitNormal = hn;
    mSpawnVecA = hl;
    if (Level.NetMode != NM_DedicatedServer)
        SpawnEffects();
}

simulated function PostNetBeginPlay()
{
    if (Role < ROLE_Authority)
        SpawnEffects();
}

simulated function SpawnImpactEffects(rotator HitRot, vector EffectLoc)
{
	Spawn(class'ShockImpactFlare',,, EffectLoc, HitRot);
	Spawn(class'ShockImpactRing',,, EffectLoc, HitRot);
	Spawn(class'ShockImpactScorch',,, EffectLoc, Rotator(-HitNormal));
	Spawn(class'ShockExplosionCore',,, EffectLoc+HitNormal*8, HitRot);
}

simulated function bool CheckMaxEffectDistance(PlayerController P, vector SpawnLocation)
{
	return !P.BeyondViewDistance(SpawnLocation,3000);
}

simulated function SpawnEffects()
{
    local ShockBeamCoil Coil;
    local xWeaponAttachment Attachment;
	
    if (Instigator != None)
    {
        if ( Instigator.IsFirstPerson() )
        {
			if ( (Instigator.Weapon != None) && (Instigator.Weapon.Instigator == Instigator) )
				SetLocation(Instigator.Weapon.GetEffectStart());
			else
				SetLocation(Instigator.Location);
            Spawn(MuzFlashClass,,, Location);
        }
        else
        {
            Attachment = xPawn(Instigator).WeaponAttachment;
            if (Attachment != None && (Level.TimeSeconds - Attachment.LastRenderTime) < 1)
                SetLocation(Attachment.GetTipLocation());
            else
                SetLocation(Instigator.Location + Instigator.EyeHeight*Vect(0,0,1) + Normal(mSpawnVecA - Instigator.Location) * 25.0); 
            Spawn(MuzFlash3Class);
        }
    }

    if ( EffectIsRelevant(mSpawnVecA + HitNormal*2,false) && (HitNormal != Vect(0,0,0)) )
		SpawnImpactEffects(Rotator(HitNormal),mSpawnVecA + HitNormal*2);
	
    if ( (!Level.bDropDetail && (Level.DetailMode != DM_Low) && (VSize(Location - mSpawnVecA) > 40) && !Level.GetLocalPlayerController().BeyondViewDistance(Location,0))
		|| ((Instigator != None) && Instigator.IsFirstPerson()) )
    {
	    Coil = Spawn(CoilClass,,, Location, Rotation);
	    if (Coil != None)
		    Coil.mSpawnVecA = mSpawnVecA;
    }
}

defaultproperties
{
    RemoteRole=ROLE_SimulatedProxy
    bReplicateInstigator=true
    bReplicateMovement=false
    bNetTemporary=true
    LifeSpan=0.75
	NetPriority=3.0

    mParticleType=PT_Beam
    mStartParticles=1
    mAttenKa=0.1
    mSizeRange(0)=24.0
    mSizeRange(1)=48.0
    mRegenDist=150.0
    mLifeRange(0)=0.75
    mMaxParticles=3

	CoilClass=class'ShockBeamCoil'
	MuzFlashClass=class'ShockMuzFlash'
	MuzFlash3Class=class'ShockMuzFlash3rd'
    Texture=Texture'ShockBeamTex'
    Skins(0)=Texture'ShockBeamTex'
    Style=STY_Additive
    bUnlit=true
}
