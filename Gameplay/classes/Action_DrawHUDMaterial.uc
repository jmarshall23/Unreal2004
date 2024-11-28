//Draw a texture on the local player's HUD
class Action_DrawHUDMaterial extends LatentScriptedAction;

var(Action) Material HUDMaterial;
var(Action) float PosX, PosY, Width, Height; //scaled to screen resolution if <= 1
var(Action) float DisplayTime;
var ScriptedHudOverlay Overlay;

function bool InitActionFor(ScriptedController C)
{
	local PlayerController PC;

	PC = C.Level.GetLocalPlayerController();
	if (PC == None)
	{
		Warn("No local player!");
		return false;
	}

	if (PC.myHUD == None)
	{
		Warn("Local player has no HUD!");
		return false;
	}

	Overlay = C.spawn(class'ScriptedHudOverlay', PC);
	Overlay.HUDMaterial = HUDMaterial;
	Overlay.PosX = PosX;
	Overlay.PosY = PosY;
	Overlay.Width = Width;
	Overlay.Height = Height;
	PC.myHUD.AddHudOverlay(Overlay);

	C.CurrentAction = self;
	C.SetTimer(DisplayTime, false);

	return true;
}

function ActionCompleted()
{
	Overlay.Destroy();
}

function bool CompleteWhenTriggered()
{
	return true;
}

function bool CompleteWhenTimer()
{
	return true;
}

defaultproperties
{
	ActionString="draw HUD texture"
	PosX=0.4
	PosY=0.4
	Width=0.2
	Height=0.2
	DisplayTime=1.0
}
