//==============================================================================
//	Loading screen that appears while you are waiting join a server
//
//	Created by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4ServerLoading extends UT2K4LoadingPageBase
	config(User);

var() config array<string> Backgrounds;

simulated event Init()
{
    Super.Init();

	SetImage();
	SetText();
}

simulated function SetImage()
{
	local int i, cnt;
	local string str;
	local material mat;

	mat = Material'MenuBlack';
	DrawOpImage(Operations[0]).Image = mat;

	if ( Backgrounds.Length == 0 )
	{
		Warn("No background images configured for"@Name);
		return;
	}

	do
	{
		i = Rand(Backgrounds.Length);
		str = Backgrounds[i];
		if ( str == "" )
			Warn("Invalid value for "$Name$".Backgrounds["$i$"]");

		else mat = DLOTexture(str);
	}

	until (mat != None || ++cnt >= 10);

	if ( mat == None )
		Warn("Unable to find any valid images for vignette class"@name$"!");

	DrawOpImage(Operations[0]).Image = mat;
}

simulated function string StripMap(string s)
{
	local int p;

	p = len(s);
	while (p>0)
	{
		if ( mid(s,p,1) == "." )
		{
			s = left(s,p);
			break;
		}
		else
		 p--;
	}

	p = len(s);
	while (p>0)
	{
		if ( mid(s,p,1) == "\\" || mid(s,p,1) == "/" || mid(s,p,1) == ":" )
			return Right(s,len(s)-p-1);
		else
		 p--;
	}

	return s;
}

simulated function SetText()
{
	local DrawOpText HintOp;
	local string Hint;

	DrawOpText(Operations[2]).Text = StripMap(MapName);

	if (Level.IsSoftwareRendering())
		return;

	HintOp = DrawOpText(Operations[3]);
	if ( HintOp == None )
		return;

	if ( GameClass == None )
	{
		Warn("Invalid game class, so cannot draw loading hint!");
		return;
	}

	Hint = GameClass.static.GetLoadingHint(Level.GetLocalPlayerController(), MapName, HintOp.DrawColor);
	if ( Hint == "" )
	{
		log("No loading hint configured for "@GameClass.Name);
		return;
	}

	HintOp.Text = Hint;
}

DefaultProperties
{
	Begin Object Class=DrawOpImage Name=OpBackground
		Top=0
		Lft=0
		Width=1.0
		Height=1.0
		DrawColor=(R=255,B=255,G=255,A=255)
		SubXL=1024
		SubYL=768
	End Object
	Operations(0)=OpBackground

	Begin Object Class=DrawOpText Name=OpLoading
		Top=0.48
		Lft=0.5
		Height=0.05
		Width=0.49
		Justification=2
		Text=". . . LOADING"
		FontName="XInterface.UT2LargeFont"
		bWrapText=False
	End Object
	Operations(1)=OpLoading

	Begin Object Class=DrawOpText Name=OpMapname
		Top=0.6
		Lft=0.5
		Height=0.05
		Width=0.49
		Justification=2
		FontName="XInterface.UT2LargeFont"
		bWrapText=False
	End Object
	Operations(2)=OpMapname

	Begin Object Class=DrawOpText Name=OpHint
		Top=0.8
		Height=0.2
		Lft=0.05
		Width=0.93
		Justification=2
		FontName="GUI2K4.fntUT2k4SmallHeader"
	End Object
	Operations(3)=OpHint

}
