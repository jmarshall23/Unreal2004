class VMeshActor extends Actor
    placeable;

var() Name StartAnim;
var() float StartAnimSpeed;

function Tick(float dt)
{
    //if (Mesh != None && StartAnim != AnimSequence)
    //    LoopAnim(StartAnim, StartAnimSpeed, 0.0);
}

defaultproperties
{
    RemoteRole=ROLE_None
    StartAnim=All
    StartAnimSpeed=1
    DrawType=DT_Mesh
    Style=STY_Normal
    bUnlit=True
}
