#include "XboxDrv.h"
#include "XboxLive.h"
#include "xbmemunit.h"

//-----------------------------------------------------------------------------
// Name: XBOnline_GetUserList()
// Desc: Extract all accounts from hard disk and MUs
//-----------------------------------------------------------------------------
HRESULT XBOnline_GetUserList( XBUserList& UserList )
{
    // On input, the list must have room for XONLINE_MAX_STORED_ONLINE_USERS
    // accounts
    XONLINE_USER pUserList[ XONLINE_MAX_STORED_ONLINE_USERS ];

    // Get accounts stored on the hard disk
    DWORD dwUsers;
    HRESULT hr = XOnlineGetUsers( pUserList, &dwUsers );
    if( SUCCEEDED(hr) )
    {
        for( DWORD i = 0; i < dwUsers; ++i )
            UserList.push_back( pUserList[i] );
    }

    return hr;
}

//-----------------------------------------------------------------------------
// Name: XBoxLiveMgr()
// Desc: Constructor
//-----------------------------------------------------------------------------
XBoxLiveMgr::XBoxLiveMgr()
{
    // Add whatever services are appropriate for your title, but no
    // more. Each service requires additional authentication time
    // and network traffic.
    m_pServices[0] = XONLINE_MATCHMAKING_SERVICE;
    m_pServices[1] = XONLINE_BILLING_OFFERING_SERVICE;

    Reset();
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects
//-----------------------------------------------------------------------------
HRESULT XBoxLiveMgr::Initialize()
{    
    // Get information on all accounts for this Xbox
    if( FAILED( XBOnline_GetUserList( m_UserAccountList ) ) )
        return E_FAIL;
    
    // If no accounts, then user needs to create an account.
    // For development purposes, accounts are created using the
    // Online Dashboard or the XDK Launcher
    if( m_UserAccountList.size() == 0 )
        m_State = STATE_CREATE_ACCOUNT;
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Reset()
// Desc: Prepare to restart the application at the front menu
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::Reset()
{
    m_hOnlineTask.Close();
    m_State            = STATE_USER_EVENTS;
    m_NextState        = STATE_USER_EVENTS;
    m_bAllowBootToDash = FALSE;
    m_bReadyForSignOn  = FALSE;
    m_bSignedOn        = FALSE;
    m_bShowHelp        = FALSE;
    m_dwMicrophoneState = 0;
    m_dwHeadphoneState  = 0;
    ZeroMemory( &m_Users, sizeof( m_Users ) );

    for( DWORD i = 0; i < XONLINE_MAX_LOGON_USERS; ++i )
    {
        m_Users[i].State = STATE_USER_PRE_SIGN_ON;
        m_Users[i].NextState = STATE_USER_PRE_SIGN_ON;
    }
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT XBoxLiveMgr::FrameMove()
{
    if( !m_NetLink.IsActive() )
    {
        UISetErrorStr( L"This Xbox has lost its online connection" );
        Reset();
        m_State = STATE_ERROR;
    }
    
    if( m_bSignedOn )
    {
        HRESULT hr = m_hOnlineTask.Continue();
        
        if( FAILED( hr ) )
        {
            Reset();
            if( hr == XONLINE_E_LOGON_KICKED_BY_DUPLICATE_LOGON )
                UISetErrorStr( L"You have been signed out because a\n"
                L"duplicate account signed in on another Xbox" );
            else
                UISetErrorStr( L"Connection was lost. Must relogin" );
            m_State = STATE_ERROR;
        }
        /*
        else
            // Check for microphone/headphone peripheral state changes
            CheckDeviceStates();
            */
    }


    // Process master state machine events
    Event ev = GetEvent(); 

    // Check for the help button
    if( ev == EV_BUTTON_WHITE && m_State != STATE_SIGNING_ON )
        m_bShowHelp = !m_bShowHelp;
    
    if( m_bShowHelp )
        return S_OK;

    switch( m_State )
    {
    case STATE_CREATE_ACCOUNT:  UpdateStateCreateAccount( ev ); break;
    case STATE_USER_EVENTS:
        // Process user events for each user as long as the master state
        // machine remains in STATE_USER_EVENTS
        for(DWORD i = 0; i < XONLINE_MAX_LOGON_USERS && 
            m_State == STATE_USER_EVENTS; ++i)
        {
            // Get an event for a specific controller
            ev = GetEvent( i ); 

            switch( m_Users[i].State )
            {
            case STATE_USER_PRE_SIGN_ON:
                UpdateUserStatePreSignOn( m_Users[i], ev );
                break;
            case STATE_USER_SELECT_ACCOUNT:
                UpdateUserStateSelectAccount( m_Users[i], ev );
                break;
            case STATE_USER_CONFIRM_SPONSOR:
                UpdateUserStateConfirmSponsor( m_Users[i], ev );
                break;
            case STATE_USER_PIN_ENTRY:
                UpdateUserStatePINEntry( m_Users[i], ev );
                break;
            case STATE_USER_WAIT_FOR_OTHERS:
                UpdateUserStateWaitForOthers( m_Users[i], ev );
                break;
            case STATE_USER_ERROR:
                UpdateUserStateError( m_Users[i], ev );
                break;
            case STATE_USER_BOOT_TO_DASH:
                BootToDash();
                break;
            case STATE_USER_DONE:
                UpdateUserStateDone( m_Users[i], ev );
                break;
            default:
                assert( FALSE );
            }            
        }
        break;
    case STATE_SIGNING_ON:      UpdateStateSigningOn( ev );     break;
    case STATE_ERROR:           UpdateStateError( ev );         break;
    default:
        assert( FALSE );
    }
    

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT XBoxLiveMgr::Render()
{
    /*
    DWORD i;
    
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
        0x000A0A6A, 1.0f, 0L );
    
    if( m_bShowHelp )
    {
        m_UI.RenderHelp();
    }
    else
    {
        switch( m_State )
        {
        case STATE_CREATE_ACCOUNT:
            m_UI.RenderCreateAccount( TRUE );
            break;
        case STATE_USER_EVENTS:
            for( i = 0; i < XONLINE_MAX_LOGON_USERS; ++i )
            {
                switch( m_Users[i].State )
                {
                case STATE_USER_PRE_SIGN_ON:
                    m_UI.RenderUserPreSignOn( i);
                    break;
                case STATE_USER_SELECT_ACCOUNT:
                    m_UI.RenderUserSelectAccount( i, 
                        m_Users[i].dwCurrItem,
                        m_Users[i].dwTopItem,
                        m_UserAccountList );
                    break;
                case STATE_USER_CONFIRM_SPONSOR:
                    m_UI.RenderConfirmSponsor( i,
                        m_UserAccountList[m_Users[i].dwCurrItem] );
                    break;
                    
                case STATE_USER_PIN_ENTRY:
                    m_UI.RenderUserPINEntry( i, 
                        m_Users[i].dwCurrItem );
                    break;
                case STATE_USER_WAIT_FOR_OTHERS:
                    m_UI.RenderUserWaitForOthers( i, m_Users[i].strName, 
                        m_bReadyForSignOn );
                    break;
                case STATE_USER_ERROR:
                    m_UI.RenderUserError( i, m_Users[i].strError,
                        m_Users[i].bAllowBootToDash );
                    break;
                case STATE_USER_DONE:
                    m_UI.RenderUserDone( i,  m_Users[i].strName,
                        m_Users[i].bSignedOn, m_Users[i].bVoice );
                    break;
                case STATE_USER_BOOT_TO_DASH:
                    break;
                default:
                    assert( FALSE );
                    
                }
            }
            break;
        case STATE_SIGNING_ON:
            m_UI.RenderSigningOn();
            break;
        case STATE_ERROR:
            m_UI.RenderError( m_bAllowBootToDash );
            break;
        default:
            assert( FALSE );
            break;
        }
        
        m_UI.RenderHeader( m_State != STATE_SIGNING_ON );
    }
    
    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    */
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetGamepadEvent()
// Desc: Return an event from a specific XBGAMEPAD
//-----------------------------------------------------------------------------
XBoxLiveMgr::Event XBoxLiveMgr::GetGamepadEvent(
                                     const XBGAMEPAD & Gamepad) const
{
    // "A" or "Start"
    if( Gamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] ||
        Gamepad.wPressedButtons & XINPUT_GAMEPAD_START )
    {
        return EV_BUTTON_A;
    }
    
    // "B"
    if( Gamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
        return EV_BUTTON_B;
    
    // "X"
    if( Gamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        return EV_BUTTON_X;
    
    // "Y"
    if( Gamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
        return EV_BUTTON_Y;

    // "Back"
    if( Gamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        return EV_BUTTON_BACK;

    // "White"
    if( Gamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] )
        return EV_BUTTON_WHITE;


    // "Black"
    if( Gamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] )
        return EV_BUTTON_BLACK;
    
    
    // "Left Trigger"
    if( Gamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] )
        return EV_LEFT_TRIGGER;

    // "Right Trigger"
    if( Gamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] )
        return EV_RIGHT_TRIGGER;

    // Movement
    if( Gamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
        return EV_UP;
    if( Gamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
        return EV_DOWN;
    if( Gamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
        return EV_LEFT;
    if( Gamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
        return EV_RIGHT;
    
    return EV_NULL;
}




//-----------------------------------------------------------------------------
// Name: GetEvent()
// Desc: Return the state of ANY of the controllers
//-----------------------------------------------------------------------------
XBoxLiveMgr::Event XBoxLiveMgr::GetEvent() const
{
    // Pass the default gamepad, which combines events of all available
    // controllers
    return GetGamepadEvent( m_DefaultGamepad );
}




//-----------------------------------------------------------------------------
// Name: GetEvent()
// Desc: Return the state of a specific controller
//-----------------------------------------------------------------------------
XBoxLiveMgr::Event XBoxLiveMgr::GetEvent( DWORD dwUserIndex ) const
{
    if( m_Gamepad[dwUserIndex].hDevice )
        return GetGamepadEvent( m_Gamepad[dwUserIndex] );
    else    
        return EV_NULL;
}




//-----------------------------------------------------------------------------
// Name: BootToDash()
// Desc: Boot to the dash
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::BootToDash()
{
    
    // Return to Dashboard. Retail Dashboard will include
    // online account creation. Development XDK Launcher
    // includes the XDK Launcher or Xbox OnlineDash for creating accounts.
    LD_LAUNCH_DASHBOARD ld;
    ZeroMemory( &ld, sizeof(ld) );
    ld.dwReason = XLD_LAUNCH_DASHBOARD_MAIN_MENU;
    XLaunchNewImage( NULL, PLAUNCH_DATA( &ld ) );
}




//-----------------------------------------------------------------------------
// Name: BeginUserPINEntry()
// Desc: Start PIN Entry for a User
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::BeginUserPINEntry( CUser & User )
{
    assert( User.Account.dwUserOptions &
        XONLINE_USER_OPTION_REQUIRE_PASSCODE );

    User.dwCurrItem = 0;
    ZeroMemory( User.Passcode, sizeof( User.Passcode ) );
    User.State = STATE_USER_PIN_ENTRY;
}




//-----------------------------------------------------------------------------
// Name: UpdateUserStatePINEntry()
// Desc: PIN Entry
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::UpdateUserStatePINEntry( CUser & User, Event ev )
{

    assert( User.Account.dwUserOptions &
        XONLINE_USER_OPTION_REQUIRE_PASSCODE );

    BYTE Key = 0;

    switch( ev )
    {
    case EV_BUTTON_A:
        // Check PIN
        User.dwCurrItem = 0;
        if( memcmp( User.Account.passcode, 
                    User.Passcode, 
                    XONLINE_PASSCODE_LENGTH ) == 0 )
        {
            User.State = STATE_USER_WAIT_FOR_OTHERS;
        }
        else
        {
            SetUserErrorStr( User, L"Invalid Passcode" );
            User.State = STATE_USER_ERROR;
            User.NextState = STATE_USER_SELECT_ACCOUNT;
        }
        return;
    case EV_BUTTON_BACK:
    case EV_BUTTON_B:
        BeginSelectAccount( User );
        return;
    case EV_BUTTON_X:
        Key = XONLINE_PASSCODE_GAMEPAD_X;
        break;
    case EV_BUTTON_Y:
        Key = XONLINE_PASSCODE_GAMEPAD_Y;
        break;
    case EV_BUTTON_WHITE:
        Key = XONLINE_PASSCODE_GAMEPAD_WHITE;
        break;
    case EV_BUTTON_BLACK:
        Key = XONLINE_PASSCODE_GAMEPAD_BLACK;
        break;
    case EV_UP:
        Key = XONLINE_PASSCODE_DPAD_UP;
        break;
    case EV_DOWN:
        Key = XONLINE_PASSCODE_DPAD_DOWN;
        break;
    case EV_LEFT:
        Key = XONLINE_PASSCODE_DPAD_LEFT;
        break;
    case EV_RIGHT:
        Key = XONLINE_PASSCODE_DPAD_RIGHT;
        break;
    case EV_LEFT_TRIGGER:
        Key = XONLINE_PASSCODE_GAMEPAD_LEFT_TRIGGER;
        break;
    case EV_RIGHT_TRIGGER:
        Key = XONLINE_PASSCODE_GAMEPAD_RIGHT_TRIGGER;
        break;

    default:
        return;
    }

    assert( Key != 0 );

    if( Key != 0 && User.dwCurrItem < XONLINE_PASSCODE_LENGTH )
    {
        User.Passcode[User.dwCurrItem++] = Key;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateCreateAccount()
// Desc: Allow User to launch account creation tool
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::UpdateStateCreateAccount( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        BootToDash();
        break;
    default:
        // If any MUs are inserted, update the User list
        // and resume if there are any accounts
        DWORD dwInsertions;
        DWORD dwRemovals;
        if( CXBMemUnit::GetMemUnitChanges( dwInsertions, dwRemovals ) )
        {
            m_UserAccountList.clear();
            XBOnline_GetUserList( m_UserAccountList );
            if( !m_UserAccountList.empty() )
            {
                m_State = STATE_USER_EVENTS;
            }
        }
        break;
       
    }
}


//-----------------------------------------------------------------------------
// Name: UpdateUserStatePreSignOn()
// Desc: Allow User to select controller for sign on
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::UpdateUserStatePreSignOn( CUser & User, Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        BeginSelectAccount( User );
        break;
    default:
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: HostAccountSelected()
// Desc: Check if one of the players has select a host (non-guest) account
//       for signon
//-----------------------------------------------------------------------------
BOOL XBoxLiveMgr::HostAccountSelected( XUID & xuid )
{
    for( DWORD i = 0; i < XONLINE_MAX_LOGON_USERS; ++i )
       if( m_Users[i].Account.xuid.qwUserID == xuid.qwUserID &&
           !m_Users[i].bGuest )
           return TRUE;


    return FALSE;
}




//-----------------------------------------------------------------------------
// Name: UpdateUserStateSelectAccount()
// Desc: Allow User to choose account
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::UpdateUserStateSelectAccount( CUser & User,
                                                Event ev )
{

    switch( ev )
    {
    case EV_BUTTON_A:
        {
            // Check if this account has already been selected 
            if( HostAccountSelected( 
                    m_UserAccountList[User.dwCurrItem].xuid ) )
            {
                // The account has been selected.  Check if the
                // user would like to sign on as a guest of this
                // account
                User.State = STATE_USER_CONFIRM_SPONSOR;
            }
            else
            {
                // Save current account information
                User.Account = m_UserAccountList[User.dwCurrItem];
                
                // Make WCHAR copy of User name
                XBUtil_GetWide( 
                    m_UserAccountList[User.dwCurrItem].szGamertag, 
                    User.strName, XONLINE_GAMERTAG_SIZE );
                // Check if a passcode is required.  Note that
                // passcodes are strictly for *client* side
                // authentication
                if( m_UserAccountList[User.dwCurrItem].dwUserOptions & 
                    XONLINE_USER_OPTION_REQUIRE_PASSCODE )
                {
                    User.State = STATE_USER_PIN_ENTRY;
                    BeginUserPINEntry( User );
                }
                else
                    User.State = STATE_USER_WAIT_FOR_OTHERS;
            }
        }
        break;
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
            User.State = STATE_USER_PRE_SIGN_ON;
        break;
       

    case EV_UP:
        // If we're at the top of the displayed list, shift the display
        if( User.dwCurrItem == User.dwTopItem )
        {
            if( User.dwTopItem > 0 )
                --User.dwTopItem;
        }
        
        // Move to the previous item
        if( User.dwCurrItem > 0 )
            --User.dwCurrItem;
        break;

        
    case EV_DOWN:
        // If we're at the bottom of the displayed list, shift the display
        if( User.dwCurrItem == 
                User.dwTopItem + MAX_ACCOUNTS_DISPLAYED - 1 )
        {
            if( User.dwTopItem + MAX_ACCOUNTS_DISPLAYED < 
                    m_UserAccountList.size() )
                ++User.dwTopItem;
        }
        
        // Move to next item
        if( User.dwCurrItem < m_UserAccountList.size() - 1 )
            ++User.dwCurrItem;

    default:
        // If any MUs are inserted/removed, need to update the
        // User account list
        DWORD dwInsertions;
        DWORD dwRemovals;
        if( CXBMemUnit::GetMemUnitChanges( dwInsertions, dwRemovals ) )
        {
            m_UserAccountList.clear();
            XBOnline_GetUserList( m_UserAccountList );
            if( m_UserAccountList.empty() )
                m_State = STATE_CREATE_ACCOUNT;
            else
                User.dwCurrItem = 0;
        }
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateUserStateConfirmSponsor()
// Desc: Allow User to confirm guest account selection
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::UpdateUserStateConfirmSponsor( CUser & User,
                                                 Event ev )
{

    // Check to make sure the sponsor account is still
    // selected...
    if( !HostAccountSelected( 
        m_UserAccountList[User.dwCurrItem].xuid ) )
    {
        // Sponsor is gone, back to account selection...
        BeginSelectAccount( User );
        return;
    }

    switch( ev )
    {
    case EV_BUTTON_A:
        {
            // Signon as a guest of the current account 
            // Note: This requires that some other user
            // sign on with the actual (sponsor) account 
            User.Account = m_UserAccountList[User.dwCurrItem];
            User.bGuest  = TRUE;
            XBUtil_GetWide( m_UserAccountList[User.dwCurrItem].szGamertag, 
                User.strName, XONLINE_GAMERTAG_SIZE );
            wcscat( User.strName, L"\n(guest)" );
            User.State = STATE_USER_WAIT_FOR_OTHERS;
        }
        break;
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
            BeginSelectAccount( User );
        break;
    default:
        break;
        
    }
    
}




//-----------------------------------------------------------------------------
// Name: UpdateUserStateWaitForOthers()
// Desc: Wait until for other users to select accounts
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::UpdateUserStateWaitForOthers( CUser & User, 
                                                Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        BeginSelectAccount( User );
        return;
    }

    m_bReadyForSignOn = TRUE;
    
    // First, make sure that every controller is either in the waiting for other
    // users state, or not selected for use
    for( DWORD i = 0; i < XONLINE_MAX_LOGON_USERS; ++i )
    {
        if( m_Users[i].State != STATE_USER_WAIT_FOR_OTHERS &&
            m_Users[i].State != STATE_USER_PRE_SIGN_ON )
        {
            m_bReadyForSignOn = FALSE;
            break;
        }
    }
        

    // Check that there are sponsor accounts for associated guest accounts
    for( DWORD i = 0; i < XONLINE_MAX_LOGON_USERS; ++i )
    {
        if( m_Users[i].State == STATE_USER_WAIT_FOR_OTHERS &&
            m_Users[i].bGuest  && 
            !HostAccountSelected( m_Users[i].Account.xuid ) )
        {
            // Sponsor account no longer available, go back to 
            // account selection
            BeginSelectAccount( m_Users[i] );
            m_bReadyForSignOn = FALSE;

        }

    }

    
    if( m_bReadyForSignOn && ev == EV_BUTTON_A )      
    {        
        // Ready to go...
        m_State = STATE_SIGNING_ON; 
        BeginSignOn();
    }
    
}




//-----------------------------------------------------------------------------
// Name: UpdateStateSigningOn()
// Desc: Authentication is underway
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::UpdateStateSigningOn( Event ev )
{
    
    switch( ev )
    {
    case EV_BUTTON_B:
    case EV_BUTTON_BACK:
        {
            // Close the task (this cancels the sign on process)
            m_hOnlineTask.Close();
            for( DWORD i = 0; i < XONLINE_MAX_LOGON_USERS; ++i )
            {
                // Set all players who tried to sign in, back to the
                // account selection state
                if( m_Users[i].State != STATE_USER_PRE_SIGN_ON )
                    BeginSelectAccount(  m_Users[i] );
            }
            m_State = STATE_USER_EVENTS;
            return;
        }
    }
    
    HRESULT hr = m_hOnlineTask.Continue();
    
    if ( hr != XONLINETASK_S_RUNNING )
    {
        if ( hr != XONLINE_S_LOGON_CONNECTION_ESTABLISHED  )
        {
            HandleSignOnError( hr );
            return;
        }

        m_bSignedOn = TRUE;

        // Next, check if the individual players
        PXONLINE_USER pLoggedOnUsers = XOnlineGetLogonUsers();
        
        assert( pLoggedOnUsers );
        
        // Get the initial states for the headphone and
        // microphone devices
        m_dwMicrophoneState = 
            XGetDevices( XDEVICE_TYPE_VOICE_MICROPHONE );
        m_dwHeadphoneState = 
            XGetDevices( XDEVICE_TYPE_VOICE_HEADPHONE );

        BOOL bAllUsersSignedOn = TRUE;

        for( DWORD i = 0; i < XONLINE_MAX_LOGON_USERS; ++i )
        {
            if( m_Users[i].State != STATE_USER_PRE_SIGN_ON )
            {
                hr = pLoggedOnUsers[i].hr;
                
                if( SUCCEEDED( hr ) )
                {
                    if( !m_Users[i].bGuest )
                    {
                        DWORD dwState = XONLINE_FRIENDSTATE_FLAG_ONLINE;

                        m_Users[i].bVoice = 
                            ( m_dwMicrophoneState & (1 << i) ) &&
                            ( m_dwHeadphoneState  & (1 << i) );

                        if( m_Users[i].bVoice )
                            dwState |= XONLINE_FRIENDSTATE_FLAG_VOICE;

                        SetPlayerOnlineState( i, dwState );
                    }
                    
                    m_Users[i].bSignedOn = TRUE;
                    
                    // Check if there are any messages for the User
                    if( pLoggedOnUsers[i].hr == 
                        XONLINE_S_LOGON_USER_HAS_MESSAGE )
                    {
                        SetUserErrorStr( m_Users[i], 
                            L"One or messages are\n"
                            L"available. You may read\n"
                            L"them by visiting\n" 
                            L"the Xbox Dashboard." );
                        m_Users[i].bAllowBootToDash = TRUE;
                        m_Users[i].State = STATE_USER_ERROR;
                        m_Users[i].NextState = STATE_USER_DONE;
                    }
                    else
                        m_Users[i].State = STATE_USER_DONE;
                }
                else
                {
                    HandleUserSignOnError( m_Users[i], hr );
                    bAllUsersSignedOn = FALSE;
                }
            }
            else // Set controllers that didn't sign in to "done" state
                m_Users[i].State = STATE_USER_DONE;
        }


        // If all the users were signed on, go ahead and check that
        // the requested services are available.  
        if( bAllUsersSignedOn )
        {
            // Check for service errors and store service information
            m_ServiceInfoList.clear();
            for( DWORD  i = 0; i < NUM_SERVICES; ++i )
            {
                // Store service information
                XONLINE_SERVICE_INFO serviceInfo;
                hr = XOnlineGetServiceInfo( m_pServices[i], &serviceInfo );
                if( FAILED( hr ) )
                {
                    HandleServiceError( hr, m_pServices[i] );
                    return;
                }
                m_ServiceInfoList.push_back( serviceInfo );
            }
        }
        
        // Back to processing individual controller events...
        m_State = STATE_USER_EVENTS;
        
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateStateError()
// Desc: An error occurred
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::UpdateStateError( Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        m_State = m_NextState;
        m_bAllowBootToDash = FALSE;
        break;
    case EV_BUTTON_X:
        if( m_bAllowBootToDash )
        {
           BootToDash();
        }
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateUserStateError()
// Desc: An user specific error occurred
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::UpdateUserStateError( CUser & User, Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:

        User.bAllowBootToDash = FALSE;
        // Transition to NextState, if this is account selection
        // then call BeginSelectAccount to prepare it first
        if( User.NextState == STATE_USER_SELECT_ACCOUNT )
            BeginSelectAccount( User );
        else
            User.State = User.NextState;

        break;
    case EV_BUTTON_X:
        if( User.bAllowBootToDash )
        {
           BootToDash();
        }
    }
}




//-----------------------------------------------------------------------------
// Name: BeginSelectAccount()
// Desc: Initiate the account selection process
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::BeginSelectAccount( CUser & User )
{
    User.dwCurrItem = 0;
    User.dwTopItem = 0;
    User.bGuest = FALSE;
    ZeroMemory( &User.Account, sizeof( User.Account ) );
    User.State = STATE_USER_SELECT_ACCOUNT;
}

    
    
    
//-----------------------------------------------------------------------------
// Name: BeginSignOn()
// Desc: Initiate the authentication process
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::BeginSignOn()
{
    assert( m_hOnlineTask == NULL );
    

    // XOnlineLogon() allows a list of up to 4 players (1 per controller)
    // to login in a single call.  The list must be a one-to-one match of
    // controller to user in order for the online system to recognize which 
    // user is using which controller.
    XONLINE_USER pUserList[XONLINE_MAX_LOGON_USERS] = { 0 };

    DWORD dwGuest = 1;
    for( DWORD i = 0; i < XONLINE_MAX_LOGON_USERS; ++i )
    {
        // Any controllers  unused controllers will be in the  
        // STATE_USER_PRE_SIGN_ON state
        if( m_Users[i].State != STATE_USER_PRE_SIGN_ON )
        {
            // If this account is a guest account, assign 
            // a unique guest id to it
            if( m_Users[i].bGuest )
            {
               XOnlineSetUserGuestNumber( 
                   m_Users[i].Account.xuid.dwUserFlags, dwGuest );
               dwGuest++;
            }
            pUserList[i] = m_Users[i].Account;
        }
    }

    // Initiate the login process. XOnlineTaskContinue() is used to poll
    // the status of the login.
    HRESULT hr = XOnlineLogon( pUserList, m_pServices, NUM_SERVICES, 
        NULL, &m_hOnlineTask );
    
    if( FAILED(hr) )
    {
        HandleSignOnError( hr );
    }
}




//-----------------------------------------------------------------------------
// Name: HandleSignOnError()
// Desc: Handle machine authentication errors
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::HandleSignOnError( HRESULT hr )
{
    HRESULT hrTitleUpdate;

    Reset();
    m_State = STATE_ERROR;

    switch( hr )
    {
    case XONLINE_E_LOGON_NO_NETWORK_CONNECTION:
        UISetErrorStr( L"No network connection detected", hr );
        m_bAllowBootToDash = TRUE;
        break;
    case XONLINE_E_LOGON_CANNOT_ACCESS_SERVICE:
        UISetErrorStr( L"Login failed with error 0x%x", hr );
        m_bAllowBootToDash = TRUE;      
        break;
    case XONLINE_E_LOGON_UPDATE_REQUIRED:
        hrTitleUpdate = XOnlineTitleUpdate( TITLE_ID ); 
        // If successful, the system will reboot and update
        assert( FALSE );  // Should have rebooted!
        UISetErrorStr( L"Title update failed with error 0x%x", hrTitleUpdate );
        break;
    case XONLINE_E_LOGON_SERVERS_TOO_BUSY:
        UISetErrorStr( L"The authentication servers are too busy. "
                          L"Try again later." );
        break;
    default:
        m_bAllowBootToDash = TRUE;      
        UISetErrorStr( L"Login failed with error 0x%x", hr );

    }

}




//-----------------------------------------------------------------------------
// Name: HandleUserSignOnError()
// Desc: Handle User logon errors
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::HandleUserSignOnError( CUser & User, HRESULT hr )
{
    User.State = STATE_USER_ERROR;
    User.NextState = STATE_USER_DONE;

    switch( hr ) 
    {
    case XONLINE_E_LOGON_USER_ACCOUNT_REQUIRES_MANAGEMENT:
        SetUserErrorStr( User,  L"This account requires\nPlayer management" );
        User.NextState = STATE_USER_BOOT_TO_DASH;
        break;
    default:
        SetUserErrorStr( User, L"User Login failed\nwith error\n0x%x", hr );

    }

}




//-----------------------------------------------------------------------------
// Name: HandleServiceError()
// Desc: Handle service errors
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::HandleServiceError( HRESULT hr, DWORD dwServiceId )
{
    Reset();
    m_State = STATE_ERROR;

    switch( hr )
    {
    case XONLINE_E_LOGON_SERVICE_NOT_AUTHORIZED:
        UISetErrorStr( L"Access to service %d is denied",
            dwServiceId );
        
        break;
    case XONLINE_E_LOGON_SERVICE_TEMPORARILY_UNAVAILABLE:
        UISetErrorStr( L"Service %d is unavailable",
            dwServiceId );
        
        break;
    default:
        
        UISetErrorStr( L"Error 0x%x logging into service %d",
            hr, dwServiceId );
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateUserStateDone()
// Desc: Display result of signon
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::UpdateUserStateDone( CUser & User, Event ev )
{
    switch( ev )
    {
    case EV_BUTTON_A:
        if( User.bSignedOn )
            Reset();
        break;
    default:
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: SetPlayerOnlineState()
// Desc: Broadcast updated User state for the world
//-----------------------------------------------------------------------------
VOID XBoxLiveMgr::SetPlayerOnlineState( DWORD dwUserIndex, DWORD dwState )
{
    HRESULT hr = XOnlineNotificationSetState( dwUserIndex, dwState,
        XNKID(), 0, NULL );
    assert( SUCCEEDED( hr ) );
    (VOID)hr; // avoid compiler warning
}




//-----------------------------------------------------------------------------
// Name: SetUserErrorStr()
// Desc: Set error string
//-----------------------------------------------------------------------------
void __cdecl XBoxLiveMgr::SetUserErrorStr( CUser & User,
                                           const WCHAR* strFormat, ... )
{
    va_list pArgList;
    va_start( pArgList, strFormat );
    
    INT iChars = wvsprintfW( User.strError, 
        strFormat, pArgList );
    assert( iChars < MAX_ERROR_STR );
    (void)iChars; // avoid compiler warning
    
    va_end( pArgList );
}

