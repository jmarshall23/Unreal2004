//-----------------------------------------------------------
//
//-----------------------------------------------------------
class utvMutator extends Mutator;

var string origcontroller;
var class<PlayerController> origcclass;

function ModifyLogin(out string Portal, out string Options)
{
	local bool bSeeAll;
	local bool bSpectator;

	super.ModifyLogin (Portal, Options);
	
	if (Level.game == none) {
		Log ("utv2004s: Level.game is none?");
		return;
	}
	
	if (origcontroller != "") {
		Level.Game.PlayerControllerClassName = origcontroller;
		Level.Game.PlayerControllerClass = origcclass;
		origcontroller = "";
	}
		
    bSpectator = ( Level.Game.ParseOption( Options, "SpectatorOnly" ) ~= "1" );
    bSeeAll = ( Level.Game.ParseOption( Options, "UTVSeeAll" ) ~= "true" );

	if (bSeeAll && bSpectator) {
		Log ("utv2004s: Creating utv controller");
		origcontroller = Level.Game.PlayerControllerClassName;
		origcclass = Level.Game.PlayerControllerClass;
		Level.Game.PlayerControllerClassName = "UTV2004S.utvSpectator";
		Level.Game.PlayerControllerClass = none;
	}    
}	


DefaultProperties
{
    IconMaterialName="MutatorArt.nosym"
    ConfigMenuClassName=""
    GroupName=""
    FriendlyName="UTV2004S"
    Description="Required to support UTV2004 SeeAll mode"
}
