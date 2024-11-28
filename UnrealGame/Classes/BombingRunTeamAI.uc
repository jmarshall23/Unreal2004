class BombingRunTeamAI extends TeamAI;

var GameObject Bomb; 
var float LastGotFlag;
var GameObjective HomeBase, EnemyBase, BombBase;

function CallForBall(Pawn Recipient)
{
	local Bot B;
	
	if ( (Bomb.Holder == None) || (Bomb.Holder.PlayerReplicationInfo.Team != Team) )
		return;
	
	B = Bot(Bomb.Holder.Controller);
	if ( B == None )
		return;
		
	BombingRunSquadAI(B.Squad).TryPassTo(vector(Bomb.Holder.Rotation),B,Recipient);
}		

function SetObjectiveLists()
{
	local GameObjective O;

	ForEach AllActors(class'GameObjective',O)
		if ( O.bFirstObjective )
		{
			Objectives = O;
			break;
		}
		
	For ( O=Objectives; O!=None; O=O.NextObjective )
	{
		if ( O.DefenderTeamIndex == 255 )
			BombBase = O;
		else if ( O.DefenderTeamIndex == Team.TeamIndex )
			HomeBase = O;
		else
			EnemyBase = O;
	}
}

function SquadAI AddSquadWithLeader(Controller C, GameObjective O)
{
	local BombingRunSquadAI S;

	if ( O == None )
		O = EnemyBase;
	S = BombingRunSquadAI(Super.AddSquadWithLeader(C,O));
	S.Bomb = Bomb;
	S.HomeBase = HomeBase;
	S.EnemyBase = EnemyBase;
	S.BombBase = BombBase;
	return S;
}

defaultproperties
{
	OrderList(0)=ATTACK
	OrderList(1)=ATTACK
	OrderList(2)=DEFEND
	OrderList(3)=ATTACK
	OrderList(4)=FREELANCE
	OrderList(5)=DEFEND
	OrderList(6)=ATTACK
	OrderList(7)=DEFEND
	SquadType=class'UnrealGame.BombingRunSquadAI'
}