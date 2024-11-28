class TimerMessage extends CriticalEventPlus;

var() Sound CountDownSounds[10]; // OBSOLETE
var name CountDown[10];
var() localized string CountDownTrailer;

static function string GetString(
    optional int Switch,
    optional PlayerReplicationInfo RelatedPRI_1,
    optional PlayerReplicationInfo RelatedPRI_2,
    optional Object OptionalObject
    )
{
    return Switch$default.CountDownTrailer;
}

static function ClientReceive(
    PlayerController P,
    optional int Switch,
    optional PlayerReplicationInfo RelatedPRI_1,
    optional PlayerReplicationInfo RelatedPRI_2,
    optional Object OptionalObject
    )
{
    super.ClientReceive(P, Switch, RelatedPRI_1, RelatedPRI_2, OptionalObject);

    if ( (Switch > 0) && (Switch < 11) && (P.GameReplicationInfo != None) && (P.GameReplicationInfo.Winner == None)
		&& ((P.GameReplicationInfo.RemainingTime > 10) || (P.GameReplicationInfo.RemainingTime == 0)) )
	{
		P.QueueAnnouncement( default.CountDown[Switch-1], 1, AP_InstantOrQueueSwitch, 1 );
	}
}

defaultproperties
{
	bIsConsoleMessage=false
	bFadeMessage=True
	bIsUnique=True
    FontSize=0
	StackMode=SM_Down
    PosY=0.10
    Lifetime=1
    DrawColor=(R=255,G=255,B=0,A=255)
    bBeep=False

    CountDownTrailer="..."
    CountDown(0)=One
    CountDown(1)=Two
    CountDown(2)=Three
    CountDown(3)=Four
    CountDown(4)=Five
    CountDown(5)=Six
    CountDown(6)=Seven
    CountDown(7)=Eight
    CountDown(8)=Nine
    CountDown(9)=Ten
}
