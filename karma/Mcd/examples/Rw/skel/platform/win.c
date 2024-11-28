/**********************************************************************
 *
 * File : main.c
 *
 * Abstract : Platform specific application entry point
 *
 **********************************************************************
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd. or
 * Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1999 Criterion Software Ltd.
 * All Rights Reserved.
 *
 * RenderWare is a trademark of Canon Inc.
 *
 **********************************************************************/

/*--- Header files ---*/

#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>

#include <rwcore.h>
#include "resource.h"
#include "platform.h"
#include "skeleton.h"
#include "mouse.h"
#include "vecfont.h"

#define MAX_SUBSYSTEMS (16)

#if (!defined(_MAX_PATH))
/*
 * See 
 * Program Files/Microsoft Visual Studio/VC98/Include/STDLIB.H
 */
#ifndef _MAC
#define _MAX_PATH   260 /* max. length of full pathname */
#else   /* def _MAC */
#define _MAX_PATH   256 /* max. length of full pathname */
#endif  /* _MAC */
#endif /* (!defined(_MAX_PATH)) */


static RwBool       RwInitialised = FALSE;
static RwSubSystemInfo GsubSysInfo[MAX_SUBSYSTEMS];
static RwInt32      GnumSubSystems = 0;
static RwInt32      GcurSel = 0, GcurSelVM = 0;

/*--- Constants ---*/

/* Class name for the MS Window's window class. */

static const RwChar *AppClassName = RWSTRING("RwAppClass");

/* Error and warning dialog box titles */

static const RwChar *ErrorDialogTitle =
RWSTRING("RenderWare(tm) Application Error");
static const RwChar *WarningDialogTitle =
RWSTRING("RenderWare(tm) Application Warning");

/* Scan code translation tables */

static const RsKeyCodes KeyTableEnglish[256] = { /* ENGLISH */
    rsNULL, rsESC, '1', '2', '3', '4', '5', '6', /* 0 */
    '7', '8', '9', '0', '-', '=', rsBACKSP, rsTAB, /* 8 */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', /* 16 */
    'o', 'p', '[', ']', rsENTER, rsLCTRL, 'a', 's', /* 24 */
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 32 */
    '\'', '`', rsLSHIFT, '#', 'z', 'x', 'c', 'v', /* 40 */
    'b', 'n', 'm', ',', '.', '/', rsRSHIFT, rsTIMES, /* 48 */
    rsLALT, ' ', rsCAPSLK, rsF1, rsF2, rsF3, rsF4, rsF5, /* 56 */
    rsF6, rsF7, rsF8, rsF9, rsF10, rsNUMLOCK, rsNULL, rsHOME, /* 64 */
    rsPADUP, rsPADPGUP, rsMINUS, rsPADLEFT, rsPAD5, rsPADRIGHT, rsPLUS, rsPADEND, /* 72 */
    rsPADDOWN, rsPADPGDN, rsPADINS, rsPADDEL, rsNULL, rsNULL, '\\', rsF11, /* 80 */
    rsF12, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 88 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 96 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 104 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 112 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 120 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 128 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 136 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 144 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsPADENTER, rsRCTRL, rsNULL, rsNULL, /* 152 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 160 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 168 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsDIVIDE, rsNULL, rsNULL, /* 176 */
    rsRALT, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 184 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNUMLOCK, rsNULL, rsHOME, /* 192 */
    rsUP, rsPGUP, rsNULL, rsLEFT, rsNULL, rsRIGHT, rsNULL, rsEND, /* 200 */
    rsDOWN, rsPGDN, rsINS, rsDEL, rsNULL, rsNULL, rsNULL, rsNULL, /* 208 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 216 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 224 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 232 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, /* 240 */
    rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL, rsNULL /* 248 */
};

/*--- Local Structure Definitions ---*/

/* platform specfic global data */

typedef struct
{
    HWND                window;
    HINSTANCE           instance;
    RwBool              fullScreen;
    RwV2d               lastMousePos;
}
psGlobalType;

static psGlobalType PsGlobal;

/*--- Macro Definitions ---*/

#define PSGLOBAL(var) (((psGlobalType *)(RsGlobal.ps))->var)

#ifdef UNDER_CE
#ifndef MAKEPOINTS
#define MAKEPOINTS(l) (*((POINTS FAR *) & (l)))
#endif
#endif

/*--- Functions ---*/

/*
 * Platform Specific Functionality
 */

void
psWindowSetText(const RwChar * text)
{
    SetWindowText(PSGLOBAL(window), text);
}

void
psErrorMessage(const RwChar * message)
{
    MessageBox(NULL, message, ErrorDialogTitle,
#ifndef UNDER_CE
               MB_TASKMODAL |
#endif
               MB_ICONERROR | MB_TOPMOST | MB_OK);
}

void
psWarningMessage(const RwChar * message)
{
    MessageBox(NULL, message, WarningDialogTitle,
#ifndef UNDER_CE
               MB_TASKMODAL |
#endif
               MB_ICONWARNING | MB_TOPMOST | MB_OK);
}

void
psCameraShowRaster(RwCamera * camera)
{
    RwCameraShowRaster(camera, PSGLOBAL(window), 0);
}

RwUInt32
psTimer(void)
{
    RwUInt32 time;

#ifdef UNDER_CE
    time = (RwUInt32)GetTickCount();
#else
    timeBeginPeriod(1);
    time = (RwUInt32)timeGetTime();
    timeEndPeriod(1);
#endif

    return (time);
}

void
psMouseSetVisibility(RwBool visible)
{
    ShowCursor(visible);
}

static RwBool       validDelta = FALSE;

void
psMouseSetPos(RwV2d * pos)
{
    POINT               point;

    validDelta = FALSE;
    point.x = (RwInt32) pos->x;
    point.y = (RwInt32) pos->y;
    ClientToScreen(PSGLOBAL(window), &point);
    SetCursorPos(point.x, point.y);
}

RwChar             *
psPathnameCreate(const RwChar * srcBuffer)
{
    RwChar             *dstBuffer;
    RwChar             *charToConvert;

    /* 
     * First duplicate the string 
     */
    dstBuffer = RwMalloc(sizeof(RwChar) * (rwstrlen(srcBuffer) + 1));

    if (dstBuffer)
    {
        rwstrcpy(dstBuffer, srcBuffer);

        /* 
         * Convert a path for use on Windows. 
         * Convert all /s and :s into \s 
         */
        while ((charToConvert = rwstrchr(dstBuffer, '/')))
        {
            *charToConvert = '\\';
        }
#if 0
        while ((charToConvert = rwstrchr(dstBuffer, ':')))
        {
            *charToConvert = '\\';
        }
#endif
    }
    return (dstBuffer);
}

void
psPathnameDestroy(RwChar * buffer)
{
    if (buffer)
    {
        RwFree(buffer);
    }
}

RwChar
psPathGetSeparator(void)
{
    return '\\';
}

RwMemoryFunctions  *
psGetMemoryFunctions()
{
    return (NULL);
}

RwBool
psInstallFileSystem(void)
{
    return (TRUE);
}

RwBool
psNativeTextureSupport(void)
{
    return (TRUE);
}

/**********************************************************************/

void
psDebugMessageHandler(RwDebugType type __RWUNUSED__, const RwChar * str)
{
    OutputDebugString(str);
    OutputDebugString(RWSTRING("\n"));
}

#ifndef UNDER_CE

/*
 * Handle queries about the window's maximum extent.
 */
static void
HandleGetMinMaxInfo(MINMAXINFO FAR * minmaxinfo)
{
    RECT                rect;

    rect.left = rect.top = 0;
    rect.right = RsGlobal.maximumWidth;
    rect.bottom = RsGlobal.maximumHeight;

    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    /*
     * Constraint the window to the maximum size defined by the 
     limits of the camera 
    */

    minmaxinfo->ptMaxSize.x = rect.right - rect.left;
    minmaxinfo->ptMaxSize.y = rect.bottom - rect.top;
    minmaxinfo->ptMaxTrackSize.x = rect.right - rect.left;
    minmaxinfo->ptMaxTrackSize.y = rect.bottom - rect.top;

    /* Don't allow to make smaller if in full screen */
    if (PSGLOBAL(fullScreen))
    {
        minmaxinfo->ptMinTrackSize.x = rect.right - rect.left;
        minmaxinfo->ptMinTrackSize.y = rect.bottom - rect.top;
    }
}

#endif

static              RwInt32
winTranslateKey(WPARAM wParam __RWUNUSED__, LPARAM lParam)
{
    RwInt32             nOutKey;

    nOutKey = (lParam & 0x00ff0000) >> 16;
    if (lParam & 0x01000000)
    {
        nOutKey |= 128;
    }

    return (nOutKey);
}

extern LRESULT      CALLBACK
MainWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

#ifdef RWMOUSE
void
ClipMouseToWindow(HWND window)
{
    RECT                wRect;

    GetWindowRect(window, &wRect);

    if (!PSGLOBAL(fullScreen))
    {
        wRect.left += GetSystemMetrics(SM_CXFRAME);
        wRect.top += GetSystemMetrics(SM_CYMENU) +
            GetSystemMetrics(SM_CYFRAME);
        wRect.right -= GetSystemMetrics(SM_CXFRAME);
        wRect.bottom -= GetSystemMetrics(SM_CYFRAME);
    }

    ClipCursor(&wRect);
}

#endif

/*
 * The window procedure for this application's window.
 */
LRESULT             CALLBACK
MainWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    POINTS              points;

    switch (message)
    {
#ifndef UNDER_CE
    case WM_CREATE:
        /*
         * Clumps are loaded into the scene by drag and drop.
         * So make this window a drop site.
         */
        DragAcceptFiles(window, TRUE);
        return 0L;

#ifdef RWMOUSE
    case WM_ACTIVATE:
        if (wParam == WA_INACTIVE)
        {
            ClipCursor(NULL);
        }
        else
        {
            ClipMouseToWindow(window);
        }
        return 0L;

    case WM_MOVE:
        ClipMouseToWindow(window);
        return 0L;
#endif

    case WM_GETMINMAXINFO:
        HandleGetMinMaxInfo((MINMAXINFO FAR *) lParam);
        return 0L;

    case WM_DROPFILES:
    {
        HDROP               drop = (HDROP) wParam;
        RwChar              path[_MAX_PATH];
        int                 i;

        for (i =
                 DragQueryFile(drop, (UINT) - 1, path,
                               _MAX_PATH) - 1; i >= 0; --i)
        {
            DragQueryFile(drop, (UINT) i, path, _MAX_PATH);
            RsEventHandler(rsFILELOAD, path);
        }
        DragFinish(drop);
    }
    return 0L;
#endif

    case WM_SIZE:
    {
        RwRect              r;

        r.x = 0;
        r.y = 0;
        r.w= LOWORD(lParam);
        r.h = HIWORD(lParam);
#ifdef RWMOUSE
        ClipMouseToWindow(window);
#endif
        if (RwInitialised)
        {
            RsEventHandler(rsCAMERASIZE, &r);
        }
    }
    return 0L;

    case WM_LBUTTONDOWN:
    {
        RsMouseStatus       ms;

        points = MAKEPOINTS(lParam);
        ms.pos.x = points.x;
        ms.pos.y = points.y;
        ms.shift = (wParam & MK_SHIFT) ? TRUE : FALSE;
        ms.control = (wParam & MK_CONTROL) ? TRUE : FALSE;

        SetCapture(window);
        RsMouseEventHandler(rsLEFTBUTTONDOWN, &ms);
    }
    return 0L;

    case WM_RBUTTONDOWN:
    {
        RsMouseStatus       ms;

        points = MAKEPOINTS(lParam);
        ms.pos.x = points.x;
        ms.pos.y = points.y;
        ms.shift = (wParam & MK_SHIFT) ? TRUE : FALSE;
        ms.control = (wParam & MK_CONTROL) ? TRUE : FALSE;

        SetCapture(window);
        RsMouseEventHandler(rsRIGHTBUTTONDOWN, &ms);
    }
    return 0L;

    case WM_MOUSEMOVE:
    {

        points = MAKEPOINTS(lParam);
        if (validDelta)
        {
            RsMouseStatus       ms;

            ms.delta.x = points.x - PSGLOBAL(lastMousePos).x;
            ms.delta.y = points.y - PSGLOBAL(lastMousePos).y;
            ms.pos.x = points.x;
            ms.pos.y = points.y;
            RsMouseEventHandler(rsMOUSEMOVE, &ms);
        }
        else
        {
            validDelta = TRUE;
        }
        PSGLOBAL(lastMousePos).x = points.x;
        PSGLOBAL(lastMousePos).y = points.y;
    }
    return 0L;

    case WM_LBUTTONUP:
    {
        ReleaseCapture();
        RsMouseEventHandler(rsLEFTBUTTONUP, NULL);
    }
    return 0L;

    case WM_RBUTTONUP:
    {
        ReleaseCapture();
        RsMouseEventHandler(rsRIGHTBUTTONUP, NULL);
    }
    return 0L;

    case WM_KEYDOWN:
    {
        RsKeyStatus         ks;

        if (!(lParam & 0x40000000)) /* ignore repeat events */
        {
            ks.keyScanCode = winTranslateKey(wParam, lParam);
            ks.keyCharCode = KeyTableEnglish[ks.keyScanCode];
            RsKeyboardEventHandler(rsKEYDOWN, &ks);
        }
    }
    return 0L;

    case WM_KEYUP:
    {
        RsKeyStatus         ks;

        ks.keyScanCode = winTranslateKey(wParam, lParam);
        ks.keyCharCode = KeyTableEnglish[ks.keyScanCode];
        RsKeyboardEventHandler(rsKEYUP, &ks);
    }
    return 0L;

    case WM_DESTROY:
        /*
         * Quit message handling.
         */
        ClipCursor(NULL);
        PostQuitMessage(0);
        return 0L;
    }

    /*
     * Let Windows handle all other messages.
     */
    return DefWindowProc(window, message, wParam, lParam);
}

/**********************************************************************/

/*
 * Perform any necessary MS Windows application initialization. Basically,
 * this means registering the window class for this application.
 */
static              BOOL
InitApplication(HANDLE instance)
{
    WNDCLASS            windowClass;

#ifdef UNDER_CE
    windowClass.style = 0;
#else
    windowClass.style = CS_BYTEALIGNWINDOW;
#endif
    windowClass.lpfnWndProc = (WNDPROC) MainWndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = instance;
    windowClass.hIcon = NULL;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = AppClassName;

    return RegisterClass(&windowClass);
}

/**********************************************************************/

/*
 * Perform any necessary initialization for this instance of the
 * application. This simply means creating the application's main
 * window.
 */
static              HWND
InitInstance(HANDLE instance)
{
    /*
     * Create the MS Window's window instance for this application. The
     * initial window size is given by the defined camera size. The window 
     * is not given a title as we set it during Init3D() with information 
     * about the version of RenderWare being used.
     */

    RECT                rect;

    rect.left = rect.top = 0;
    rect.right = RsGlobal.maximumWidth;
    rect.bottom = RsGlobal.maximumHeight;

#ifdef UNDER_CE
    {
        DWORD               style, exStyle;

        style = WS_BORDER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
        exStyle = WS_EX_NODRAG | WS_EX_CAPTIONOKBTN | WS_EX_WINDOWEDGE;

        AdjustWindowRectEx(&rect, style, FALSE, exStyle);

        return CreateWindow(AppClassName, RsGlobal.appName,
                            style,
                            rect.left, rect.top,
                            rect.right - rect.left,
                            rect.bottom - rect.top, NULL, NULL,
                            instance, NULL);
    }
#else
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    return CreateWindow(AppClassName, RsGlobal.appName,
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        rect.right - rect.left, rect.bottom - rect.top,
                        NULL, NULL, instance, NULL);
#endif
}

/**********************************************************************/

#ifdef UNDER_CE
#define CMDSTR  LPWSTR
#else
#define CMDSTR  LPSTR
#endif

RwBool
psInitialise(void)
{
    PsGlobal.lastMousePos.x = PsGlobal.lastMousePos.y = 0.0;
    PsGlobal.fullScreen = FALSE;
    RsGlobal.ps = &PsGlobal;
    return (TRUE);
}

void
psTerminate(void)
{
}

RwBool
psAlwaysOnTop(RwBool AlwaysOnTop)
{
    RECT                winRect;
    HWND                hwnd;

    hwnd = GetForegroundWindow(); /* PSGLOBAL( window ); */

    GetWindowRect(hwnd, &winRect);

    if (AlwaysOnTop)
    {
        return (RwBool) SetWindowPos(hwnd, HWND_NOTOPMOST,
                                     winRect.left, winRect.top,
                                     winRect.right - winRect.left,
                                     winRect.bottom - winRect.top, 0);
    }
    else
    {
        return (RwBool) SetWindowPos(hwnd, HWND_TOPMOST,
                                     winRect.left, winRect.top,
                                     winRect.right - winRect.left,
                                     winRect.bottom - winRect.top, 0);
    }
}

#ifdef RWMETRICS
void
psMetricsRender(VecFont * vecFont __RWUNUSED__,
                RwV2d * pos __RWUNUSED__,
                RwMetrics * metrics __RWUNUSED__)
{
}

#endif /* RWMETRICS */

/**********************************************************************/

/*
 * Dialog box functions for selecting a device
 * 
 */

static void
dialogAddModes(HWND wndListVideMode)
{
    RwInt32             vidMode, numVidModes;
    RwVideoMode         vidModemInfo;
    RwChar              modeStr[100];

    numVidModes = RwEngineGetNumVideoModes();

    /* Add the available video modes to the dialog */
    for (vidMode = 0; vidMode < numVidModes; vidMode++)
    {
        int                 index;

        RwEngineGetVideoModeInfo(&vidModemInfo, vidMode);

        rwsprintf(modeStr, RWSTRING("%lu x %lu x %lu %s"),
                  vidModemInfo.width, vidModemInfo.height,
                  vidModemInfo.depth,
                  vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE ?
                  RWSTRING("(Fullscreen)") : RWSTRING(""));

        /* Add name and an index so we can ID it later */
        index =
            SendMessage(wndListVideMode, CB_ADDSTRING, 0,
                        (LPARAM) modeStr);
        SendMessage(wndListVideMode, CB_SETITEMDATA, index,
                    (LPARAM) vidMode);
    }
}

static void
dialogInit(HWND hDlg,
           UINT message __RWUNUSED__,
           WPARAM wParam __RWUNUSED__, LPARAM lParam __RWUNUSED__)
{
    HWND                wndList, wndListVideMode;
    RwInt32             subSysNum;
    RwInt32             width, height;

    /* Handle the list box */
    wndList = GetDlgItem(hDlg, IDC_DEVICESEL);
    wndListVideMode = GetDlgItem(hDlg, IDC_VIDMODE);

    width = RsGlobal.maximumWidth;
    height = RsGlobal.maximumHeight;

    /* Add the names of the sub systems to the dialog */
    for (subSysNum = 0; subSysNum < GnumSubSystems; subSysNum++)
    {
        /* Add name and an index so we can ID it later */
        SendMessage(wndList, CB_ADDSTRING, 0,
                    (LPARAM) GsubSysInfo[subSysNum].name);
        SendMessage(wndList, CB_SETITEMDATA, subSysNum,
                    (LPARAM) subSysNum);
    }
    SendMessage(wndList, CB_SETCURSEL, GcurSel, 0);

    /* display avalible modes */
    dialogAddModes(wndListVideMode);

    GcurSelVM = RwEngineGetCurrentVideoMode();
    SendMessage(wndListVideMode, CB_SETCURSEL, GcurSelVM, 0);

    SetFocus(wndList);
}

static void
dialogDevSelect(HWND hDlg,
                UINT message __RWUNUSED__,
                WPARAM wParam __RWUNUSED__, LPARAM lParam __RWUNUSED__)
{
    HWND                wndList, wndListVideMode;
    RwInt32             selection;
    RwInt32             width, height;

    /* Handle the list box */
    wndList = GetDlgItem(hDlg, IDC_DEVICESEL);
    wndListVideMode = GetDlgItem(hDlg, IDC_VIDMODE);

    width = RsGlobal.maximumWidth;
    height = RsGlobal.maximumHeight;

    /* Update the selected entry */
    selection = SendMessage(wndList, CB_GETCURSEL, 0, 0);
    if (selection != GcurSel)
    {
        GcurSel = SendMessage(wndList, CB_GETITEMDATA, selection, 0);

        RwEngineSetSubSystem(GcurSel);

        wndListVideMode = GetDlgItem(hDlg, IDC_VIDMODE);
        /* changed device so update video modes listbox */
        SendMessage(wndListVideMode, CB_RESETCONTENT, 0, 0);

        /* display avalible modes */
        dialogAddModes(wndListVideMode);

        GcurSelVM = RwEngineGetCurrentVideoMode();
        SendMessage(wndListVideMode, CB_SETCURSEL, GcurSelVM, 0);
    }
}

static BOOL         CALLBACK
DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        dialogInit(hDlg, message, wParam, lParam);

        return (FALSE);
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_DEVICESEL:
        {
            dialogDevSelect(hDlg, message, wParam,
                            lParam);

            return (TRUE);
        }
        case IDC_VIDMODE:
        {
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                HWND                wndListVideMode;
                RwInt32             vmSel;

                wndListVideMode =
                    GetDlgItem(hDlg, IDC_VIDMODE);

                                /* Update the selected entry */
                vmSel =
                    SendMessage(wndListVideMode,
                                CB_GETCURSEL, 0, 0);
                GcurSelVM =
                    SendMessage(wndListVideMode,
                                CB_GETITEMDATA, vmSel,
                                0);
            }
            return (TRUE);
        }
        case IDOK:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                EndDialog(hDlg, TRUE);
            }
            return (TRUE);
        }
        case IDEXIT:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                EndDialog(hDlg, FALSE);
            }
            return (TRUE);
        }
        default:
        {
            return (FALSE);
        }
        }
    }
    default:
    {
        return (FALSE);
    }
    }

    return (FALSE);
}

RwBool
psSelectDevice(void)
{
    HWND                hWnd;
    HINSTANCE           hInstance;
    RwVideoMode         vm;
    RwInt32             subSysNum;

    hWnd = PSGLOBAL(window);
    hInstance = PSGLOBAL(instance);

    GnumSubSystems = RwEngineGetNumSubSystems();
    if (!GnumSubSystems)
    {
        return (FALSE);
    }

    /* Just in case */
    GnumSubSystems =
        (GnumSubSystems >
         MAX_SUBSYSTEMS) ? MAX_SUBSYSTEMS : GnumSubSystems;

    /* Get the names of all the sub systems */
    for (subSysNum = 0; subSysNum < GnumSubSystems; subSysNum++)
    {
        RwEngineGetSubSystemInfo(&GsubSysInfo[subSysNum], subSysNum);
    }

    /* Get the default selection */
    GcurSel = RwEngineGetCurrentSubSystem();

    /* Allow the user to choose */
    if (!DialogBox
        (hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogProc))
    {
        return (FALSE);
    }

    /* Set the driver to use the correct sub system */
    if (!RwEngineSetSubSystem(GcurSel))
    {
        return (FALSE);
    }

    /* Set up the video mode and set the apps window
     * dimensions to match */
    if (!RwEngineSetVideoMode(GcurSelVM))
    {
        return (FALSE);
    }

    RwEngineGetVideoModeInfo(&vm, GcurSelVM);
    if (vm.flags & rwVIDEOMODEEXCLUSIVE)
    {
        RsGlobal.maximumWidth = vm.width;
        RsGlobal.maximumHeight = vm.height;
        PSGLOBAL(fullScreen) = TRUE;
    }

    return (TRUE);
}

/*
 * MS Windows application entry point.
 */
int                 PASCAL
WinMain(HINSTANCE instance, 
        HINSTANCE prevInstance,
        CMDSTR cmdLine, 
        int cmdShow)
{
    MSG                 message;
    RwV2d               pos;

    if (prevInstance)
    {
        /*
         * Only allow one viewer application to run at any one time.
         */
        MessageBox(NULL,
                   TEXT("A RenderWare(tm) Application is already running..."),
                   ErrorDialogTitle,
                   MB_OK | MB_APPLMODAL | MB_ICONSTOP);
        return FALSE;
    }

    /* Initialize the platform independent data 
     * This will in turn initialise the platform specific data
     */

    if (RsEventHandler(rsINITIALISE, NULL) == rsEVENTERROR)
    {
        return FALSE;
    }

    /*
     * Register the window class.
     */
    if (!InitApplication(instance))
    {
        return FALSE;
    }

    /*
     * Create the window.
     */
    PSGLOBAL(window) = InitInstance(instance);
    if (PSGLOBAL(window) == NULL)
    {
        return FALSE;
    }

    PSGLOBAL(instance) = instance;

    /* Initialize the 3D (RenderWare) components of the app. */

    if (rsEVENTERROR ==
        RsEventHandler(rsRWINITIALISE, PSGLOBAL(window)))
    {
        MessageBox(NULL,
                   TEXT("rsEVENTERROR == RsEventHandler(rsRWINITIALISE, PSGLOBAL(window)"),
                   ErrorDialogTitle,
                   MB_OK | MB_APPLMODAL | MB_ICONSTOP);

        DestroyWindow(PSGLOBAL(window));
        RsEventHandler(rsTERMINATE, NULL);
        return FALSE;

    }

    RwInitialised = TRUE;

    /* Full screen or not? */
    if (PSGLOBAL(fullScreen))
    {
        SetWindowLong(PSGLOBAL(window), GWL_STYLE, WS_POPUP);
        SetWindowPos(PSGLOBAL(window), 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_FRAMECHANGED);
    }

    /* Parse any command line parameters. */

    RsEventHandler(rsCOMMANDLINE, cmdLine);

    /* Force a camera resize event */
    {
        RwRect              r;

        r.x = 0;
        r.y = 0;
        r.w = RsGlobal.maximumWidth;
        r.h = RsGlobal.maximumHeight;

        RsEventHandler(rsCAMERASIZE, &r);
    }
    /*
     * Show the window, and refresh it.
     */
    ShowWindow(PSGLOBAL(window), cmdShow);
    UpdateWindow(PSGLOBAL(window));

    /* Set the initial mouse position */
    pos.x = (RwReal) (RsGlobal.maximumWidth) * 0.5f;
    pos.y = (RwReal) (RsGlobal.maximumHeight) * 0.5f;

    RsMouseSetPos(&pos);

    /*
     * Enter the message processing loop.
     */
    while (!RsGlobal.quit)
    {
        if (PeekMessage(&message, NULL, 0U, 0U, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                break;
            }
            else
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
        else
        {
            RsEventHandler(rsIDLE, NULL);
        }
    }

    /*
     * Tidy up the 3D (RenderWare) components of the application.
     */
    RsEventHandler(rsRWTERMINATE, NULL);

    /* kill the window */
    DestroyWindow(PSGLOBAL(window));

    /* free the platform dependent data */
    RsEventHandler(rsTERMINATE, NULL);

    return message.wParam;
}
