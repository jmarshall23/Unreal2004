/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:39 $ - Revision: $Revision: 1.72.2.4 $

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

/* Create windows and D3D objects */

#define INITGUID

#ifdef WITH_D3D

#include <stdio.h>

#include "MeViewer_d3d.h"
#include "Resource_d3d.h"
#include <MeSimpleFile.h>
#include <MeMemory.h>
#include <MeVersion.h>
#include <MeString.h>

/* Moved to x-platform RMouseCam.h grrr */
/* Globals for mouse movement */
int D3D_NewMouseStart = 0;
/* int D3D_MouseStartX = 0; */
/* int D3D_MouseStartY = 0; */
/* AcmeReal D3D_CameraStartDist = 0; */
/* AcmeReal D3D_CameraStartTheta = 0; */
/* AcmeReal D3D_CameraStartPhi = 0; */
/* int D3D_CameraStartX = 0; */

MeI64 D3D_startframeclock;

HMODULE D3D_DllHm = NULL;


BOOL D3D_CreateWin(D3DVars* D3D,HINSTANCE hInst)
{
    WNDCLASS wc;
    char fullname[150];
    char count = 0;

    while( !D3D_DllHm && MeGetDefaultFileLocation(count) != NULL )
    {
        fullname[0] = '\0';
        strcat(fullname, MeGetDefaultFileLocation(count++));
        strcat(fullname, "MeViewerResources.dll");
        D3D_DllHm = LoadLibrary(fullname);
    }
    if( !D3D_DllHm )
        MeWarning(1,"Couldn't load MeViewerResources.dll");

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = DummyD3D_WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof (DWORD);
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon(D3D_DllHm, MAKEINTRESOURCE( IDI_ME_ICON ));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "MathEngine D3DIM";

    if (!RegisterClass(&wc))
        return FALSE;

        D3D->m_hWnd = CreateWindow(
                "MathEngine D3DIM",                     /* Class */
                D3D->m_strWindowTitle,
                WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,                          /* Init. x pos */
                CW_USEDEFAULT,                          /* Init. y pos */
                640,                                    /* Init. x size */
                480,                                    /* Init. y size */
                NULL,                                   /* Parent window */
                LoadMenu(D3D_DllHm,MAKEINTRESOURCE( IDR_MENU )),    /* Menu handle */
                hInst,                                  /* Program handle */
                0L);

    if (!D3D->m_hWnd)
    {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0,
            NULL);

        MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
        LocalFree( lpMsgBuf );
        return FALSE;
    }
        SetWindowLong(D3D->m_hWnd,GWL_WNDPROC,(LONG)D3D_WndProc);
        SetWindowLong(D3D->m_hWnd,GWL_USERDATA,(LONG)D3D->rc);
    SendMessage(D3D->m_hWnd,WM_SIZE,0,0);
    UpdateWindow( D3D->m_hWnd );

    return TRUE;
}


void D3D_InitGlobals(RRender* rc)
{
        D3DVars* D3D = rc->D3D;
    memset(D3D, 0, sizeof (struct D3DVars));

    D3D->m_hWnd = NULL;
    D3D->m_bActive = FALSE;
    D3D->m_bReady = FALSE;
    D3D->m_bIsFrameLocked = TRUE;
    D3D->m_bDisplayFps = TRUE;
    D3D->m_dwFpsUpdate = 60;
    D3D->m_uiTicksPerFrame = 0;
    snprintf(D3D->m_strWindowTitle, MAX_TITLE_LENGTH - 1
        , "%s (v%s) [Direct3D]", ME_PRODUCT_NAME, ME_VERSION_STRING);
    D3D->m_strWindowTitle[MAX_TITLE_LENGTH] = '\0';
    D3D->ConfirmDevFn = NULL;
    D3D->m_bLockAspectRatio = TRUE;
    D3D->m_bAlphaBlend = FALSE;
    D3D->m_bAlphaBlend2D = TRUE;
    D3D->m_bAllowWireFrame = TRUE;
    if(rc->m_options.m_bWireFrame != MEFALSE)
        D3D->m_bForceWireFrame = !MEFALSE;
    else
        D3D->m_bForceWireFrame = MEFALSE;
    if(rc->m_options.m_bTextures != MEFALSE)
        D3D->m_bUseTextures = !MEFALSE;
    else
        D3D->m_bUseTextures = MEFALSE;
    D3D->m_bDisplayLookAt = FALSE;
    D3D->m_bUseAntiAliasing = FALSE;
    D3D->m_bLinearFilter = TRUE;
    D3D->m_bDisplay2D = TRUE;
    D3D->m_bDisplayPS = TRUE;
    D3D->m_bDoStep = FALSE;
    D3D->m_bNoVSync = TRUE;
    D3D->rc = rc;

    RMouseDrag.p_rc = &D3D->rc;
    RMouseDrag.p_width = &D3D->m_dwRenderWidth;
    RMouseDrag.p_height = &D3D->m_dwRenderHeight;
    RMouseDrag.m_allowContextMenu = 0;

    strncpy(rc->m_ButtonText[0], "Up Arrow", 20);
    strncpy(rc->m_ButtonText[1], "Down Arrow", 20);
    strncpy(rc->m_ButtonText[2], "Left Arrow", 20);
    strncpy(rc->m_ButtonText[3], "Right Arrow", 20);
    strncpy(rc->m_ButtonText[4], "w", 20);
    strncpy(rc->m_ButtonText[5], "s", 20);
    strncpy(rc->m_ButtonText[6], "a", 20);
    strncpy(rc->m_ButtonText[7], "d", 20);
    strncpy(rc->m_ButtonText[8], "h", 20);
    strncpy(rc->m_ButtonText[9], "m", 20);
    strncpy(rc->m_ButtonText[10], "Enter", 20);
    strncpy(rc->m_ButtonText[11], "Space", 20);
    strncpy(rc->m_ButtonText[12], "o", 20);
    strncpy(rc->m_ButtonText[13], "p", 20);

}

long WINAPI DummyD3D_WndProc(HWND win, UINT msg, WPARAM wparam,
    LPARAM lparam)
{
    return DefWindowProc( win, msg, wparam, lparam );
}

long WINAPI D3D_WndProc(HWND win, UINT msg, WPARAM wparam,
    LPARAM lparam)
{
    RMouseButtonEvent mouse_event = 0;
        D3DVars* D3D;
        RRender* rc;
        rc = (RRender*)GetWindowLong(win,GWL_USERDATA);
        D3D = rc->D3D;

        switch(msg)
        {
    case WM_COMMAND:
                switch(LOWORD(wparam))
        {
        case IDM_ARROWUP:
            RExecuteUpCallback(rc);
            break;

        case IDM_ARROWDOWN:
            RExecuteDownCallback(rc);
            break;

        case IDM_ARROWLEFT:
            RExecuteLeftCallback(rc);
            break;

        case IDM_ARROWRIGHT:
            RExecuteRightCallback(rc);
            break;

        case IDM_DEVDLG:
            if( D3D->m_bActive && D3D->m_bReady )
            {
                D3D_Pause(D3D,TRUE);
                if( D3D_doDevDlg(D3D) == S_OK )
                {
                    D3D_CleanUpD3D(D3D);
                    D3D_setupD3D(D3D);
                }
                D3D_Pause(D3D,FALSE);
            }
            break;

        case IDM_TOGGLEFRAMELOCK:
            D3D->m_bIsFrameLocked = !D3D->m_bIsFrameLocked;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bIsFrameLocked )
                    CheckMenuItem( hMenu, IDM_TOGGLEFRAMELOCK, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_TOGGLEFRAMELOCK, MF_UNCHECKED );
            }
            break;

         case IDM_TOGGLEVSYNC:
            D3D->m_bNoVSync = !D3D->m_bNoVSync;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bNoVSync )
                    CheckMenuItem( hMenu, IDM_TOGGLEVSYNC, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_TOGGLEVSYNC, MF_UNCHECKED );
            }
            break;


        case IDM_TOGGLEFPS:
            D3D->m_bDisplayFps = !D3D->m_bDisplayFps;

            if( !D3D->m_bDisplayFps && D3D->rc->m_fpsG )
            {
                RGraphicDelete(rc, rc->m_fpsG, 1);
                rc->m_fpsG = 0;
            }

            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bDisplayFps )
                    CheckMenuItem( hMenu, IDM_TOGGLEFPS, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_TOGGLEFPS, MF_UNCHECKED );
            }
            break;

        case IDM_TOGGLEALPHABLEND:
            D3D->m_bAlphaBlend = !D3D->m_bAlphaBlend;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bAlphaBlend )
                    CheckMenuItem( hMenu, IDM_TOGGLEALPHABLEND, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_TOGGLEALPHABLEND, MF_UNCHECKED );
            }
            break;

        case IDM_TOGGLEALPHABLEND2D:
            D3D->m_bAlphaBlend2D = !D3D->m_bAlphaBlend2D;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bAlphaBlend2D )
                    CheckMenuItem( hMenu, IDM_TOGGLEALPHABLEND2D, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_TOGGLEALPHABLEND2D, MF_UNCHECKED );
            }
            break;


        case IDM_TOGGLEANTIALIAS:
            D3D->m_bUseAntiAliasing = !D3D->m_bUseAntiAliasing;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bUseAntiAliasing )
                    CheckMenuItem( hMenu, IDM_TOGGLEANTIALIAS, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_TOGGLEANTIALIAS, MF_UNCHECKED );
            }
            break;


        case IDM_TOGGLEWIREFRAME:
            D3D->m_bAllowWireFrame = !D3D->m_bAllowWireFrame;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bAllowWireFrame )
                    CheckMenuItem( hMenu, IDM_TOGGLEWIREFRAME, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_TOGGLEWIREFRAME, MF_UNCHECKED );
            }
            break;

        case IDM_FORCEWIREFRAME:
            D3D->m_bForceWireFrame = !D3D->m_bForceWireFrame;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bForceWireFrame )
                {
                    CheckMenuItem( hMenu, IDM_FORCEWIREFRAME, MF_CHECKED );
                    EnableMenuItem( hMenu, IDM_TOGGLEWIREFRAME, MF_GRAYED );
                }
                else
                {
                    CheckMenuItem( hMenu, IDM_FORCEWIREFRAME, MF_UNCHECKED );
                    EnableMenuItem( hMenu, IDM_TOGGLEWIREFRAME, MF_ENABLED );
                }
            }
            break;

        case IDM_RECALIBTIMER:
            MeInfo(1,"Recalibrating timer..." );
            D3D_CalibrateTimer(D3D);
            MeInfo(1,"%d ticks per second.", D3D->m_uiTicksPerSec );
            break;

        case IDM_TOGGLEFULLSCREEN:
            if( D3D->m_bActive && D3D->m_bReady )
            {
                
                D3D->m_bReady = FALSE;
                D3D->m_pDeviceInfo->bWindowed = !D3D->m_pDeviceInfo->bWindowed;
                if( D3D_CleanUpD3D(D3D) == S_OK )
                    D3D_setupD3D(D3D);
                else
                    return 0;


                D3D->m_bReady = TRUE;
            }
            break;

        case IDM_TOGGLEASPECTRATIO:
            D3D->m_bLockAspectRatio = !D3D->m_bLockAspectRatio;

            if(D3D->m_bLockAspectRatio)
                rc->m_AspectRatio = (float)640/(float)448;

            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bLockAspectRatio )
                    CheckMenuItem( hMenu, IDM_TOGGLEASPECTRATIO, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_TOGGLEASPECTRATIO, MF_UNCHECKED );

            }
            D3D_CleanUpD3D(D3D);
            D3D_setupD3D(D3D);
            break;

        case IDM_TOGGLEDISPLAYLOOKAT:
            D3D->m_bDisplayLookAt = !D3D->m_bDisplayLookAt;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bDisplayLookAt )
                    CheckMenuItem( hMenu, IDM_TOGGLEDISPLAYLOOKAT, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_TOGGLEDISPLAYLOOKAT, MF_UNCHECKED );
            }
            break;

        case IDM_TOGGLEDISPLAY2D:
            D3D->m_bDisplay2D = !D3D->m_bDisplay2D;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bDisplay2D )
                    CheckMenuItem( hMenu, IDM_TOGGLEDISPLAY2D, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_TOGGLEDISPLAY2D, MF_UNCHECKED );
            }
            break;

        case IDM_TOGGLEPS:
            D3D->m_bDisplayPS = !D3D->m_bDisplayPS;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bDisplayPS )
                    CheckMenuItem( hMenu, IDM_TOGGLEPS, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_TOGGLEPS, MF_UNCHECKED );
            }
            break;

        case IDM_TOGGLEHELP:
            RRenderToggleUserHelp(rc);
           /* Bit of a hack for pause state... */
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( rc->m_bPause )
                    CheckMenuItem( hMenu, IDM_ISPAUSED, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_ISPAUSED, MF_UNCHECKED );
            }
            break;

        case IDM_ISPAUSED:
        {
            HMENU hMenu = GetMenu(D3D->m_hWnd);
            if(rc->m_bPause )
            {
               /* Play */
                rc->m_bPause = 0;
                CheckMenuItem( hMenu, IDM_ISPAUSED, MF_UNCHECKED );
            }
            else
            {
               /* Pause */
                rc->m_bPause = 1;
                CheckMenuItem( hMenu, IDM_ISPAUSED, MF_CHECKED );
            }
        }
        break;

        case IDM_DRAWSTEP:
        {
            HMENU hMenu = GetMenu(D3D->m_hWnd);
            D3D->m_bDoStep = 1;
            rc->m_bPause = 1;
            CheckMenuItem( hMenu, IDM_ISPAUSED, MF_CHECKED );
        }
        break;

        case IDM_TOGGLEENABLETEXTURES:
            D3D->m_bUseTextures = !D3D->m_bUseTextures;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                if( D3D->m_bUseTextures )
                    CheckMenuItem( hMenu, IDM_TOGGLEENABLETEXTURES, MF_CHECKED );
                else
                    CheckMenuItem( hMenu, IDM_TOGGLEENABLETEXTURES, MF_UNCHECKED );
            }
            break;

        case IDM_TEXPOINTFILTER:
            D3D->m_bLinearFilter = 0;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                CheckMenuItem( hMenu, IDM_TEXPOINTFILTER, MF_CHECKED );
                CheckMenuItem( hMenu, IDM_TEXLINEARFILTER, MF_UNCHECKED );
            }
            break;

        case IDM_TEXLINEARFILTER:
            D3D->m_bLinearFilter = 1;
            {
                HMENU hMenu = GetMenu(D3D->m_hWnd);
                CheckMenuItem( hMenu, IDM_TEXPOINTFILTER, MF_UNCHECKED );
                CheckMenuItem( hMenu, IDM_TEXLINEARFILTER, MF_CHECKED );
            }
            break;

        case IDM_EXIT:
            SendMessage(win, WM_CLOSE, 0, 0);
            D3D->m_bQuit = TRUE;
        }
        break;

    case WM_KEYDOWN:
        if( wparam == VK_CONTROL && !(lparam & (1<<30)) )
            D3D_NewMouseStart = 1;
        else if( wparam == VK_SHIFT && !(lparam & (1<<30)) )
            D3D_NewMouseStart = 1;
        else if( wparam == VK_F12 )
            RMouseDrag.m_allowContextMenu = !RMouseDrag.m_allowContextMenu;

        else if(wparam == VK_NEXT)
        {
            MeVector3 newLookAt;
            RCameraGetLookAt(rc, newLookAt);
            newLookAt[2] -=0.5f;
            RCameraSetLookAt(rc, newLookAt);
        }
        else if(wparam == VK_PRIOR)
        {
            MeVector3 newLookAt;
            RCameraGetLookAt(rc, newLookAt);
            newLookAt[2] +=0.5f;
            RCameraSetLookAt(rc, newLookAt);
        }
       break;

    case WM_KEYUP:
        if( wparam == VK_CONTROL )
            D3D_NewMouseStart = 1;
        else if( wparam == VK_SHIFT )
            D3D_NewMouseStart = 1;
        break;

    case WM_CHAR:
        if( wparam == 'a' || wparam == 'A' )
            RExecuteLeft2Callback(rc);
        else if( wparam == 's' || wparam == 'S' )
            RExecuteDown2Callback(rc);
        else if( wparam == 'd' || wparam == 'D')
            RExecuteRight2Callback(rc);
        else if( wparam == 'w' || wparam == 'W')
            RExecuteUp2Callback(rc);
        else if (wparam == 'h' || wparam == 'H')
            RExecuteActionNCallback(rc, 0);
        else if (wparam == 'm' || wparam == 'M')
            RExecuteActionNCallback(rc, 1);
        
        else if( wparam == (rc->m_pCallBacks->m_actionKeys[0]))
            RExecuteActionNCallback(rc, 2);
        else if( wparam == (rc->m_pCallBacks->m_actionKeys[1]))
            RExecuteActionNCallback(rc, 3);
        else if( wparam == (rc->m_pCallBacks->m_actionKeys[2]))
            RExecuteActionNCallback(rc, 4);
        else if( wparam == (rc->m_pCallBacks->m_actionKeys[3]))
            RExecuteActionNCallback(rc, 5);

        else if( wparam=='+') /* Zoom in */
            RCameraZoom(rc, -0.5f);
        else if(wparam =='-') /* Zoom out */
            RCameraZoom(rc, 0.5f);

        break;

    case WM_MOUSEMOVE:
        mouse_event = kRStillPressed;
        break;

    case WM_LBUTTONDOWN:
        if(RMouseDrag.m_button &&
            (RMouseDrag.m_button != kRLeftButton) &&
            rc->m_pCallBacks->m_Mouse.m_CallBack &&
            RMouseDrag.m_meModifiers) /* We've overridden a previously pressed button */
            RExecuteMouseCallback(rc,
                GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), RMouseDrag.m_meModifiers,
                RMouseDrag.m_button, kRNewlyReleased);
        RMouseDrag.m_button = kRLeftButton;
        mouse_event = kRNewlyPressed;
        SetCapture(win);
        break;

    case WM_MBUTTONDOWN:
        if(RMouseDrag.m_button &&
            (RMouseDrag.m_button != kRMiddleButton) &&
            rc->m_pCallBacks->m_Mouse.m_CallBack &&
            RMouseDrag.m_meModifiers) /* We've overridden a previously pressed button */
            RExecuteMouseCallback(rc,
                GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), RMouseDrag.m_meModifiers,
                RMouseDrag.m_button, kRNewlyReleased);

        RMouseDrag.m_button = kRMiddleButton;
        mouse_event = kRNewlyPressed;
        SetCapture(win);
        break;

    case WM_RBUTTONDOWN:
        if(RMouseDrag.m_button &&
            (RMouseDrag.m_button != kRRightButton) &&
            rc->m_pCallBacks->m_Mouse.m_CallBack &&
            RMouseDrag.m_meModifiers) /* We've overridden a previously pressed button */
            RExecuteMouseCallback(rc,
                GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), RMouseDrag.m_meModifiers,
                RMouseDrag.m_button, kRNewlyReleased);

        RMouseDrag.m_button = kRRightButton;
        mouse_event = kRNewlyPressed;
        SetCapture(win);
        break;

    case WM_LBUTTONUP:
        if(RMouseDrag.m_button == kRLeftButton)
            mouse_event = kRNewlyReleased;
        /* if all mouse buttons up, release capture. */
        if(0 == (wparam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)))
            ReleaseCapture();
        break;

    case WM_MBUTTONUP:
        if(RMouseDrag.m_button == kRMiddleButton)
            mouse_event = kRNewlyReleased;
        if(0 == (wparam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)))
            ReleaseCapture();
        break;

    case WM_RBUTTONUP:
        if(RMouseDrag.m_button == kRRightButton)
            mouse_event = kRNewlyReleased;
        if(0 == (wparam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)))
            ReleaseCapture();
        break;

    case WM_PAINT:
        if( D3D->m_pDD && !D3D->m_bReady )
        {
            if( D3D->m_pDeviceInfo->bWindowed )
                D3D_RenderD3D(rc);
            else
                D3D_FlipToGDISurface(D3D,TRUE);
        }
        break;

    case WM_MOVE:
        if( D3D->m_bActive && D3D->m_bReady && D3D->m_pDeviceInfo->bWindowed )
            if(!D3D->m_bIsFullscreen)
                SetRect( &D3D->m_rcScreenRect, (SHORT)LOWORD(lparam), (SHORT)HIWORD(lparam), (SHORT)LOWORD(lparam) + rc->D3D->m_dwRenderWidth, (SHORT)HIWORD(lparam) + rc->D3D->m_dwRenderHeight );
        break;

    case WM_SIZE:
        if( SIZE_MAXHIDE==wparam || SIZE_MINIMIZED==wparam )
            D3D->m_bActive = FALSE;
        else
            D3D->m_bActive = TRUE;

        if( D3D->m_bActive && D3D->m_bReady && D3D->m_pDeviceInfo->bWindowed && !D3D->m_bIsFullscreen)
        {
            D3D->m_bReady = FALSE;

            if( D3D_CleanUpD3D(D3D) != S_OK )
            {
                ME_REPORT(MeWarning(1,"Problem with CleanUpD3D in resize."));
                return 0;
            }
            if( D3D_setupD3D(D3D) != S_OK )
            {
                ME_REPORT(MeWarning(1,"Problem with setupD3D in resize."));
                return 0;
            }

            D3D->m_bReady = TRUE;
        }
        break;

    case WM_ENTERMENULOOP:
        D3D_Pause(D3D,TRUE);
        break;
    case WM_EXITMENULOOP:
        D3D_Pause(D3D,FALSE);
        break;

    case WM_ENTERSIZEMOVE:
        /* TODO stop render ticks... */
        break;

    case WM_CONTEXTMENU:
        if(RMouseDrag.m_allowContextMenu)
        {
            TrackPopupMenuEx(
                GetSubMenu( LoadMenu( D3D_DllHm, MAKEINTRESOURCE(IDR_POPUP) ), 0 ),
                TPM_VERTICAL, LOWORD(lparam), HIWORD(lparam), win, NULL );
        }
        break;

    case WM_NCHITTEST:
        if( !D3D->m_pDeviceInfo->bWindowed )
            return HTCLIENT;
        break;

    case WM_POWERBROADCAST:
        switch( wparam )
        {
        case PBT_APMQUERYSUSPEND:
            return D3D_OnQuerySuspend( (DWORD)lparam );

        case PBT_APMRESUMESUSPEND:
            return D3D_OnResumeSuspend( (DWORD)lparam );
        }
        break;

    case WM_SYSCOMMAND:
       /* Prevent moving/sizing and power loss in fullscreen mode */
        switch( wparam )
        {
        case SC_MOVE:
        case SC_SIZE:
        case SC_MAXIMIZE:
        case SC_MONITORPOWER:
            if( !D3D->m_pDeviceInfo->bWindowed )
                return 1;
            break;
        }
        break;


    case WM_GETMINMAXINFO:
        ((MINMAXINFO*)lparam)->ptMinTrackSize.x = 100;
        ((MINMAXINFO*)lparam)->ptMinTrackSize.y = 100;
        break;

    case WM_CLOSE:
        D3D->m_bQuit = TRUE;
        return 0;

    case WM_DESTROY:
        D3D->m_bQuit = TRUE;
        return 0;


    }

    /* Handle click, drag, release */
    if(mouse_event) {

        if(mouse_event == kRNewlyPressed) {
            /* Log info about this drag */
            RMouseDrag.m_meModifiers = 0;
            if( wparam & MK_SHIFT )
                RMouseDrag.m_meModifiers |= RSHIFT;
            if( wparam & MK_CONTROL )
                RMouseDrag.m_meModifiers |= RCONTROL;

            RMouseDrag.m_startx = GET_X_LPARAM(lparam);
            RMouseDrag.m_starty = GET_Y_LPARAM(lparam);
            RMouseDrag.m_camstartDist = rc->m_CameraDist;
            RMouseDrag.m_camstartTheta = rc->m_CameraTheta;
            RMouseDrag.m_camstartPhi = rc->m_CameraPhi;
            RMouseDrag.m_camstartX = (AcmeReal)RMouseDrag.m_startx;
            RMouseDrag.m_camstartY = (AcmeReal)RMouseDrag.m_starty;
            RCameraGetLookAt(rc, RMouseDrag.m_camstartLookAt);
        }

        /* If modifiers, call the callback */
        if(rc->m_pCallBacks->m_Mouse.m_CallBack && RMouseDrag.m_meModifiers && RMouseDrag.m_button)
            RExecuteMouseCallback(rc,
                GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), RMouseDrag.m_meModifiers,
                RMouseDrag.m_button, mouse_event);

        else if(mouse_event == kRStillPressed)
            switch(RMouseDrag.m_button) {
            case kRLeftButton:
                RCameraRotate(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                break;
            case kRMiddleButton:
                RCameraZoomDolly(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                break;
            case kRRightButton:
                RCameraPan(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
            }

        if(mouse_event == kRNewlyReleased) {
            RMouseDrag.m_modifiers = 0;
            RMouseDrag.m_button = 0;
        }
    } /* Mouse Event */

    return DefWindowProc( win, msg, wparam, lparam );
}


/*These are for suspend mode...*/
LRESULT D3D_OnQuerySuspend( DWORD dwFlags )
{
    return TRUE;
}

LRESULT D3D_OnResumeSuspend( DWORD dwData )
{
    return TRUE;
}


void D3D_RunApp(RRender *rc, RMainLoopCallBack func,void *userdata)
{
        D3DVars* D3D = rc->D3D;
   /* Load keyboard accelerators */
    HACCEL hAccel = LoadAccelerators( D3D_DllHm, MAKEINTRESOURCE( IDR_MAIN_ACCEL ) );

    BOOL bRcvdMsg;
    MSG msg;

    MeProfileTimerResult timerResult;

    D3D->callbackfunc = func;
    D3D->userdata = userdata;

    if ( D3D_LoadTextures(D3D) != D3D_OK )
        ME_REPORT(MeWarning(1,"Error loading textures."));

    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );


    MeProfileGetTimerValue(&timerResult);
    D3D_startframeclock = timerResult.cpuCycles;

    while( !D3D->m_bQuit )
    {

        if( D3D->m_bActive )    /* use peekmessage so can render in idle time */
            bRcvdMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
        else    /* block using GetMessage */
            bRcvdMsg = GetMessage( &msg, NULL, 0U, 0U );

        if( bRcvdMsg )
        {
            if( TranslateAccelerator( D3D->m_hWnd, hAccel, &msg ) == 0 )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            if( D3D->m_bActive && D3D->m_bReady )
            {
                if( FAILED( D3D_RenderD3D(rc) ) )
                {
                    D3D_CleanUpD3D(D3D);
                    DestroyWindow( D3D->m_hWnd );
                    break;
                }
                MeProfileGetTimerValue(&timerResult);
                D3D_startframeclock = timerResult.cpuCycles;
            }
        }
    }
   /* this is reached when quitting */
    if(rc->m_bProfiling)
         MeProfileOutputResults();
    MeProfileStopTiming();

    D3D_CleanUpD3D(D3D);
    DestroyWindow( D3D->m_hWnd );
    FreeLibrary(D3D_DllHm);
    return;
}



HRESULT D3D_initD3D(D3DVars* D3D)
{

    HRESULT hr;

   /* Enumerate available D3D devices. */
    __try {

        if( FAILED( hr = D3D_EnumerateDevices( D3D,D3D_DeviceAcceptable ) ) )
        {
            ME_REPORT(MeWarning(1,"Failed to enumerate devices."));
            return hr;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        MeInfo(0,"Couldn't load DirectX libraries");
        return ERROR_PROC_NOT_FOUND;
    }

    if( FAILED( hr = D3D_SelectDefaultDevice( D3D, 0L ) ) )
    {
        ME_REPORT(MeWarning(1,"Failed to select default device."));
        return hr;
    }

   /* initialise some variables (probably not necessary here...) */
    D3D->m_pDD = NULL;
    D3D->m_pD3D = NULL;
    D3D->m_pd3dDevice = NULL;

    return S_OK;
}


HRESULT D3D_EnumerateDevices( D3DVars* D3D,HRESULT(*ConfirmDevFn)(DDCAPS*, D3DDEVICEDESC7*) )
{
    D3D->ConfirmDevFn = ConfirmDevFn;
    D3D->m_dwNumDevicesEnumerated = 0L;
    D3D->m_dwNumDevices = 0L;

   /* enumerate devices (DDraw function), DriverEnumCallback is in this file */
    DirectDrawEnumerateEx( D3D_DriverEnumCallback, D3D,
        DDENUM_ATTACHEDSECONDARYDEVICES |
        DDENUM_DETACHEDSECONDARYDEVICES |
        DDENUM_NONDISPLAYDEVICES );

   /* Make sure devices were actually enumerated */
    if( D3D->m_dwNumDevicesEnumerated == 0 )
    {
        ME_REPORT(MeWarning(1,"Device enumeration failed."));
        return S_FALSE;
    }
    if( D3D->m_dwNumDevices == 0 )
    {
        ME_REPORT(MeWarning(1,"No acceptable D3D devices - try enabling the reference rasteriser."));
        return S_FALSE;
    }

    return S_OK;
}



BOOL WINAPI D3D_DriverEnumCallback( GUID* pGUID, TCHAR* strDesc, TCHAR* strName, VOID* vp, HMONITOR hm)
{
    D3DEnum_DeviceInfo d3dDeviceInfo;
    LPDIRECTDRAW7 lpDD;
    LPDIRECT3D7 lpD3D;
    HRESULT hr;
        D3DVars* D3D = (D3DVars*)vp;
    hr = DirectDrawCreateEx( pGUID, (void**)&lpDD, &IID_IDirectDraw7, NULL );
    if( FAILED(hr) )
        return D3DENUMRET_OK;

    hr = lpDD->lpVtbl->QueryInterface( lpDD, &IID_IDirect3D7, (void**)&lpD3D );
    if( FAILED(hr) )
    {
        lpDD->lpVtbl->Release(lpDD);
        return D3DENUMRET_OK;
    }


   /* Copy data to a device info structure */
    ZeroMemory( &d3dDeviceInfo, sizeof(d3dDeviceInfo) );
    lstrcpyn( d3dDeviceInfo.strDesc, strDesc, 39 );
    d3dDeviceInfo.ddDriverCaps.dwSize = sizeof(DDCAPS);
    d3dDeviceInfo.ddHELCaps.dwSize = sizeof(DDCAPS);
    lpDD->lpVtbl->GetCaps( lpDD, &d3dDeviceInfo.ddDriverCaps, &d3dDeviceInfo.ddHELCaps );
    if( pGUID )
    {
        d3dDeviceInfo.guidDriver = (*pGUID);
        d3dDeviceInfo.pDriverGUID = &d3dDeviceInfo.guidDriver;
    }

    strcpy( D3D->m_strDevicename, d3dDeviceInfo.strDesc);

    if( d3dDeviceInfo.ddDriverCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED )
        if( d3dDeviceInfo.pDriverGUID == NULL )
            d3dDeviceInfo.bDesktopCompatible = TRUE;

    lpDD->lpVtbl->EnumDisplayModes( lpDD, 0, NULL, &d3dDeviceInfo, D3D_ModeEnumCallback );
    qsort( d3dDeviceInfo.pddsdModes, d3dDeviceInfo.dwNumModes, sizeof(DDSURFACEDESC2), D3D_SortModesCallback );

        d3dDeviceInfo.D3D = D3D;
    lpD3D->lpVtbl->EnumDevices( lpD3D, D3D_DeviceEnumCallback, &d3dDeviceInfo );

    MeMemoryAPI.destroy( d3dDeviceInfo.pddsdModes );
    lpD3D->lpVtbl->Release(lpD3D);
    lpDD->lpVtbl->Release(lpDD);

    return DDENUMRET_OK;
}


int __cdecl D3D_SortModesCallback( const void* arg1, const void* arg2 )
{
   /* this is a callback for qsort */
    DDSURFACEDESC2* a = (DDSURFACEDESC2*)arg1;
    DDSURFACEDESC2* b = (DDSURFACEDESC2*)arg2;

    if( a->dwWidth < b->dwWidth )
        return -1;
    if( a->dwWidth > b->dwWidth )
        return +1;

    if( a->dwHeight < b->dwHeight )
        return -1;
    if( a->dwHeight > b->dwHeight )
        return +1;

    if( a->ddpfPixelFormat.dwRGBBitCount < b->ddpfPixelFormat.dwRGBBitCount )
        return -1;
    if( a->ddpfPixelFormat.dwRGBBitCount > b->ddpfPixelFormat.dwRGBBitCount )
        return +1;

    return 0;
}


HRESULT WINAPI D3D_ModeEnumCallback( DDSURFACEDESC2* pddsd, VOID* pParentInfo )
{
    D3DEnum_DeviceInfo *pDevice = (D3DEnum_DeviceInfo*)pParentInfo;

    DDSURFACEDESC2* pddsdNewModes = MeMemoryAPI.create( sizeof(DDSURFACEDESC2)*(pDevice->dwNumModes+1) );

    memcpy( pddsdNewModes, pDevice->pddsdModes, pDevice->dwNumModes * sizeof(DDSURFACEDESC2) );

    if (pDevice->pddsdModes)
        MeMemoryAPI.destroy( pDevice->pddsdModes );

    pDevice->pddsdModes = pddsdNewModes;

    pDevice->pddsdModes[pDevice->dwNumModes++] = (*pddsd);

    return DDENUMRET_OK;
}


HRESULT WINAPI D3D_DeviceEnumCallback( TCHAR* strDesc, TCHAR* strName, D3DDEVICEDESC7* pDesc, VOID* pParentInfo )
{
    DWORD i;
    D3DEnum_DeviceInfo *pDriverInfo = (D3DEnum_DeviceInfo*)pParentInfo;
        D3DVars* D3D = pDriverInfo->D3D;
    D3DEnum_DeviceInfo *pDeviceInfo = &D3D->m_pDeviceList[D3D->m_dwNumDevices];
    ZeroMemory( pDeviceInfo, sizeof(D3DEnum_DeviceInfo) );

    D3D->m_dwNumDevicesEnumerated++;

    pDeviceInfo->bHardware = pDesc->dwDevCaps & D3DDEVCAPS_HWRASTERIZATION;
    memcpy( &pDeviceInfo->ddDeviceDesc, pDesc, sizeof(D3DDEVICEDESC7) );

   /* Set up device info for this device */
    pDeviceInfo->bDesktopCompatible = pDriverInfo->bDesktopCompatible;
    pDeviceInfo->ddDriverCaps = pDriverInfo->ddDriverCaps;
    pDeviceInfo->ddHELCaps = pDriverInfo->ddHELCaps;
    pDeviceInfo->guidDevice = pDesc->deviceGUID;
    pDeviceInfo->pDeviceGUID = &pDeviceInfo->guidDevice;

    if( pDriverInfo->pDriverGUID )
    {
        pDeviceInfo->guidDriver = pDriverInfo->guidDriver;
        pDeviceInfo->pDriverGUID = &pDeviceInfo->guidDriver;
        lstrcpyn( pDeviceInfo->strDesc, pDriverInfo->strDesc, 39 );
    }
    else
    {
        pDeviceInfo->pDriverGUID = NULL;
        lstrcpyn( pDeviceInfo->strDesc, strName, 39 );
    }

   /* Avoid duplicates: only enum HW devices for secondary DDraw drivers. */
    if( pDeviceInfo->pDriverGUID != NULL && pDeviceInfo->bHardware == FALSE)
        return D3DENUMRET_OK;

   /* Give the app a chance to accept or reject this device. */
    if( D3D->ConfirmDevFn )
        if( FAILED( D3D->ConfirmDevFn( &pDeviceInfo->ddDriverCaps,
            &pDeviceInfo->ddDeviceDesc ) ) )
            return D3DENUMRET_OK;

    pDeviceInfo->pddsdModes = (DDSURFACEDESC2 *)MeMemoryAPI.create( sizeof(DDSURFACEDESC2) * pDriverInfo->dwNumModes );

    /* Build list of supported modes for the device */
    for( i=0; i<pDriverInfo->dwNumModes; i++ )
    {
        DDSURFACEDESC2 ddsdMode = pDriverInfo->pddsdModes[i];
        DWORD dwRenderDepths = pDeviceInfo->ddDeviceDesc.dwDeviceRenderBitDepth;
        DWORD dwDepth = ddsdMode.ddpfPixelFormat.dwRGBBitCount;

       /* Accept modes that are compatable with the device */
        if( ( ( dwDepth == 32 ) && ( dwRenderDepths & DDBD_32 ) ) ||
            ( ( dwDepth == 24 ) && ( dwRenderDepths & DDBD_24 ) ) ||
            ( ( dwDepth == 16 ) && ( dwRenderDepths & DDBD_16 ) ) )
        {
            pDeviceInfo->pddsdModes[pDeviceInfo->dwNumModes++] = ddsdMode;

            if( ddsdMode.ddsCaps.dwCaps2 & DDSCAPS2_STEREOSURFACELEFT )
                pDeviceInfo->bStereoCompatible = TRUE;
        }
    }

    if( pDeviceInfo->dwNumModes == 0 ) {
        MeMemoryAPI.destroy(pDeviceInfo->pddsdModes);
        return D3DENUMRET_OK;
    }

   /* Find a 640x480x16 mode for the default fullscreen mode */
    for( i=0; i<pDeviceInfo->dwNumModes; i++ )
    {
        if( ( pDeviceInfo->pddsdModes[i].dwWidth == 640 ) &&
            ( pDeviceInfo->pddsdModes[i].dwHeight == 480 ) &&
            ( pDeviceInfo->pddsdModes[i].ddpfPixelFormat.dwRGBBitCount == 16 ) )
        {
            pDeviceInfo->ddsdFullscreenMode = pDeviceInfo->pddsdModes[i];
            pDeviceInfo->dwCurrentMode = i;
        }
    }

    MeMemoryAPI.destroy(pDeviceInfo->pddsdModes);

   /* If possible make the device windowed */
    pDeviceInfo->bWindowed = pDeviceInfo->bDesktopCompatible;

   /* Accept the device and return */
    D3D->m_dwNumDevices++;

    return D3DENUMRET_OK;
}

HRESULT D3D_DeviceAcceptable( DDCAPS *pddCaps, D3DDEVICEDESC7 *pd3dDevDesc )
{
   /* accept or reject devices according to application requirements
       eg. if(pd3dDevDesc->wMaxSimultaneousTextures > 1) return S_OK; else return E_FAIL; */

    return S_OK;
}

/* Selects default device
 as specified by Microsoft D3D Framework */
HRESULT D3D_SelectDefaultDevice( D3DVars* D3D, DWORD dwFlags )
{
    DWORD i;
        D3DEnum_DeviceInfo** ppDevice = &D3D->m_pDeviceInfo;
    D3DEnum_DeviceInfo* pDeviceList;
    DWORD dwNumDevices;
        
    D3DEnum_DeviceInfo *pRefRastDevice = NULL;
    D3DEnum_DeviceInfo *pSoftwareDevice = NULL;
    D3DEnum_DeviceInfo *pHardwareDevice = NULL;
    D3DEnum_DeviceInfo *pHardwareTnLDevice = NULL;

    pDeviceList = D3D->m_pDeviceList;
    dwNumDevices = D3D->m_dwNumDevices;

    for( i=0; i<dwNumDevices; i++ )
    {
        if( pDeviceList[i].bDesktopCompatible )
        {
            if( pDeviceList[i].bHardware )
            {
                if( IsEqualIID( &(*pDeviceList[i].pDeviceGUID), &IID_IDirect3DTnLHalDevice ) )
                    pHardwareTnLDevice = &pDeviceList[i];
                else
                    pHardwareDevice = &pDeviceList[i];
            }
            else
            {
                if( IsEqualIID( &(*pDeviceList[i].pDeviceGUID), &IID_IDirect3DRefDevice ) )
                    pRefRastDevice = &pDeviceList[i];
                else
                    pSoftwareDevice = &pDeviceList[i];
            }
        }
    }

    if( 0 == ( dwFlags & D3DENUM_SOFTWAREONLY ) && pHardwareTnLDevice )
        (*ppDevice) = pHardwareTnLDevice;
    else if( 0 == ( dwFlags & D3DENUM_SOFTWAREONLY ) && pHardwareDevice )
        (*ppDevice) = pHardwareDevice;
    else if( pSoftwareDevice )
        (*ppDevice) = pSoftwareDevice;
    else if( pRefRastDevice )
        (*ppDevice) = pRefRastDevice;
    else
        return D3DENUMERR_NOCOMPATIBLEDEVICES;

   /* Set the windowed state of the newly selected device */
    (*ppDevice)->bWindowed = TRUE;

    return S_OK;
}

HRESULT D3D_setupD3D(D3DVars* D3D)
{
    static BOOL bOldWindowedState = TRUE;
    static DWORD dwSavedStyle;
    static RECT rcSaved;

    DWORD dwFlags;


   /* Check if going from fullscreen to windowed mode, or vice versa. */
    if( bOldWindowedState != D3D->m_pDeviceInfo->bWindowed )
    {
        if( D3D->m_pDeviceInfo->bWindowed )
        {
           /* Coming from fullscreen mode, so restore window properties */
            SetWindowLong( D3D->m_hWnd, GWL_STYLE, dwSavedStyle );
            SetWindowPos( D3D->m_hWnd, HWND_NOTOPMOST, rcSaved.left, rcSaved.top,
                ( rcSaved.right - rcSaved.left ),
                ( rcSaved.bottom - rcSaved.top ), SWP_SHOWWINDOW );
        }
        else
        {
           /* Going to fullscreen mode, save/set window properties as needed */
            dwSavedStyle = GetWindowLong( D3D->m_hWnd, GWL_STYLE );
            GetWindowRect( D3D->m_hWnd, &rcSaved );
            SetWindowLong( D3D->m_hWnd, GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE );
        }

        bOldWindowedState = D3D->m_pDeviceInfo->bWindowed;
    }
    D3D->m_bIsFullscreen = !D3D->m_pDeviceInfo->bWindowed;



    if( IsEqualIID( &(*D3D->m_pDeviceInfo->pDeviceGUID), &IID_IDirect3DHALDevice) )
        D3D->m_dwDeviceMemType = DDSCAPS_VIDEOMEMORY;
    else if( IsEqualIID( &(*D3D->m_pDeviceInfo->pDeviceGUID), &IID_IDirect3DTnLHalDevice) )
        D3D->m_dwDeviceMemType = DDSCAPS_VIDEOMEMORY;
    else
        D3D->m_dwDeviceMemType = DDSCAPS_SYSTEMMEMORY;

   /* Create the DDraw object */
    if( FAILED( DirectDrawCreateEx( D3D->m_pDeviceInfo->pDriverGUID, (void**)&D3D->m_pDD, &IID_IDirectDraw7, NULL ) ))
    {
        ME_REPORT(MeWarning(1,"Couldn't create directdraw object."));
        return S_FALSE;
    }

    dwFlags = DDSCL_NORMAL;
    if(D3D->m_bIsFullscreen)
        dwFlags = DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN;
    if(dwFlags & D3DFW_NO_FPUSETUP)
        dwFlags |= DDSCL_FPUSETUP;

    if( FAILED( D3D->m_pDD->lpVtbl->SetCooperativeLevel( D3D->m_pDD, D3D->m_hWnd, dwFlags ) ))
    {
        ME_REPORT(MeWarning(1,"Couldn't set cooperative level."));
        return S_FALSE;
    }

    if(D3D->m_bIsFullscreen)
    {
        if( D3D_CreateFullscreenSurfaces(D3D) != S_OK )
        {
            ME_REPORT(MeWarning(1,"Failed to create fullscreen surfaces."));
            return S_FALSE;
        }
    }
    else
    {
        if( D3D_CreateWindowSurfaces(D3D) != S_OK )
        {
            ME_REPORT(MeWarning(1,"Failed to create window surfaces."));
            return S_FALSE;
        }
    }


    if(FAILED( D3D->m_pDD->lpVtbl->QueryInterface( D3D->m_pDD, &IID_IDirect3D7, (void**)&D3D->m_pD3D ) ))
    {
        ME_REPORT(MeWarning(1,"Failed to get the D3D7 Interface."));
        return S_FALSE;
    }

    if(FAILED( D3D->m_pD3D->lpVtbl->CreateDevice( D3D->m_pD3D, D3D->m_pDeviceInfo->pDeviceGUID, D3D->m_pddsBackBuffer, &D3D->m_pd3dDevice ) ))
    {
        ME_REPORT(MeWarning(1,"Failed to get D3D Device."));
        return S_FALSE;
    }

    if( D3D_CreateZBuffer(D3D) != S_OK )
    {
        ME_REPORT(MeWarning(1,"Failed to create Z Buffer."));
        return S_FALSE;
    }


    D3D_SceneInit(D3D);

    if ( D3D_LoadTextures(D3D) != D3D_OK )
        ME_REPORT(MeWarning(1,"Error loading textures." ));

    return S_OK;
}

HRESULT D3D_CreateFullscreenSurfaces(D3DVars* D3D)
{
    DWORD dwFlags = 0;
    DDSCAPS2 scaps = {DDSCAPS_BACKBUFFER, 0, 0, 0};
    DDSURFACEDESC2 sdesc;

    SetRect( &D3D->m_rcScreenRect, 0, 0, D3D->m_pDeviceInfo->ddsdFullscreenMode.dwWidth, D3D->m_pDeviceInfo->ddsdFullscreenMode.dwHeight );
    D3D->m_dwRenderHeight = D3D->m_rcScreenRect.bottom - D3D->m_rcScreenRect.top;
    D3D->m_dwRenderWidth = D3D->m_rcScreenRect.right - D3D->m_rcScreenRect.left;

   /* watch out for mode x */
    if( (D3D->m_dwRenderWidth == 320) && (D3D->m_dwRenderHeight == 200)
        && (D3D->m_pDeviceInfo->ddsdFullscreenMode.ddpfPixelFormat.dwRGBBitCount == 8) )
        dwFlags |= DDSDM_STANDARDVGAMODE;

    if( FAILED( D3D->m_pDD->lpVtbl->SetDisplayMode( D3D->m_pDD, D3D->m_dwRenderWidth, D3D->m_dwRenderHeight,
        D3D->m_pDeviceInfo->ddsdFullscreenMode.ddpfPixelFormat.dwRGBBitCount,
        D3D->m_pDeviceInfo->ddsdFullscreenMode.dwRefreshRate,
        dwFlags ) ) )
    {
        ME_REPORT(MeWarning(1,"Failed to set fullscreen display mode."));
        return S_FALSE;
    }

    ZeroMemory( &sdesc, sizeof(sdesc) );
    sdesc.dwSize = sizeof(sdesc);
    sdesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    sdesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    sdesc.dwBackBufferCount = 1;

    if( FAILED( D3D->m_pDD->lpVtbl->CreateSurface( D3D->m_pDD, &sdesc, &D3D->m_pddsFrontBuffer, NULL ) ))
    {
        ME_REPORT(MeWarning(1,"Failed to create primary surface."));
        return S_FALSE;
    }

    if(FAILED( D3D->m_pddsFrontBuffer->lpVtbl->GetAttachedSurface( D3D->m_pddsFrontBuffer, &scaps, &D3D->m_pddsBackBuffer ) ))
    {
        ME_REPORT(MeWarning(1,"Failed to get back buffer."));
        return S_FALSE;
    }

    D3D->m_pddsBackBuffer->lpVtbl->AddRef(D3D->m_pddsBackBuffer);

    ZeroMemory( &D3D->m_ddpfBackBufferPixelFormat, sizeof(DDPIXELFORMAT) );
    D3D->m_ddpfBackBufferPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    D3D->m_pddsBackBuffer->lpVtbl->GetPixelFormat(D3D->m_pddsBackBuffer, &D3D->m_ddpfBackBufferPixelFormat);
    D3D->m_pddsBackBuffer->lpVtbl->GetSurfaceDesc(D3D->m_pddsBackBuffer, &sdesc);

    ZeroMemory( &D3D->m_ddpfBackBufferPixelFormat, sizeof(DDPIXELFORMAT) );
    D3D->m_ddpfBackBufferPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    D3D->m_pddsBackBuffer->lpVtbl->GetPixelFormat(D3D->m_pddsBackBuffer, &D3D->m_ddpfBackBufferPixelFormat);
    D3D->m_pddsBackBuffer->lpVtbl->GetSurfaceDesc(D3D->m_pddsBackBuffer, &sdesc);

    return S_OK;
}

HRESULT D3D_CreateWindowSurfaces(D3DVars* D3D)
{
    DDSURFACEDESC2 sdesc;
    LPDIRECTDRAWCLIPPER pddclip;

    GetClientRect( D3D->m_hWnd, &D3D->m_rcScreenRect );
    ClientToScreen( D3D->m_hWnd, (POINT *)&D3D->m_rcScreenRect.left );
    ClientToScreen( D3D->m_hWnd, (POINT *)&D3D->m_rcScreenRect.right );

    D3D->m_dwRenderHeight = D3D->m_rcScreenRect.bottom - D3D->m_rcScreenRect.top;
    D3D->m_dwRenderWidth = D3D->m_rcScreenRect.right - D3D->m_rcScreenRect.left;

    ZeroMemory( &sdesc, sizeof(sdesc) );
    sdesc.dwSize = sizeof(sdesc);
    sdesc.dwFlags = DDSD_CAPS;
    sdesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if(FAILED( D3D->m_pDD->lpVtbl->CreateSurface( D3D->m_pDD, &sdesc, &D3D->m_pddsFrontBuffer, NULL ) ))
    {
        ME_REPORT(MeWarning(1,"Failed to create primary surface."));
        return S_FALSE;
    }

    if(FAILED( D3D->m_pDD->lpVtbl->CreateClipper( D3D->m_pDD, 0, &pddclip, NULL ) ))
    {
        ME_REPORT(MeWarning(1,"Failed to create DDraw Clipper." ));
        return S_FALSE;
    }

    pddclip->lpVtbl->SetHWnd( pddclip, 0, D3D->m_hWnd );
    D3D->m_pddsFrontBuffer->lpVtbl->SetClipper( D3D->m_pddsFrontBuffer, pddclip );
    pddclip->lpVtbl->Release(pddclip);

    sdesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    sdesc.dwWidth = D3D->m_dwRenderWidth;
    sdesc.dwHeight = D3D->m_dwRenderHeight;
    sdesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

    if(FAILED( D3D->m_pDD->lpVtbl->CreateSurface( D3D->m_pDD, &sdesc, &D3D->m_pddsBackBuffer, NULL ) ))
    {
        ME_REPORT(MeWarning(1,"Failed to create backbuffer."));
        return S_FALSE;
    }

    ZeroMemory( &D3D->m_ddpfBackBufferPixelFormat, sizeof(DDPIXELFORMAT) );
    D3D->m_ddpfBackBufferPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    D3D->m_pddsBackBuffer->lpVtbl->GetPixelFormat(D3D->m_pddsBackBuffer, &D3D->m_ddpfBackBufferPixelFormat);
    D3D->m_pddsBackBuffer->lpVtbl->GetSurfaceDesc(D3D->m_pddsBackBuffer, &sdesc);

    return S_OK;
}

HRESULT D3D_doFlip(D3DVars* D3D)
{
    if( D3D->m_pddsFrontBuffer == NULL )
    {
        ME_REPORT(MeWarning(1,"Front buffer is not initialised. Cannot flip/blit."));
        return S_FALSE;
    }

    if( D3D->m_bIsFullscreen )
    {
        if( D3D->m_bNoVSync )
            return D3D->m_pddsFrontBuffer->lpVtbl->Flip( D3D->m_pddsFrontBuffer, NULL, DDFLIP_WAIT | DDFLIP_NOVSYNC );
        else
            return D3D->m_pddsFrontBuffer->lpVtbl->Flip( D3D->m_pddsFrontBuffer, NULL, DDFLIP_WAIT );
    }
    else
    {
        return D3D->m_pddsFrontBuffer->lpVtbl->Blt( D3D->m_pddsFrontBuffer, &D3D->m_rcScreenRect, D3D->m_pddsBackBuffer, NULL, DDBLT_WAIT, NULL );
    }
}

HRESULT D3D_CleanUpD3D(D3DVars* D3D)
{
    int i;

    D3D_SceneEnd(D3D);

    if( D3D->m_pDD )
        D3D->m_pDD->lpVtbl->SetCooperativeLevel( D3D->m_pDD, D3D->m_hWnd, DDSCL_NORMAL );

    if( D3D->m_pd3dDevice )
    {
        if( D3D->m_pd3dDevice->lpVtbl->Release(D3D->m_pd3dDevice) > 0 )
            ME_REPORT(MeWarning(1,"D3D Device still referenced."));
    }
    D3D->m_pd3dDevice = NULL;

    for( i = 0; i < 25; i++ )
    {
        if( D3D->m_pddsTexture[i] )
        {
            D3D->m_pddsTexture[i]->lpVtbl->Release(D3D->m_pddsTexture[i]);
            D3D->m_pddsTexture[i] = NULL;
        }
    }
    if( D3D->m_pddsBackBuffer )
    {
        D3D->m_pddsBackBuffer->lpVtbl->Release(D3D->m_pddsBackBuffer);
        D3D->m_pddsBackBuffer = NULL;
    }
    if( D3D->m_pddsFrontBuffer )
    {
        D3D->m_pddsFrontBuffer->lpVtbl->Release(D3D->m_pddsFrontBuffer);
        D3D->m_pddsFrontBuffer = NULL;
    }
    if( D3D->m_pddsZBuffer )
    {
        D3D->m_pddsZBuffer->lpVtbl->Release(D3D->m_pddsZBuffer);
        D3D->m_pddsZBuffer = NULL;
    }
    if( D3D->m_pD3D )
    {
        D3D->m_pD3D->lpVtbl->Release(D3D->m_pD3D);
        D3D->m_pD3D = NULL;
    }

    if( D3D->m_pDD )
    {
        if( D3D->m_pDD->lpVtbl->Release(D3D->m_pDD) > 0 )
            ME_REPORT(MeWarning(1,"DDraw object still referenced."));
    }
    D3D->m_pDD = NULL;
    return S_OK;
}

HRESULT D3D_CreateZBuffer(D3DVars* D3D)
{
   /* create and attach Z buffer */
    D3DDEVICEDESC7 devdesc;
    DDSURFACEDESC2 sdesc;

    D3D->m_pd3dDevice->lpVtbl->GetCaps( D3D->m_pd3dDevice, &devdesc );
    if( devdesc.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ZBUFFERLESSHSR )
        return S_OK;    /* don't need a z-buffer */

    sdesc.dwSize = sizeof(sdesc);
    D3D->m_pddsBackBuffer->lpVtbl->GetSurfaceDesc( D3D->m_pddsBackBuffer, &sdesc );
    sdesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
    sdesc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | D3D->m_dwDeviceMemType;

    sdesc.ddpfPixelFormat.dwSize = 0;
    D3D->m_pD3D->lpVtbl->EnumZBufferFormats( D3D->m_pD3D, D3D->m_pDeviceInfo->pDeviceGUID, D3D_EnumZCallback, (void*)&sdesc.ddpfPixelFormat );
    if( sdesc.ddpfPixelFormat.dwSize == 0 )
    {/* accept any 16bit z-buffer */
        sdesc.ddpfPixelFormat.dwRGBBitCount = 16;
        D3D->m_pD3D->lpVtbl->EnumZBufferFormats( D3D->m_pD3D, D3D->m_pDeviceInfo->pDeviceGUID, D3D_EnumZCallback, (void*)&sdesc.ddpfPixelFormat );
        if( sdesc.ddpfPixelFormat.dwSize == 0 )
        {
            ME_REPORT(MeWarning(1,"Device doesn't support requested (16bit) z-buffer format."));
            return S_FALSE;
        }
    }

    if(FAILED( D3D->m_pDD->lpVtbl->CreateSurface( D3D->m_pDD, &sdesc, &D3D->m_pddsZBuffer, NULL ) ))
    {
        ME_REPORT(MeWarning(1,"Couldn't create Z-buffer surface."));
        return S_FALSE;
    }

    if(FAILED( D3D->m_pddsBackBuffer->lpVtbl->AddAttachedSurface( D3D->m_pddsBackBuffer, D3D->m_pddsZBuffer ) ))
    {
        ME_REPORT(MeWarning(1,"Couldn't attach Z-buffer." ));
        return S_FALSE;
    }

    if(FAILED( D3D->m_pd3dDevice->lpVtbl->SetRenderTarget( D3D->m_pd3dDevice, D3D->m_pddsBackBuffer, 0L ) ))
    {
        ME_REPORT(MeWarning(1,"SetRenderTarget failed after Z-buffer attached."));
        return S_FALSE;
    }

    return S_OK;
}

HRESULT WINAPI D3D_EnumZCallback( DDPIXELFORMAT* pf, void* pcon )
{
    DDPIXELFORMAT *pft = (DDPIXELFORMAT *)pcon;
    if( pft->dwRGBBitCount == pf->dwRGBBitCount )
    {
        (*pft)=(*pf);
        return D3DENUMRET_CANCEL;
    }
    return D3DENUMRET_OK;
}

HRESULT D3D_FlipToGDISurface( D3DVars* D3D,BOOL bDrawFrame )
{
    if( D3D->m_pDD && D3D->m_bIsFullscreen )
    {
        D3D->m_pDD->lpVtbl->FlipToGDISurface(D3D->m_pDD);

        if( bDrawFrame )
        {
            DrawMenuBar( D3D->m_hWnd );
            RedrawWindow( D3D->m_hWnd, NULL, NULL, RDW_FRAME );
        }
    }

    return S_OK;
}


void D3D_Pause( D3DVars* D3D,BOOL bPause )
{
    static DWORD dwPausedCount = 0L;

    dwPausedCount += ( bPause ? +1 : -1 );
    D3D->m_bReady = ( dwPausedCount ? FALSE : TRUE );

    if( bPause && ( dwPausedCount == 1) )
    {
        D3D_FlipToGDISurface( D3D,TRUE );
       /* stop rendering... */
       /* D3D->rc->m_bPause = 1; */
       /* This doesn't need to do anything now... */
    }

    if( dwPausedCount == 0 )
    {
       /* start rendering */
       /* D3D->rc->m_bPause = 0; */
       /* This doesn't need to do anything now... */
    }
}


HRESULT D3D_doDevDlg(D3DVars* D3D)
{
        if( DialogBoxParam( D3D_DllHm, MAKEINTRESOURCE(IDD_DEVDLG), GetForegroundWindow(), D3D_DevDlgMsgProc, (LPARAM)D3D ) == IDOK )
        return S_OK;

    return E_FAIL;
}

BOOL CALLBACK D3D_DevDlgMsgProc( HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam )
{
        static D3DVars* D3D;
    static D3DEnum_DeviceInfo** ppDeviceArg;

    static D3DEnum_DeviceInfo* pCurrentDevice;
    static DWORD dwCurrentMode;
    static BOOL bCurrentWindowed;

   /* Create local versions of D3D things */
    static D3DEnum_DeviceInfo* pDeviceList = 0;
    static DWORD dwNumDevices = 0;

    if( msg == WM_INITDIALOG )
    {
                D3D = (D3DVars*)lparam;
                pDeviceList = D3D->m_pDeviceList;
                dwNumDevices = D3D->m_dwNumDevices;
                ppDeviceArg = &D3D->m_pDeviceInfo;
        if( ppDeviceArg == NULL )
            return FALSE;

        pCurrentDevice = (*ppDeviceArg);
        dwCurrentMode = pCurrentDevice->dwCurrentMode;
        bCurrentWindowed = pCurrentDevice->bWindowed;

        D3D_UpdateDialog( hDlg, D3D, dwCurrentMode, bCurrentWindowed );

        return FALSE;
    }
    else if( msg == WM_COMMAND )
    {
        HWND hwndDevice = GetDlgItem( hDlg, IDC_DEVICE_COMBO );
        HWND hwndMode = GetDlgItem( hDlg, IDC_MODE_COMBO );
        HWND hwndWindowed = GetDlgItem( hDlg, IDC_WINDOWED_CHECKBOX );

        DWORD dwDevice = ComboBox_GetCurSel( hwndDevice );
        DWORD dwModeItem = ComboBox_GetCurSel( hwndMode );
        DWORD dwMode = ComboBox_GetItemData( hwndMode, dwModeItem );
        BOOL bWindowed = hwndWindowed ? Button_GetCheck( hwndWindowed ) : 0;

        D3DEnum_DeviceInfo* pDevice = &pDeviceList[dwDevice];

        if( LOWORD(wparam) == IDOK )
        {
            if( pDevice != pCurrentDevice || dwMode != dwCurrentMode || bWindowed != bCurrentWindowed )
            {    /* Something has changed */
                (*ppDeviceArg) = pDevice;
                //pDevice->bWindowed = bWindowed;
                pDevice->dwCurrentMode = dwMode;
                pDevice->ddsdFullscreenMode = pDevice->pddsdModes[dwMode];
                EndDialog( hDlg, IDOK );
            }
            else
                EndDialog( hDlg, IDCANCEL );

            return TRUE;
        }
        else if( LOWORD(wparam) == IDCANCEL )
        {
            EndDialog( hDlg, IDCANCEL );
            return TRUE;
        }
        else if( HIWORD(wparam) == CBN_EDITUPDATE )
        {
            if( LOWORD(wparam) == IDC_DEVICE_COMBO )
            {
                dwMode = pDeviceList[dwDevice].dwCurrentMode;
                bWindowed = pDeviceList[dwDevice].bWindowed;
            }
        }
    }
    return FALSE;
}

/* this is largely from MS D3D Framework */
VOID D3D_UpdateDialog( HWND hDlg, D3DVars* D3D, DWORD dwCurrentMode, BOOL bWindowed )
{
        D3DEnum_DeviceInfo* pCurrentDevice = D3D->m_pDeviceInfo;
    D3DEnum_DeviceInfo* pDeviceList;
    DWORD dwNumDevices;
    DWORD device;
    DWORD dwItem;
    DWORD mode;

    HWND hwndDevice = GetDlgItem( hDlg, IDC_DEVICE_COMBO );
    HWND hwndMode = GetDlgItem( hDlg, IDC_MODE_COMBO );
    HWND hwndWindowed = GetDlgItem( hDlg, IDC_WINDOWED_CHECKBOX );
    HWND hwndFullscreenText = GetDlgItem( hDlg, IDC_FULLSCREEN_TEXT );

    pDeviceList = D3D->m_pDeviceList;
    dwNumDevices = D3D->m_dwNumDevices;

    ComboBox_ResetContent( hwndDevice );
    ComboBox_ResetContent( hwndMode );

    if( pCurrentDevice->bDesktopCompatible == FALSE )
        bWindowed = FALSE;

    for( device = 0; device < dwNumDevices; device++ )
    {
        D3DEnum_DeviceInfo* pDevice = &pDeviceList[device];

        dwItem = ComboBox_AddString( hwndDevice, pDevice->strDesc );

        if( pDevice == pCurrentDevice )
        {
            ComboBox_SetCurSel( hwndDevice, dwItem );

            if( hwndWindowed )
            {
                EnableWindow( hwndWindowed, pDevice->bDesktopCompatible );
                Button_SetCheck( hwndWindowed, bWindowed );
            }

            EnableWindow( hwndMode, !bWindowed );
            EnableWindow( hwndFullscreenText, !bWindowed );

            for( mode = 0; mode < pDevice->dwNumModes; mode++ )
            {
                DDSURFACEDESC2* pddsdMode = &pDevice->pddsdModes[mode];

                TCHAR strMode[80];
                wsprintf( strMode, "%ld x %ld x %ld",
                    pddsdMode->dwWidth, pddsdMode->dwHeight,
                    pddsdMode->ddpfPixelFormat.dwRGBBitCount );

                dwItem = ComboBox_AddString( hwndMode, strMode );
                ComboBox_SetItemData( hwndMode, dwItem, mode );

                if( mode == dwCurrentMode )
                    ComboBox_SetCurSel( hwndMode, dwItem );

            }
        }
    }
}

HRESULT D3D_LoadTextures(D3DVars* D3D)
{
   /* Walk through D3D->rc->m_TextureList and create surfaces as appropriate */
    int i;
    int err = 0;
    DDCOLORKEY ck;
    ck.dwColorSpaceHighValue = 0x00000000;
    ck.dwColorSpaceLowValue = 0x00000000;

    for( i = 0; i < 25 ; i++ )
    {
        if( D3D->rc->m_TextureList[i] )
        {
            HINSTANCE hInst = GetModuleHandle(NULL);
            HBITMAP hbm = NULL;
            char count = 0;

            /* This works only in 32bit color mode,
               so using windows stuff instead... */
            //if(D3D->m_ddpfBackBufferPixelFormat.dwRGBBitCount == 32)
            if(0)                    
            {
                RImage ri = {0};

                RBmpLoad(D3D->rc, D3D->rc->m_TextureList[i], &ri, 1);

                                if (ri.m_pImage) {
                                        hbm = CreateBitmap(ri.m_width, ri.m_height, 4, 8, ri.m_pImage);
                                        free( ri.m_pImage);
                                }
            }
            else
            {
                char *bmpfilename = (char *)MeMemoryAPI.create(strlen(D3D->rc->m_TextureList[i])+50);

#if 0
                MeWarning(0, "D3D_LoadTextures: Alpha textures incorrect in \n"
                    "non-32bit color-depth. Adjust your Windows settings.");
#endif
                bmpfilename[0] = '\0';
                strcat(bmpfilename, D3D->rc->m_TextureList[i]);
                strcat(bmpfilename, ".bmp");
                hbm = LoadImage(hInst, bmpfilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

                /* try other locations if necessary */
                while( hbm == NULL && MeGetDefaultFileLocation(count) != NULL )
                {
                    bmpfilename[0] = '\0';
                    strcat(bmpfilename, MeGetDefaultFileLocation(count++));
                    strcat(bmpfilename, D3D->rc->m_TextureList[i]);
                    strcat(bmpfilename, ".bmp");
                    hbm = LoadImage(hInst, bmpfilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
                }
                                MeMemoryAPI.destroy(bmpfilename);
            }

            if( hbm != NULL )
            {
                D3D->m_pddsTexture[i] = D3D_CreateTextureFromBitmap(D3D,hbm, 1);
                if ( D3D->m_pddsTexture[i] == NULL )
                {
                    ME_REPORT(MeWarning(1,"Couldn't create texture surface for %s.", D3D->rc->m_TextureList[i] ));
                    err = 1;
                }
                else
                    D3D->m_pddsTexture[i]->lpVtbl->SetColorKey(D3D->m_pddsTexture[i], DDCKEY_SRCBLT, &ck);
                if ( !DeleteObject(hbm) )
                {
                    ME_REPORT(MeWarning(1,"Couldn't free HBITMAP."));
                    err = 1;
                }
            }
            else
            {
                ME_REPORT(MeWarning(1,"Couldn't create bitmap: %s", D3D->rc->m_TextureList[i] ));
                err = 1;
            }
        }
    }
    if( err )
        return E_FAIL;
    return D3D_OK;
}

HRESULT CALLBACK D3D_TextureSearchCallBack( DDPIXELFORMAT* pddpf, VOID* param )
{

    DDPIXELFORMAT *pf = (DDPIXELFORMAT*)param;

    if( pddpf->dwRGBBitCount == pf->dwRGBBitCount )
    {
        memcpy( pf, pddpf, sizeof(DDPIXELFORMAT) );
        return DDENUMRET_CANCEL;
    }
    return DDENUMRET_OK;
}



LPDIRECTDRAWSURFACE7 D3D_CreateTextureFromBitmap( D3DVars* D3D,HBITMAP hbm, int bUseColorKey )
{
    LPDIRECTDRAWSURFACE7 pddsTexture;
    D3DDEVICEDESC7 d3ddevdesc;
    DDSURFACEDESC2 ddsd;
    BITMAP bm;
    DWORD dwWidth, dwHeight;
    HDC hdcBitmap;
    HDC hdcTexture;
    LPDIRECTDRAW7 pDD;

    if( FAILED( D3D->m_pd3dDevice->lpVtbl->GetCaps( D3D->m_pd3dDevice, &d3ddevdesc ) ) )
        return NULL;

   /* Get bitmap structure */
    GetObject(hbm, sizeof(BITMAP), &bm);
    dwWidth = (DWORD)bm.bmWidth;
    dwHeight = (DWORD)bm.bmHeight;

   /* Create surface description */
    ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
    ddsd.dwWidth = dwWidth;
    ddsd.dwHeight = dwHeight;

    if( bUseColorKey )
        ddsd.dwFlags |= DDSD_CKSRCBLT;

   /* Turn on texture management for hardware devices */
    if( IsEqualIID( &d3ddevdesc.deviceGUID, &IID_IDirect3DTnLHalDevice ) )
        ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    else if( IsEqualIID( &d3ddevdesc.deviceGUID, &IID_IDirect3DHALDevice ) )
        ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    else
        ddsd.ddsCaps.dwCaps2 |= DDSCAPS_SYSTEMMEMORY;

   /* If driver requires it, adjust width and height */
    if( d3ddevdesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2 )
    {
        for( ddsd.dwWidth=1; dwWidth > ddsd.dwWidth; ddsd.dwWidth <<= 1 );
        for( ddsd.dwHeight=1; dwHeight > ddsd.dwHeight; ddsd.dwHeight <<= 1 );
    }
    if( d3ddevdesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
    {
        if( ddsd.dwWidth > ddsd.dwHeight )
            ddsd.dwHeight = ddsd.dwWidth;
        else
            ddsd.dwWidth = ddsd.dwHeight;
    }

   /* Let's ask for 16bit texture mode*/
    ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
    D3D->m_pd3dDevice->lpVtbl->EnumTextureFormats( D3D->m_pd3dDevice, D3D_TextureSearchCallBack, &ddsd.ddpfPixelFormat );
    if( ddsd.ddpfPixelFormat.dwRGBBitCount == 0L )
    {
        ME_REPORT(MeWarning(1,"Failed to find suitable Texture Format."));
        return NULL;
    }

    D3D->m_pddsBackBuffer->lpVtbl->GetDDInterface( D3D->m_pddsBackBuffer, (VOID**)&pDD );

    if( FAILED( D3D->m_pDD->lpVtbl->CreateSurface( pDD, &ddsd, &pddsTexture, NULL ) ) )
    {
        ME_REPORT(MeWarning(1,"Couldn't create surface."));
        pDD->lpVtbl->Release(pDD);
        return NULL;
    }

    pDD->lpVtbl->Release(pDD);

   /* Now we get DCs and copy the bitmap to the surface */
    hdcBitmap = CreateCompatibleDC(NULL);
    if( hdcBitmap == NULL )
    {
        ME_REPORT(MeWarning(1,"Couldn't create hdcBitmap."));
        pddsTexture->lpVtbl->Release(pddsTexture);
        return NULL;
    }
    SelectObject(hdcBitmap, hbm);

    if( SUCCEEDED( pddsTexture->lpVtbl->GetDC( pddsTexture, &hdcTexture ) ) )
    {
        BitBlt( hdcTexture, 0, 0, bm.bmWidth, bm.bmHeight, hdcBitmap, 0, 0, SRCCOPY );
        pddsTexture->lpVtbl->ReleaseDC( pddsTexture, hdcTexture );
    }
    else
        ME_REPORT(MeWarning(1,"Couldn't get hdcTexture."));

    DeleteDC(hdcBitmap);

    return pddsTexture;
}

void D3D_DisplayConsoleHelp()
{
    MeInfo(0, "------------------MeViewer (Direct3D)---------------------");
    RDisplayBanner();
    MeInfo(0, "Key Controls:");
    MeInfo(0, "\tF1\t\t- Toggles application specific help");
    MeInfo(0, "\tF2\t\t- Change display settings");
    MeInfo(0, "\tShift F2\t- Toggle WireFrame");
    MeInfo(0, "\tCtrl F2\t\t- Toggle Texturing");
    MeInfo(0, "\tF3\t\t- Toggle 60Hz frame rate lock");
    MeInfo(0, "\tCtrl F3\t\t- Toggle fps display");
    MeInfo(0, "\tF8\t\t- Single step");
    MeInfo(0, "\tCtrl F8\t\t- Toggle pause");
    MeInfo(0, "\tF9\t\t- Toggle Fullscreen mode (see also F2)");
    MeInfo(0, "\tCtrl F9\t\t- Recalibrate timer");
    MeInfo(0, "\tAlt F9\t\t- Toggle 640:448 Aspect Ratio lock");
    MeInfo(0, "\tF12\t\t- Toggle right button functionality");
    MeInfo(0, "\tESC\t\t- Quit");
    MeInfo(0, "");
    MeInfo(0, "Mouse Controls:");
    MeInfo(0, "\tLeft drag\t- Rotate Camera");
    MeInfo(0, "\tMiddle drag\t- Dolly (X), Zoom (Y)");
    MeInfo(0, "\tRight drag\t- Pan, or Display D3D menu (see F12)");
    MeInfo(0, "\tShift + drag\t- Mouse forces (when available)");
    RDisplayCommandLineHelp();
    MeInfo(0, "----------------------------------------------------------");
    MeInfo(0, "");
}

#endif
