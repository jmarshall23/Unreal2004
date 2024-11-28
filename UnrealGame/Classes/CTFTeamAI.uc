class CTFTeamAI extends TeamAI;

var CTFFlag FriendlyFlag, EnemyFlag; 
var float LastGotFlag;

function SquadAI AddSquadWithLeader(Controller C, GameObjective O)
{
	local CTFSquadAI S;

	if ( O == None )
		O = EnemyFlag.HomeBase;
	S = CTFSquadAI(Super.AddSquadWithLeader(C,O));
	if ( S != None )
	{
		S.FriendlyFlag = FriendlyFlag;
		S.EnemyFlag = EnemyFlag;
	}
	return S;
}

defaultproperties
{
	OrderList(0)=ATTACK
	OrderList(1)=DEFEND
	OrderList(2)=ATTACK
	OrderList(3)=ATTACK
	OrderList(4)=ATTACK
	OrderList(5)=DEFEND
	OrderList(6)=FREELANCE
	OrderList(7)=ATTACK
	SquadType=class'UnrealGame.CTFSquadAI'
}