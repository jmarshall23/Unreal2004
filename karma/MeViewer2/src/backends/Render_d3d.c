/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/15 13:06:26 $ - Revision: $Revision: 1.46.2.7 $

   This software and its accompanying manuals have been developed
   by MathEngine PLC ("MathEngine") and the copyright and all other
   intellectual property rights in them belong to MathEngine. All
   rights conferred by law (including rights under international
   copyright conventions) are reserved to MathEngine. This software
   may also incorporate information which is confidential to
   MathEngine.

   Save to the extent permitted by law, or as otherwise expressly
   permitted by MathEngine, this software and the manuals must not
   be copied (in whole or in part), re-arranged, altered or adapted
   in any way without the prior written consent of the Company. In
   addition, the information contained in the software may not be
   disseminated without the prior written consent of MathEngine.

 */

#define SHADOWS 0
#define SHADOWS_WITH_ALPHA 0

#ifdef WITH_D3D

#if defined(_WIN32)
  /* Disable conversion warnings for ReadTsc(). */
# pragma warning (disable:4244)
#endif

#include <stdio.h>

#include <MeViewer.h>
#include <MeVersion.h>
#include "MeViewer_d3d.h"
#include <MeString.h>
#include <MeProfile.h>

void D3D_SetWindowTitle(RRender *rc, const char * title)
{
    if (!(title && *title)) {
        MeWarning(0, "D3D_SetWindowTitle: You need to pass a valid char* in title.");
        return;
    }

/*    snprintf(D3D->m_strWindowTitle, MAX_TITLE_LENGTH - 1
        , "%s - %s (v%s) [Direct3D]", title, ME_PRODUCT_NAME, ME_VERSION_STRING);*/
                snprintf(rc->D3D->m_strWindowTitle, MAX_TITLE_LENGTH - 1
        , "%s (v %s) [Direct3D]", ME_PRODUCT_NAME, ME_VERSION_STRING);

    rc->D3D->m_strWindowTitle[MAX_TITLE_LENGTH - 1] = '\0';

    SetWindowText(rc->D3D->m_hWnd, rc->D3D->m_strWindowTitle);

    strncpy(rc->m_AppName, title, sizeof(rc->m_AppName));
    rc->m_AppName[sizeof(rc->m_AppName) -1] = '\0';

}

int D3D_CreateRenderer(RRender *rc,void *pWnd)
{
        D3DVars* D3D;
        HINSTANCE hInst = GetModuleHandle(NULL);
        D3D = (D3DVars*)MeMemoryAPI.create(sizeof(D3DVars));
        rc->D3D = D3D;
        rc->D3D->rc = rc;
        
        D3D_InitGlobals(rc);
        
        if( D3D_initD3D(rc->D3D) != S_OK )
        {
                ME_REPORT(MeWarning(1,"...D3D didn't want to play."));
                return 1;
        }
        
        if (!pWnd)
        {
                if(!D3D_CreateWin(rc->D3D,hInst))
                {
                        ME_REPORT(MeWarning(1,"...window creation failed."));
                        return 1;
                }
        }
        else    /* a window was created externally */
        {
                D3D->m_hWnd = pWnd;
        }
        
    if( GetDeviceCaps(GetDC(rc->D3D->m_hWnd), BITSPIXEL) < 16 )
    {
        ME_REPORT(MeWarning(1, "MeViewer (D3D) requires at least 16bit color."));
        return FALSE;
    }

    rc->D3D->m_bReady = TRUE;

    if(rc->m_options.m_bDisplayHelpOnConsole)
        D3D_DisplayConsoleHelp();
        
        if (rc->m_options.m_bCalibrateTimer)
        {
                MeInfo(1,"Calibrating timer...");
                D3D_CalibrateTimer(D3D);
                MeInfo(1,"%d ticks per second. (Ctrl-F9 recalibrates)", D3D->m_uiTicksPerSec );
        }
        MeInfo(1,"D3D is using %s", D3D->m_pDeviceInfo->strDesc);
        
        
        if( D3D_setupD3D(D3D) != S_OK )
        {
                ME_REPORT(MeWarning(1,"...D3D is bad. It doesn't like you."));
                D3D_CleanUpD3D(rc->D3D);
                return 1;
        }
        
    return 0;
}


HRESULT D3D_DrawText( D3DVars* D3D,TCHAR* txt, DWORD x, DWORD y, COLORREF rgb ) /* use RGB(r,g,b) to get COLORREF */
{
    HDC hDC;

    if( SUCCEEDED( D3D->m_pddsBackBuffer->lpVtbl->GetDC(D3D->m_pddsBackBuffer, &hDC) ) )
    {
        SetTextColor( hDC, rgb );
        SetBkMode( hDC, TRANSPARENT );
        ExtTextOut( hDC, x, y, 0, NULL, txt, lstrlen(txt), NULL );
        D3D->m_pddsBackBuffer->lpVtbl->ReleaseDC(D3D->m_pddsBackBuffer, hDC);
    }

    return S_OK;
}


double D3D_highResClock()
{
    static LARGE_INTEGER lpFrequency;
    static BOOL firsttime = TRUE;
    LARGE_INTEGER lpCounter;
    BOOL cc;

    if( firsttime )
    {
        cc=QueryPerformanceFrequency(&lpFrequency);
        if(!cc){
            ME_REPORT(MeWarning(1,"Oh no!!! No Query Frequency ... :("));
            exit(-1);
        }
        firsttime = FALSE;
    }

    cc=QueryPerformanceCounter(&lpCounter);
    if(!cc){
        ME_REPORT(MeWarning(1,"Oh no!!! No Query Performance Counter ... :("));
        exit(-1);
    }

    return (double)(lpCounter.QuadPart) / (double)(lpFrequency.QuadPart);
}


void D3D_CalibrateTimer(D3DVars* D3D)
{

    MeProfileTimerMode tmode;
    tmode.counterMode = 0;
    tmode.granularity = 0;
    tmode.count0Label = 0;
    tmode.count1Label = 0;
    MeProfileStartTiming(tmode, D3D->rc->m_bProfiling);

    D3D->m_uiTicksPerSec = (unsigned int)MeProfileGetClockSpeed();
    D3D->m_uiTicksPerFrame = D3D->m_uiTicksPerSec / 60;
}


void D3D_fpsCalc(D3DVars* D3D,MeI64 starttime)
{
    AcmeReal fFPS = 0.0f;
    static DWORD dwFrames  = 0L;
    int cyclesdiff;
    double tdiff;
    MeProfileTimerResult ctTime;
   
    MeProfileGetTimerValue(&ctTime);

    ++dwFrames;

    cyclesdiff = (int)(ctTime.cpuCycles - starttime);
    tdiff = (double)cyclesdiff/(double)(D3D->m_uiTicksPerSec);
    if( tdiff == 0.0f )
        tdiff = 0.0001f;
    fFPS = 1.0f / tdiff;

    if( dwFrames > D3D->m_dwFpsUpdate )
    {
      RRenderDisplayFps( D3D->rc, fFPS );
      dwFrames = 0L;
    }

}


HRESULT D3D_RenderD3D(RRender* rc)
{
    MeProfileTimerResult timerResult;
        D3DVars* D3D = rc->D3D;
    MeProfileStartFrame();

  /* swap to the front-buffer */
  if(FAILED( D3D_doFlip(D3D) ))
    return E_FAIL;

    if( !rc->m_bPause || D3D->m_bDoStep )
    {
        D3D->m_bDoStep = FALSE;

        /* callback */
        if( D3D->callbackfunc )
            D3D->callbackfunc(rc,D3D->userdata);
    }

    MeProfileStartSection("Rendering", 0);

    /* update matrices */
    RRenderUpdateGraphicMatrices(rc);

    D3D_DrawFrame(D3D);

    /* see if we have to quit next time */
    if( rc->m_bQuitNextFrame )
        D3D->m_bQuit = 1;

    MeProfileEndSection("Rendering");

    MeProfileStartSection("Idle Time",0);

    if( D3D->m_bIsFrameLocked )
    {
        MeProfileGetTimerValue(&timerResult);
        while (timerResult.cpuCycles - D3D_startframeclock
		< D3D->m_uiTicksPerFrame )
            MeProfileGetTimerValue(&timerResult);

    }
    MeProfileEndSection("Idle Time");

    if( D3D->m_bDisplayFps )
        D3D_fpsCalc(D3D,D3D_startframeclock);

    MeProfileStopTimers();
    MeProfileEndFrame();

    {
        AcmeReal coltime,dyntime,rentime,idletime,frametime;
#if DO_BENCHMARK_TXT
        int count;
        char buf[256];
#endif

        coltime = MeProfileGetSectionTime("Collision");
        dyntime = MeProfileGetSectionTime("Dynamics");
        rentime = MeProfileGetSectionTime("Rendering");
        idletime = MeProfileGetSectionTime("Idle Time");

        MeProfileGetTimerValue(&timerResult);
        frametime = (MeI64) (timerResult.cpuCycles-D3D_startframeclock)
            *1000.0f/(MeI64) MeProfileGetClockSpeed();

        if (D3D->rc->m_bProfiling != kMeProfileLogTotals)
            RPerformanceBarUpdate(D3D->rc,
                coltime,dyntime,rentime,frametime-coltime-dyntime-rentime);
        else
        {
            static AcmeReal colprev = 0.0f, dynprev = 0.0f;
            static AcmeReal renprev = 0.0f, idleprev = 0.0f;
            const AcmeReal
                colthis = coltime-colprev,
                dynthis = dyntime-dynprev,
                renthis = rentime-renprev,
                idlethis = idletime-idleprev;

            RPerformanceBarUpdate(D3D->rc,
                colthis,dynthis,renthis,frametime-colthis-dynthis-renthis);

            colprev = coltime; dynprev = dyntime;
            renprev = rentime; idleprev = idletime;
        }

#if DO_BENCHMARK_TXT
        count = sprintf(buf,"%f\n",MeProfileGetSectionTime("Dynamics"));
        MeWrite(file,buf,count);
#endif
    }

    return S_OK;
}


HRESULT D3D_SceneInit(D3DVars* D3D)
{
    ZeroMemory( &D3D->m_d3dmtrl, sizeof(D3DMATERIAL7) );
    D3D->m_d3dmtrl.power = 40.0f;

    D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_LIGHTING, TRUE );
    D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_DITHERENABLE, TRUE );
    D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_CULLMODE, D3DCULL_NONE );
    D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ZENABLE, D3DZB_TRUE );
    D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD );
    D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE );



  /* Create ViewPort */
  D3D->m_vpViewPort.dvMinZ = 0.0f;
  D3D->m_vpViewPort.dvMaxZ = 1.0f;
  if( !D3D->m_bLockAspectRatio )
  {
    D3D->m_vpViewPort.dwX = 0;
    D3D->m_vpViewPort.dwY = 0;
    D3D->m_vpViewPort.dwWidth = D3D->m_dwRenderWidth;
    D3D->m_vpViewPort.dwHeight = D3D->m_dwRenderHeight;

        D3D->rc->m_AspectRatio = (float)D3D->m_dwRenderWidth/(float)D3D->m_dwRenderHeight;
  }
  else
  {
    if( D3D->m_dwRenderWidth > (D3D->m_dwRenderHeight * D3D->rc->m_AspectRatio) )
    {
      D3D->m_vpViewPort.dwHeight = D3D->m_dwRenderHeight;
      D3D->m_vpViewPort.dwWidth = D3D->m_dwRenderHeight * D3D->rc->m_AspectRatio;
      D3D->m_vpViewPort.dwX = 0.5*(D3D->m_dwRenderWidth - D3D->m_dwRenderHeight*D3D->rc->m_AspectRatio);
      D3D->m_vpViewPort.dwY = 0;
    }
    else
    {
      D3D->m_vpViewPort.dwWidth = D3D->m_dwRenderWidth;
      D3D->m_vpViewPort.dwHeight = D3D->m_dwRenderWidth / D3D->rc->m_AspectRatio;
      D3D->m_vpViewPort.dwX = 0;
      D3D->m_vpViewPort.dwY = 0.5*(D3D->m_dwRenderHeight - D3D->m_dwRenderWidth/D3D->rc->m_AspectRatio);
    }
  }
  D3D->m_pd3dDevice->lpVtbl->SetViewport( D3D->m_pd3dDevice, &D3D->m_vpViewPort );

  return S_OK;
}

HRESULT D3D_SceneEnd(D3DVars* D3D)
{
    return S_OK;
}

void D3D_DrawFrame(D3DVars* D3D) /* render to back buffer */
{
    HRESULT rval;
    int do2D;
    D3DMATRIX ident = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    D3DMATRIX cam = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,1};
    
    DWORD backColor = D3DRGBA(D3D->rc->m_backgroundColour[0],
        D3D->rc->m_backgroundColour[1],
        D3D->rc->m_backgroundColour[2], 0);

    if(D3D->m_bLockAspectRatio)
    {
        /*we have to clear the screen outside the viewport,
          so let's clear everything.
          this should probably use a gentler approach... */
        DDBLTFX blt;

        ZeroMemory(&blt, sizeof(blt));

        blt.dwSize = sizeof(blt);
        blt.dwFillColor = 0x00000000;
        D3D->m_pddsBackBuffer->lpVtbl->Blt(D3D->m_pddsBackBuffer, NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blt);
    }

    D3D->m_pd3dDevice->lpVtbl->Clear( D3D->m_pd3dDevice, 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, backColor, 1.0f, 0L );

    if( D3D->m_bUseAntiAliasing )
        D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_SORTINDEPENDENT);
    else
        D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_NONE);


  if(FAILED( D3D->m_pd3dDevice->lpVtbl->BeginScene(D3D->m_pd3dDevice) ))
  {
    ME_REPORT(MeWarning(1,"Failed to begin scene."));
    return;
  }

    /* Set Projection Matrix */
    rval = D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_PROJECTION, (D3DMATRIX *)D3D->rc->m_ProjMatrix );
    if( rval != D3D_OK )
        ME_REPORT(MeWarning(1,"Failed to set projection matrix."));


    /* D3D is now ready to render the scene */
    { /* {} for local vars */
        RGraphic *current, *floor=0, *sky=0;
        D3DCOLOR ambcol;
        BOOL bLightOn;
        HRESULT hres;

        /* lighting */

        /* Ambient */
        if( D3D->rc->m_bUseAmbientLight )
            ambcol = D3DRGBA(D3D->rc->m_rgbAmbientLightColor[0], D3D->rc->m_rgbAmbientLightColor[1], D3D->rc->m_rgbAmbientLightColor[2], 1.0f );
        else  /* set color to black */
            ambcol = D3DRGBA(0,0,0,0);

        D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_AMBIENT, ambcol);


        /* First Directional Light */
        hres = D3D->m_pd3dDevice->lpVtbl->GetLightEnable(D3D->m_pd3dDevice, 0, &bLightOn);
        if( D3D->rc->m_DirectLight1.m_bUseLight && (!bLightOn || hres != D3D_OK || D3D->rc->m_bForceDirectLight1Update) )
        {
            /* fill in d3dlight7 struct and enable light */
            D3DVECTOR  dirVec;

            D3D->m_lgtDir1.dltType = D3DLIGHT_DIRECTIONAL;

            dirVec.dvX = D3D->rc->m_DirectLight1.m_Direction[0];
            dirVec.dvY = D3D->rc->m_DirectLight1.m_Direction[1];
            dirVec.dvZ = D3D->rc->m_DirectLight1.m_Direction[2];

            D3D->m_lgtDir1.dvDirection = dirVec;

            D3D->m_lgtDir1.dcvAmbient.dvR = D3D->rc->m_DirectLight1.m_rgbAmbient[0];
            D3D->m_lgtDir1.dcvAmbient.dvG = D3D->rc->m_DirectLight1.m_rgbAmbient[1];
            D3D->m_lgtDir1.dcvAmbient.dvB = D3D->rc->m_DirectLight1.m_rgbAmbient[2];
            D3D->m_lgtDir1.dcvAmbient.dvA = 1.0f;

            D3D->m_lgtDir1.dcvDiffuse.dvR = D3D->rc->m_DirectLight1.m_rgbDiffuse[0];
            D3D->m_lgtDir1.dcvDiffuse.dvG = D3D->rc->m_DirectLight1.m_rgbDiffuse[1];
            D3D->m_lgtDir1.dcvDiffuse.dvB = D3D->rc->m_DirectLight1.m_rgbDiffuse[2];
            D3D->m_lgtDir1.dcvDiffuse.dvA = 1.0f;

            D3D->m_lgtDir1.dcvSpecular.dvR = D3D->rc->m_DirectLight1.m_rgbSpecular[0];
            D3D->m_lgtDir1.dcvSpecular.dvG = D3D->rc->m_DirectLight1.m_rgbSpecular[1];
            D3D->m_lgtDir1.dcvSpecular.dvB = D3D->rc->m_DirectLight1.m_rgbSpecular[2];
            D3D->m_lgtDir1.dcvSpecular.dvA = 1.0f;


            D3D->m_pd3dDevice->lpVtbl->SetLight(D3D->m_pd3dDevice, 0, &D3D->m_lgtDir1);
            D3D->m_pd3dDevice->lpVtbl->LightEnable(D3D->m_pd3dDevice, 0, TRUE);
            D3D->rc->m_bForceDirectLight1Update = 0;
        }
        else if( !D3D->rc->m_DirectLight1.m_bUseLight && bLightOn )
        {
            /* disable light */
            D3D->m_pd3dDevice->lpVtbl->LightEnable(D3D->m_pd3dDevice, 0, FALSE);
        }


        /* Second Directional Light */
        hres = D3D->m_pd3dDevice->lpVtbl->GetLightEnable(D3D->m_pd3dDevice, 1, &bLightOn);
        if( D3D->rc->m_DirectLight2.m_bUseLight && (!bLightOn || hres != D3D_OK || D3D->rc->m_bForceDirectLight2Update) )
        {
            /* fill in d3dlight7 struct and enable light */
            D3DVECTOR  dirVec;

            D3D->m_lgtDir2.dltType = D3DLIGHT_DIRECTIONAL;

            dirVec.dvX = D3D->rc->m_DirectLight2.m_Direction[0];
            dirVec.dvY = D3D->rc->m_DirectLight2.m_Direction[1];
            dirVec.dvZ = D3D->rc->m_DirectLight2.m_Direction[2];

            D3D->m_lgtDir2.dvDirection = dirVec;

            D3D->m_lgtDir2.dcvAmbient.dvR = D3D->rc->m_DirectLight2.m_rgbAmbient[0];
            D3D->m_lgtDir2.dcvAmbient.dvG = D3D->rc->m_DirectLight2.m_rgbAmbient[1];
            D3D->m_lgtDir2.dcvAmbient.dvB = D3D->rc->m_DirectLight2.m_rgbAmbient[2];
            D3D->m_lgtDir2.dcvAmbient.dvA = 1.0f;

            D3D->m_lgtDir2.dcvDiffuse.dvR = D3D->rc->m_DirectLight2.m_rgbDiffuse[0];
            D3D->m_lgtDir2.dcvDiffuse.dvG = D3D->rc->m_DirectLight2.m_rgbDiffuse[1];
            D3D->m_lgtDir2.dcvDiffuse.dvB = D3D->rc->m_DirectLight2.m_rgbDiffuse[2];
            D3D->m_lgtDir2.dcvDiffuse.dvA = 1.0f;

            D3D->m_lgtDir2.dcvSpecular.dvR = D3D->rc->m_DirectLight2.m_rgbSpecular[0];
            D3D->m_lgtDir2.dcvSpecular.dvG = D3D->rc->m_DirectLight2.m_rgbSpecular[1];
            D3D->m_lgtDir2.dcvSpecular.dvB = D3D->rc->m_DirectLight2.m_rgbSpecular[2];
            D3D->m_lgtDir2.dcvSpecular.dvA = 1.0f;


            D3D->m_pd3dDevice->lpVtbl->SetLight(D3D->m_pd3dDevice, 1, &D3D->m_lgtDir2);
            D3D->m_pd3dDevice->lpVtbl->LightEnable(D3D->m_pd3dDevice, 1, TRUE);
            D3D->rc->m_bForceDirectLight2Update = 0;
        }
        else if( !D3D->rc->m_DirectLight2.m_bUseLight && bLightOn )
        {
            /* disable light */
            D3D->m_pd3dDevice->lpVtbl->LightEnable(D3D->m_pd3dDevice, 1, FALSE);
        }

        /* Point Light */
        hres = D3D->m_pd3dDevice->lpVtbl->GetLightEnable(D3D->m_pd3dDevice, 2, &bLightOn);
        if( D3D->rc->m_PointLight.m_bUseLight && (!bLightOn || hres != D3D_OK || D3D->rc->m_bForcePointLightUpdate) )
        {
            /* fill in d3dlight7 struct and enable light */
            D3DVECTOR  posVec;

            D3D->m_lgtPt.dltType = D3DLIGHT_POINT;

            posVec.dvX = D3D->rc->m_PointLight.m_Position[0];
            posVec.dvY = D3D->rc->m_PointLight.m_Position[1];
            posVec.dvZ = D3D->rc->m_PointLight.m_Position[2];

            D3D->m_lgtPt.dvPosition = posVec;

            D3D->m_lgtPt.dcvAmbient.dvR = D3D->rc->m_PointLight.m_rgbAmbient[0];
            D3D->m_lgtPt.dcvAmbient.dvG = D3D->rc->m_PointLight.m_rgbAmbient[1];
            D3D->m_lgtPt.dcvAmbient.dvB = D3D->rc->m_PointLight.m_rgbAmbient[2];
            D3D->m_lgtPt.dcvAmbient.dvA = 1.0f;

            D3D->m_lgtPt.dcvDiffuse.dvR = D3D->rc->m_PointLight.m_rgbDiffuse[0];
            D3D->m_lgtPt.dcvDiffuse.dvG = D3D->rc->m_PointLight.m_rgbDiffuse[1];
            D3D->m_lgtPt.dcvDiffuse.dvB = D3D->rc->m_PointLight.m_rgbDiffuse[2];
            D3D->m_lgtPt.dcvDiffuse.dvA = 1.0f;

            D3D->m_lgtPt.dcvSpecular.dvR = D3D->rc->m_PointLight.m_rgbSpecular[0];
            D3D->m_lgtPt.dcvSpecular.dvG = D3D->rc->m_PointLight.m_rgbSpecular[1];
            D3D->m_lgtPt.dcvSpecular.dvB = D3D->rc->m_PointLight.m_rgbSpecular[2];
            D3D->m_lgtPt.dcvSpecular.dvA = 1.0f;

            D3D->m_lgtPt.dvAttenuation0 = D3D->rc->m_PointLight.m_AttenuationConstant;
            D3D->m_lgtPt.dvAttenuation1 = D3D->rc->m_PointLight.m_AttenuationLinear;
            D3D->m_lgtPt.dvAttenuation2 = D3D->rc->m_PointLight.m_AttenuationQuadratic;

            D3D->m_lgtPt.dvRange = D3DLIGHT_RANGE_MAX;

            D3D->m_pd3dDevice->lpVtbl->SetLight(D3D->m_pd3dDevice, 2, &D3D->m_lgtPt);
            D3D->m_pd3dDevice->lpVtbl->LightEnable(D3D->m_pd3dDevice, 2, TRUE);
            D3D->rc->m_bForcePointLightUpdate = 0;
        }
        else if( !D3D->rc->m_PointLight.m_bUseLight && bLightOn )
        {
            /* disable light */
            D3D->m_pd3dDevice->lpVtbl->LightEnable(D3D->m_pd3dDevice, 2, FALSE);
        }



        /* Particle Systems */
        if(D3D->m_bDisplayPS)
        {
            RParticleSystem *ps = D3D->rc->m_pPS_First;
            D3DVERTEX vs[3];
            D3DMATERIAL7 ps_mtrl;
            MeVector3 *curr_pos;
            int i;
            AcmeReal x,y,z;
            AcmeReal x1,y1,z1;

            ZeroMemory( &ps_mtrl, sizeof(D3DMATERIAL7) );
            ps_mtrl.power = 40.0f;

            D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_WORLD, &ident );
            D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_VIEW, &ident );

            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID );
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_COLORKEYENABLE, TRUE );

            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ALPHAREF, 0x000000FE);
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATER);
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ALPHATESTENABLE, TRUE);

            while( ps )
            {
                /* set up material, texture, etc */
                ps_mtrl.dcvEmissive.r = ps->m_rgbEmissive[0];
                ps_mtrl.dcvEmissive.g = ps->m_rgbEmissive[1];
                ps_mtrl.dcvEmissive.b = ps->m_rgbEmissive[2];
                ps_mtrl.dcvEmissive.a = 1;

                ps_mtrl.dcvAmbient.r = ps->m_rgbAmbient[0];
                ps_mtrl.dcvAmbient.g = ps->m_rgbAmbient[1];
                ps_mtrl.dcvAmbient.b = ps->m_rgbAmbient[2];
                ps_mtrl.dcvAmbient.a = 1;

                ps_mtrl.dcvDiffuse.r = ps->m_rgbDiffuse[0];
                ps_mtrl.dcvDiffuse.g = ps->m_rgbDiffuse[1];
                ps_mtrl.dcvDiffuse.b = ps->m_rgbDiffuse[2];
                ps_mtrl.dcvDiffuse.a = 1;

                ps_mtrl.dcvSpecular.r = ps->m_rgbSpecular[0];
                ps_mtrl.dcvSpecular.g = ps->m_rgbSpecular[1];
                ps_mtrl.dcvSpecular.b = ps->m_rgbSpecular[2];
                ps_mtrl.dcvSpecular.a = 1;

                D3D->m_pd3dDevice->lpVtbl->SetMaterial( D3D->m_pd3dDevice, &ps_mtrl );

                if( D3D->m_bUseTextures && ps->m_nTextureID > -1 && ps->m_nTextureID < 25 )
                    D3D->m_pd3dDevice->lpVtbl->SetTexture( D3D->m_pd3dDevice, 0, D3D->m_pddsTexture[ps->m_nTextureID] );
                else
                    D3D->m_pd3dDevice->lpVtbl->SetTexture( D3D->m_pd3dDevice, 0, NULL );

                /* loop through particles */
                curr_pos = ps->m_Positions;
                for( i = 0; i < ps->m_nNumParticles; i++ )
                {
                    /* create triangle and draw it */
                    vs[0].dvNX = 0.0f;
                    vs[0].dvNY = 0.0f;
                    vs[0].dvNZ = -1.0f;
                    vs[0].dvTU = 0.0f;
                    vs[0].dvTV = 0.0f;
                    vs[1].dvNX = 0.0f;
                    vs[1].dvNY = 0.0f;
                    vs[1].dvNZ = -1.0f;
                    vs[1].dvTU = 0.0f;
                    vs[1].dvTV = 1.0f;
                    vs[2].dvNX = 0.0f;
                    vs[2].dvNY = 0.0f;
                    vs[2].dvNZ = -1.0f;
                    vs[2].dvTU = 1.0f;
                    vs[2].dvTV = 0.5f;

                    x = (*curr_pos)[0];
                    y = (*curr_pos)[1];
                    z = (*curr_pos)[2];
                    curr_pos++;  /* m_Positions is array of 4-vectors, so skip one co-ord */

                    x1 = x*D3D->rc->m_CamMatrix[0][0] + y*D3D->rc->m_CamMatrix[1][0] + z*D3D->rc->m_CamMatrix[2][0] + D3D->rc->m_CamMatrix[3][0];
                    y1 = x*D3D->rc->m_CamMatrix[0][1] + y*D3D->rc->m_CamMatrix[1][1] + z*D3D->rc->m_CamMatrix[2][1] + D3D->rc->m_CamMatrix[3][1];
                    z1 = x*D3D->rc->m_CamMatrix[0][2] + y*D3D->rc->m_CamMatrix[1][2] + z*D3D->rc->m_CamMatrix[2][2] + D3D->rc->m_CamMatrix[3][2];

                    vs[0].dvX = x1 - 0.5f*ps->m_TriangleSize;
                    vs[0].dvY = y1 - 0.5f*ps->m_TriangleSize;
                    vs[0].dvZ = z1;

                    vs[1].dvX = x1 + 0.5f*ps->m_TriangleSize;
                    vs[1].dvY = y1 - 0.5f*ps->m_TriangleSize;
                    vs[1].dvZ = z1;

                    vs[2].dvX = x1;
                    vs[2].dvY = y1 + 0.5f*ps->m_TriangleSize;
                    vs[2].dvZ = z1;

                    D3D->m_pd3dDevice->lpVtbl->DrawPrimitive( D3D->m_pd3dDevice, D3DPT_TRIANGLELIST, D3DFVF_VERTEX, vs, 3, 0 );

                }
                ps = ps->m_pNextSystem;
            }
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_COLORKEYENABLE, FALSE );
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
        }


        /* Render RGraphic lists */

        do2D = 0;
        current = D3D->rc->m_pRG_First;

        D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);

        if( D3D->m_bAlphaBlend )
        {
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA );
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_DESTALPHA );
         }
         else
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );

        rval = D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_VIEW, (D3DMATRIX *)D3D->rc->m_CamMatrix );
        if( rval != D3D_OK )
            ME_REPORT(MeWarning(1,"Failed to set view matrix."));

        while( current )
        {
            D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_WORLD,  (D3DMATRIX *)current->m_pObject->m_Matrix);

            D3D->m_d3dmtrl.dcvAmbient.r = current->m_pObject->m_ColorAmbient[0];
            D3D->m_d3dmtrl.dcvAmbient.g = current->m_pObject->m_ColorAmbient[1];
            D3D->m_d3dmtrl.dcvAmbient.b = current->m_pObject->m_ColorAmbient[2];
            D3D->m_d3dmtrl.dcvAmbient.a = current->m_pObject->m_ColorAmbient[3];

            D3D->m_d3dmtrl.dcvDiffuse.r = current->m_pObject->m_ColorDiffuse[0];
            D3D->m_d3dmtrl.dcvDiffuse.g = current->m_pObject->m_ColorDiffuse[1];
            D3D->m_d3dmtrl.dcvDiffuse.b = current->m_pObject->m_ColorDiffuse[2];
            D3D->m_d3dmtrl.dcvDiffuse.a = current->m_pObject->m_ColorDiffuse[3];

            D3D->m_d3dmtrl.dcvSpecular.r = current->m_pObject->m_ColorSpecular[0];
            D3D->m_d3dmtrl.dcvSpecular.g = current->m_pObject->m_ColorSpecular[1];
            D3D->m_d3dmtrl.dcvSpecular.b = current->m_pObject->m_ColorSpecular[2];
            D3D->m_d3dmtrl.dcvSpecular.a = current->m_pObject->m_ColorSpecular[3];

            D3D->m_d3dmtrl.dcvEmissive.r = current->m_pObject->m_ColorEmissive[0];
            D3D->m_d3dmtrl.dcvEmissive.g = current->m_pObject->m_ColorEmissive[1];
            D3D->m_d3dmtrl.dcvEmissive.b = current->m_pObject->m_ColorEmissive[2];
            D3D->m_d3dmtrl.dcvEmissive.a = current->m_pObject->m_ColorEmissive[3];

            D3D->m_d3dmtrl.dvPower = current->m_pObject->m_SpecularPower;

            D3D->m_pd3dDevice->lpVtbl->SetMaterial( D3D->m_pd3dDevice, &D3D->m_d3dmtrl );

            if( (current->m_pObject->m_bIsWireFrame && D3D->m_bAllowWireFrame) || (D3D->m_bForceWireFrame && !do2D) )
                D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME );
            else
                D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID );


            if( (D3D->m_bUseTextures || do2D) && current->m_pObject->m_nTextureID > -1 && current->m_pObject->m_nTextureID < 25 && !((current->m_pObject->m_bIsWireFrame && D3D->m_bAllowWireFrame) || (D3D->m_bForceWireFrame && !do2D ))  )
            {
                if( D3D->m_bLinearFilter )
                {
                    D3D->m_pd3dDevice->lpVtbl->SetTextureStageState(D3D->m_pd3dDevice, 0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
                    D3D->m_pd3dDevice->lpVtbl->SetTextureStageState(D3D->m_pd3dDevice, 0, D3DTSS_MAGFILTER, D3DTFN_LINEAR);
                }
                else
                {
                    D3D->m_pd3dDevice->lpVtbl->SetTextureStageState(D3D->m_pd3dDevice, 0, D3DTSS_MINFILTER, D3DTFN_POINT);
                    D3D->m_pd3dDevice->lpVtbl->SetTextureStageState(D3D->m_pd3dDevice, 0, D3DTSS_MAGFILTER, D3DTFN_POINT);
                }

                if( D3D_OK != D3D->m_pd3dDevice->lpVtbl->SetTexture( D3D->m_pd3dDevice, 0, D3D->m_pddsTexture[current->m_pObject->m_nTextureID] ) )
                    ME_REPORT(MeWarning(1,"Problem setting texture."));
            }
            else
            {
                if( D3D_OK != D3D->m_pd3dDevice->lpVtbl->SetTexture( D3D->m_pd3dDevice, 0, NULL ) )
                    ME_REPORT(MeWarning(1,"Problem setting texture to NULL."));
            }

            D3D->m_pd3dDevice->lpVtbl->DrawPrimitive( D3D->m_pd3dDevice, D3DPT_TRIANGLELIST, D3DFVF_VERTEX, current->m_pVertices, current->m_pObject->m_nNumVertices, 0 );

            current = current->m_pNext;

            if( !do2D && !current && D3D->m_bDisplay2D )
            {
                do2D = 1;
                current = D3D->rc->m_pRG_First2D;

                rval = D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_VIEW, (D3DMATRIX *)D3D->rc->m_CamMatrix2D);
                if( rval != D3D_OK )
                    ME_REPORT(MeWarning(1,"Failed to set view matrix for 2D."));

                D3D->m_pd3dDevice->lpVtbl->LightEnable(D3D->m_pd3dDevice, 0, FALSE);
                D3D->m_pd3dDevice->lpVtbl->LightEnable(D3D->m_pd3dDevice, 1, FALSE);
                D3D->m_pd3dDevice->lpVtbl->LightEnable(D3D->m_pd3dDevice, 2, FALSE);
                ambcol = D3DRGBA(1,1,1,1);
                D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_AMBIENT, ambcol);

                D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);


                if( D3D->m_bAlphaBlend2D )
                {
                    D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
                    D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA );
                    D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_DESTALPHA );
                }
                else
                    D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
            }
        }

#if SHADOWS

        /* Render Shadows of all RGraphics */

        do2D = 0;
        current = D3D->rc->m_pRG_First;


        D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
        D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
        D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ALPHABLENDENABLE, SHADOWS_WITH_ALPHA );
        D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA );
        D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA );

        rval = D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_VIEW, (D3DMATRIX *)D3D->rc->m_CamMatrix );
        if( rval != D3D_OK )
            ME_REPORT(MeWarning(1,"Failed to set view matrix."));

        /*  A heuristic to find the floor and sky */

        current = D3D->rc->m_pRG_First;
        while( current )
        {
            if (current->m_pLWMatrix==0 && current->m_nMaxNumVertices==48)
                floor = current;
            current = current->m_pNext;
        }

        current = D3D->rc->m_pRG_First;
        if (current->m_nMaxNumVertices==864)
            sky = current;

        while( current && floor )
        {
            float alpha, shade;

            //  Apply the shadow transformation.  For a flat floor with elevation e, 
            //  the transformation is..
            //
            //         | y  -x  0  e*x |
            //     S = | 0   0  0  e*y |  (transposed)
            //         | 0  -z  y  e*z |
            //         | 0  -w  0  y   |
            //
            //  where w=1 is a spot light, and w=0 is a directional light.

            MeReal x=-.65f, y=.6f, z=-.5f, e=floor->m_pVertices->m_Y + 0.02f;
            MeMatrix4 shadow = {{ y,0,0,0 }, {-x,0,-z,0}, {0,0,y,0}, {e*x,e*y,e*z,y}};

            D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *)shadow );
            D3D->m_pd3dDevice->lpVtbl->MultiplyTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_WORLD,  (D3DMATRIX *)current->m_pObject->m_Matrix);
            
#if SHADOWS_WITH_ALPHA
            alpha = 0.3f;
            shade = 0;
#else
            alpha = 1;
            shade = 0.4f;
#endif
            D3D->m_d3dmtrl.dcvAmbient.r = shade;
            D3D->m_d3dmtrl.dcvAmbient.g = shade;
            D3D->m_d3dmtrl.dcvAmbient.b = shade;
            D3D->m_d3dmtrl.dcvAmbient.a = alpha;

            D3D->m_d3dmtrl.dcvDiffuse = D3D->m_d3dmtrl.dcvAmbient;
            D3D->m_d3dmtrl.dcvSpecular = D3D->m_d3dmtrl.dcvAmbient;
            D3D->m_d3dmtrl.dcvEmissive = D3D->m_d3dmtrl.dcvAmbient;

            D3D->m_d3dmtrl.dvPower = current->m_pObject->m_SpecularPower;

            D3D->m_pd3dDevice->lpVtbl->SetMaterial( D3D->m_pd3dDevice, &D3D->m_d3dmtrl );

            if( (current->m_pObject->m_bIsWireFrame && D3D->m_bAllowWireFrame) || (D3D->m_bForceWireFrame && !do2D) )
                D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME );
            else
                D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID );

            if( D3D_OK != D3D->m_pd3dDevice->lpVtbl->SetTexture( D3D->m_pd3dDevice, 0, NULL ) )
                ME_REPORT(MeWarning(1,"Problem setting texture to NULL.\n"));

            if (current->m_pLWMatrix && current!=sky)
                D3D->m_pd3dDevice->lpVtbl->DrawPrimitive( D3D->m_pd3dDevice, D3DPT_TRIANGLELIST, D3DFVF_VERTEX, current->m_pVertices, current->m_pObject->m_nNumVertices, 0 );

            current = current->m_pNext;
        }

        /*  Restore non-shadow matrix */
        rval = D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_PROJECTION, (D3DMATRIX *)D3D->rc->m_ProjMatrix );
        if( rval != D3D_OK )
            ME_REPORT(MeWarning(1,"Failed to set projection matrix."));

    D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_CULLMODE, D3DCULL_NONE );

#endif
        /* Native lines */
        {
            RNativeLine* curLine;

            curLine = D3D->rc->m_pNL_First;
            
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);            
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
            
            rval = D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_VIEW, (D3DMATRIX *)D3D->rc->m_CamMatrix );
            if( rval != D3D_OK )
                ME_REPORT(MeWarning(1,"Failed to set view matrix."));
            
            while( curLine )
            {
                D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_WORLD,  (D3DMATRIX *)curLine->m_matrix);
                
                D3D->m_d3dmtrl.dcvAmbient.r = curLine->m_color[0];
                D3D->m_d3dmtrl.dcvAmbient.g = curLine->m_color[1];
                D3D->m_d3dmtrl.dcvAmbient.b = curLine->m_color[2];
                D3D->m_d3dmtrl.dcvAmbient.a = curLine->m_color[3];
                
                D3D->m_d3dmtrl.dcvDiffuse.r = curLine->m_color[0];
                D3D->m_d3dmtrl.dcvDiffuse.g = curLine->m_color[1];
                D3D->m_d3dmtrl.dcvDiffuse.b = curLine->m_color[2];
                D3D->m_d3dmtrl.dcvDiffuse.a = curLine->m_color[3];
                
                D3D->m_d3dmtrl.dcvEmissive.r = 0;
                D3D->m_d3dmtrl.dcvEmissive.g = 0;
                D3D->m_d3dmtrl.dcvEmissive.b = 0;
                D3D->m_d3dmtrl.dcvEmissive.a = 0;
                
                D3D->m_d3dmtrl.dvPower = 0;
                
                D3D->m_pd3dDevice->lpVtbl->SetMaterial( D3D->m_pd3dDevice, &D3D->m_d3dmtrl );
                if( D3D_OK != D3D->m_pd3dDevice->lpVtbl->SetTexture( D3D->m_pd3dDevice, 0, NULL ) )
                    ME_REPORT(MeWarning(1,"Problem setting texture to NULL."));

                D3D->m_pd3dDevice->lpVtbl->DrawPrimitive( D3D->m_pd3dDevice, D3DPT_LINELIST, D3DFVF_VERTEX, curLine->m_vectors, 2, 0 );               

                curLine = curLine->m_next;                
            }   
        }

        if(D3D->m_bDisplayLookAt)
        {/* draws lookat */
            D3DVERTEX vs[2];
            D3DVERTEX vt = {D3D->rc->m_CameraLookAt[0]-0.1f,D3D->rc->m_CameraLookAt[1],D3D->rc->m_CameraLookAt[2],0,0,0,0,0};
            D3DVERTEX vt1 = {D3D->rc->m_CameraLookAt[0]+0.1f,D3D->rc->m_CameraLookAt[1],D3D->rc->m_CameraLookAt[2],0,0,0,0,0};
            D3DMATRIX ident = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
            D3D->m_pd3dDevice->lpVtbl->SetTexture( D3D->m_pd3dDevice, 0, NULL );
            D3D->m_pd3dDevice->lpVtbl->SetRenderState(D3D->m_pd3dDevice, D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
            D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_WORLD, &ident );
            rval = D3D->m_pd3dDevice->lpVtbl->SetTransform( D3D->m_pd3dDevice, D3DTRANSFORMSTATE_VIEW, (D3DMATRIX *)D3D->rc->m_CamMatrix );
            if( rval != D3D_OK )
                ME_REPORT(MeWarning(1,"Failed to set view matrix. (for lookat)"));

            vs[0] = vt;
            vs[1] = vt1;
            D3D->m_d3dmtrl.dcvDiffuse.r = D3D->m_d3dmtrl.dcvAmbient.r = 1;
            D3D->m_d3dmtrl.dcvDiffuse.g = D3D->m_d3dmtrl.dcvAmbient.g = 1;
            D3D->m_d3dmtrl.dcvDiffuse.b = D3D->m_d3dmtrl.dcvAmbient.b = 1;
            D3D->m_d3dmtrl.dcvDiffuse.a = D3D->m_d3dmtrl.dcvAmbient.a = 1;
            D3D->m_pd3dDevice->lpVtbl->SetMaterial( D3D->m_pd3dDevice, &D3D->m_d3dmtrl );
            D3D->m_pd3dDevice->lpVtbl->DrawPrimitive( D3D->m_pd3dDevice, D3DPT_LINELIST, D3DFVF_VERTEX, vs, 2, 0 );
        }

    }
    /* end of rendering */

    if(FAILED( D3D->m_pd3dDevice->lpVtbl->EndScene(D3D->m_pd3dDevice) ))
    {
        ME_REPORT(MeWarning(1,"Failed to end scene."));
        return;
    }
}


#endif
