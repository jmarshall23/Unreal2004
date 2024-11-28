// ====================================================================
//  Written by Joe Wilcox
//  (c) 2002, Epic Games, Inc.  All Rights Reserved
// ====================================================================

class UT2K4GUIController extends GUIController;




#exec OBJ LOAD FILE=InterfaceContent.utx
#exec OBJ LOAD FILE=2K4MenuSounds.uax

function ReturnToMainMenu()
{
	CloseAll(true);

	if ( MenuStack.Length == 0 )
		OpenMenu(GetMainMenuClass());
}

function bool SetFocusTo( FloatingWindow Menu )
{
	local int i;

	if ( ActivePage == Menu )
		return true;

	for ( i = 0; i < MenuStack.Length; i++ )
	{
		if ( FloatingWindow(MenuStack[i]) == None )
			continue;

		if ( MenuStack[i] == Menu )
		{
			if ( i + 1 < MenuStack.Length )
			{
				MenuStack[i+1].ParentPage = Menu.ParentPage;
				Menu.ParentPage = MenuStack[MenuStack.Length - 1];
			}

			MenuStack[MenuStack.Length] = Menu;
			MenuStack.Remove(i,1);
			ActivePage = Menu;
			return true;
		}
	}

	return false;
}

// If the disconnect menu is opened while any other menus are on the stack, they will remain there, since the
// disconnect options menu cannot be closed, only replaced
event bool OpenMenu(string NewMenuName, optional string Param1, optional string Param2)
{
	if ( NewMenuName ~= class'GameEngine'.default.DisconnectMenuClass
	&& ( InStr(Param1,"?closed") != -1 || InStr(Param1,"?failed") != -1 || InStr(Param1,"?disconnect") != -1 ) )
	{
		if ( bModAuthor )
			log("Opening disconnect menu with failed, closed, or disconnect in URL",'ModAuthor');

		CloseAll(True,True);
	}

	return Super.OpenMenu(NewMenuName, Param1, Param2);
}

// Should override this function if you have less options in your custom start menu
static simulated event Validate()
{
	if ( default.MainMenuOptions.Length < 7 )
		ResetConfig();
}

static simulated function string GetSinglePlayerPage()
{
	Validate();
	return default.MainMenuOptions[0];
}

static simulated function string GetServerBrowserPage()
{
	Validate();
	return default.MainMenuOptions[1];
}

static simulated function string GetMultiplayerPage()
{
	Validate();
	return default.MainMenuOptions[2];
}

static simulated function string GetInstantActionPage()
{
	Validate();
	return default.MainMenuOptions[3];
}

static simulated function string GetModPage()
{
	Validate();
	return default.MainMenuOptions[4];
}

static simulated function string GetSettingsPage()
{
	Validate();
	return default.MainMenuOptions[5];
}

static simulated function string GetQuitPage()
{
	Validate();
	return default.MainMenuOptions[6];
}

// 20%!! increase in menu load speed for menus that contain large numbers of the same component
// (such as GUIMenuOption)
function class<GUIComponent> AddComponentClass(string ClassName)
{
	local int i;
	local class<GUIComponent> Cls;


	for ( i = 0; i < RegisteredClasses.Length; i++ )
		if ( string(RegisteredClasses[i]) ~= ClassName )
			return RegisteredClasses[i];

	Cls = class<GUIComponent>(DynamicLoadObject(ClassName,class'Class'));
	if ( Cls != None )

		RegisteredClasses[RegisteredClasses.Length] = Cls;

	return Cls;
}

function PurgeComponentClasses()
{
	if ( RegisteredClasses.Length > 0 )
		RegisteredClasses.Remove(0, RegisteredClasses.Length);

	Super.PurgeComponentClasses();
}

defaultproperties
{
    Begin Object Class=fntUT2k4Menu Name=GUIMenuFont
    End Object
    FontStack(0)=fntUT2k4Menu'GUIMenuFont'

    Begin Object Class=fntUT2k4Default Name=GUIDefaultFont
    End Object
    FontStack(1)=fntUT2k4Default'GUIDefaultFont'

    Begin Object Class=fntUT2k4Large Name=GUILargeFont
    End Object
    FontStack(2)=fntUT2k4Large'GUILargeFont'

    Begin Object Class=fntUT2k4Header Name=GUIHeaderFont
    End Object
    FontStack(3)=fntUT2k4Header'GUIHeaderFont'

    Begin Object Class=fntUT2k4Small Name=GUISmallFont
    End Object
    FontStack(4)=fntUT2k4Small'GUISmallFont'

    Begin Object Class=fntUT2k4MidGame Name=GUIMidGameFont
    End Object
    FontStack(5)=fntUT2k4MidGame'GUIMidGameFont'

    Begin Object Class=fntUT2k4SmallHeader Name=GUISmallHeaderFont
    End Object
    FontStack(6)=fntUT2k4SmallHeader'GUISmallHeaderFont'

    Begin Object Class=fntUT2k4ServerList Name=GUIServerListFont
    End Object
    FontStack(7)=fntUT2k4ServerList'GUIServerListFont'

    Begin Object Class=fntUT2k4IRC Name=GUIIRCFont
    End Object
	FontStack(8)=fntUT2K4IRC'GUIIRCFont'

    Begin Object Class=fntUT2K4MainMenu Name=GUIMainMenuFont
    End Object
	FontStack(9)=fntUT2K4MainMenu'GUIMainMenuFont'

	Begin Object Class=fntUT2K4Medium Name=GUIMediumMenuFont
	End Object
	FontStack(10)=GUIMediumMenuFont

	FONT_NUM=11

    DefaultStyleNames(0)="GUI2K4.STY2RoundButton"
    DefaultStyleNames(1)="GUI2K4.STY2RoundScaledButton"
    DefaultStyleNames(2)="GUI2K4.STY2SquareButton"
    DefaultStyleNames(3)="GUI2K4.STY2ListBox"
    DefaultStyleNames(4)="GUI2K4.STY2ScrollZone"
    DefaultStyleNames(5)="GUI2K4.STY2TextButton"
    DefaultStyleNames(6)="GUI2K4.STY2Page"
    DefaultStyleNames(7)="GUI2K4.STY2Header"
    DefaultStyleNames(8)="GUI2K4.STY2Footer"
    DefaultStyleNames(9)="GUI2K4.STY2TabButton"
    DefaultStyleNames(10)="GUI2K4.STY2CharButton"
    DefaultStyleNames(11)="GUI2K4.STY2ArrowLeft"
    DefaultStyleNames(12)="GUI2K4.STY2ArrowRight"
    DefaultStyleNames(13)="GUI2K4.STY2ServerBrowserGrid"
    DefaultStyleNames(14)="GUI2K4.STY2NoBackground"
    DefaultStyleNames(15)="GUI2K4.STY2ServerBrowserGridHeader"
    DefaultStyleNames(16)="GUI2K4.STY2SliderCaption"
    DefaultStyleNames(17)="GUI2K4.STY2LadderButton"
    DefaultStyleNames(18)="GUI2K4.STY2LadderButtonHi"
    DefaultStyleNames(19)="GUI2K4.STY2LadderButtonActive"
    DefaultStyleNames(20)="GUI2K4.STY2BindBox"
    DefaultStyleNames(21)="GUI2K4.STY2SquareBar"
    DefaultStyleNames(22)="GUI2K4.STY2MidGameButton"
    DefaultStyleNames(23)="GUI2K4.STY2TextLabel"
    DefaultStyleNames(24)="GUI2K4.STY2ComboListBox"
    DefaultStyleNames(25)="GUI2K4.STY2SquareMenuButton"
    DefaultStyleNames(26)="GUI2K4.STY2IRCText"
    DefaultStyleNames(27)="GUI2K4.STY2IRCEntry"
    DefaultStyleNames(28)="GUI2K4.STY2BrowserButton"
    DefaultStyleNames(29)="GUI2K4.STY2ContextMenu"
    DefaultStyleNames(30)="GUI2K4.STY2ServerListContextMenu"
    DefaultStyleNames(31)="GUI2K4.STY2ListSelection"
    DefaultStyleNames(32)="GUI2K4.STY2TabBackground"
    DefaultStyleNames(33)="GUI2K4.STY2BrowserListSel"
    DefaultStyleNames(34)="GUI2K4.STY2EditBox"
    DefaultStyleNames(35)="GUI2K4.STY2CheckBox
    DefaultStyleNames(36)="GUI2K4.STY2CheckBoxCheck"
    DefaultStyleNames(37)="GUI2K4.STY2SliderKnob"
    DefaultStyleNames(38)="GUI2K4.STY2BottomTabButton"
    DefaultStyleNames(39)="GUI2K4.STY2ListSectionHeader"
    DefaultStyleNames(40)="GUI2K4.STY2ItemOutline"
    DefaultStyleNames(41)="GUI2K4.STY2ListHighlight"
    DefaultStyleNames(42)="GUI2K4.STY2MouseOverLabel"
    DefaultStyleNames(43)="GUI2K4.STY2SliderBar"
    DefaultStyleNames(44)="GUI2K4.STY2DarkTextLabel"
    DefaultStyleNames(45)="GUI2K4.STY2TextButtonEffect"
    DefaultStyleNames(46)="GUI2K4.STY2ArrowRightDbl"
    DefaultStyleNames(47)="GUI2K4.STY2ArrowLeftDbl"
    DefaultStyleNames(48)="GUI2K4.STY2FooterButton"
    DefaultStyleNames(49)="GUI2K4.STY2SectionHeaderText"
    DefaultStyleNames(50)="GUI2K4.STY2ComboButton"
    DefaultStyleNames(51)="GUI2K4.STY2VertUpButton"
    DefaultStyleNames(52)="GUI2K4.STY2VertDownButton"
    DefaultStyleNames(53)="GUI2K4.STY2VertGrip"
    DefaultStyleNames(54)="GUI2K4.STY2Spinner"
    DefaultStyleNames(55)="GUI2K4.STY2SectionHeaderTop"
    DefaultStyleNames(56)="GUI2K4.STY2SectionHeaderBar"
    DefaultStyleNames(57)="GUI2K4.STY2CloseButton"
    DefaultStyleNames(58)="GUI2K4.STY2CoolScroll"
    DefaultStyleNames(59)="GUI2K4.sTY2AltComboButton"
    STYLE_NUM=60

	ImageList(0)=Material'2K4Menus.Controls.checkboxball_b'
    ImageList(1)=Material'2K4Menus.NewControls.ComboListDropDown'
	ImageList(2)=Material'2K4Menus.Newcontrols.LeftMark'
	ImageList(3)=Material'2K4Menus.Newcontrols.RightMark'
	ImageList(4)=Material'2K4Menus.Controls.plus_b'
	ImageList(5)=Material'2K4Menus.Controls.minus_b'
	ImageList(6)=Material'2K4Menus.NewControls.UpMark'
	ImageList(7)=Material'2K4Menus.NewControls.DownMark'

	// Preload these menus to avoid hitches
	MainMenuOptions(0)="GUI2K4.UT2K4SP_Main"	// This must match the value for GameEngine.SinglePlayerMenuClass
	MainMenuOptions(1)="GUI2K4.UT2K4ServerBrowser"
	MainMenuOptions(2)="GUI2K4.UT2K4GamePageMP"
	MainMenuOptions(3)="GUI2K4.UT2K4GamePageSP"
	MainMenuOptions(4)="GUI2K4.UT2K4ModsAndDemos"
	MainMenuOptions(5)="GUI2k4.UT2K4SettingsPage"
	MainMenuOptions(6)="GUI2K4.UT2K4QuitPage"

    MouseCursors(0)=material'2K4Menus.Cursors.Pointer'          // Arrow
    MouseCursors(1)=material'2K4Menus.Cursors.ResizeAll'       // SizeAll
    MouseCursors(2)=material'2K4Menus.Cursors.ResizeSWNE'       // Size NE SW
    MouseCursors(3)=material'2K4Menus.Cursors.Resize'   // Size NS
    MouseCursors(4)=material'2K4Menus.Cursors.ResizeNWSE'       // Size NW SE
    MouseCursors(5)=material'2K4Menus.Cursors.ResizeHorz'       // Size WE
    MouseCursors(6)=material'2K4Menus.Cursors.Pointer'          // Wait
    CURSOR_NUM=7

    MouseCursorOffset(0)=(X=0,Y=0,Z=0)
    MouseCursorOffset(1)=(X=0.5,Y=0.5,Z=0)
    MouseCursorOffset(2)=(X=0.5,Y=0.5,Z=0)
    MouseCursorOffset(3)=(X=0.5,Y=0.5,Z=0)
    MouseCursorOffset(4)=(X=0.5,Y=0.5,Z=0)
    MouseCursorOffset(5)=(X=0.5,Y=0.5,Z=0)
    MouseCursorOffset(6)=(X=0,Y=0,Z=0)

    MouseOverSound=sound'2K4MenuSounds.msfxMouseOver'
    ClickSound=sound'2K4MenuSounds.msfxMouseClick'
    EditSound=sound'2K4MenuSounds.msfxEdit'
    UpSound=sound'2K4MenuSounds.msfxUp'
    DownSound=sound'2K4MenuSounds.msfxDown'
	DragSound=sound'2K4MenuSounds.msfxDrag'
    FadeSound=sound'2K4MenuSounds.msfxFade'

	QuestionMenuClass="GUI2K4.GUI2K4QuestionPage"
	AutoLoad(0)=(MenuClassName="GUI2K4.UT2K4InGameChat",bPreInitialize=True)
	NetworkMsgMenu="GUI2K4.UT2K4NetworkStatusMsg"
}
