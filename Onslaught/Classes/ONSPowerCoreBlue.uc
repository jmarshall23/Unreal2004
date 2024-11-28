//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSPowerCoreBlue extends ONSPowerCore;

var ONSPowerCoreEnergy  PCEnergyEffect;

simulated function UpdatePrecacheMaterials()
{
	Level.AddPrecacheMaterial(Material'ONSstructureTextures.CoreBreachGroup.coreBreachACCRETIONblue');
	Level.AddPrecacheMaterial(Material'ONSstructureTextures.CoreBreachGroup.CoreBreachShockRingTRANS');
	Level.AddPrecacheMaterial(Material'ONSstructureTextures.CoreBreachGroup.CoreBreachShockRINGedge');
	Level.AddPrecacheMaterial(Material'VMParticleTextures.PowerNodeEXP.powerNodeEXPblueTEX');

    Super.UpdatePrecacheMaterials();
}

simulated function PostBeginPlay()
{
    Super.PostBeginPlay();

    PCEnergyEffect = Spawn(class'ONSPowerCoreEnergyBlue');
}

simulated function PowerCoreDestroyed()
{
    Super.PowerCoreDestroyed();

    if (PCEnergyEffect != None)
        PCEnergyEffect.bHidden = True;
}

simulated function PowerCoreActive()
{
    Super.PowerCoreActive();

    if (PCEnergyEffect != None)
        PCEnergyEffect.bHidden = False;
}

defaultproperties
{
	DefenderTeamIndex=1
    bStartNeutral=False
}
