class xBotGibGroup extends xPawnGibGroup;

defaultproperties
{
	LowGoreBloodEmitClass=class'BotSparks'
	BloodEmitClass=class'BotSparks'
	GibSounds(0)=sound'PlayerSounds.RobotCrunch3'
	GibSounds(1)=sound'PlayerSounds.RobotCrunch4'
	GibSounds(2)=sound'PlayerSounds.RobotCrunch3'

    Gibs(0)=class'XEffects.GibBotCalf'
    Gibs(1)=class'XEffects.GibBotForearm'
    Gibs(2)=class'XEffects.GibBotForearm'
    Gibs(3)=class'XEffects.GibBotHead'
    Gibs(4)=class'XEffects.GibBotTorso'
    Gibs(5)=class'XEffects.GibBotUpperArm'
    BloodHitClass=class'XEffects.BotSmallHit'
    LowGoreBloodHitClass=class'XEffects.BotSmallHit'
    BloodGibClass=class'XEffects.BotBloodExplosion'
    LowGoreBloodGibClass=class'XEffects.BotBloodExplosion'
}
