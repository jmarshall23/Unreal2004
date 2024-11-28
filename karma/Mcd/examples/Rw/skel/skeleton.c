#include <string.h>

#include <rwcore.h>
#include <rtpng.h>

#include "skeleton.h"
#include "platform.h"

#ifdef RWMOUSE
#include "mouse.h"
#endif

#ifdef RWTERMINAL
#include "terminal.h"
#endif /* RWTERMINAL */

/* 
 * TODO: we need a pikeyboa.c equivalent to take care of 
 * scancode to rs conversion... 
 */

/* *INDENT-OFF* */
static RwUInt8 
KeysNormal[]=
{
    rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 0-7   */
    rsNULL,rsNULL,0xd,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 8-15  */
    rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 16-23 */
    rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 24-31 */
    ' ','!','"','#', '$','%','&','\'',                          /* 32-39 */
    '(',')','*','+',',','-','.','/',                            /* 40-47 */
    '0','1','2','3','4','5','6','7',                            /* 48-55 */
    '8','9',':',';','<','=','>','?',                            /*   -3f */
    '@','A','B','C','D','E','F','G',                            /* 40-47 */
    'H','I','J','K','L','M','N','O',                            /* 48-4f */
    'P','Q','R','S','T','U','V','W',                            /* 50-57 */
    'X','Y','Z','[','\\',']','^','_',                           /* 58-5f */
    '`','a','b','c','d','e','f','g',                            /* 60-67 */
    'h','i','j','k','l','m','n','o',                            /* 68-6f */
    'p','q','r','s','t','u','v','w',                            /* 70-77 */
    'x','y','z','{','|','}','~',0x7f,                           /* 78-7f */

    27,rsF1,rsF2,rsF3,rsF4,rsF5,rsF6,rsF7,                      /* 80-87 */
    rsF8,rsF9,rsF10,rsF11, rsF12,rsINS,0x7f,rsHOME,             /* 88-8f */
    rsEND,rsPGUP,rsPGDN,rsUP,rsDOWN,rsLEFT,rsRIGHT,rsINS,       /* 90-97 */
    0x7f,rsHOME,rsEND,rsPGUP,rsPGDN,rsUP,rsDOWN,rsLEFT,         /* 98-9f */
    rsRIGHT,rsNUMLOCK,'/','*', '-','+',0xd,' ',                 /* a0-a7 */
    0x8,0x9,rsCAPSLK,0xd, rsNULL,rsNULL,rsNULL,rsNULL,          /* a8-af */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* b0-b7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* b8-bf */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* c0-c7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* c8-cf */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* d0-d7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* d8-df */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* e0-e7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* e8-ef */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* f0-f7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL    /* f8-ff */
};

static RwUInt8 
KeysShifted[]=
{
    rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 0-7   */
    rsNULL,rsNULL,0xd,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 8-15  */
    rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 16-23 */
    rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 24-31 */
    ' ','!','"','#', '$','%','&','\"',                          /* 32-39 */
    '(',')','*','+','<','_','>','?',                            /* 40-47 */
    ')','!','@','#','$','%','^','&',                            /* 48-55 */
    '*','(',':',':','<','+','>','?',                            /*   -3f */
    '@','A','B','C','D','E','F','G',                            /* 40-47 */
    'H','I','J','K','L','M','N','O',                            /* 48-4f */
    'P','Q','R','S','T','U','V','W',                            /* 50-57 */
    'X','Y','Z','{','|','}','^','_',                            /* 58-5f */
    '~','A','B','C','D','E','F','G',                            /* 60-67 */
    'H','I','J','K','L','M','N','O',                            /* 68-6f */
    'P','Q','R','S','T','U','V','W',                            /* 70-77 */
    'X','Y','Z','{','|','}','~',0x7f,                           /* 78-7f */

    27,rsF1,rsF2,rsF3,rsF4,rsF5,rsF6,rsF7,                      /* 80-87 */
    rsF8,rsF9,rsF10,rsF11, rsF12,rsINS,0x7f,rsHOME,             /* 88-8f */
    rsEND,rsPGUP,rsPGDN,rsUP,rsDOWN,rsLEFT,rsRIGHT,rsINS,       /* 90-97 */
    0x7f,rsHOME,rsEND,rsPGUP,rsPGDN,rsUP,rsDOWN,rsLEFT,         /* 98-9f */
    rsRIGHT,rsNUMLOCK,'/','*', '-','+',0xd,' ',                 /* a0-a7 */
    0x8,0x9,rsCAPSLK,0xd, rsNULL,rsNULL,rsNULL,rsNULL,          /* a8-af */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* b0-b7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* b8-bf */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* c0-c7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* c8-cf */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* d0-d7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* d8-df */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* e0-e7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* e8-ef */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* f0-f7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL    /* f8-ff */
};
/* *INDENT-ON* */

RsGlobalType        RsGlobal;

RwUInt8
RsKeyFromScanCode(RwUInt8 scan, RwBool ShiftKeyDown)
{
    return ((ShiftKeyDown) ? (KeysShifted[scan]) : (KeysNormal[scan]));
}

RwUInt32
RsTimer(void)
{
    return (psTimer());
}

void
RsWindowSetText(const RwChar * text)
{
    psWindowSetText(text);

    return;
}

void
RsErrorMessage(const RwChar * text)
{
    psErrorMessage(text);

    return;
}

void
RsWarningMessage(const RwChar * text)
{
    psWarningMessage(text);

    return;
}

RwChar
RsPathGetSeparator(void)
{
    return (psPathGetSeparator());
}

void
RsCameraShowRaster(RwCamera * camera)
{
#ifdef RWMOUSE
    rsMouseRender(camera);
#endif
    psCameraShowRaster(camera);

    return;
}

RwBool
RsAlwaysOnTop(RwBool AlwaysOnTop)
{
    return psAlwaysOnTop(AlwaysOnTop);
}

RwBool
RsRegisterImageLoader(void)
{
    if (!RwImageRegisterImageFormat(RWSTRING("bmp"), RwImageReadBMP, NULL))
    {
        return (FALSE);
    }

    if (!RwImageRegisterImageFormat(RWSTRING("png"), RwImageReadPNG, NULL))
    {
        return (FALSE);
    }

    return (TRUE);
}

static              RwBool
RsSetDebug(void)
{
    RwDebugSetHandler(psDebugMessageHandler);
    RwDebugSendMessage(rwDEBUGMESSAGE, RsGlobal.appName,
                       RWSTRING("Debugging Initialised"));
    return (TRUE);
}

void
RsMouseSetVisibility(RwBool visible)
{
#ifdef RWMOUSE
    rsMouseVisible(visible);
#else
    psMouseSetVisibility(visible);
#endif

    return;
}

void
RsMouseSetPos(RwV2d * pos)
{
#ifdef RWMOUSE
    rsMouseSetPos(pos);
#endif
    psMouseSetPos(pos);
    return;
}

RwBool
RsSelectDevice(void)
{
    return (psSelectDevice());
}

RwBool
RsInputDeviceAttach(RsInputDeviceType inputDevice, 
                    RsInputEventHandler inputEventHandler)
{
    switch (inputDevice)
    {
        case rsKEYBOARD:
        {
            RsGlobal.keyboard.inputEventHandler = inputEventHandler;
            RsGlobal.keyboard.used = TRUE;
            break;
        }
        case rsMOUSE:
        {
            RsGlobal.mouse.inputEventHandler = inputEventHandler;
            RsGlobal.mouse.used = TRUE;
            break;
        }
        case rsPAD:
        {
            RsGlobal.pad.inputEventHandler = inputEventHandler;
            RsGlobal.pad.used = TRUE;
            break;
        }
        default:
        {
            return (FALSE);
        }
    }

    return (TRUE);
}

static RwBool
rsCommandLine(RwChar *cmdLine)
{
    RwChar *s1;
    RwChar *s2;
    RwChar  fileName[256];

    if (cmdLine)
    {
        s1 = &cmdLine[0];
        s2 = &fileName[0];
        while (*s1)
        {
            /* Extract one file name from the list. */
            while (*s1 && (*s1 != ' '))
            {
                *s2++ = *s1++;
            }
            *s2 = '\0';
            if( *s1 )  /* only increment s1 if we have not reached the end of the command line */
            {
                s1++;
            }

            if (fileName[0])
            {
                RsEventHandler(rsFILELOAD, fileName);
            }
            s2 = &fileName[0];
        }
    }

    return (TRUE);
}

RsEventStatus
RsKeyboardEventHandler(RsEvent event, void *param)
{
    if (RsGlobal.keyboard.used)
    {
        return (RsGlobal.keyboard.inputEventHandler(event, param));
    }

    return (rsEVENTNOTPROCESSED);
}

RsEventStatus
RsMouseEventHandler(RsEvent event, void *param)
{
#ifdef RWMOUSE
    /* snoop the mouse move event to update the mouse position for
     * a skeleton drawn cursor
     */
    if (event == rsMOUSEMOVE)
    {
        rsMouseAddDelta(&((RsMouseStatus *) param)->delta);
        rsMouseGetPos(&((RsMouseStatus *) param)->pos);
    }
    if ((event == rsLEFTBUTTONDOWN) || (event == rsRIGHTBUTTONDOWN))
    {
        rsMouseGetPos(&((RsMouseStatus *) param)->pos);
    }
#endif

    if (RsGlobal.mouse.used)
    {
        return (RsGlobal.mouse.inputEventHandler(event, param));
    }

    return (rsEVENTNOTPROCESSED);
}

RsEventStatus
RsPadEventHandler(RsEvent event, void *param)
{
    if (RsGlobal.pad.used)
    {
        return (RsGlobal.pad.inputEventHandler(event, param));
    }

    return (rsEVENTNOTPROCESSED);
}

RsEventStatus
RsEventHandler(RsEvent event, void *param)
{
    RsEventStatus       result;
    RsEventStatus       es;

#ifdef RWMOUSE
    /* snoop the mouse move event to update the mouse position for
     * a skeleton drawn cursor
     */
    if (event == rsMOUSEMOVE)
    {
        rsMouseAddDelta(&((RsMouseStatus *) param)->delta);
        rsMouseGetPos(&((RsMouseStatus *) param)->pos);
    }
    if ((event == rsLEFTBUTTONDOWN) || (event == rsRIGHTBUTTONDOWN))
    {
        rsMouseGetPos(&((RsMouseStatus *) param)->pos);
    }
#endif
    /* Give the application an opportunity to override any events */

    es = AppEventHandler(event, param);

    /* We never allow the app to replace the quit behaviour, only to intercept */

    if (event == rsQUITAPP)
    {
        /* Set the flag which causes the event loop to exit */
        RsGlobal.quit = TRUE;
    }

    if (es == rsEVENTNOTPROCESSED)
    {
        switch (event)
        {
            case rsSELECTDEVICE:
                result = (RsSelectDevice()?
                          rsEVENTPROCESSED : rsEVENTERROR);
                break;

            case rsCOMMANDLINE:
                result = (rsCommandLine(param) ?
                          rsEVENTPROCESSED : rsEVENTERROR);
                break;

            case rsINITDEBUG:
                result = (RsSetDebug()?
                          rsEVENTPROCESSED : rsEVENTERROR);
                break;

            case rsREGISTERIMAGELOADER:
                result = (RsRegisterImageLoader()?
                          rsEVENTPROCESSED : rsEVENTERROR);
                break;

            case rsRWTERMINATE:
                RsRwTerminate();
                result = (rsEVENTPROCESSED);
                break;

            case rsRWINITIALISE:
                result = (RsRwInitialise(param) ?
                          rsEVENTPROCESSED : rsEVENTERROR);
                break;

            case rsTERMINATE:
                RsTerminate();
                result = (rsEVENTPROCESSED);
                break;

            case rsINITIALISE:
                result = (RsInitialise()?
                          rsEVENTPROCESSED : rsEVENTERROR);
                break;

            default:
                result = (es);
                break;

        }
    }
    else
    {
        result = (es);
    }

    return (result);
}

void
RsPathnameDestroy(RwChar * buffer)
{
    psPathnameDestroy(buffer);

    return;
}

RwChar             *
RsPathnameCreate(const RwChar * srcBuffer)
{
    return (psPathnameCreate(srcBuffer));
}

#ifdef RWMOUSE
#define RSMOUSETERM()  rsMouseTerm()
#define RSMOUSEINIT()  rsMouseInit()
#endif

#if (!defined(RSMOUSETERM))
#define RSMOUSETERM()          /* No op */
#endif /* (!defined(RSMOUSETERM)) */

#if (!defined(RSMOUSEINIT))
#define RSMOUSEINIT()          /* No op */
#endif /* (!defined(RSMOUSEINIT)) */

void
RsRwTerminate(void)
{
#ifdef RWTERMINAL
    if (NULL != RsGlobal.terminal)
    {
        RsTerminalDestroy(RsGlobal.terminal);
        RsGlobal.terminal = NULL;
    }
#endif /* RWTERMINAL */

    RSMOUSETERM();

    /* Close RenderWare */

    RwEngineStop();
    RwEngineClose();
    RwEngineTerm();

    return;
}

RwBool
RsRwInitialise(void *displayID)
{
    RwEngineOpenParams  openParams;

    /* Start RenderWare */

    if (!RwEngineInit(psGetMemoryFunctions()))
    {
        return (FALSE);
    }

    /* Install any platform specific file systems */
    psInstallFileSystem();

    /* Initialise debug message handling */
    RsEventHandler(rsINITDEBUG, NULL);

    /* Attach all plugins */
    if (RsEventHandler(rsPLUGINATTACH, NULL) == rsEVENTERROR)
    {
        return (FALSE);
    }

    /* Attach input devices */
    if (RsEventHandler(rsINPUTDEVICEATTACH, NULL) == rsEVENTERROR)
    {
        return (FALSE);
    }

    openParams.displayID = displayID;

    if (!RwEngineOpen(&openParams))
    {
        return (FALSE);
    }

    if (RsEventHandler(rsSELECTDEVICE, displayID) == rsEVENTERROR)
    {
        RwEngineClose();
        RwEngineTerm();
        return (FALSE);
    }

    if (!RwEngineStart())
    {
        RwEngineClose();
        RwEngineTerm();
        return (FALSE);
    }

    /* Register loaders for an image with a particular file extension */
    RsEventHandler(rsREGISTERIMAGELOADER, NULL);

    psNativeTextureSupport();

    RSMOUSEINIT();

#ifdef RWTERMINAL
    /* Start up the terminal */
    {
        static RwRGBA       fore = { 0, 0xff, 0xff, 0xff };
        static RwRGBA       back = { 0, 0, 0, 0 };

        RsGlobal.terminal =
            RsTerminalCreate(80, 16, &fore, &back, NULL, NULL);

        if (!RsGlobal.terminal)
        {
            RwEngineStop();
            RwEngineClose();
            RwEngineTerm();
            return (FALSE);
        }
        RsTerminalPrintf(RsGlobal.terminal,
                         RWSTRING("RenderWare V%d.%d%d started.\n"),
                         (RwEngineGetVersion() & 0xff00) >> 8,
                         (RwEngineGetVersion() & 0xf0) >> 4,
                         (RwEngineGetVersion() & 0xf));
    }
#endif /* RWTERMINAL */

    return (TRUE);
}

void
RsTerminate(void)
{
    psTerminate();

    return;
}

/* Initialise Platform independent data */
RwBool
RsInitialise(void)
{
    RwBool              result;

    RsGlobal.appName = RWSTRING("RenderWare Application");
    RsGlobal.maximumWidth = 0;
    RsGlobal.maximumHeight = 0;
#ifdef RWTERMINAL
    RsGlobal.terminal = NULL;
#endif /* RWTERMINAL */
    RsGlobal.quit = FALSE;

    /* setup the keyboard */
    RsGlobal.keyboard.inputDeviceType = rsKEYBOARD;
    RsGlobal.keyboard.inputEventHandler = NULL;
    RsGlobal.keyboard.used = FALSE;

    /* setup the mouse */
    RsGlobal.mouse.inputDeviceType = rsMOUSE;
    RsGlobal.mouse.inputEventHandler = NULL;
    RsGlobal.mouse.used = FALSE;

    /* setup the pad */
    RsGlobal.pad.inputDeviceType = rsPAD;
    RsGlobal.pad.inputEventHandler = NULL;
    RsGlobal.pad.used = FALSE;

    result = psInitialise();

    return result;
}
