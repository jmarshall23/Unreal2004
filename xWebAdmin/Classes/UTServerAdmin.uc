///////////////////////////////////////////////////////////
//
//	xWebAdmin.UTServerAdmin
//	Web Application to handle remote administration of server
//

class UTServerAdmin extends WebApplication config;

// TODO:
// Do something with colspan.inc
var config string ActiveSkin;

// Each query handler represents a section in webadmin
var() config array<string>	QueryHandlerClasses;
var array<xWebQueryHandler>	QueryHandlers;

// Global Objects
var() array<class<WebSkin> >		WebSkins;	// array of webadmin skins
var() class<UTServerAdminSpectator> SpectatorType;
var() class<WebSkin> 				DefaultWebSkinClass;
var WebSkin					CurrentSkin;// Currently loaded webadmin skin
var UTServerAdminSpectator 	Spectator;	// Used to get console messages
var xAdminUser 				CurAdmin;	// Currently logged admin (not thread safe)
var PlayInfo				GamePI;		// Contains all confiurable variables
var WebResponse				Resp;		// Non-thread safe reference to a WebResponse object

// Lists
var array<CacheManager.MutatorRecord> AllMutators;	// All mutators as determined by .int entries
var StringArray			AGameType;		// All available Game Types
var StringArray			AExcMutators;	// All available Mutators (Excluded)
var StringArray			AIncMutators;	// All Mutators currently in play
var StringArray			Skins;			// All Webadmin custom skins
var const string DemoMaps[5];

// Characters which aren't rendered correctly in HTML
struct HtmlChar
{
	var string Plain;
	var string Coded;
};
var array<HtmlChar> SpecialChars;
var config string DefaultBG;		// Non-highlighted items
var config string HighlightedBG;	// Active links

// Pages
var config string RootFrame;			// This is the master frame divided in 2: Top = Header, bottom = frame page
var config string HeaderPage;			// This is the header menu
var config string MessagePage;			// Name of the file containing the message template
var config string FramedMessagePage;	// Name of file containing message template for sub-frames
var config string RestartPage;			// This is the page that users will be transferred to when restarting the server
var string htm;


var config string AdminRealm;		// Used by browsers to cache login information

// HTML variables used for skins
var string SkinPath;					// Path to use for .htm and .inc content
var string SiteCSSFile;					// CSS file to use
var string SiteBG;						// Background color for this skin
var string StatusOKColor;				// Color of status ok messages
var string StatusErrorColor;			// Color of status error messages

// Table cells
var config string CellLeft;			// Table cell, left justified
var config string CellCenter;		// Table cell, center justified
var config string CellRight;		// Table cell, right justified
var config string CellColSpan;		// Spanned table cell

var config string NowrapLeft;		// Nowrap table cell, left justified
var config string NowrapCenter;		// Nowrap table cell, center justified
var config string NowrapRight;		// Nowrap table cell, right justified

var config string RowLeft;			// Table row, left justified
var config string RowCenter;		// Table row, center justified

// Form objects
var config string CheckboxInclude;
var config string TextboxInclude;
var config string SubmitButtonInclude;
var config string RadioButtonInclude;
var config string SelectInclude;
var config string ResetButtonInclude;
var config string HiddenInclude;
var config string SkinSelectInclude;

// Global localization
var localized string Accept;
var localized string Deny;
var localized string Update;
var localized string Custom;
var localized string Error;
var localized string NoneText;
var localized string SwitchText;
var localized string DeleteText;
var localized string NewText;

var localized string WaitTitle;
var localized string MapChanging;
var localized string MapChangingTo;

var localized string AccessDeniedText;
var localized string ErrorAuthenticating;
var localized string NoPrivs;
// --- evo

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// INITIALIZATION FUNCTIONS
event Init()
{
	Super.Init();

	if (SpectatorType != None)
		Spectator = Level.Spawn(SpectatorType);
	else
		Spectator = Level.Spawn(class'UTServerAdminSpectator');

	if (Spectator != None)
		Spectator.Server = self;

	// won't change as long as the server is up and the map hasnt changed
	LoadGameTypes();
	LoadMutators();
	LoadQueryHandlers();
	Log(class@"Initialized on Port"@WebServer.ListenPort,'WebAdmin');
}

function LoadGameTypes()
{
	local array<CacheManager.GameRecord> GameClasses;
	local int				i;

	Log("Loading Game Types",'WebAdmin');
	// reinitialize list if needed
	AGameType = New(None) class'SortedStringArray';

	class'CacheManager'.static.GetGameTypeList(GameClasses);
	for (i = 0; i < GameClasses.Length; i++)
		AGameType.Add( GameClasses[i].ClassName, GameClasses[i].GameName );
}

function LoadMutators()
{
local int NumMutatorClasses;
local class<Mutator> MClass;
local Mutator M;
local int i, id;

	AExcMutators = New(None) class'StringArray';
	AIncMutators = New(None) class'SortedStringArray';

	if ( Level.IsDemoBuild() )
		return;

	// Load All mutators
	class'CacheManager'.static.GetMutatorList(AllMutators);
	for (i = 0; i<AllMutators.Length; i++)
	{
		MClass = class<Mutator>(DynamicLoadObject(AllMutators[i].ClassName, class'Class'));
		if (MClass != None)
		{
			AExcMutators.Add(string(i), AllMutators[i].ClassName);
			NumMutatorClasses++;
		}
	}

	// Check Current Mutators
	for (M = Level.Game.BaseMutator; M != None; M = M.NextMutator)
	{
		if (M.bUserAdded)
		{
			id = AExcMutators.FindTagId(String(M.Class));
			if (id >= 0)
			{
				i = int(AExcMutators.GetItem(id));
				AIncMutators.Add(string(i), AllMutators[i].FriendlyName);
			}
			else
				log("Unknown Mutator in use: "@String(M.Class),'WebAdmin');
		}
	}
}

function LoadQueryHandlers()
{
local int i, j;
local xWebQueryHandler	QH;
local class<xWebQueryHandler> QHC;

	LoadSkins();
	for (i=0; i<QueryHandlerClasses.Length; i++)
	{
		QHC = class<xWebQueryHandler>(DynamicLoadObject(QueryHandlerClasses[i],class'Class'));

		// skip invalid classes
		if (QHC != None)
		{
			// Make sure we dont have duplicate instance of the same class
			for (j=0;j<QueryHandlers.Length; j++)
			{
				if (QueryHandlers[j].Class == QHC)
				{
					QHC = None;
					break;
				}
			}

			if (QHC != None)
			{
				QH = new(self) QHC;
				if (QH != None)
				{
					if (QH.Init())
					{
						QueryHandlers.Length = QueryHandlers.Length+1;
						QueryHandlers[QueryHandlers.Length - 1] = QH;
					}
					else Log("WebQueryHandler:"@QHC@"could not be initialized",'WebAdmin');
				}
			}
		}

		else
		{
			log("Invalid QueryHandlerClass:"$QueryHandlerClasses[i]$".  Removing invalid entry.",'WebAdmin');
			QueryHandlerClasses.Remove(i,1);
			SaveConfig();
		}
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	SERVER MANAGEMENT & CONTROL

function LoadSkins()
{
	local int i;
	local string 			S;
	local class<WebSkin> 	SkinClass;

	Skins = new(None) class'StringArray';
	S = Level.GetNextInt("XWebAdmin.WebSkin", i++);
	while (S != "")
	{
		SkinClass = class<WebSkin>(DynamicLoadObject(S, class'Class'));
		if (SkinClass != None)
		{
			Skins.Add(Level.GetItemName(string(SkinClass)), SkinClass.default.DisplayName);
			WebSkins[WebSkins.Length] = SkinClass;
		}
		S = Level.GetNextInt("XWebAdmin.WebSkin", i++);
	}
	ApplySkinSettings();
}

function ApplySkinSettings()
{
	local int i;

	if (Skins == None)
		return;

	if (CurrentSkin != None && Level.GetItemName(string(CurrentSkin.Class)) ~= ActiveSkin)
		return;

	i = Skins.FindItemId(ActiveSkin);
	if (i < 0)
		CurrentSkin = new(None) class'XWebAdmin.UT2K3Skin';
	else CurrentSkin = new(None) WebSkins[i];

	if (CurrentSkin != None)
		CurrentSkin.Init(Self);
}

function ServerChangeMap(WebRequest Request, WebResponse Response, string MapName, string GameType)
{
local int i;
local bool bConflict;
local string Conflicts, Str, ShortName, Muts;


	if (Level.NextURL != "")
	{
		ShowMessage(Response, WaitTitle, MapChanging);
	}

	if (Request.GetVariable("Save", "") != "")
	{
		// All we need to do is override settings as required
		for (i = 0; i<GamePI.Settings.Length; i++)
		{
			ShortName = Level.GetItemName(GamePI.Settings[i].SettingName);

			if (Request.GetVariable(GamePI.Settings[i].SettingName, "") != "")
				Level.UpdateURL(ShortName, GamePI.Settings[i].Value, false);
		}
	}
	else
	{
		bConflict = false;
		Conflicts = "";

		// Make sure we have a GamePI with the right GameType selected
		GameType = SetGamePI(GameType);

		// Check each parameter and see if it conflicts with the settings on the command line
		for (i = 0; i<GamePI.Settings.Length; i++)
		{
			// Hack to get around "AdminName bug"
			if (HasURLOption(GamePI.Settings[i].SettingName, Str) && !(GamePI.Settings[i].Value ~= Str) && GamePI.Settings[i].SettingName != "GameReplicationInfo.AdminName")
			{
				// We have a conflicting setting, prepare a table row for it.
				Response.Subst("SettingName", GamePI.Settings[i].SettingName);
				Response.Subst("SettingText", GamePI.Settings[i].DisplayName);
				Response.Subst("DefVal", GamePI.Settings[i].Value);
				Response.Subst("URLVal", Str);
				Response.Subst("MapName", MapName);
				Response.Subst("GameType", GameType);
				Conflicts = Conflicts $ WebInclude(RestartPage$"_row");//skinme
				bConflict = true;
			}
		}

		if (bConflict)
		{
			// Conflicts exist .. show the RestartPage
			Response.Subst("Conflicts", Conflicts);
			Response.Subst("PostAction", RestartPage);
			Response.Subst("Section", "Restart Conflicts");
			Response.Subst("SubmitValue", Accept);

			ShowPage(Response, RestartPage);
			return;
		}
	}

	Muts = UsedMutators();
	if (Muts != "")
		Muts = "?Mutator=" $ Muts;
	Level.ServerTravel(MapName$"?Game="$GameType$Muts, false);
	ShowMessage(Response, WaitTitle, MapChanging);
}

// What does this function do?  Guess I'll leave it here
/*
function ApplyMapList(StringArray ExcludeMaps, StringArray IncludeMaps, String GameType, String MapListType)
{
local MapList List;
local int IncludeCount, i, id;

	List = Level.Game.GetMapList(MapListType);
	if (List != None)
	{
		ExcludeMaps = ReloadExcludeMaps(GameType);
		IncludeMaps.Reset();

		IncludeCount = List.Maps.Length;
		for(i=0; i<IncludeCount; i++)
		{
			if (ExcludeMaps.Count() > 0)
			{
				id = ExcludeMaps.FindTagId(List.Maps[i]);
				if (id >= 0)
				{
					IncludeMaps.Add(ExcludeMaps.GetItem(i), ExcludeMaps.GetTag(i));
					ExcludeMaps.Remove(i);
				}
				else
					Log("*** Unknown map in Map List: "$List.Maps[i],'WebAdmin');
			}
			else
				Log("*** Empty exclude list, i="$i,'WebAdmin');
		}
		List.Destroy();
	}
	else
		Log("Invalid Map List Type : '"$MapListType$"'",'WebAdmin');
}
*/
// Save custom maplist
function UpdateCustomMapList(int GameIndex, int Index, string NewName)
{
	if ( !(Level.Game.MaplistHandler.GetMapListTitle(GameIndex, Index) == NewName) )
		Index = Level.Game.MaplistHandler.RenameList(GameIndex, Index, NewName);

	if ( Level.Game.MaplistHandler.GetActiveList(GameIndex) == Index )
		Level.Game.MaplistHandler.ApplyMaplist(GameIndex, Index);
	else Level.Game.MaplistHandler.SaveMapList(GameIndex, Index);
}

function string SetGamePI(optional string GameType)
{
	local class<Info> TmpClass;
	local array<class<Info> > InfoClasses;
	local int i;

	if (GameType == "")
	{
		InfoClasses[0] = Level.Game.Class;
		GameType = string(Level.Game.Class);
	}

	else
	{
		TmpClass = class<Info>(DynamicLoadObject(GameType, class'Class'));
		if (TmpClass == None)
		{
			Log("Error loading gametype"@GameType,'WebAdmin');
			return "";
		}

		InfoClasses[0] = TmpClass;
	}

	if (GamePI == None)
		GamePI = new(None) class'PlayInfo';

	if (Level.Game.AccessControl != None)
		InfoClasses[1] = Level.Game.AccessControl.Class;

	while (i < AIncMutators.Count())
	{
		TmpClass = class<Info>(DynamicLoadObject(AllMutators[int(AIncMutators.GetItem(i++))].ClassName, class'Class'));
		if (TmpClass != None)
			InfoClasses[InfoClasses.Length] = TmpClass;
	}

	GamePI.Init( InfoClasses );
	return GameType;
}

// Called at end of game
function CleanupApp()
{
	local int i;

	if (Spectator != None)
		Spectator = None;

	for (i = 0; i < QueryHandlers.Length; i++)
		QueryHandlers[i].Cleanup();

	// In case a query is hung
	if (Resp != None)
		CleanupQuery();

	Super.CleanupApp();		// Always call Super.CleanupApp();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	ACCESS & INFORMATION FUNCTIONS

// Returns whether this map is a valid map
function bool ValidMap(string MapName)
{
	local int i;

	if (Level.IsDemoBuild())
	{
		for (i = 0; i < ArrayCount(DemoMaps); i++)
			if (MapName ~= DemoMaps[i])
				return false;
	}

	if (Left(MapName, 4) ~= "TUT-")
		return false;

	return true;
}

function FormatMapName(out string FullName, out string ShortName)
{
	local string ext;
	ext = ".ut2";

	if (FullName == "" && ShortName == "") return;

	if (FullName != "" && ShortName == "")
	{
		if (Right(FullName, 4) ~= ext)
			ShortName = Left(FullName, Len(FullName) - 4);

		else
		{
			ShortName = FullName;
			FullName = FullName $ ext;
		}
	}

	else if (FullName == "" && ShortName != "")
	{
		if (Right(ShortName,4) ~= ext)
		{
			FullName = ShortName;
			ShortName = Left(ShortName, Len(ShortName) - 4);
		}

		else
			FullName = ShortName $ ext;
	}

	else
	{
		if (!(Right(FullName,4) ~= ext))
			FullName = FullName $ ext;

		ShortName = Left(FullName, Len(FullName) - 4);
	}
}

function String UsedMutators()
{
local int i;
local String OutStr;

	while (i < AIncMutators.Count())
	{
		if (OutStr != "") OutStr $= ",";
		OutStr $= AllMutators[int(AIncMutators.GetItem(i++))].ClassName;
	}

	return OutStr;
}

function bool HasURLOption(string ParamName, out string Value)
{
local string Param;
local int i;

	Param = ParamName;
	while (true)
	{
		i = Instr(Param, ".");
		if (i < 0)
			break;

		Param = Mid(Param, i+1);
	}

	Value = Level.GetUrlOption(Param);
	return Value != "";
}

function string NextPriv(out string PrivString)
{
local int pos;
local string Priv;

	pos = Instr(PrivString, "|");
	if (pos < 0)
		pos = Len(PrivString);
	EatStr(Priv, PrivString, Pos);
	if (PrivString != "")	// Remove leading pipe char
		PrivString = Mid(PrivString, 1);

	return Priv;
}

function bool CanPerform(string privs)
{
local string priv;

	priv = NextPriv(privs);
	while (priv != "")
	{
		if (Level.Game.AccessControl.AllowPriv(priv) && CurAdmin.HasPrivilege(priv))
			return true;

		priv = NextPriv(privs);
	}
	return false;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// LIST GENERATION

// Returns a list of active maps for this gametype
function StringArray ReloadIncludeMaps(StringArray ExMaps, int GameIndex, int MapListIndex)
{
	local int i;
	local array<string> Maps;
	local StringArray Arr;

	Arr = new(None) class'StringArray';
	Maps = Level.Game.MaplistHandler.GetMapList(GameIndex, MapListIndex);
	for (i=0; i<Maps.Length; i++)
		Arr.MoveFrom(ExMaps, Maps[i]);

	return Arr;
}

// Returns a list of all maps for this gametype
function StringArray ReloadExcludeMaps(String GameType)
{
local int i;
local class<GameInfo>	GameClass;
local string MapPrefix, MapName;
local StringArray	AMaps;
local array<string> Maps;

	GameClass = class<GameInfo>(DynamicLoadObject(GameType, class'Class'));
	AMaps = New(None) class'SortedStringArray';

	if (GameClass == None)
	{
		Warn("Could not load gametype"@GameType@"for maplist.");
		return AMaps;
	}

	MapPrefix = GameClass.default.MapPrefix;
	if (MapPrefix != "")
	{
		GameClass.static.LoadMapList(MapPrefix, Maps);
		for (i = 0; i<Maps.Length; i++)
		{
			MapName = Maps[i];
			if (ValidMap(MapName))
				AMaps.Add(MapName,MapName);
		}
	}

	else return HackMapList();
	return AMaps;
}

// HACK until GameInfo.LoadAllMaps() is changed to support gametypes that use multiple maptypes
function StringArray HackMapList()
{
	local class<GameInfo> TempClass;
	local string NextGame;
	local array<string> HackArray, Used;
	local StringArray AMaps;
	local int i, j, k;

	AMaps = new(None) class'SortedStringArray';
	NextGame = Level.GetNextInt("Engine.GameInfo", i++);
	while (NextGame != "")
	{
		HackArray.Length = 0;
		TempClass = class<GameInfo>(DynamicLoadObject(NextGame, class'Class'));
		if (TempClass != None && TempClass.default.MapPrefix != "")
		{
			for (k = 0; k < Used.Length; k++)
				if (Used[k] ~= TempClass.default.MapPrefix)
					break;

			if (k == Used.Length)
			{
				Used[k] = TempClass.default.MapPrefix;
				TempClass.static.LoadMapList(TempClass.default.MapPrefix, HackArray);
				for (j = 0; j < HackArray.Length; j++)
					AMaps.Add(HackArray[j],HackArray[j]);
			}
		}

		NextGame = Level.GetNextInt("Engine.GameInfo", i++);
	}
	return AMaps;
}

// Only 'Mutators' has value when function is called
function CreateFullMutatorList(out StringArray Mutators, out StringArray GroupsOnly)
{
	local StringArray Grouped;
	local int i,j,z;
	local string GrpName;
	local string thisgroup, nextgroup;

// This class provides a specialized sorting function
// since mutators may be grouped - mutators in the same groups
// are not allowed to be selected together

	Grouped = new(None) class'SortedStringArray';

// Create array sorted on GroupName...this allows to flag
// the mutator for a different .inc file (radio instead of checkbox)
	for (i = 0; i < Mutators.Count(); i++)
	{
		j = int(Mutators.GetItem(i));

// If the mutator author forgot to configure a group name for the mutator,
// generate a groupname "Z" + number, so that every mutator without group name
// has its own group, and isn't grouped together
		if (AllMutators[j].GroupName == "")
			GrpName = "Z" $ string (z++);

		else GrpName = AllMutators[j].GroupName;

		Grouped.Add(string(j),GrpName $ "." $ AllMutators[j].FriendlyName $ AllMutators[j].ClassName);
	}

// Move all grouped mutators to GroupsOnly StringArray for sorting by friendly name
	for (i = 0; i < Grouped.Count(); i++)
	{
		thisgroup = AllMutators[int(Grouped.GetItem(i))].GroupName;
		nextgroup = "";

		if (thisgroup == "") continue;
		if (i+1 < Grouped.Count())
			nextgroup = AllMutators[int(Grouped.GetItem(i+1))].GroupName;

		if (thisgroup ~= nextgroup)
		{
			j=i;
			while(nextgroup ~= thisgroup && j < Grouped.Count())
			{
				GroupsOnly.MoveFromId(Grouped, Grouped.FindItemId(Grouped.GetItem(j)));
				thisgroup = nextgroup;
				if (j+1 == Grouped.Count())
					nextgroup="";
				else nextgroup = AllMutators[int(Grouped.GetItem(j+1))].GroupName;
			}

			if (j < Grouped.Count())
				GroupsOnly.MoveFromId(Grouped,Grouped.FindItemId(Grouped.GetItem(j)));

			i=-1;
		}
	}

// Move all non-grouped mutators back to Mutators StringArray
// for re-sorting by Friendly Name
	Mutators.Reset();
	for (i=0;i<Grouped.Count();i++)
	{
		j=int(Grouped.GetItem(i));
		Mutators.Add(string(j),AllMutators[j].FriendlyName);
	}
	Grouped.Reset();
}

// Generates the list of possible gametypes for dropdown-control
function string GenerateGameTypeOptions(String CurrentGameType)
{
local int i;
local string SelectedStr, OptionStr;

	for (i=0; i < AGameType.Count(); i++)
	{
		if (CurrentGameType ~= AGameType.GetItem(i))
			SelectedStr = " selected";
		else
			SelectedStr = "";

		OptionStr = OptionStr$"<option value=\""$AGameType.GetItem(i)$"\""$SelectedStr$">"$AGameType.GetTag(i)$"</option>";
	}
	return OptionStr;
}

// Generates the list of mutators for dropdown-control
function string GenerateMutatorOptions(String SelectedMutators)
{
	local int i, j;
	local string SelectedStr, OptionStr;
	local array<string> MutatorList;
	local StringArray SortedMutatorList;

	Split( SelectedMutators, ",", MutatorList);

	SortedMutatorList = new(None) class'SortedStringArray';

	for(i=0; i < AllMutators.Length; i++)
		SortedMutatorList.Add(AllMutators[i].ClassName,AllMutators[i].FriendlyName);

	for(i=0; i < SortedMutatorList.Count(); i++)
	{
		SelectedStr = "";
		for( j=0; j < MutatorList.Length; j++)
		{
			if( MutatorList[j] ~= SortedMutatorList.GetItem(i) )
			{
				SelectedStr = " selected";
				break;
			}
		}
		OptionStr $= "<option value=\"" $ SortedMutatorList.GetItem(i) $ "\"" $ SelectedStr $ ">" $
		            SortedMutatorList.GetTag(i) $ "</option>";
	}
	SortedMutatorList.Reset();
	return OptionStr;
}

// Generates list of possible map lists for drop-down control
function string GenerateMapListOptions(string GameType, int Active)
{
	local int i, idx;
	local array<string> Ar;
	local string Result, selected;

	idx = Level.Game.MaplistHandler.GetGameIndex(GameType);
	Ar = Level.Game.MaplistHandler.GetMapListNames(idx);
	for (i = 0; i < Ar.Length; i++)
	{
		selected =Eval(i == Active, " selected", "");
		Result $= "<option value=\"" $ string(i) $ "\"" $ selected $ ">" $ Ar[i] $ "</option>";
	}

	return Result;
}

function string GenerateMapListSelect(StringArray MapList, StringArray MovedMaps)
{
local int i;
local String ResponseStr, SelectedStr;

	if (MapList.Count() == 0)
		return "<option value=\"\">***"@NoneText@"***</option>";

	for (i = 0; i<MapList.Count(); i++)
	{
		if (ValidMap(MapList.GetTag(i)))
		{
			SelectedStr = "";
			if (MovedMaps != None && MovedMaps.FindTagId(MapList.GetTag(i)) >= 0)
				SelectedStr = " selected";
			ResponseStr = ResponseStr$"<option value=\""$MapList.GetTag(i)$"\""$SelectedStr$">"$MapList.GetTag(i)$"</option>";
		}
	}

	return ResponseStr;
}

function string GenerateSkinSelect()
{
	local string S, selectedstring;
	local int i;

	if (Skins.Count() == 0)
		return "<option value=\"\">***"@NoneText@"***</option>";

	for (i = 0; i < Skins.Count(); i++)
	{
		SelectedString = Eval(CurrentSkin != None && Level.GetItemName(string(CurrentSkin.Class)) ~= Skins.GetItem(i), " selected", "");
		S = S $ "<option value=\""$Skins.GetItem(i)$"\""$SelectedString$">"$Skins.GetTag(i)$"</option>";
	}

	return S;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HTML SHORTCUTS
// Replaces any occurences of special characters with HTML friendly representations
static function string HtmlEncode(string src)
{
	local int i;

	for (i = 0; i < default.SpecialChars.Length; i++)
		src = Repl(src, default.SpecialChars[i].Plain, default.SpecialChars[i].Coded);

	return src;
}

// Replaces any occurences of HTML coded characters with their text representations
static function string HtmlDecode(string src)
{
	local int i;

	for (i = 0; i < default.SpecialChars.Length; i++)
		src = Repl(src, default.SpecialChars[i].Coded, default.SpecialChars[i].Plain);

	return src;
}

function string PadLeft(String src, int width, optional string with)
{
	local String OutStr;

	if (with == "")
		with = " ";

	for (OutStr = src; Len(OutStr) < Width; OutStr = with$OutStr);

	return Right(OutStr, Width); // in case PadStr is more than one character
}

function string PadRight(string src, int w, optional string with)
{
local string outstr;

	if (with == "")
		with = " ";

	for (outstr = src; len(outstr) < w; outstr = outstr$with);

	return Left(outstr, w);
}

function MapTitle(WebResponse Response)
{
local string str, smap;

	str = Level.Game.GameReplicationInfo.GameName$" in ";
	if (Level.Title ~= "untitled")
	{
		smap = Level.GetURLMap();
		if (Right(smap, 4) ~= ".ut2")
			str $= Left(smap, Len(smap) - 4);
		else
			str $= smap;
	}
	else
		str $= Level.Title;

	Response.Subst("SubTitle", str);
}

function bool MapIsChanging()
{
	if (Level.NextURL != "")
	{
		ShowMessage(Resp, WaitTitle, MapChanging);
		return true;
	}
	return false;
}

function string HyperLink(string url, string text, bool bEnabled, optional string target)
{
local string hlink;

	if (bEnabled)
	{
		hlink = "<a href='"$url$"'";
		if (target != "")
			hlink = hlink@"target='"$target$"'";
		hlink = hlink$">"$text$"</a>";
		return hlink;
	}
	return text;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QUERY RESPONSE FUNCTIONS

function bool PreQuery(WebRequest Request, WebResponse Response)
{
	local bool bResult;
	local int i;

	if (Level == None || Level.Game == None || Level.Game.AccessControl == None)
	{
		ShowMessage(Response, Error, ErrorAuthenticating);
		return false;
	}

	// Prevent admin credentials from being stored across threads
	// Also ensure that hanging connections are closed before beginning another Query()
	if (Resp != None)
		CleanupQuery();

	if (Spectator == None)
	{
		if (SpectatorType != None)
			 Spectator = Level.Spawn(SpectatorType);
		else Spectator = Level.Spawn(class'UTServerAdminSpectator');

		if (Spectator != None)
			Spectator.Server = self;
	}

	if (Spectator == None)
	{
		ShowMessage(Response, Error, ErrorAuthenticating);
		return false;
	}

	// Check authentication:
	if (!Level.Game.AccessControl.AdminLogin(Spectator, Request.Username, Request.Password))
	{
		Response.FailAuthentication(AdminRealm);
		return false;
	}

	CurAdmin = Level.Game.AccessControl.GetLoggedAdmin(Spectator);
	if (CurAdmin == None)
	{
		ShowMessage(Response, Error, ErrorAuthenticating);
		Level.Game.AccessControl.AdminLogout(Spectator);
		return false;
	}

	Resp = Response;
	bResult = True;
	for (i=0; i<QueryHandlers.Length; i++)
	{
		if (!QueryHandlers[i].PreQuery(Request, Response))
			bResult = False;
	}

	return bResult;
}

event Query(WebRequest Request, WebResponse Response)
{
	local int i;

	Response.Subst("BugAddress", "utbugs"$Level.EngineVersion$"@epicgames.com");
	Response.Subst("CSS", SiteCSSFile);
	Response.Subst("BODYBG", SiteBG);
	// Match query function.  checks URI and calls appropriate input/output function

	if (CurrentSkin != None && CurrentSkin.SpecialQuery.Length > 0)
	{
		for (i = 0; i < CurrentSkin.SpecialQuery.Length; i++)
		{
			if (CurrentSkin.SpecialQuery[i] ~= Mid(Request.URI,1))
			{
				if (CurrentSkin.HandleSpecialQuery(Request, Response))
					return;
				break;
			}
		}
	}

	switch (Mid(Request.URI, 1))
	{
	case "":
	case RootFrame:		QueryRootFrame(Request, Response); return;
	case HeaderPage:	QueryHeaderPage(Request, Response); return;
	case RestartPage:	if (!MapIsChanging()) QuerySubmitRestartPage(Request, Response); return;
	case SiteCSSFile:	Response.SendCachedFile( Path $ SkinPath $ "/" $ Mid(Request.URI, 1), "text/css"); return;
	}

	for (i=0; i<QueryHandlers.Length; i++)
		if (QueryHandlers[i].Query(Request, Response))
			return;

	ShowMessage(Response, Error, "Page not found!");
	return;
}

event PostQuery(WebRequest Request, WebResponse Response)
{
	local int i;

	for (i=0; i<QueryHandlers.Length; i++)
		if (!QueryHandlers[i].PostQuery(Request, Response))
			Response.Connection.bDelayCleanup = True;

	if (Response.Connection.IsHanging())
		return;

	CleanupQuery();
}

function CleanupQuery()
{
	if (Resp != None && Resp.Connection.IsHanging())
		Resp.Connection.Timer();
	Resp = None;
	CurAdmin = None;
	Level.Game.AccessControl.AdminLogout(Spectator);
}

function QueryRootFrame(WebRequest Request, WebResponse Response)
{
local String GroupPage;

	if (QueryHandlers.Length > 0)
		GroupPage = QueryHandlers[0].DefaultPage;

	if (Request.GetVariable("ChangeSkin") != "")
	{
		ActiveSkin = Request.GetVariable("WebSkin", ActiveSkin);
		ApplySkinSettings();
		SaveConfig();
	}
	GroupPage = Request.GetVariable("Group", GroupPage);

	Response.Subst("HeaderURI", HeaderPage$"?Group="$GroupPage);
	Response.Subst("BottomURI", GroupPage);
	Response.Subst("ServerName", class'GameReplicationInfo'.default.ServerName);

	ShowFrame(Response, RootFrame);
}

function QueryHeaderPage(WebRequest Request, WebResponse Response)
{
local int i;
local string menu, GroupPage, Dis, CurPageTitle;

	Response.Subst("AdminName", CurAdmin.UserName);
	Response.Subst("HeaderColSpan", "2");

	if (QueryHandlers.Length > 0)
	{
		GroupPage = Request.GetVariable("Group", QueryHandlers[0].DefaultPage);
		// We build a multi-column table for each QueryHandler
		menu = "";
		CurPageTitle = "";
		for (i=0; i<QueryHandlers.Length; i++)
		{
			if (QueryHandlers[i].DefaultPage == GroupPage)
				CurPageTitle = QueryHandlers[i].Title;

			Dis = "";
			if (QueryHandlers[i].NeededPrivs != "" && !CanPerform(QueryHandlers[i].NeededPrivs))
				Dis = "d";

			Response.Subst("MenuLink", RootFrame$"?Group="$QueryHandlers[i].DefaultPage);
			Response.Subst("MenuTitle", QueryHandlers[i].Title);
			menu = menu$WebInclude(HeaderPage$"_item"$Dis);//skinme
		}
		Response.Subst("Location", CurPageTitle);
		Response.Subst("HeaderMenu", menu);
	}

	if ( CanPerform("Xs") )
	{
		Response.Subst("HeaderColSpan", "3");
		Response.Subst("SkinSelect", Select("WebSkin", GenerateSkinSelect()));
		Response.Subst("WebSkinSelect", WebInclude(SkinSelectInclude));
	}
	// Set URIs
	ShowPage(Response, HeaderPage);
}

function QueryRestartPage(WebRequest Request, WebResponse Response)
{
	if ( CanPerform("Mr|Mt|Mm|Ms|Mu|Ml") )
		ServerChangeMap(Request, Response, Level.GetURLMap(), String(Level.Game.Class));
}

function QuerySubmitRestartPage(WebRequest Request, WebResponse Response)
{
	if ( CanPerform("Mr|Mt|Mm|Ms|Mu|Ml") )
		ServerChangeMap(Request, Response, Request.GetVariable("MapName"), Request.GetVariable("GameType"));
}

function AccessDenied(WebResponse Response)
{
	ShowMessage(Response, AccessDeniedText, NoPrivs);
}


///////////////////////////////////////////////////////////////////
// HTML Generation
//
//

function string WebInclude(string file)
{
	local string S;
	if (CurrentSkin != None)
	{
		S = CurrentSkin.HandleWebInclude(Resp, file);
		if (S != "") return S;
	}
	return Resp.LoadParsedUHTM(Path $ "/" $ SkinPath $ "/" $ file $ ".inc");
}

function bool ShowFrame(WebResponse Response, string Page)
{
	if (CurrentSkin != None && CurrentSkin.HandleHTM(Response, Page))
		return true;

	Response.IncludeUHTM( Path $ SkinPath $ "/" $ Page $ htm);
	return true;
}

function bool ShowPage(WebResponse Response, string Page)
{
	if (CurrentSkin != None && CurrentSkin.HandleHTM(Response, Page))
	{
		Response.ClearSubst();
		return true;
	}
	Response.IncludeUHTM( Path $ SkinPath $ "/" $ Page $ htm);
	Response.ClearSubst();
	return true;
}

function StatusError(WebResponse Response, string Message)
{
	if (Left(Message,1) == "@")
		Message = Mid(Message,1);

	Response.Subst("Status", "<font color='"$StatusErrorColor$"'>"$Message$"</font><br>");
}

function StatusOk(WebResponse Response, string Message)
{
	Response.Subst("Status", "<font color='"$StatusOKColor$"'>"$Message$"</font>");
}

function bool StatusReport(WebResponse Response, string ErrorMessage, string SuccessMessage)
{
	if (ErrorMessage == "")
		StatusOk(Response, SuccessMessage);
	else
		StatusError(Response, ErrorMessage);

	return ErrorMessage=="";
}

function ShowMessage(WebResponse Response, string Title, string Message)
{
	if (CurrentSkin != None && CurrentSkin.HandleMessagePage(Response, Title, Message))
		return;
	Response.Subst("Section", Title);
	Response.Subst("Message", Message);
	Response.IncludeUHTM(Path $ SkinPath $ "/" $ MessagePage $ htm);
}

// Framed message page (eliminates double headers)
function ShowFramedMessage(WebResponse Response, string Message, bool bIsErrorMsg)
{
	if (CurrentSkin != None && CurrentSkin.HandleFrameMessage(Response, Message, bIsErrorMsg))
		return;

	if (bIsErrorMsg)
		StatusError(Response,Message);
	else Response.Subst("Message", Message);
	Response.IncludeUHTM(Path $ SkinPath $ "/" $ FramedMessagePage $ htm);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 		Form Controls

// Creates a checkbox, optionally greying it out (disabled)
function string Checkbox(string tag, bool bChecked, optional bool bDisabled)
{
	Resp.Subst("CheckName", tag);
	Resp.Subst("Checked", Eval(bChecked, " checked", ""));
	Resp.Subst("Disabled", Eval(bDisabled, " disabled", ""));

	return WebInclude(CheckboxInclude);
}

function string Hidden(string Tag, string Value)
{
	Resp.Subst("HiddenName", Tag);
	Resp.Subst("HiddenValue", Value);

	return WebInclude(HiddenInclude);
}

// Creates a submit button
function string SubmitButton(string SubmitButtonName, string SubmitButtonValue)
{
	Resp.Subst("SubmitName", SubmitButtonName);
	Resp.Subst("SubmitValue", SubmitButtonValue);

	return WebInclude(SubmitButtonInclude);
}

function string ResetButton(string ResetButtonName, string ResetButtonValue)
{
	Resp.Subst("ResetName", ResetButtonName);
	Resp.Subst("ResetValue", ResetButtonValue);

	return WebInclude(ResetButtonInclude);
}

// Creates a textbox, optionally providing a default value
function string TextBox(string TextName, coerce string Size, coerce string MaxLength, optional string DefaultValue)
{
	Resp.Subst("TextName", TextName);
	if (int(Size) > 0) Resp.Subst("TextSize", "size=\""$Size$"\"");
		else Resp.Subst("TextSize", "");
	if (int(MaxLength) > 0) Resp.Subst("TextLength", "maxlength=\""$MaxLength$"\"");
		else Resp.Subst("TextLength", "");
	Resp.Subst("TextValue", DefaultValue);

	return WebInclude(TextboxInclude);
}

function string RadioButton(string Group, string Value, bool bSelected)
{
	Resp.Subst("RadioGroup", Group);
	Resp.Subst("RadioValue", Value);
	Resp.Subst("Selected", Eval(bSelected, " checked", ""));

	return WebInclude(RadioButtonInclude);
}

function string Select(string SelectName, string SelectOptions)
{
	Resp.Subst("SelectName", SelectName);
	Resp.Subst("ListOptions", SelectOptions);

	return WebInclude(SelectInclude);
}

defaultproperties
{
     SpectatorType=class'XWebAdmin.UTServerAdminSpectator'
     DefaultWebSkinClass=class'XWebAdmin.UT2K3Skin'
     QueryHandlerClasses(0)="XWebAdmin.xWebQueryCurrent"
     QueryHandlerClasses(1)="XWebAdmin.xWebQueryDefaults"
     QueryHandlerClasses(2)="XWebAdmin.xWebQueryAdmins"
     RootFrame="rootframe"
     HeaderPage="mainmenu"
     MessagePage="message"
     RestartPage="server_restart"
     DefaultBG="#aaaaaa"
     HighlightedBG="#3a7c8c"
     SiteBG="#243954"
     AdminRealm="UT Remote Admin Server"
     htm=".htm"
     SpecialChars(0)=(Plain="&",Coded="&amp;")
     SpecialChars(1)=(Plain="\"",Coded="&quot;")
     SpecialChars(2)=(Plain=" ",Coded="&nbsp;")
     SpecialChars(3)=(Plain="<",Coded="&lt;")
     SpecialChars(4)=(Plain=">",Coded="&gt;")
     SpecialChars(5)=(Plain="©",Coded="&copy;")
     SpecialChars(6)=(Plain="™",Coded="&#8482;")
     SpecialChars(7)=(Plain="®",Coded="&reg;")
     StatusOKColor="#33cc66"
     StatusErrorColor="Yellow"

	 Accept="Accept"
	 Deny="Deny"
	 Update="Update"
	 Custom="Custom"
	 Error="Error"
	 NoneText="None"
	 SwitchText="Switch"
	 DeleteText="Delete"
	 NewText="New"
	 AccessDeniedText="Access Denied"
	 ErrorAuthenticating="Exception Occurred During Authentication!"
	 NoPrivs="Your privileges are not sufficient to view this page."
	 WaitTitle="Please Wait"
	 MapChanging="The server is now switching maps.  Please allow 10 - 15 seconds while the server changes maps."
	 MapChangingTo="The server is now switching to map '%MapName%'.    Please allow 10-15 seconds while the server changes maps."

	 FramedMessagePage="frame_message"
     SiteCSSFile="ut2003.css"
     CheckboxInclude="checkbox"
     TextboxInclude="textbox"
     RadioButtonInclude="radio_button"
     SelectInclude="select"
     SubmitButtonInclude="submit_button"
     ResetButtonInclude="reset_button"
     HiddenInclude="hidden"
     SkinSelectInclude="mainmenu_items"

     CellLeft="cell_left"
     CellCenter="cell_center"
     CellRight="cell_right"
     CellColSpan="cell_colspan"
     NowrapLeft="cell_left_nowrap"
     NowrapCenter="cell_center_nowrap"
     NowrapRight="cell_right_nowrap"
     RowLeft="row_left"
     RowCenter="row_center"

     DemoMaps(0)="DM-Antalus"
     DemoMaps(1)="DM-Asbestos"
     DemoMaps(2)="CTF-Citadel"
     DemoMaps(3)="CTF-Orbital2"
     DemoMaps(4)="BR-Anubis"
}
