//-----------------------------------------------------------------------------
// File: XBLevelMeters.h
//
// Desc: Helper class for displaying audio level meters on screen
//
// Hist: 3.06.02 - New for December XDK release
//
// Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "XBLevelMeters.h"




//-----------------------------------------------------------------------------
// Name: CXBLevelMeters (ctor)
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CXBLevelMeters::CXBLevelMeters()
{
    m_pd3dDevice = NULL;
    m_pDSound    = NULL;
}



//-----------------------------------------------------------------------------
// Name: ~CXBLevelMeters
// Desc: Releases any resources held by the object
//-----------------------------------------------------------------------------
CXBLevelMeters::~CXBLevelMeters()
{
    if( m_pd3dDevice )
        m_pd3dDevice->Release();
    if( m_pDSound )
        m_pDSound->Release();
}




//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Initializes the object
//-----------------------------------------------------------------------------
HRESULT CXBLevelMeters::Initialize( LPDIRECT3DDEVICE8 pD3DDevice, 
                                    LPDIRECTSOUND8 pDSound )
{
    m_pd3dDevice = pD3DDevice;
    m_pDSound    = pDSound;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: DrawBar
// Desc: Draws an audio level meter bar
//-----------------------------------------------------------------------------
HRESULT CXBLevelMeters::DrawBar( FLOAT fX, 
                                 FLOAT fY, 
                                 FLOAT fWidth, 
                                 FLOAT fHeight,
                                 FLOAT fPercent )
{
    DWORD dwBarFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
    struct BARVERTEX { XGVECTOR4 p; D3DCOLOR c; };
    BARVERTEX aVertices[4];

    // Set up vertices for the quad
    aVertices[0].p = XGVECTOR4( fX, fY, 0.0f, 1.0f );
    aVertices[0].c = 0x8000FF00;
    aVertices[1].p = XGVECTOR4( fX, fY - fPercent * fHeight, 0.0f, 1.0f );
    aVertices[1].c = 0x8000FF00 | ( DWORD( fPercent * 0xFF ) << 16 );
    aVertices[2].p = XGVECTOR4( fX + fWidth, fY, 0.0f, 1.0f );
    aVertices[2].c = 0x8000FF00;
    aVertices[3].p = XGVECTOR4( fX + fWidth, fY - fPercent * fHeight, 0.0f, 1.0f );
    aVertices[3].c = 0x8000FF00 | ( DWORD( fPercent * 0xFF ) << 16 );

    // Render the quad
    m_pd3dDevice->SetVertexShader( dwBarFVF );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->DrawVerticesUP( D3DPT_TRIANGLESTRIP, 4, aVertices, sizeof( BARVERTEX ) );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateMeters
// Desc: Draws a quick-and-dirty set of level meters in the specified
//          rectangle.  It uses the results of IDirectSound::GetOutputLevels,
//          specifically the peak levels, for the given frame.  Note that 
//          since we only draw once per frame, and the audio chip's encode
//          processor cycles every 5ms, we're really only getting 1 out of
//          every 3 EP frames.
//-----------------------------------------------------------------------------
HRESULT CXBLevelMeters::UpdateMeters( FLOAT fLeft, 
                                      FLOAT fTop,
                                      FLOAT fWidth,
                                      FLOAT fHeight )
{
    DSOUTPUTLEVELS dsLevels;
    m_pDSound->GetOutputLevels( &dsLevels, FALSE );
    DWORD adwLevels[] = {
        dsLevels.dwDigitalFrontLeftPeak,
        dsLevels.dwDigitalFrontRightPeak,
        dsLevels.dwDigitalFrontCenterPeak,
        dsLevels.dwDigitalLowFrequencyPeak,
        dsLevels.dwDigitalBackLeftPeak,
        dsLevels.dwDigitalBackRightPeak,
        0, // Spacer
        0, // Spacer
        dsLevels.dwAnalogLeftTotalPeak,
        dsLevels.dwAnalogRightTotalPeak,
    };
    const DWORD dwNumChannels = sizeof( adwLevels ) / sizeof( adwLevels[0] );

    // Set up parameters for looping
    FLOAT fX = fLeft;
    FLOAT fY = fTop + fHeight;
    FLOAT fBarSpace = fWidth / dwNumChannels;
    FLOAT fBarWidth = fBarSpace * 0.7f;

    for( DWORD i = 0; i < dwNumChannels; i++ )
    {
        FLOAT fDB;

        // Calculate
        FLOAT fLevel = (FLOAT)fabs( adwLevels[i] ) / 0x7FFFFF;
        if( fLevel != 0 )
            fDB = 20.0f * (FLOAT)log10( fLevel );
        else
            fDB = -60;

        // The minimum output from the EP is actually -60dB
        FLOAT fPercent = ( 60.0f + fDB ) / 60.0f;

        DrawBar( fX, fY, fBarWidth, fHeight, fPercent );
        fX += fBarSpace;
    }

    return S_OK;
}
