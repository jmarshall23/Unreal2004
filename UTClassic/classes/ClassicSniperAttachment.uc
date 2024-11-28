class ClassicSniperAttachment extends xWeaponAttachment;

var xEmitter            mMuzFlash3rd;

simulated function Destroyed()
{
    if (mMuzFlash3rd != None)
        mMuzFlash3rd.Destroy();
    Super.Destroyed();
}

simulated event ThirdPersonEffects()
{
	local vector SmokeLoc, SmokeOffset;
	local coords	C;

    if ( (FlashCount != 0) && (Level.NetMode != NM_DedicatedServer) )
	{
        if (FiringMode == 0)
 			WeaponLight();

 		if ( Instigator.IsFirstPerson() )
 		{
	 		SmokeLoc = Instigator.Location + Instigator.Eyeheight * vect(0,0,1) + Instigator.CollisionRadius * vector(Instigator.Controller.Rotation);
			Spawn(class'ClassicSniperSmoke',,,SmokeLoc);
		}
		else if ( Level.TimeSeconds - Instigator.LastRenderTime < 0.2 )
		{
			if (mMuzFlash3rd == None)
				mMuzFlash3rd = Spawn(class'XEffects.AssaultMuzFlash3rd');

			C = Instigator.GetBoneCoords('righthand');
			SmokeOffset =  -1 * C.ZAxis * (Instigator.CollisionRadius + 35);
			mMuzFlash3rd.SetLocation( C.Origin + SmokeOffset + C.ZAxis * 23 + C.YAxis*4.5);
			mMuzFlash3rd.SetDrawScale(1.0);
			mMuzFlash3rd.SetRotation(rotator(-1 * C.ZAxis));
			mMuzFlash3rd.mStartParticles++;
	 		SmokeLoc = C.Origin + SmokeOffset;
			Spawn(class'ClassicSniperSmoke',,,SmokeLoc);
		}
     }

    Super.ThirdPersonEffects();
}


simulated function Vector GetTipLocation()
{
    return Location -  vector(Rotation) * 100;
}

defaultproperties
{
    bHeavy=false
    bRapidFire=false
    bAltRapidFire=false
    Mesh=Mesh'NewWeapons2004.Sniper3rd'
    DrawType=DT_Mesh
    DrawScale=+0.16

    bDynamicLight=false
    LightType=LT_Steady
    LightEffect=LE_NonIncidence
    LightPeriod=3
    LightBrightness=255
    LightHue=30
    LightSaturation=170
    LightRadius=5

    RelativeLocation=(X=-30.0,Y=0.0,Z=4.0)
    RelativeRotation=(Pitch=32768,Yaw=0,Roll=0)
}
