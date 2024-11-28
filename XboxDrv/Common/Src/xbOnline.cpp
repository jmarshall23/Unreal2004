//-----------------------------------------------------------------------------
// File: XbOnline.cpp
//
// Desc: Shortcut macros and helper functions for the Xbox online samples.
//       Requires linking with XONLINE[D][S].LIB.
//
// Hist: 10.11.01 - New for November XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "XBOnline.h"
#include "XBMemUnit.h"
#include "XBApp.h"
#include "XBNet.h"
#include <cassert>
#include <algorithm>





//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD MAX_MEMORY_UNITS  = 2 * XGetPortCount();

// How often (per second) the caret blinks during PIN entry
const FLOAT fCARET_BLINK_RATE = 1.0f;

// During the blink period, the amount the caret is visible. 0.5 equals
// half the time, 0.75 equals 3/4ths of the time, etc.
const FLOAT fCARET_ON_RATIO = 0.75f;




//-----------------------------------------------------------------------------
// Name: XBNet_OnlineInit()
// Desc: Performs all required initialization for XOnline and network APIs
//-----------------------------------------------------------------------------
HRESULT XBNet_OnlineInit( BYTE cfgFlags, PXONLINE_STARTUP_PARAMS pxosp)
{
    XONLINE_STARTUP_PARAMS xosp = { 0 };

    if( pxosp == NULL )
    {
        pxosp = &xosp;
    }

    HRESULT hr = XBNet_Init( cfgFlags );
    if( SUCCEEDED( hr ) )
    {
        hr = XOnlineStartup( pxosp );
    }

    return hr;
}




//-----------------------------------------------------------------------------
// Name: XBNet_OnlineCleanup()
// Desc: Performs all required cleanup for online APIs
//-----------------------------------------------------------------------------
HRESULT XBNet_OnlineCleanup()
{

  HRESULT hr = XOnlineCleanup();

  if( SUCCEEDED( hr ) )
  {
      hr = XBNet_Cleanup();
  }

  return hr;
}




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
// Name: CXBOnlineUI()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBOnlineUI::CXBOnlineUI( WCHAR* strFrameRate, WCHAR* strHeader )
:
    m_strFrameRate( strFrameRate ),
    m_strHeader   ( strHeader ),
    m_CaretTimer  ( TRUE )
{
    m_ptMenuSel = NULL;
    *m_strError = 0;
}




//-----------------------------------------------------------------------------
// Name: SetErrorStr()
// Desc: Set error string
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::SetErrorStr( const WCHAR* strFormat, va_list pArglist )
{
    INT iChars = wvsprintfW( m_strError, strFormat, pArglist );
    assert( iChars < MAX_ERROR_STR );
    (VOID)iChars; // avoid compiler warning
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects
//-----------------------------------------------------------------------------
HRESULT CXBOnlineUI::Initialize( DWORD dwNumResources, DWORD dwMenuSelectorOffset,
                                 DWORD dwOnlineIconsOffset )
{
    // Create a font
    if( FAILED( m_Font.Create( g_pd3dDevice, "Font.xpr" ) ) )
    {
        OUTPUT_DEBUG_STRING( "Failed to load fonts\n" );
        return XBAPPERR_MEDIANOTFOUND;
    }

    // Initialize the help system
    if( FAILED( m_Help.Create( g_pd3dDevice, "Gamepad.xpr" ) ) )
    {
        OUTPUT_DEBUG_STRING( "Failed to load help\n" );
        return XBAPPERR_MEDIANOTFOUND;
    }

    // Load our textures
    if( FAILED( m_xprResource.Create( g_pd3dDevice, "Resource.xpr", dwNumResources ) ) )
    {
        OUTPUT_DEBUG_STRING( "Failed to load textures\n" );
        return XBAPPERR_MEDIANOTFOUND;
    }

    // Set up texture ptrs
    m_ptMenuSel = m_xprResource.GetTexture( dwMenuSelectorOffset );
    m_ptIconsTexture = m_xprResource.GetTexture( dwOnlineIconsOffset );

    // Set projection transform
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 0.1f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set view position
    D3DXMATRIX matView;
    D3DXMatrixTranslation( &matView, 0.0f, 0.0f, 40.0f);
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderCreateAccount()
// Desc: Allow player to launch account creation tool
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderCreateAccount( BOOL bHasMachineAccount ) const
{
    RenderHeader();

    WCHAR* strInfo;
    if( bHasMachineAccount )
    {
        strInfo = L"No online accounts exist on this Xbox.\n\n"
                  L"Run the Xbox Online Setup Tool to create accounts.\n\n"
                  L"Press A to continue.";
    }
    else
    {
        strInfo = L"This Xbox does not have a machine account.\n\n"
                  L"Run the Xbox Online Setup Tool to create accounts.\n\n"
                  L"Press A to continue.";
    }

    m_Font.DrawText( 320, 140, COLOR_NORMAL, strInfo, XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderSelectAccount()
// Desc: Display list of accounts
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderSelectAccount( DWORD dwCurrItem, 
                                       const XBUserList& UserList ) const
{
    assert( !UserList.empty() );

    RenderHeader();

    m_Font.DrawText( 320, 140, COLOR_NORMAL, L"Select an account",
                     XBFONT_CENTER_X );

    FLOAT fYtop = 220.0f;
    FLOAT fYdelta = 30.0f;

    // Show list of user accounts
    for( DWORD i = 0; i < UserList.size(); ++i )
    {
        DWORD dwColor = ( dwCurrItem == i ) ? COLOR_HIGHLIGHT : COLOR_NORMAL;

        // Convert user name to WCHAR string
        WCHAR strUserName[ XONLINE_GAMERTAG_SIZE ];
        XBUtil_GetWide( UserList[i].szGamertag, strUserName, XONLINE_GAMERTAG_SIZE );

        m_Font.DrawText( 160, fYtop + (fYdelta * i), dwColor, strUserName );
    }

    // Show selected item with little triangle
    RenderMenuSelector( 120.0f, fYtop + (fYdelta * dwCurrItem ) );
}




//-----------------------------------------------------------------------------
// Name: RenderLoggingOn()
// Desc: Display "logging on" animation
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderLoggingOn() const
{
    RenderHeader();
    m_Font.DrawText( 320, 200, COLOR_NORMAL, L"Authenticating Xbox Account", 
                     XBFONT_CENTER_X );
    m_Font.DrawText( 320, 260, COLOR_NORMAL, L"Press B to cancel", 
                     XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderError()
// Desc: Display error (or any other) message
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderError( BOOL bBootToDash ) const
{
    RenderHeader();
    m_Font.DrawText( 320, 200, COLOR_NORMAL, m_strError, XBFONT_CENTER_X );
    m_Font.DrawText( 320, 300, COLOR_NORMAL, L"Press A to continue", 
                     XBFONT_CENTER_X );
    if( bBootToDash )
        m_Font.DrawText( 320, 360, COLOR_NORMAL, 
            L"Press X to boot to the Xbox Dashboard", XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: RenderHeader()
// Desc: Display standard text
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderHeader() const
{
    m_Font.DrawText(  64, 50, COLOR_NORMAL, m_strHeader );
    m_Font.DrawText( 450, 50, COLOR_HIGHLIGHT, m_strFrameRate );
}




//-----------------------------------------------------------------------------
// Name: RenderMenuSelector()
// Desc: Display menu selector
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderMenuSelector( FLOAT fLeft, FLOAT fTop ) const
{
    D3DXVECTOR4 rc( fLeft, fTop, fLeft + 20.0f, fTop + 20.0f );

    // Show selected item
    struct TILEVERTEX
    {
        D3DXVECTOR4 p;
        D3DXVECTOR2 t;
    };
    TILEVERTEX* pVertices;

    LPDIRECT3DVERTEXBUFFER8 pvbTemp;
    g_pd3dDevice->CreateVertexBuffer( 4 * sizeof( TILEVERTEX ), 
                                      D3DUSAGE_WRITEONLY, 
                                      D3DFVF_XYZRHW | D3DFVF_TEX1, 
                                      D3DPOOL_MANAGED, &pvbTemp );

    // Create a quad for us to render our texture on
    pvbTemp->Lock( 0, 0, (BYTE **)&pVertices, 0L );
    pVertices[0].p = D3DXVECTOR4( rc.x - 0.5f, rc.w - 0.5f, 1.0f, 1.0f );  pVertices[0].t = D3DXVECTOR2( 0.0f, 1.0f ); // Bottom Left
    pVertices[1].p = D3DXVECTOR4( rc.x - 0.5f, rc.y - 0.5f, 1.0f, 1.0f );  pVertices[1].t = D3DXVECTOR2( 0.0f, 0.0f ); // Top    Left
    pVertices[2].p = D3DXVECTOR4( rc.z - 0.5f, rc.w - 0.5f, 1.0f, 1.0f );  pVertices[2].t = D3DXVECTOR2( 1.0f, 1.0f ); // Bottom Right
    pVertices[3].p = D3DXVECTOR4( rc.z - 0.5f, rc.y - 0.5f, 1.0f, 1.0f );  pVertices[3].t = D3DXVECTOR2( 1.0f, 0.0f ); // Top    Right
    pvbTemp->Unlock();

    // Set up our state
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetStreamSource( 0, pvbTemp, sizeof( TILEVERTEX ) );

    // Render the quad with our texture
    g_pd3dDevice->SetTexture( 0, m_ptMenuSel );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    g_pd3dDevice->SetTexture( 0, NULL );
    pvbTemp->Release();
}




//-----------------------------------------------------------------------------
// Name: RenderOnlineNotificationIcon()
// Desc: Render an online notification icon
//-----------------------------------------------------------------------------
VOID CXBOnlineUI::RenderOnlineNotificationIcon( FLOAT fLeft, 
                                                FLOAT fTop, ONLINEICON Icon) const
{

    if( Icon == ONLINEICON_NONE )
    {
        return;
    }

    // Width and height of each icon in the texture
    const FLOAT fIconHeight = 44.0f;
    const FLOAT fIconWidth = 44.0f;

    // Since the icon list is vertical, the u position is always zero,
    // and the v position is the index of the icon * icon height
    FLOAT tvIcon = FLOAT(Icon-1) * fIconHeight;

    // Render width and height of the icon
    FLOAT fWidth = 22.0f;
    FLOAT fHeight = 22.0f;

    struct
    {
        float x, y, z, w;
        float u, v;
    }
    rgQuad[4] =
    {
        {fLeft - 0.5f,           fTop - 0.5f,           1.0f, 1.0f, 0.0f,  tvIcon },
        {fLeft + fWidth - 0.5f,  fTop - 0.5f,           1.0f, 1.0f, fIconWidth, tvIcon },
        {fLeft + fWidth - 0.5f,  fTop + fHeight - 0.5f, 1.0f, 1.0f, fIconWidth, tvIcon + fIconHeight },
        {fLeft - 0.5f,           fTop + fHeight - 0.5f, 1.0f, 1.0f, 0.0f,  tvIcon + fIconHeight }
    };


    // Render the quad
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );
    g_pd3dDevice->SetTexture( 0, m_ptIconsTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_NONE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    g_pd3dDevice->DrawVerticesUP( D3DPT_QUADLIST, 4, rgQuad, sizeof( rgQuad[0] ) );
    g_pd3dDevice->SetTexture( 0, NULL );

 
}




//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Display text using UI font
//-----------------------------------------------------------------------------
HRESULT CXBOnlineUI::DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, 
                               const WCHAR* strText, DWORD dwFlags ) const
{
    return m_Font.DrawText( sx, sy, dwColor, strText, dwFlags );
}




//-----------------------------------------------------------------------------
// Name: RenderHelp()
// Desc: Renders the help screen
//-----------------------------------------------------------------------------
HRESULT CXBOnlineUI::RenderHelp( XBHELP_CALLOUT* tags, 
                                 DWORD dwNumCallouts )
{
    return m_Help.Render( &m_Font, tags, dwNumCallouts );
}


