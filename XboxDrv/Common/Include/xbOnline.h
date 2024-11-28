//-----------------------------------------------------------------------------
// File: XbOnline.h
//
// Desc: Shortcut macros and helper functions for the Xbox online samples.
//       Requires linking with XONLINE[D][S].LIB
//
// Hist: 10.11.01 - New for November XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBONLINE_H
#define XBONLINE_H

#pragma warning( disable: 4786 ) // ignore STL identifier truncation
#include <xtl.h>
#include <vector>
#include "xonline.h"
#include "XBResource.h"
#include "XBFont.h"
#include "XBHelp.h"
#include "XBStopWatch.h"




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_ERROR_STR = 1024;




//-----------------------------------------------------------------------------
// Name: XBOnline_GetUserList() / XBOnline_UpdateUserList
// Desc: Extract accounts from hard disk and MUs
//-----------------------------------------------------------------------------
typedef std::vector< XONLINE_USER > XBUserList;
HRESULT XBOnline_GetUserList( XBUserList& UserList );




//-----------------------------------------------------------------------------
// Name: XBNet_OnlineInit() / XBNet_OnlineCleanup()
// Desc: Performs all required initialization/shutdown for online and 
//       network play
//-----------------------------------------------------------------------------
HRESULT XBNet_OnlineInit( BYTE cfgFlags = 0, 
              PXONLINE_STARTUP_PARAMS pxosp = NULL );
HRESULT XBNet_OnlineCleanup();





//-----------------------------------------------------------------------------
// Name: class CXBOnlineUI
// Desc: UI class for standard online UI
//-----------------------------------------------------------------------------
class CXBOnlineUI
{
    CXBPackedResource   m_xprResource;               // app resources
    mutable CXBFont     m_Font;                      // game font
    CXBHelp             m_Help;                      // help screen
    LPDIRECT3DTEXTURE8  m_ptMenuSel;                 // menu selection image
    LPDIRECT3DTEXTURE8  m_ptIconsTexture;            // online icons image
    WCHAR               m_strError[ MAX_ERROR_STR ]; // generic err
    WCHAR*              m_strFrameRate;              // from CXBApp
    WCHAR*              m_strHeader;                 // header string
    CXBStopWatch        m_CaretTimer;                // for PIN entry

public:

    enum ONLINEICON
    {
        ONLINEICON_NONE,                       // No icon
        ONLINEICON_FRIEND_ONLINE,              // Friend is online
        ONLINEICON_FRIEND_RECEIVEDREQUEST,     // Friend request received
        ONLINEICON_FRIEND_SENTREQUEST,         // Friend request sent
        ONLINEICON_GAMEINVITE,                 // Game invite notification
                                               // (to be displayed in game)
        ONLINEICON_FRIEND_RECEIVEDINVITE,      // Game invite from friend
        ONLINEICON_FRIEND_SENTINVITE,          // Game invite to friend
        ONLINEICON_PLAYER_VOICE,               // Player has voice capability
        ONLINEICON_PLAYER_LOCKEDOUT,           // Player has been locked out
        ONLINEICON_PLAYER_MUTED,               // Player has been muted
        ONLINEICON_PLAYER_TVCHAT               // TV Chat -  no voice comm.
    };

    static const D3DCOLOR COLOR_HIGHLIGHT = 0xffffff00; // Yellow
    static const D3DCOLOR COLOR_GREEN     = 0xff00ff00; // Green
    static const D3DCOLOR COLOR_NORMAL    = 0xffffffff; // White

public:

    explicit CXBOnlineUI( WCHAR* strFrameRate, WCHAR* strHeader );

    HRESULT Initialize( DWORD dwNumResources, DWORD dwMenuSelectorOffset, 
                        DWORD dwOnlineIconsOffset );

    // Accessors
    VOID SetErrorStr( const WCHAR*, va_list );

    // UI functions
    VOID RenderCreateAccount( BOOL bHasMachineAccount ) const;
    VOID RenderSelectAccount( DWORD, const XBUserList& ) const;
    VOID RenderLoggingOn() const;
    VOID RenderError( BOOL bBootToDash = FALSE ) const;

    VOID RenderHeader() const;
    VOID RenderMenuSelector( FLOAT, FLOAT ) const;
    VOID RenderOnlineNotificationIcon( FLOAT, FLOAT, ONLINEICON ) const;

    HRESULT DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, 
                      const WCHAR* strText, DWORD dwFlags=0L ) const;
    HRESULT RenderHelp( XBHELP_CALLOUT* tags, DWORD dwNumCallouts );

private:

    // Disabled
    CXBOnlineUI();
    CXBOnlineUI( const CXBOnlineUI& );

};

#endif // XBONLINE_H

