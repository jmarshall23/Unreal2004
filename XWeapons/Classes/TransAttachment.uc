class TransAttachment extends xWeaponAttachment;

function InitFor(Inventory I)
{
    Super.InitFor(I);
}

simulated event ThirdPersonEffects()
{
    Super.ThirdPersonEffects();
}

simulated event BaseChange()
{
	if ( (Pawn(Base) != None) && (Pawn(Base).PlayerReplicationInfo != None) && (Pawn(Base).PlayerReplicationInfo.Team != None) )
	{
		if ( Pawn(Base).PlayerReplicationInfo.Team.TeamIndex == 1 )
			Skins[1] = Material'WeaponSkins.NEWTranslocatorBlue';
		else
			Skins[1] = Material'WeaponSkins.NEWTranslocatorTEX';
	}
}

defaultproperties
{
    bHeavy=true
    bRapidFire=false
    bAltRapidFire=false
    Mesh=mesh'NewWeapons2004.NewTransLauncher_3rd'
    Skins(0)=FinalBlend'EpicParticles.NewTransLaunBoltFB'
    Skins(1)=Material'WeaponSkins.NEWTranslocatorTEX'
    Skins(2)=Material'WeaponSkins.NEWTranslocatorPUCK'
    Skins(3)=FinalBlend'WeaponSkins.NEWTransGlassFB'
}
 