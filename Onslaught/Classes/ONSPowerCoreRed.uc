//-----------------------------------------------------------
//
//-----------------------------------------------------------
class ONSPowerCoreRed extends ONSPowerCore;

var ONSPowerCoreEnergy  PCEnergyEffect;

simulated function UpdatePrecacheMaterials()
{
	Level.AddPrecacheMaterial(Material'ONSstructureTextures.CoreBreachGroup.coreBreachACCRETIONred');
	Level.AddPrecacheMaterial(Material'ONSstructureTextures.CoreBreachGroup.CoreBreachShockRingTRANS');
	Level.AddPrecacheMaterial(Material'ONSstructureTextures.CoreBreachGroup.CoreBreachShockRINGedge');
	Level.AddPrecacheMaterial(Material'VMParticleTextures.PowerNodeEXP.powerNodeEXPredTEX');

    Super.UpdatePrecacheMaterials();
}

simulated function PostBeginPlay()
{
    Super.PostBeginPlay();

    PCEnergyEffect = Spawn(class'ONSPowerCoreEnergyRed');
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

DefaultProperties
{
	DefenderTeamIndex=0
	bStartNeutral=False
}
