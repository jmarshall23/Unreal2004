#ifndef XBOX_LIVE_H
#define XBOX_LIVE_H

#include "xtl.h"
#include "xonline.h"
#include <vector>

#include "XBApp.h"
#include "XBNet.h"
#include "XBOnlineTask.h"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const DWORD MAX_ACCOUNTS_DISPLAYED = 3;
const DWORD TITLE_ID = 0xFFFF010B;
const DWORD MAX_ERROR_STR = 1024;
const DWORD NUM_SERVICES = 2; // Number of services to authenticate

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef std::vector< XONLINE_SERVICE_INFO > ServiceInfoList;
typedef std::vector< XONLINE_USER > XBUserList;


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class XBoxLiveMgr
{
    // Master state machine
    enum State
    {
        STATE_CREATE_ACCOUNT,   // Create user account
        STATE_USER_EVENTS,      // Run user state machines
        STATE_SIGNING_ON,       // Perform authentication
        STATE_ERROR,            // Error
    };


    // Per-user signon states
    enum UserState
    {
        STATE_USER_PRE_SIGN_ON,      // Wait for controller activation
        STATE_USER_SELECT_ACCOUNT,   // Select user account
        STATE_USER_CONFIRM_SPONSOR,  // Confirm sponsor account selection
        STATE_USER_PIN_ENTRY,        // PIN Entry
        STATE_USER_WAIT_FOR_OTHERS,  // Wait for others to select accounts
        STATE_USER_BOOT_TO_DASH,     // Boot to dash
        STATE_USER_ERROR,            // Error
        STATE_USER_DONE              // Sign in process done (may not have succeeded)
    };

    enum Event
    {
        EV_BUTTON_A,
        EV_BUTTON_B,
        EV_BUTTON_X,
        EV_BUTTON_Y,
        EV_BUTTON_BACK,
        EV_BUTTON_WHITE,
        EV_BUTTON_BLACK,
        EV_UP,
        EV_DOWN,
        EV_LEFT,
        EV_RIGHT,
        EV_LEFT_TRIGGER,
        EV_RIGHT_TRIGGER,
        EV_NULL
    };

    State               m_State;             // current state
    State               m_NextState;         // return to this state
    BOOL                m_bAllowBootToDash;  // Boot to dash allowed on error
    XBUserList          m_UserAccountList;   // list of available accounts
    // Online Info / state for users using this XBox
    // The list is in controller order. A real game title might also include
    // a pointer or index into an entry in a "players" list used to maintain
    // all the participants in a game session.
    class CUser
    {
    public:
        BYTE          Passcode[XONLINE_PASSCODE_LENGTH];    // Entered PIN Code
        WCHAR         strName[ XONLINE_GAMERTAG_SIZE + 30]; // Player name
        UserState     State;                   // current sign on state
        UserState     NextState;               // return to this state
        BOOL          bAllowBootToDash;        // Boot to dash allowed
        BOOL          bSignedOn;               // Is the player signed on
        BOOL          bGuest;                  // Signing on as a guest
        BOOL          bVoice;                  // Voice enabled
        DWORD         dwTopItem;               // tracks the index of the top item
        DWORD         dwCurrItem;              // current selected menu item
        XONLINE_USER  Account;                 // XOnline user account
        WCHAR         strError[MAX_ERROR_STR]; // STATE_USER_ERROR error
    };
    
    CUser               m_Users[ XONLINE_MAX_LOGON_USERS ];


    CXBNetLink          m_NetLink;                   // Network link checking
    DWORD               m_pServices[ NUM_SERVICES ]; // List of desired services
    ServiceInfoList     m_ServiceInfoList;           // List of service info
    CXBOnlineTask       m_hOnlineTask;               // Online task
    BOOL                m_bReadyForSignOn;           // Signon is now possible
    BOOL                m_bSignedOn;                 // Successfully signed
    BOOL                m_bShowHelp;                 // Display Help
    DWORD               m_dwMicrophoneState;         // Mic Devices state
    DWORD               m_dwHeadphoneState;          // Headphone Devices state

public:

    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();

    XBoxLiveMgr();

private:

    Event GetEvent() const;
    Event GetEvent( DWORD ) const;
    Event GetGamepadEvent( const XBGAMEPAD & ) const;

    VOID Reset();
    VOID HandleSignOnError( HRESULT  );
    VOID HandleServiceError( HRESULT , DWORD );
    VOID HandleUserSignOnError( CUser &, HRESULT );

    VOID UpdateStateCreateAccount( Event );
    VOID BeginSignOn();
    VOID BeginSelectAccount( CUser & );
    VOID UpdateStateSigningOn( Event );
    VOID UpdateStateError( Event );

    VOID UpdateUserStatePreSignOn( CUser & , Event );
    VOID UpdateUserStateSelectAccount( CUser & , Event );
    VOID UpdateUserStateWaitForOthers( CUser & , Event );

    VOID BeginUserPINEntry( CUser & );
    VOID UpdateUserStatePINEntry( CUser &, Event );
    VOID UpdateUserStateDone( CUser &, Event );
    VOID UpdateUserStateError( CUser &, Event );
    VOID UpdateUserStateConfirmSponsor( CUser &, Event );

    VOID SetPlayerOnlineState( DWORD, DWORD );
    VOID BootToDash();
    VOID __cdecl SetUserErrorStr( CUser &, const WCHAR* , ... );

    BOOL HostAccountSelected( XUID & );
    VOID CheckDeviceStates();

    void UISetErrorStr(TCHAR* str) {} // temp
    void UISetErrorStr(TCHAR* str, HRESULT result) {} // temp
    void UISetErrorStr(TCHAR* str, HRESULT result, int id) {} // temp

    XBGAMEPAD              m_DefaultGamepad; // temp
    XBGAMEPAD*             m_Gamepad; // temp


};

#endif//XBOX_LIVE_H
