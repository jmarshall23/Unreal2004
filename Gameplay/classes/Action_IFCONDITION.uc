class ACTION_IfCondition extends ScriptedAction;

var(Action) name TriggeredConditionTag;
var TriggeredCondition T;

function ProceedToNextAction(ScriptedController C)
{
	if ( (T == None) && (TriggeredConditionTag != 'None') )
		ForEach C.AllActors(class'TriggeredCondition',T,TriggeredConditionTag)
			break;

	C.ActionNum += 1;
	if ( T == None )
		warn("No TriggeredCondition with tag "$TriggeredConditionTag$" found, breaking "$C.SequenceScript);
	if ( !T.bEnabled )
		ProceedToSectionEnd(C);
}

function bool StartsSection()
{
	return true;
}

function string GetActionString()
{
	return ActionString@T@TriggeredConditionTag;
}

defaultproperties
{
	ActionString="If condition"
}