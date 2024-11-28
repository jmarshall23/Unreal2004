//-----------------------------------------------------------------------------
// File: XBLevelMeters.h
//
// Desc: Helper class for displaying audio level meters on screen
//
// Hist: 3.06.02 - New for December XDK release
//
// Copyright (c) 2000-2002 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef XBLEVELMETERS_H
#define XBLEVELMETERS_H

#include "XBApp.h"
#include <dsound.h>

class CXBLevelMeters
{
public:
    CXBLevelMeters();
    ~CXBLevelMeters();

    HRESULT Initialize( LPDIRECT3DDEVICE8 pD3DDevice,
                        LPDIRECTSOUND8    pDSound );

    HRESULT UpdateMeters( FLOAT fLeft, FLOAT fTop, FLOAT fWidth, FLOAT fHeight );
    
private:
    LPDIRECT3DDEVICE8   m_pd3dDevice;
    LPDIRECTSOUND8      m_pDSound;

    // Helper function for drawing an individual bar on screen
    HRESULT DrawBar( FLOAT fX, 
                     FLOAT fY, 
                     FLOAT fWidth, 
                     FLOAT fHeight,
                     FLOAT fPercent );
};


#endif // XBLEVELMETERS_H
