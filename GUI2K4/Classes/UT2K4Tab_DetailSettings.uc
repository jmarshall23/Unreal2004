//==============================================================================
//	Description
//
//	Created by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class UT2K4Tab_DetailSettings extends Settings_Tabs;

struct DisplayMode
{
	var int	Width,
			Height;
};

const RENDERMODECOUNT = 2;

var() DisplayMode 		DisplayModes[17];
var() localized string	BitDepthText[2];
var() localized string	DetailLevels[9];
var() localized string  ShadowOptions[3];
var() localized string  RenderModeText[RENDERMODECOUNT];
var() string            RenderMode[RENDERMODECOUNT];
var() string            DisplayPromptMenu;
var() bool		        bPlayedSound;

var automated GUISectionBackground sb_Section1, sb_Section2, sb_Section3;

var automated GUIImage i_Gamma, i_GammaBar;
var automated moComboBox	co_Texture, co_Char, co_World, co_Physics, co_Decal, co_MeshLOD,
							co_Resolution, co_ColorDepth, co_RenderDevice, co_Shadows;

var automated moCheckBox	ch_Decals, ch_DynLight, ch_Coronas,
							ch_Textures, ch_Projectors, ch_DecoLayers,
							ch_Trilinear, ch_FullScreen, ch_Weather;


var automated moSlider		sl_Gamma, sl_Brightness, sl_Contrast, sl_DistanceLOD;

// Instance values
var() noexport transient string sRes, sResD, sRenDev, sRenDevD;
var() noexport transient float fGamma, fBright, fContrast, fDistance, fDistanceD;
var() noexport transient bool bDecal, bDynLight, bTexture, bCorona, bTrilin,
                              bProj, bFol, bFullScreen, bWeather,
							  bDecalD, bDynLightD, bTextureD, bCoronaD, bTrilinD,
							  bProjD, bFolD, bFullScreenD, bWeatherD;
var() noexport transient int iColDepth, iTexture, iChar, iPhys, iWorld, iDecal, iShadow, iMeshLOD,
                             iColDepthD, iTextureD, iCharD, iWorldD, iPhysD, iDecalD, iShadowD, iMeshLODD;

var() private noexport editconst transient bool bDemo;
var() noexport bool bIgnoreResNotice, bInvalidRes, bIgnoreChange;
var() localized string RelaunchQuestion, InvalidSelectionText;


function InitComponent(GUIController MyController, GUIComponent MyOwner)
{
	Super.InitComponent(MyController, MyOwner);
	bDemo = PlayerOwner().Level.IsDemoBuild();

	InitializeCombos();
	SetupPositions();
}

event Opened(GUIComponent Sender)
{
	Super.Opened(Sender);

	CheckSupportedResolutions();
}

function ShowPanel(bool bShow)
{
	Super.ShowPanel(bShow);

	if ( bShow )
		CheckSliders();
}

function InitializeCombos()
{
	local int i;
	local array<GUIListElem> Options;

	for (i = 0; i < Components.Length; i++)
	{
		if (moComboBox(Components[i]) != None)
		{
			GetComboOptions( moComboBox(Components[i]), Options );
			moComboBox(Components[i]).MyComboBox.List.Elements = Options;
			moComboBox(Components[i]).MyComboBox.List.ItemCount = Options.Length;
			moComboBox(Components[i]).ReadOnly(True);
		}
	}

	if ( !bDemo )
	{
		co_Texture.MyComboBox.MaxVisibleItems = 9;
		co_Char.MyComboBox.MaxVisibleItems = 9;
	}
}

function CheckSliders()
{
	// SDLDrv can adjust gamma ramps in a Window, WinDrv can't...  --ryan.
	if ( ( bFullscreen ) || ( !PlatformIsWindows() ) )
	{
		EnableComponent(sl_Gamma);
		EnableComponent(sl_Contrast);
		EnableComponent(sl_Brightness);
	}

	else
	{
		DisableComponent(sl_Gamma);
		DisableComponent(sl_Contrast);
		DisableComponent(sl_Brightness);
	}
}

function SetupPositions()
{
    sb_Section1.ManageComponent(co_RenderDevice);
	sb_Section1.ManageComponent(co_Resolution);
    sb_Section1.ManageComponent(co_ColorDepth);
    sb_Section1.ManageComponent(ch_Fullscreen);
    sb_Section1.ManageComponent(sl_Gamma);
    sb_Section1.ManageComponent(sl_Brightness);
    sb_Section1.ManageComponent(sl_Contrast);

    sb_Section2.Managecomponent(co_Texture);
    sb_Section2.ManageComponent(co_Char);
    sb_Section2.ManageComponent(co_World);
    sb_Section2.ManageComponent(co_Physics);
	sb_Section2.ManageComponent(co_Decal);
    sb_Section2.ManageComponent(co_Shadows);
    sb_Section2.ManageComponent(co_MeshLOD);
    sb_Section2.ManageComponent(ch_Decals);
    sb_Section2.ManageComponent(ch_DynLight);
    sb_Section2.ManageComponent(ch_Coronas);
    sb_Section2.ManageComponent(ch_Textures);
    sb_Section2.ManageComponent(ch_Projectors);
    sb_Section2.ManageComponent(ch_DecoLayers);
    sb_Section2.ManageComponent(ch_Trilinear);
    sb_Section2.ManageComponent(ch_Weather);
    sb_Section2.ManageComponent(sl_DistanceLOD);

	sb_Section3.ManageComponent(i_GammaBar);
}

function CheckSupportedResolutions()
{
	local int		i, Index, BitDepth;
	local string	CurrentSelection, Str, W, H, WH;
	local bool 		bStandard, bSupported, bOldIgnoreChange;
	local PlayerController PC;

	PC = PlayerOwner();
	CurrentSelection = co_Resolution.GetText();

	// Disable all notifications!!
	bOldIgnoreChange = bIgnoreChange;
	bIgnoreChange = True;

	if (co_ColorDepth != None && co_ColorDepth.ItemCount() > 0)
		BitDepth = int(Left(co_ColorDepth.GetText(), 2));

	// Don't let user create non-fullscreen window bigger than highest
	//  supported resolution, or MacOS X client crashes. --ryan.
	for(i = 0; i < ArrayCount(DisplayModes); i++)
	{
		W = string(DisplayModes[i].Width);
		H = string(DisplayModes[i].Height);
		WH = W $ "x" $ H;

		Str = "SUPPORTEDRESOLUTION WIDTH="$W@"HEIGHT="$H@"BITDEPTH="$BitDepth;

		if (CurrentSelection ~= WH)
			bStandard = True;

		Index = co_Resolution.FindIndex(WH);
		bSupported = bool(PC.ConsoleCommand(Str));

		if ( !bSupported && co_Resolution.MyComboBox.List.IsValidIndex(Index) )
			co_Resolution.RemoveItem(Index, 1);

		else if ( bSupported && !co_Resolution.MyComboBox.List.IsValidIndex(Index) )
			AddNewResolution(WH);
	}

	if (!bStandard)
	{
		Divide(CurrentSelection, "x", W, H);

		Str = "SUPPORTEDRESOLUTION WIDTH="$W@"HEIGHT="$H@"BITDEPTH="$BitDepth;
		bSupported = bool(PC.ConsoleCommand(Str));
		if ( !bSupported )
		{
			Index = co_Resolution.FindIndex(CurrentSelection);
			bInvalidRes = True;
		}
	}

	CheckSliders();
	co_Resolution.SetText(CurrentSelection);
	bIgnoreChange = bOldIgnoreChange;
}

function bool InternalOnPreDraw(Canvas C)
{
	if ( Controller.ActivePage == PageOwner && bInvalidRes )
	{
		bInvalidRes = False;
		Controller.OpenMenu("GUI2K4.UT2K4GenericMessageBox","",InvalidSelectionText);
	}

	return False;
}

function ResolutionChanged(int NewX, int NewY)
{
	if ( !bIgnoreResNotice )
	{
		if ( Controller.ActivePage == PageOwner )
			PageOwner.CheckResolution(false, Controller);

		Refresh();
	}
	else
		InternalOnLoadINI(co_ColorDepth,"");
}

function Refresh()
{
	InternalOnLoadINI(co_Resolution,"");
	InternalOnLoadINI(co_ColorDepth,"");
	InternalOnLoadINI(ch_FullScreen,"");
	CheckSupportedResolutions();
}

function int AddNewResolution(string NewDisplayMode)
{
	local int i, ItemW, ItemH, InWidth, InHeight;
	local bool bTemp;
	local string Str, StrW, StrH;

	bTemp = bIgnoreChange;
	bIgnoreChange = True;

	// first, find out if this resolution exists already
	i = co_Resolution.FindIndex(NewDisplayMode);
	if (i >= 0)
	{
		bIgnoreChange = bTemp;
		return i;
	}

	Divide(NewDisplayMode, "x", StrW, StrH);

	InWidth = int(StrW);
	InHeight = int(StrH);

	// not there, so add it in the appropriate spot
	for (i = 0; i < co_Resolution.ItemCount(); i++)
	{
		Str = co_Resolution.GetItem(i);
		Divide(Str, "x", StrW, StrH);

		ItemW = int(StrW);
		ItemH = int(StrH);

		if ( ItemW == InWidth )
		{
			if ( ItemH > InHeight )
				break;
		}

		else if ( ItemW > InWidth )
			break;
	}

	if (i == co_Resolution.ItemCount())
		co_Resolution.AddItem(NewDisplayMode);

	else
		co_Resolution.MyComboBox.List.Insert(i, NewDisplayMode);

	bIgnoreChange = bTemp;
	return i;
}

function InternalOnLoadINI(GUIComponent Sender, string s)
{
	local bool a, b;
	local int i;
	local PlayerController PC;
	local string tempStr;

	PC = PlayerOwner();
	switch (Sender)
	{
	case co_Texture:
		s = GetGUIString(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager TextureDetailWorld"));
		iTexture = co_Texture.FindIndex(s);
		iTextureD = iTexture;
		co_Texture.SilentSetIndex(iTexture);
		break;

	case co_Char:
		s = GetGUIString(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager TextureDetailPlayerSkin"));
		iChar = co_Char.FindIndex(s);
		iCharD = iChar;
		co_Char.SilentSetIndex(iChar);
		break;

	case co_World:
		a = bool(PC.ConsoleCommand("get ini:Engine.Engine.RenderDevice HighDetailActors"));
		b = bool(PC.ConsoleCommand("get ini:Engine.Engine.RenderDevice SuperHighDetailActors"));

		if(b)
			iWorld = 5;
		else if(a)
			iWorld = 4;
		else
			iWorld = 3;

		iWorldD = iWorld;
		i = co_World.FindIndex(DetailLevels[iWorld]);
		if ( i != -1 )
			co_World.SilentSetIndex(i);

		break;

	case co_MeshLOD:
		switch ( class'LevelInfo'.default.MeshLODDetailLevel )
		{
		case MDL_Low:    iMeshLOD = 3; break;
		case MDL_Medium: iMeshLOD = 4; break;
		case MDL_High:   iMeshLOD = 5; break;
		case MDL_Ultra:  iMeshLOD = 8; break;
		}

		iMeshLODD = iMeshLOD;

		i = co_MeshLOD.FindIndex(DetailLevels[iMeshLOD]);
		if ( i != -1 )
			co_MeshLOD.SilentSetIndex(i);
		break;


	case co_Physics:
		if(class'LevelInfo'.default.PhysicsDetailLevel == PDL_Low)
		{
			iPhys = 3;
			i = co_Physics.FindIndex(DetailLevels[3]);
			if ( i != -1 )
				co_Physics.SilentSetIndex(i);
		}
		else if(class'LevelInfo'.default.PhysicsDetailLevel == PDL_Medium)
		{
			iPhys = 4;
			i = co_Physics.FindIndex(DetailLevels[4]);
			if ( i != -1 )
				co_Physics.SilentSetIndex(i);
		}
		else
		{
			iPhys = 5;
			i = co_Physics.FindIndex(DetailLevels[5]);
			if ( i != -1 )
				co_Physics.SilentSetIndex(i);
		}

		iPhysD = iPhys;
		break;

	case co_Decal:
		iDecal = class'LevelInfo'.default.DecalStayScale;
		iDecalD = iDecal;
		co_Decal.SilentSetIndex(iDecal);
		break;

	case co_Resolution:
		// Resolution
		// GameResolution is set if menu requires 640x480 but current resolution is smaller than that
		if(Controller.GameResolution != "")
			sRes = Controller.GameResolution;
		else sRes = Controller.GetCurrentRes();
		sResD = sRes;
		i = AddNewResolution(sRes);

		if ( i >= 0 && i < co_Resolution.ItemCount() )
			co_Resolution.SilentSetIndex(i);
		break;

	case co_ColorDepth:
		if (bool(PC.ConsoleCommand("get ini:Engine.Engine.RenderDevice Use16bit")))
			iColDepth = 0;
		else iColDepth = 1;
		iColDepthD = iColDepth;
		co_ColorDepth.SilentSetIndex(iColDepth);

		if (! bool(PC.ConsoleCommand("ISFULLSCREEN")) )
			co_ColorDepth.DisableMe();
		else
			co_ColorDepth.EnableMe();

		break;

	case co_RenderDevice:
		sRenDev = GetNativeClassName("Engine.Engine.RenderDevice");
		sRenDevD = sRenDev;
		co_RenderDevice.SetComponentValue(sRenDev,true);
		break;

	case co_Shadows:
		tempStr = GetNativeClassName("Engine.Engine.RenderDevice");

		// No render-to-texture on anything but Direct3D.
		if ((tempStr == "D3DDrv.D3DRenderDevice") ||
		    (tempStr == "D3D9Drv.D3D9RenderDevice"))
		{
			a = bool(PC.ConsoleCommand("get UnrealGame.UnrealPawn bPlayerShadows"));
			b = bool(PC.ConsoleCommand("get UnrealGame.UnrealPawn bBlobShadow"));

			if ( b )
				iShadow = 1;
			else if (a)
				iShadow = 2;
			else
				iShadow = 0;
		}
        else
		{
			b = bool(PC.ConsoleCommand("get UnrealGame.UnrealPawn bBlobShadow"));
			if ( b )
				iShadow = 1;
			else
				iShadow = 0;
		}

		iShadowD = iShadow;
		co_Shadows.SilentSetIndex(iShadow);
		break;

	case ch_DynLight:
		bDynLight = !bool(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager NoDynamicLights"));
		bDynLightD = bDynLight;
		ch_DynLight.SetComponentValue(bDynLight,true);
		break;

	case ch_FullScreen:
		bFullScreen = bool(PC.ConsoleCommand("ISFULLSCREEN"));
		bFullScreenD = bFullScreen;
		moCheckBox(Sender).SetComponentValue(bFullScreen,true);
		break;

	case ch_Trilinear:
		bTrilin = bool(PC.ConsoleCommand("get ini:Engine.Engine.RenderDevice UseTrilinear"));
		bTrilinD = bTrilin;
		ch_Trilinear.SetComponentValue(bTrilin,true);
		break;

	case ch_Projectors:
		bProj = bool(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager Projectors"));
		bProjD = bProj;
		ch_Projectors.SetComponentValue(bProj,true);
		break;

	case ch_DecoLayers:
		bFol = bool(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager DecoLayers"));
		bFolD = bFol;
		ch_DecoLayers.SetComponentValue(bFol,true);
		break;

	case ch_Textures:
		bTexture = bool(PC.ConsoleCommand("get ini:Engine.Engine.RenderDevice DetailTextures"));
		bTextureD = bTexture;
		ch_Textures.SetComponentValue(bTexture,true);
		break;

	case ch_Coronas:
		bCorona = bool(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager Coronas"));
		bCoronaD = bCorona;
		ch_Coronas.SetComponentValue(bCorona,true);
		break;

	case ch_Decals:
		bDecal = bool(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager Decals"));
		bDecalD = bDecal;
		ch_Decals.SetComponentValue(bDecal,true);

		UpdateDecalStay();
		break;

	case sl_Gamma:
		fGamma = float(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager Gamma"));
		sl_Gamma.SetComponentValue(fGamma,true);
		break;

	case sl_Brightness:
		fBright = float(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager Brightness"));
		sl_Brightness.SetComponentValue(fBright,true);
		break;

	case sl_Contrast:
		fContrast = float(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager Contrast"));
		sl_Contrast.SetComponentValue(fContrast,true);
		break;

	case ch_Weather:
		bWeather = bool(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager WeatherEffects"));
		bWeatherD = bWeather;
		ch_Weather.SetComponentValue(bWeather,true);
		break;

	case sl_DistanceLOD:
		fDistance = float(PC.ConsoleCommand("get ini:Engine.Engine.ViewportManager DrawDistanceLOD"));
		fDistanceD = fDistance;
		sl_DistanceLOD.SetComponentValue(fDistance,true);
		break;

	default:
		log(Name@"Unknown component calling LoadINI:"$ GUIMenuOption(Sender).Caption);
		GUIMenuOption(Sender).SetComponentValue(s,true);
	}
}

function ResetClicked()
{
	local int i;

	Super.ResetClicked();

	class'LevelInfo'.static.ResetConfig("MeshLODDetailLevel");
	class'LevelInfo'.static.ResetConfig("PhysicsDetailLevel");
	class'LevelInfo'.static.ResetConfig("DecalStayScale");

	class'UnrealPawn'.static.ResetConfig("bPlayerShadows");
	class'UnrealPawn'.static.ResetConfig("bBlobShadow");

	ResetViewport();
	ResetRenderDevice();

	for (i = 0; i < Components.Length; i++)
		Components[i].LoadINI();
}

function ResetViewport()
{
	local class<Client> ViewportClass;

	ViewportClass = class<Client>(DynamicLoadObject(GetNativeClassName("Engine.Engine.ViewportManager"), class'Class'));

	ViewportClass.static.ResetConfig("WindowedViewportX");
	ViewportClass.static.ResetConfig("WindowedViewportY");
	ViewportClass.static.ResetConfig("FullscreenViewportX");
	ViewportClass.static.ResetConfig("FullscreenViewportY");
	ViewportClass.static.ResetConfig("Brightness");
	ViewportClass.static.ResetConfig("Contrast");
	ViewportClass.static.ResetConfig("Gamma");
	ViewportClass.static.ResetConfig("StartupFullScreen");
	ViewportClass.static.ResetConfig("NoLighting");
	ViewportClass.static.ResetConfig("MinDesiredFrameRate");
	ViewportClass.static.ResetConfig("Decals");
	ViewportClass.static.ResetConfig("Coronas");
	ViewportClass.static.ResetConfig("DecoLayers");
	ViewportClass.static.ResetConfig("Projectors");
	ViewportClass.static.ResetConfig("NoDynamicLights");
	ViewportClass.static.ResetConfig("ReportDynamicUploads");
	ViewportClass.static.ResetConfig("TextureDetailInterface");
	ViewportClass.static.ResetConfig("TextureDetailTerrain");
	ViewportClass.static.ResetConfig("TextureDetailWeaponSkin");
	ViewportClass.static.ResetConfig("TextureDetailPlayerSkin");
	ViewportClass.static.ResetConfig("TextureDetailWorld");
	ViewportClass.static.ResetConfig("TextureDetailRendermap");
	ViewportClass.static.ResetConfig("TextureDetailLightmap");
	ViewportClass.static.ResetConfig("NoFractalAnim");
	ViewportClass.static.ResetConfig("WeatherEffects");
	ViewportClass.static.ResetConfig("DrawDistanceLOD");
}

function ResetRenderDevice()
{
	local class<RenderDevice>	RenderClass;

	if ( !(sRenDev ~= sRenDevD) )
		Controller.SetRenderDevice(sRenDevD);

	RenderClass = class<RenderDevice>(DynamicLoadObject(sRenDevD, class'Class'));
	RenderClass.static.ResetConfig("DetailTextures");
	RenderClass.static.ResetConfig("HighDetailActors");
	RenderClass.static.ResetConfig("SuperHighDetailActors");
	RenderClass.static.ResetConfig("UsePrecaching");
	RenderClass.static.ResetConfig("UseTrilinear");
	RenderClass.static.ResetConfig("UseTripleBuffering");
	RenderClass.static.ResetConfig("UseHardwareTL");
	RenderClass.static.ResetConfig("UseHardwareVS");
	RenderClass.static.ResetConfig("UseCubemaps");
	RenderClass.static.ResetConfig("DesiredRefreshRate");
	RenderClass.static.ResetConfig("UseCompressedLightmaps");
	RenderClass.static.ResetConfig("UseStencil");
	RenderClass.static.ResetConfig("Use16bit");
	RenderClass.static.ResetConfig("Use16bitTextures");
	RenderClass.static.ResetConfig("MaxPixelShaderVersion");
	RenderClass.static.ResetConfig("UseVSync");
	RenderClass.static.ResetConfig("LevelOfAnisotropy");
	RenderClass.static.ResetConfig("DetailTexMipBias");
	RenderClass.static.ResetConfig("DefaultTexMipBias");
	RenderClass.static.ResetConfig("UseNPatches");
	RenderClass.static.ResetConfig("TesselationFactor");
	RenderClass.static.ResetConfig("CheckForOverflow");
	RenderClass.static.ResetConfig("AvoidHitches");
	RenderClass.static.ResetConfig("OverrideDesktopRefreshRate");

}

function SaveSettings()
{
	local string t, v, Str;
	local PlayerController PC;
	local bool bUnreal, bLevel;

	Super.SaveSettings();

	PC = PlayerOwner();

	if ( sRenDev != sRenDevD )
	{
		if ( Controller.SetRenderDevice(sRenDev) )
			sRenDevD = sRenDev;
	}

	if (iTexture != iTextureD)
	{
		t = "set ini:Engine.Engine.ViewportManager TextureDetail";

		Str = DetailLevels[iTexture];
		v = GetConfigString(Str);
		PC.ConsoleCommand(t$"Terrain"@v);
		PC.ConsoleCommand(t$"World"@v);
		PC.ConsoleCommand(t$"Rendermap"@v);
		PC.ConsoleCommand(t$"Lightmap"@v);
		PC.ConsoleCommand("flush");
		iTextureD = iTexture;
	}

	if (iChar != iCharD)
	{
		t = "set ini:Engine.Engine.ViewportManager TextureDetail";

		Str = DetailLevels[iChar];
		v = GetConfigString(Str);

		PC.ConsoleCommand(t$"WeaponSkin"@v);
		PC.ConsoleCommand(t$"PlayerSkin"@v);
		PC.ConsoleCommand("flush");

		iCharD = iChar;
	}

	if (iWorld != iWorldD)
	{
		Str = DetailLevels[iWorld];
		v = GetConfigString(Str);

		switch (iWorld)
		{
			case 3:
				PC.ConsoleCommand("set ini:Engine.Engine.RenderDevice HighDetailActors False");
				PC.ConsoleCommand("set ini:Engine.Engine.RenderDevice SuperHighDetailActors False");
				PC.Level.DetailChange(DM_Low);
				break;

			case 4:
				PC.ConsoleCommand("set ini:Engine.Engine.RenderDevice HighDetailActors True");
				PC.ConsoleCommand("set ini:Engine.Engine.RenderDevice SuperHighDetailActors False");
				PC.Level.DetailChange(DM_High);
				break;

			case 5:
				PC.ConsoleCommand("set ini:Engine.Engine.RenderDevice HighDetailActors True");
				PC.ConsoleCommand("set ini:Engine.Engine.RenderDevice SuperHighDetailActors True");
				PC.Level.DetailChange(DM_SuperHigh);
				break;
		}

		iWorldD = iWorld;
	}

	if ( iMeshLOD != iMeshLODD )
	{
		switch (iMeshLOD)
		{
		case 3:
			class'LevelInfo'.default.MeshLODDetailLevel = MDL_Low;
			PC.Level.MeshLODDetailLevel = MDL_Low;
			break;
		case 4:
			class'LevelInfo'.default.MeshLODDetailLevel = MDL_Medium;
			PC.Level.MeshLODDetailLevel = MDL_Medium;
			break;

		case 5:
			class'LevelInfo'.default.MeshLODDetailLevel = MDL_High;
			PC.Level.MeshLODDetailLevel = MDL_High;
			break;
		case 8:
			class'LevelInfo'.default.MeshLODDetailLevel = MDL_Ultra;
			PC.Level.MeshLODDetailLevel = MDL_Ultra;
			break;
		}

		iMeshLODD = iMeshLOD;
		bLevel = True;
	}

	if (iPhys != iPhysD)
	{
		switch (iPhys)
		{
			case 3:
				class'LevelInfo'.default.PhysicsDetailLevel = PDL_Low;
				PC.Level.PhysicsDetailLevel = PDL_Low;
				break;

			case 4:
				class'LevelInfo'.default.PhysicsDetailLevel = PDL_Medium;
				PC.Level.PhysicsDetailLevel = PDL_Medium;
				break;

			case 5:
				class'LevelInfo'.default.PhysicsDetailLevel = PDL_High;
				PC.Level.PhysicsDetailLevel = PDL_High;
				break;
		}

		iPhysD = iPhys;
		bLevel = True;
	}

	if ( iShadow != iShadowD )
	{
		if ( PC.Pawn != None && UnrealPawn(PC.Pawn) != None )
		{
			UnrealPawn(PC.Pawn).bBlobShadow = iShadow == 1;
			UnrealPawn(PC.Pawn).bPlayerShadows = iShadow > 0;
		}

		class'UnrealPawn'.default.bBlobShadow = iShadow == 1;
		class'UnrealPawn'.default.bPlayerShadows = iShadow > 0;
		iShadowD = iShadow;
		bUnreal = True;
	}

	if ( class'Vehicle'.default.bVehicleShadows != (iShadow > 0) )
	{
		class'Vehicle'.default.bVehicleShadows = iShadow > 0;
		class'Vehicle'.static.StaticSaveConfig();
	}

	if (bDynLight != bDynLightD)
	{
		PC.ConsoleCommand("set ini:Engine.Engine.ViewportManager NoDynamicLights"@!bDynLight);
		bDynLightD = bDynLight;
	}

	if (iDecal != iDecalD)
	{
		if (PC.Level != None)
			PC.Level.DecalStayScale = iDecal;

		class'LevelInfo'.default.DecalStayScale = iDecal;

		iDecalD = iDecal;
		bLevel = True;
	}

	if (bTrilin != bTrilinD)
	{
		PC.ConsoleCommand("set ini:Engine.Engine.RenderDevice UseTrilinear"@bTrilin);
		bTrilinD = bTrilin;
	}

	if (bFol != bFolD)
	{
		PC.ConsoleCommand("set ini:Engine.Engine.ViewportManager DecoLayers"@bFol);
		bFolD = bFol;
	}


	if (bProj != bProjD)
	{
		PC.ConsoleCommand("set ini:Engine.Engine.ViewportManager Projectors"@bProj);
		bProjD = bProj;
	}

	if (bTexture != bTextureD)
	{
		PC.ConsoleCommand("set ini:Engine.Engine.RenderDevice DetailTextures"@bTexture);
		bTextureD = bTexture;
	}

	if (bCorona != bCoronaD)
	{
		PC.ConsoleCommand("set ini:Engine.Engine.ViewportManager Coronas"@bCorona);
		bCoronaD = bCorona;
	}

	if (bDecal != bDecalD)
	{
		PC.ConsoleCommand("set ini:Engine.Engine.ViewportManager Decals"@bDecal);
		bDecalD = bDecal;
	}

	if (bWeather != bWeatherD)
	{
		PC.ConsoleCommand("set ini:Engine.Engine.ViewportManager WeatherEffects"@bWeather);
		bWeatherD = bWeather;
	}

	if ( fDistance != fDistanceD )
	{
		PC.ConsoleCommand("set ini:Engine.Engine.ViewportManager DrawDistanceLOD" @ fDistance);
		PC.Level.UpdateDistanceFogLOD(fDistance);
		fDistanceD = fDistance;
	}

	if (bUnreal)
	{
		if (PC.Pawn != None && UnrealPawn(PC.Pawn) != None)
			UnrealPawn(PC.Pawn).SaveConfig();

		else class'UnrealPawn'.static.StaticSaveConfig();
	}

	if (bLevel)
	{
		if (PC.Level != None)
			PC.Level.SaveConfig();
		else class'LevelInfo'.static.StaticSaveConfig();
	}
}

function InternalOnChange(GUIComponent Sender)
{
	local string str;
	local bool bGoingUp;
	local int i;
	local sound snd;

	Super.InternalOnChange(Sender);

	if ( bIgnoreChange )
		return;

	switch (Sender)
	{
		// These changes take effect immediately
		case co_Resolution:
			sRes = co_Resolution.GetText();
			if (bFullScreen)
				str = "f";
			else
				str = "w";

			if (Controller.OpenMenu(DisplayPromptMenu))
			{
				bIgnoreResNotice = True;
				UT2K4VideoChangeOK(Controller.ActivePage).OnClose = VideoChangeClose;
				UT2K4VideoChangeOK(Controller.ActivePage).Execute(sRes$"x"$Left(BitDepthText[iColDepth], 2)$str);
			}
			break;

		case ch_FullScreen:
			bFullScreen = ch_FullScreen.IsChecked();
			if (bFullScreen)
				str = "f";
			else
				str = "w";

			if (Controller.OpenMenu(DisplayPromptMenu))
			{
				bIgnoreResNotice = True;
				UT2K4VideoChangeOK(Controller.ActivePage).OnClose = VideoChangeClose;
				UT2K4VideoChangeOK(Controller.ActivePage).Execute(sRes$"x"$Left(BitDepthText[iColDepth], 2)$str);
			}
			break;

		case co_RenderDevice:
			sRenDev = co_RenderDevice.GetExtra();
			if ( sRenDev != sRenDevD && Controller.OpenMenu(Controller.QuestionMenuClass) )
			{
				GUIQuestionPage(Controller.ActivePage).SetupQuestion(RelaunchQuestion, QBTN_YesNoCancel, QBTN_Cancel);
				GUIQuestionPage(Controller.ActivePage).NewOnButtonClick = RenderDeviceClick;
			}
			break;

		case co_ColorDepth:
			iColDepth = co_ColorDepth.GetIndex();
			if (bFullScreen)
				str = "f";
			else
				str = "w";

			if (Controller.OpenMenu(DisplayPromptMenu))
			{
				bIgnoreResNotice = True;
				UT2K4VideoChangeOK(Controller.ActivePage).OnClose = VideoChangeClose;
				UT2K4VideoChangeOK(Controller.ActivePage).Execute(sRes$"x"$Left(BitDepthText[iColDepth], 2)$str);
			}
			break;


		// These changes are saved all together during SaveSettings()
		case co_Texture:
			i = co_Texture.GetIndex();
			bGoingUp = i > iTexture && i != iTextureD;
			iTexture = i;
			break;

		case co_Char:
			i = co_Char.GetIndex();
			bGoingUp = i > iChar && i != iCharD;
			iChar = i;
			break;

		case co_World:
			str = co_World.GetText();
			i = GetDetailIndex(str);
			bGoingUp = i > iWorld && i != iWorldD;
			iWorld = i;
			break;

		case co_MeshLOD:
			str = co_MeshLOD.GetText();
			i = GetDetailIndex(str);
			bGoingUp = i > iMeshLOD && i != iMeshLODD;
			iMeshLOD = i;
			break;

		case co_Physics:
			str = co_Physics.GetText();
			i = GetDetailIndex(str);
			bGoingUp = i > iPhys && i != iPhysD;
			iPhys = i;
			break;

		case co_Decal:
			iDecal = co_Decal.GetIndex();
			break;

		case co_Shadows:
			i = co_Shadows.GetIndex();
			bGoingUp = i > iShadow && i != iShadowD;
			iShadow = i;
			break;

		case ch_DynLight:
			bDynLight = ch_DynLight.IsChecked();
			bGoingUp = bDynLight && bDynLight != bDynLightD;
			break;

		case ch_Trilinear:
			bTrilin = ch_Trilinear.IsChecked();
			bGoingUp = bTrilin && bTrilin != bTrilinD;
			break;

		case ch_Projectors:
			bProj = ch_Projectors.IsChecked();
			bGoingUp = bProj && bProjD != bProj;
			break;

		case ch_DecoLayers:
			bFol = ch_DecoLayers.IsChecked();
			bGoingUp = bFol && bFol != bFolD;
			break;

		case ch_Textures:
			bTexture = ch_Textures.IsChecked();
			bGoingUp = bTexture && bTexture != bTextureD;
			break;

		case ch_Coronas:
			bCorona = ch_Coronas.IsChecked();
			bGoingUp = bCorona && bCorona != bCoronaD;
			break;

		case ch_Decals:
			bDecal = ch_Decals.IsChecked();
			bGoingUp = bDecal && bDecal != bDecalD;
			UpdateDecalStay();
			break;

		case sl_Gamma:
			fGamma = sl_Gamma.GetValue();
			PlayerOwner().ConsoleCommand("GAMMA"@fGamma);
			break;

		case sl_Brightness:
			fBright = sl_Brightness.GetValue();
			PlayerOwner().ConsoleCommand("BRIGHTNESS"@fBright);
			break;

		case sl_Contrast:
			fContrast = sl_Contrast.GetValue();
			PlayerOwner().ConsoleCommand("CONTRAST"@fContrast);
			break;

		case sl_DistanceLOD:
			fDistance = sl_DistanceLOD.GetValue();
			break;

		case ch_Weather:
			bWeather = ch_Weather.IsChecked();
			bGoingUp = bWeather && bWeather != bWeatherD;
			break;

	}

	if (bGoingUp)
		ShowPerformanceWarning();

	// Check if we are maxed out (and mature-enabled)!
	if( !bPlayedSound && !PlayerOwner().bNoMatureLanguage && iTexture == 8
		&& iChar == 8 && iWorld == 5 && iPhys == 5 && iShadow == 2 &&
		bWeather && bDecal && bDynLight && bProj && bFol &&
		bCorona && bTexture && iDecal == 2 && iMeshLOD == 8 )
	{
		snd = sound(DynamicLoadObject("AnnouncerMale2K4.HolyShit_F", class'Sound'));
		if ( snd != None ) PlayerOwner().ClientPlaySound(snd);
		bPlayedSound = true;
	}
}

function bool RenderDeviceClick( byte Btn )
{
	switch ( Btn )
	{
	case QBTN_Yes:
		SaveSettings();
		Console(Controller.Master.Console).DelayedConsoleCommand("relaunch");
		break;

	case QBTN_Cancel:
		sRenDev = sRenDevD;
		co_RenderDevice.Find(sRenDev);
		co_RenderDevice.SetComponentValue(sRenDev,true);
		break;
	}

	return true;
}

// Video change OK page was closed
function VideoChangeClose(optional bool bCancelled)
{
	local bool bTemp;
	local string NewX, NewY;

	Divide(sRes,"x",NewX,NewY);
	if (bCancelled)
	{
		// Prevent the components from sending OnChange events
		bTemp = bIgnoreChange;
		bIgnoreChange = True;

		// Reload the components values
		co_Resolution.LoadINI();
		co_ColorDepth.LoadINI();
		ch_Fullscreen.LoadINI();
		bIgnoreChange = bTemp;
	}

	else
	{
		if ( int(NewX) < 640 || int(NewY) < 480 )
			Controller.GameResolution = NewX $ "x" $ NewY;
		else Controller.GameResolution = "";

		iColDepthD = iColDepth;
		sResD = sRes;

		if ( bFullScreen != bFullScreenD )
		{
			PlayerOwner().ConsoleCommand("set ini:Engine.Engine.ViewportManager StartupFullScreen"@bFullScreen);
			bFullScreenD = bFullScreen;
		}
	}

	CheckSliders();
	bIgnoreResNotice = False;
}

function int GetDetailIndex(string DetailString)
{
	local int i;

	for (i = 0; i < ArrayCount(DetailLevels); i++)
		if (DetailString == DetailLevels[i])
			return i;

	return 0;
}

final function string GetGUIString(string ConfigString)
{
	switch (ConfigString)
	{
		case "UltraLow":	return DetailLevels[0];
		case "VeryLow":		return DetailLevels[1];
		case "Low":			return DetailLevels[2];
		case "Lower":		return DetailLevels[3];
		case "Normal":		return DetailLevels[4];
		case "Higher":		return DetailLevels[5];
		case "High":		return DetailLevels[6];
		case "VeryHigh":	return DetailLevels[7];
		case "UltraHigh":	return DetailLevels[8];
	}
	return "";
}

final function string GetConfigString(string DetailString)
{
	switch (DetailString)
	{
		case DetailLevels[0]:	return "UltraLow";
		case DetailLevels[1]:	return "VeryLow";
		case DetailLevels[2]:	return "Low";
		case DetailLevels[3]:	return "Lower";
		case DetailLevels[4]:	return "Normal";
		case DetailLevels[5]:	return "Higher";
		case DetailLevels[6]:	return "High";
		case DetailLevels[7]:	return "VeryHigh";
		case DetailLevels[8]:	return "UltraHigh";
	}
	return "";
}

final function GetComboOptions(moComboBox Combo, out array<GUIListElem> Ar)
{
	local int i;
	local string tempStr;

	Ar.Remove(0, Ar.Length);
	if (Combo == None)
		return;

	switch (Combo)
	{
		case co_Texture:
		case co_Char:
			for (i = 0; i < ArrayCount(DetailLevels); i++)
			{
				if (bDemo && i == 5)
					break;

				Ar.Length = Ar.Length + 1;
				Ar[i].Item = DetailLevels[i];
			}
			break;

		case co_Physics:
		case co_World:
		case co_Decal:
			Ar.Length = 3;
			Ar[0].Item = DetailLevels[3];
			Ar[1].Item = DetailLevels[4];
			Ar[2].Item = DetailLevels[5];
			break;

		case co_ColorDepth:
			Ar.Length = 2;
			Ar[0].Item = BitDepthText[0];
			Ar[1].Item = BitDepthText[1];
			break;

		case co_Resolution:
			Ar.Length = ArrayCount(DisplayModes);
			for (i = 0; i < Ar.Length; i++)
				Ar[i].Item = DisplayModes[i].Width$"x"$DisplayModes[i].Height;
			break;

		case co_RenderDevice:
			// Win64 has OpenGLDrv and D3D9Drv (NOT D3DDrv!). --ryan.
			if ( ( PlatformIsWindows() ) && ( PlatformIs64Bit() ) )
			{
				Ar.Length = 2;
				Ar[0].Item = RenderModeText[0] @ "9";
				Ar[0].ExtraStrData = "D3D9Drv.D3D9RenderDevice";
				Ar[1].Item = "OpenGL";
				Ar[1].ExtraStrData = "OpenGLDrv.OpenGLRenderDevice";
			}
			else if ( PlatformIsMacOS() )  // Just OpenGLDrv on MacOSX. --ryan.
			{
				Ar.Length = 1;
				Ar[0].Item = "OpenGL";
				Ar[0].ExtraStrData = "OpenGLDrv.OpenGLRenderDevice";
			}
			else if ( PlatformIsUnix() ) // OpenGLDrv and maybe Pixomatic.
			{
				Ar.Length = 1;
				Ar[0].Item = "OpenGL";
				Ar[0].ExtraStrData = "OpenGLDrv.OpenGLRenderDevice";

				if ( !PlatformIs64Bit() )
				{
					Ar.Length = 2;
					Ar[1].Item = RenderModeText[1];
					Ar[1].ExtraStrData = "PixoDrv.PixoRenderDevice";
				}
			}
			else  // pretty much win32.
			{
				Ar.Length = RENDERMODECOUNT;
				for ( i = 0; i < RENDERMODECOUNT; i++ )
				{
					Ar[i].Item = RenderModeText[i];
					Ar[i].ExtraStrData = RenderMode[i];
				}
			}
			break;

		case co_Shadows:
			tempStr = GetNativeClassName("Engine.Engine.RenderDevice");

			// No render-to-texture on anything but Direct3D.
			if ((tempStr == "D3DDrv.D3DRenderDevice") ||
			    (tempStr == "D3D9Drv.D3D9RenderDevice"))
			{
				Ar.Length = ArrayCount(ShadowOptions);
				for ( i = 0; i < ArrayCount(ShadowOptions); i++ )
					Ar[i].Item = ShadowOptions[i];
			}
			else
			{
				Ar.Length = 2;
				Ar[0].Item = ShadowOptions[0];
				Ar[1].Item = ShadowOptions[1];
			}
			break;

		case co_MeshLOD:
			Ar.Length = 4;
			Ar[0].Item = DetailLevels[3];
			Ar[1].Item = DetailLevels[4];
			Ar[2].Item = DetailLevels[5];
			Ar[3].Item = DetailLevels[8];
			break;
	}
}

function UpdateDecalStay()
{
	if ( ch_Decals.IsChecked() )
		EnableComponent(co_Decal);
	else DisableComponent(co_Decal);
}

defaultproperties
{
	OnPreDraw=InternalOnPreDraw
	RelaunchQuestion="The graphics mode has been successfully changed.  However, it will not take effect until the next time the game is started.  Would you like to restart UT2004 right now?"
	Begin Object class=GUISectionBackground Name=sbSection1
		WinWidth=0.491849
		WinHeight=0.440729
		WinLeft=0.000948
		WinTop=0.012761
        Caption="Resolution"
        RenderWeight=0.01
	End Object
	sb_Section1=sbSection1

	Begin Object class=GUISectionBackground Name=sbSection2
		WinWidth=0.502751
		WinHeight=0.975228
		WinLeft=0.495826
		WinTop=0.012761
        Caption="Options"
        RenderWeight=0.01
	End Object
	sb_Section2=sbSection2

	Begin Object class=GUISectionBackground Name=sbSection3
		WinWidth=0.462891
		WinHeight=0.511261
		WinLeft=0.011132
		WinTop=0.476061
        Caption="Gamma Test"
        RenderWeight=0.01
        bFillClient=true
	End Object
	sb_Section3=sbSection3

	Begin Object Class=moComboBox Name=RenderDeviceCombo
	    Caption="Render Device"
	    ComponentJustification=TXTA_Left
	    Hint="Alternate rendering devices may offer better performance on your machine."
		WinWidth=0.401953
		WinLeft=0.547773
		WinTop=0.335021
		CaptionWidth=0.55
	    TabOrder=0
	    bBoundToParent=True
	    bScaleToParent=True
	    OnLoadINI=InternalOnLoadINI
	    OnChange=InternalOnChange
	    IniOption="@Internal"
	End Object
	co_RenderDevice=RenderDeviceCombo

	Begin Object class=moComboBox Name=VideoResolution
		WinWidth=0.390000
		WinLeft=0.030508
		WinTop=0.060417
		Caption="Resolution"
		INIOption="@INTERNAL"
		INIDefault="640x480"
		OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
		Hint="Select the video resolution at which you wish to play."
		bReadOnly=true
		CaptionWidth=0.55
        bHeightFromComponent=false
        TabOrder=1
	End Object
	co_Resolution=VideoResolution

	Begin Object class=moComboBox Name=VideoColorDepth
		WinWidth=0.390000
		WinLeft=0.030234
		WinTop=0.117188
		Caption="Color Depth"
		INIOption="@Internal"
		INIDefault="false"
        OnChange=InternalOnChange
		OnLoadINI=InternalOnLoadINI
		Hint="Select the maximum number of colors to display at one time."
		CaptionWidth=0.55
        bHeightFromComponent=false
        TabOrder=2
	End Object
	co_ColorDepth=VideoColorDepth

	Begin Object class=moCheckBox Name=VideoFullScreen
		WinWidth=0.387500
		WinLeft=0.030976
		WinTop=0.169531
		Caption="Full Screen"
		INIOption="@Internal"
		INIDefault="True"
		OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
		Hint="Check this box to run the game full screen."
		bSquare=true
		ComponentJustification=TXTA_Left
		CaptionWidth=0.94
        TabOrder=3
	End Object
	ch_FullScreen=VideoFullScreen

	Begin Object class=moSlider Name=BrightnessSlider
		WinWidth=0.461445
		WinLeft=0.012188
		WinTop=0.229951
		MinValue=0.0
		MaxValue=1.0
		Caption="Brightness"
		CaptionWidth=0.55
		ComponentWidth=-1
		LabelJustification=TXTA_Left
		OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
		INIOption="@Internal"
		INIDefault="0.8"
		Hint="Use the slider to adjust the Brightness to suit your monitor."
        TabOrder=4
        bHeightFromComponent=False
        SliderCaptionStyleName=""
	End Object
	sl_Brightness=BrightnessSlider

	Begin Object class=moSlider Name=GammaSlider
		WinWidth=0.461133
		WinLeft=0.012501
		WinTop=0.272918
		MinValue=0.5
		MaxValue=2.5
		Caption="Gamma"
		CaptionWidth=0.55
		ComponentWidth=-1
		LabelJustification=TXTA_Left
		bHeightFromComponent=False
		OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
		INIOption="@Internal"
		INIDefault="0.8"
		Hint="Use the slider to adjust the Gamma to suit your monitor."
        TabOrder=5
        SliderCaptionStyleName=""
	End Object
	sl_Gamma=GammaSlider

	Begin Object class=moSlider Name=ContrastSlider
		WinWidth=0.461133
		WinLeft=0.012188
		WinTop=0.313285
		MinValue=0.0
		MaxValue=1.0
		Caption="Contrast"
		CaptionWidth=0.55
		ComponentWidth=-1
		bHeightFromComponent=False
		LabelJustification=TXTA_Left
		OnLoadINI=InternalOnLoadINI
        OnChange=InternalOnChange
		INIOption="@Internal"
		INIDEfault="0.8"
		Hint="Use the slider to adjust the Contrast to suit your monitor."
        TabOrder=6
        SliderCaptionStyleName=""
	End Object
	sl_Contrast=ContrastSlider

	Begin Object class=GUIImage Name=GammaBar
		WinWidth=0.456250
		WinHeight=0.532117
		WinLeft=0.013477
		WinTop=0.450001
		Image=material'2K4Menus.Controls.gamma'
		ImageColor=(R=255,G=255,B=255,A=255)
        OnChange=InternalOnChange
		ImageRenderStyle=MSTY_Normal
		ImageStyle=ISTY_Scaled
	End Object
	i_GammaBar=GammaBar

	Begin Object class=moComboBox Name=DetailTextureDetail
		WinWidth=0.400000
		WinLeft=0.550000
		WinTop=0.063021
		Caption="Texture Detail"
		INIOption="@Internal"
		INIDefault="High"
		OnLoadINI=InternalOnLoadINI
		Hint="Changes how much world detail will be rendered."
		CaptionWidth=0.65
		ComponentJustification=TXTA_Left
        TabOrder=7
        bHeightFromComponent=false
        OnChange=InternalOnChange
	End Object
	co_Texture=DetailTextureDetail

	Begin Object class=moComboBox Name=DetailCharacterDetail
		WinWidth=0.400000
		WinLeft=0.550000
		WinTop=0.116667
		Caption="Character Detail"
		INIOption="@Internal"
		INIDefault="High"
		OnLoadINI=InternalOnLoadINI
		Hint="Changes how much character detail will be rendered."
		CaptionWidth=0.65
		ComponentJustification=TXTA_Left
        TabOrder=8
        bHeightFromComponent=false
        OnChange=InternalOnChange
	End Object
	co_Char=DetailCharacterDetail

	Begin Object class=moComboBox Name=DetailWorldDetail
		WinWidth=0.400000
		WinLeft=0.550000
		WinTop=0.170312
		Caption="World Detail"
		INIOption="@Internal"
		INIDefault="High"
		OnLoadINI=InternalOnLoadINI
		Hint="Changes the level of detail used for optional geometry and effects."
		CaptionWidth=0.65
		ComponentJustification=TXTA_Left
        TabOrder=9
        bHeightFromComponent=false
        OnChange=InternalOnChange
	End Object
	co_World=DetailWorldDetail

	Begin Object class=moComboBox Name=DetailPhysics
		WinWidth=0.400000
		WinLeft=0.550000
		WinTop=0.223958
		Caption="Physics Detail"
		INIOption="@Internal"
		INIDefault="High"
		OnLoadINI=InternalOnLoadINI
		Hint="Changes the physics simulation level of detail."
		CaptionWidth=0.65
		ComponentJustification=TXTA_Left
        TabOrder=10
        OnChange=InternalOnChange
	End Object
	co_Physics=DetailPhysics

	Begin Object class=moComboBox Name=MeshLOD
		WinWidth=0.4
		WinLeft=0.55
		WinTop=0.223958
		Caption="Dynamic Mesh LOD"
		Hint="Adjusts how aggressively character and vehicle details are reduced at a distance.  Higher settings increase the distance at which details are reduced, possibly improving visual detail at a cost in performance"
		IniOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		CaptionWidth=0.65
		ComponentJustification=TXTA_Left
		TabOrder=11
		OnChange=InternalOnChange
	End Object
	co_MeshLOD=MeshLOD

	Begin Object class=moComboBox Name=DetailDecalStay
		WinWidth=0.400000
		WinLeft=0.550000
		WinTop=0.282032
		Caption="Decal Stay"
		INIOption="@Internal"
		INIDefault="Normal"
		OnLoadINI=InternalOnLoadINI
		Hint="Changes how long weapon scarring effects stay around."
		CaptionWidth=0.65
		ComponentJustification=TXTA_Left
        TabOrder=12
        OnChange=InternalOnChange
	End Object
	co_Decal=DetailDecalStay

	Begin Object class=moComboBox Name=DetailCharacterShadows
		WinWidth=0.300000
		WinLeft=0.600000
		WinTop=0.431378
		Caption="Character Shadows"
		INIOption="@Internal"
		OnLoadINI=InternalOnLoadINI
		Hint="Adjust the detail of character shadows.  'Blob' or 'None' recommended for low-performance PC's"
		CaptionWidth=0.65
		ComponentJustification=TXTA_Left
        TabOrder=13
        OnChange=InternalOnChange
	End Object
	co_Shadows=DetailCharacterShadows

	Begin Object class=moCheckBox Name=DetailDecals
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.600000
		WinTop=0.479308
		Caption="Decals"
		INIOption="@Internal"
		INIDefault="True"
		OnLoadINI=InternalOnLoadINI
		Hint="Enables weapon scarring effects."
		CaptionWidth=0.94
		bSquare=true
		ComponentJustification=TXTA_Left
        TabOrder=14
        OnChange=InternalOnChange
	End Object
	ch_Decals=DetailDecals

	Begin Object class=moCheckBox Name=DetailDynamicLighting
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.600000
		WinTop=0.526716
		Caption="Dynamic Lighting"
		INIOption="@Internal"
		INIDefault="True"
		OnLoadINI=InternalOnLoadINI
		Hint="Enables dynamic lights."
		CaptionWidth=0.94
		bSquare=true
		ComponentJustification=TXTA_Left
        TabOrder=15
        OnChange=InternalOnChange
	End Object
	ch_DynLight=DetailDynamicLighting

	Begin Object class=moCheckBox Name=DetailDetailTextures
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.600000
		WinTop=0.575425
		Caption="Detail Textures"
		INIOption="@Internal"
		INIDefault="True"
		OnLoadINI=InternalOnLoadINI
		Hint="Enables detail textures."
		CaptionWidth=0.94
		bSquare=true
		ComponentJustification=TXTA_Left
        TabOrder=16
        OnChange=InternalOnChange
	End Object
	ch_Textures=DetailDetailTextures

	Begin Object class=moCheckBox Name=DetailCoronas
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.600000
		WinTop=0.624136
		Caption="Coronas"
		INIOption="@Internal"
		INIDefault="True"
		OnLoadINI=InternalOnLoadINI
		Hint="Enables coronas."
		CaptionWidth=0.94
		bSquare=true
		ComponentJustification=TXTA_Left
        TabOrder=17
        OnChange=InternalOnChange
	End Object
	ch_Coronas=DetailCoronas

	Begin Object class=moCheckBox Name=DetailTrilinear
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.600000
		WinTop=0.673263
		Caption="Trilinear Filtering"
		INIOption="@Internal"
		INIDefault="False"
		OnLoadINI=InternalOnLoadINI
		Hint="Enables trilinear filtering, recommended for high-performance PCs."
		CaptionWidth=0.94
		bSquare=true
		ComponentJustification=TXTA_Left
        TabOrder=18
        OnChange=InternalOnChange
	End Object
	ch_Trilinear=DetailTrilinear

	Begin Object class=moCheckBox Name=DetailProjectors
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.600000
		WinTop=0.721195
		Caption="Projectors"
		INIOption="@Internal"
		INIDefault="True"
		OnLoadINI=InternalOnLoadINI
		Hint="Enables projectors."
		CaptionWidth=0.94
		bSquare=true
		ComponentJustification=TXTA_Left
        TabOrder=19
        OnChange=InternalOnChange
	End Object
	ch_Projectors=DetailProjectors

	Begin Object class=moCheckBox Name=DetailDecoLayers
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.599727
		WinTop=0.769906
		Caption="Foliage"
		INIOption="@Internal"
		INIDefault="True"
		OnLoadINI=InternalOnLoadINI
		Hint="Enables grass and other decorative foliage."
		CaptionWidth=0.94
		bSquare=true
		ComponentJustification=TXTA_Left
        TabOrder=20
        OnChange=InternalOnChange
	End Object
	ch_DecoLayers=DetailDecoLayers

	Begin Object Class=moCheckBox Name=WeatherEffects
		WinWidth=0.300000
		WinHeight=0.040000
		WinLeft=0.599727
		WinTop=0.864910
		Caption="Weather Effects"
		Hint="Enable weather effects like rain-drops and lightning."
		OnLoadINI=InternalOnLoadIni
		IniOption="@Internal"
		IniDefault="False"
		CaptionWidth=0.94
		bSquare=True
		ComponentJustification=TXTA_Left
		TabOrder=21
		OnChange=InternalOnChange
	End Object
	ch_Weather=WeatherEffects

	Begin Object Class=moSlider Name=DistanceLODSlider
	    MaxValue=1.000000
	    Value=0.500000
	    Caption="Fog Distance"
	    SliderCaptionStyleName=""
	    Hint="Reduce the fog distance to improve performance."
		IniOption="@Internal"
	    bAutoSizeCaption=True
		CaptionWidth=0.65
	    OnLoadIni=InternalOnLoadIni
	    OnChange=InternalOnChange
	    WinTop=0.910000
	    WinLeft=0.560000
	    WinWidth=0.400000
	    TabOrder=22
	End Object
	sl_DistanceLOD=DistanceLODSlider

	WinTop=0.15
	WinLeft=0
	WinWidth=1
	WinHeight=0.74
	bAcceptsInput=false

	DetailLevels(0)="Lowest"
	DetailLevels(1)="Very Low"
	DetailLevels(2)="Lower"
	DetailLevels(3)="Low"
	DetailLevels(4)="Normal"
	DetailLevels(5)="High"
	DetailLevels(6)="Higher"
	DetailLevels(7)="Very High"
	DetailLevels(8)="Highest"
    bExpert=false;

	DisplayModes(0)=(Width=320,Height=240)
	DisplayModes(1)=(Width=512,Height=384)
	DisplayModes(2)=(Width=640,Height=480)
	DisplayModes(3)=(Width=800,Height=500)
	DisplayModes(4)=(Width=800,Height=600)
	DisplayModes(5)=(Width=1024,Height=640)
	DisplayModes(6)=(Width=1024,Height=768)
	DisplayModes(7)=(Width=1152,Height=768)
	DisplayModes(8)=(Width=1152,Height=864)
	DisplayModes(9)=(Width=1280,Height=800)
	DisplayModes(10)=(Width=1280,Height=854)
	DisplayModes(11)=(Width=1280,Height=960)
	DisplayModes(12)=(Width=1280,Height=1024)
	DisplayModes(13)=(Width=1600,Height=1024)
	DisplayModes(14)=(Width=1600,Height=1200)
	DisplayModes(15)=(Width=1680,Height=1050)
	DisplayModes(16)=(Width=1920,Height=1200)

	RenderModeText(0)="Direct 3D"
	RenderModeText(1)="Software"
	RenderMode(0)="D3DDrv.D3DRenderDevice"
	RenderMode(1)="PixoDrv.PixoRenderDevice"

	BitDepthText(0)="16-bit"
	BitDepthText(1)="32-bit"

	ShadowOptions(0)="None"
	ShadowOptions(1)="Blob"
	ShadowOptions(2)="Full"

    PanelCaption="Display"
    DisplayPromptMenu="GUI2K4.UT2K4VideoChangeOK"
    InvalidSelectionText="The selected custom resolution is reported to be incompatible with your machine.  In order to ensure maximum stability, it is recommended that you choose a compatible resolution from the 'Resolution' drop-down menu"
}
