//=============================================================================
// FM_Turret_AltFire_Shield
//=============================================================================
// Created by Laurent Delayen
// © 2003, Epic Games, Inc.  All Rights Reserved
//=============================================================================

class FM_Turret_AltFire_Shield extends WeaponFire;

var()	float	AmmoRegenTime;
var()	float	ChargeupTime;
var		float	RampTime;
var		Sound	ChargingSound;  

function DoFireEffect()
{
    local WA_Turret Attachment;

    Attachment = WA_Turret(Weapon.ThirdPersonActor);
    Instigator.AmbientSound = ChargingSound;
    
    if ( Attachment != None && Attachment.ShieldEffect3rd != None )
        Attachment.ShieldEffect3rd.bHidden = false;

    SetTimer(AmmoRegenTime, true);
}

function PlayFiring()
{
    SetTimer(AmmoRegenTime, true);
}

function StopFiring()
{
    local WA_Turret Attachment;

    Attachment = WA_Turret(Weapon.ThirdPersonActor);
	Instigator.AmbientSound = None;
    
    if ( Attachment != None && Attachment.ShieldEffect3rd != None )
        Attachment.ShieldEffect3rd.bHidden = true;

    SetTimer(AmmoRegenTime, true);
}

function TakeHit( int Drain )
{
    SetBrightness( true );
}

function SetBrightness(bool bHit)
{
    local WA_Turret		Attachment;
 	local float			Brightness;

	Brightness = Weapon.AmmoAmount(1);
	if ( RampTime < ChargeUpTime )
		Brightness *= RampTime/ChargeUpTime; 

    Attachment = WA_Turret(Weapon.ThirdPersonActor);
    if ( Attachment != None )
        Attachment.SetBrightness(Brightness, bHit);
}

function Timer()
{
    if ( !bIsFiring )
    {
		RampTime = 0;
        if ( !Weapon.AmmoMaxed(1) )
			Weapon.AddAmmo(1,1);
        else
            SetTimer(0, false);
    }
    else
    {
        if ( !Weapon.ConsumeAmmo(1,1) )
        {
            if ( Weapon.ClientState == WS_ReadyToFire )
                Weapon.PlayIdle();
            StopFiring();
        }
        else
			RampTime += AmmoRegenTime;
    }
	
	SetBrightness( false );
}

defaultproperties
{
	AmmoClass=class'Ammo_BallTurret'

    AmmoPerFire=15
    AmmoRegenTime=0.2
    ChargeUpTime=3.0

    ChargingSound=Sound'WeaponSounds.BShield1'
    FireSound=Sound'WeaponSounds.Translocator.TranslocatorModuleRegeneration'
    FireForce="TranslocatorModuleRegeneration"  // jdf
    bPawnRapidFireAnim=true
    FireRate=1.0
    bModeExclusive=true
    bWaitForRelease=true
    MaxHoldTime=0.0

    BotRefireRate=1.0
}