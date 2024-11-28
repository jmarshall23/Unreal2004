class InvasionMessage extends CriticalEventPlus
	abstract;
	
var(Message) localized string OutMessage;

//
// Messages common to GameInfo derivatives.
//
static function string GetString(
    optional int Switch,
    optional PlayerReplicationInfo RelatedPRI_1, 
    optional PlayerReplicationInfo RelatedPRI_2,
    optional Object OptionalObject
    )
{
    switch (Switch)
    {
        case 1:
            return RelatedPRI_1.PlayerName@Default.OutMessage;
            break;
    }
    return "";
}

defaultproperties
{
	OutMessage="is OUT!"	

	bIsUnique=True

    FontSize=1

	StackMode=SM_Down
    PosY=0.65
}