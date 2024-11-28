// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class ProGUIController extends GUIController;

event bool OpenMenu(string NewMenuName, optional string Param1, optional string Param2)
{
	if (NewMenuName~="xinterface.ut2midgamemenu")
    	NewMenuName="ProUI.ProMidGameMenu";

    else if (NewMenuName ~= "xinterface.ut2mainmenu" )
    	NewMenuName="ProUI.ProMainMenu";

    return Super.OpenMenu(NewMenuName, Param1, Param2);
}


defaultproperties
{
}