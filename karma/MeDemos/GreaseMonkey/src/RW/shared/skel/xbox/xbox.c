
/****************************************************************************
 *
 * xbox.c
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1999, 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/* D3D8 Libraries */
#include <xtl.h>
#include <d3d8.h>

#include <stdio.h>

#include "rwcore.h"
#include "platform.h"
#include "skeleton.h"
#include "mouse.h"
#include "vecfont.h"

#ifdef RWSPLASH
/* Splash screen */
#include "splash.h"
#endif /* RWSPLASH */

/*--- Macro Definitions ---*/

#define JOYRANGE            (RwReal)32767.5     /* (65536 - 1) / 2 */
#define JOYDEADZONE         (RwReal)0.3

#define JOYSKELRANGE        (RwReal)(1)
#define JOYRANGESCALE       (RwReal)(JOYSKELRANGE / (RwReal)(JOYRANGE))

#define JOYRANGECONVERT(_d)  \
    ( ((-(RwReal)(_d)) + ((RwReal)0.5)) * JOYRANGESCALE)


/*--- Constants ---*/

/*--- Local Structure Definitions ---*/

typedef struct _padStatus padStatus;
struct _padStatus
{
    HANDLE      device;         /* Device handle for the gamepad */
    RwUInt32    oldButtonState; /* Buttons changed since last poll */
};

/*--- Static ---*/

static RwChar       MediaPath[] = "D:\\";
static padStatus    Pad = {NULL, 0};

static RwTextureCallBackRead    DefaultTextureReadCallBack;

/*
 *****************************************************************************
 */
#ifdef RWMETRICS
void
psMetricsRender(RsVectorFont *vecFont,
                RwV2d *pos,
                RwMetrics *metrics __RWUNUSED__)
{
    RwXboxMetrics   *xboxMetrics;

    xboxMetrics = (RwXboxMetrics *)metrics->devSpecificMetrics;

    /* check z compression ratio */
    {
        RwChar message[256];
        D3DTILE PrimaryDepthBufferTile;
        DWORD startTag, endTag, NumberCompressedTags;

        D3DDevice_GetTile( 1, &PrimaryDepthBufferTile );

        startTag = PrimaryDepthBufferTile.ZStartTag;
        endTag = D3DTILE_ZENDTAG( &PrimaryDepthBufferTile );

        NumberCompressedTags = D3DDevice_GetTileCompressionTags( startTag, endTag );

		sprintf(message,"percent z tags compressed %d \n", 100 * NumberCompressedTags / (endTag - startTag + 1));

        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;
    }
}
#endif /* RWMETRICS */


/*
 *****************************************************************************
 */
#ifdef RWSPLASH
RwBool
psDisplaySplashScreen(RwBool state __RWUNUSED__)
{
    return TRUE;
}
#endif /* RWSPLASH */


/*
 *****************************************************************************
 */
void
psWindowSetText(const RwChar *text __RWUNUSED__)
{
}

/*
 *****************************************************************************
 */
void
psWarningMessage(const RwChar *message )
{
    OutputDebugString(message);
    OutputDebugString(RWSTRING("\n"));
}


/*
 *****************************************************************************
 */
void
psErrorMessage(const RwChar *message)
{
    OutputDebugString(message);
    OutputDebugString(RWSTRING("\n"));
}

/*
 *****************************************************************************
 */
void
psDebugMessageHandler(RwDebugType type __RWUNUSED__, const RwChar *str)
{
    OutputDebugString(str);
    OutputDebugString(RWSTRING("\n"));
}


/*
 *****************************************************************************
 */
void
psMouseSetVisibility(RwBool visible __RWUNUSED__)
{
}


/*
 *****************************************************************************
 */
void
psMouseSetPos(RwV2d *pos __RWUNUSED__)
{
}


/*
 *****************************************************************************
 */
RwImage* 
psGrabScreen(RwCamera *camera)
{
    RwRaster *camRas;
    RwInt32 width, height;
    RwImage *image;

    camRas = RwCameraGetRaster(camera);

    if (camRas)
    {
        width = RwRasterGetWidth(camRas);
        height = RwRasterGetHeight(camRas);
        image = RwImageCreate(width, height, 32);

        if (image)
        {
            RwImageAllocatePixels(image);
            RwImageSetFromRaster(image, camRas);

            return (image);
        }
        else
        {
            return (NULL);
        }
    }
    else
    {
        return (NULL);
    }
}


/*
 *****************************************************************************
 */
RwMemoryFunctions *
psGetMemoryFunctions()
{
    return (NULL);
}


/*
 *****************************************************************************
 */
RwBool
psInstallFileSystem(void)
{
    return (TRUE);
}

/*
 *****************************************************************************
 */
static RwTexture *
XboxTextureReadCallBack(const RwChar *name, const RwChar *maskName)
{
    RwTexture   *texture = NULL;

    texture = RwXboxDDSTextureRead(name, maskName);
    if (!texture)
    {
        return (DefaultTextureReadCallBack(name, maskName));
    }

    return (texture);
}

/*
 *****************************************************************************
 */
RwBool
psNativeTextureSupport(void)
{
    DefaultTextureReadCallBack = RwTextureGetReadCallBack();
    RwTextureSetReadCallBack(XboxTextureReadCallBack);

    return (TRUE);
}


/*
 *****************************************************************************
 */
RwBool
psInitialize(void)
{
    return (TRUE);
}


/*
 *****************************************************************************
 */
void
psTerminate(void)
{
}


/*
 *****************************************************************************
 */
RwBool
psAlwaysOnTop(RwBool AlwaysOnTop __RWUNUSED__)
{
    return TRUE;
}


/*
 *****************************************************************************
 */
RwUInt32
psTimer(void)
{
    RwUInt32 time;

    time = (RwUInt32)timeGetTime();

    return time;
}


/*
 *****************************************************************************
 */
RwChar *
psPathnameCreate(const RwChar *srcBuffer)
{
    RwChar *dstBuffer;
    RwChar *charToConvert;
    RwInt32 length;

    length = rwstrlen(srcBuffer) + rwstrlen(MediaPath) + 1;

    /* 
     * First duplicate the string 
     */
    dstBuffer = RwOsGetMemoryInterface()->rwmalloc(sizeof(RwChar) * length);
    
    if (dstBuffer)
    {
        /* Drop ./,.\\ */

        if (*srcBuffer == '.')
        {
            srcBuffer++;
            length--;
        }

        if (*srcBuffer == '/')
        {
            srcBuffer++;
            length--;
        }

        if (*srcBuffer == '\\')
        {
            srcBuffer++;
            length--;
        }

        if (*srcBuffer == ':')
        {
            srcBuffer++;
            length--;
        }

        /* Media path */
        if (strncmp(srcBuffer, MediaPath, rwstrlen(MediaPath)) != 0)
        {
            rwstrcpy(dstBuffer, MediaPath);
        }
        else
        {
            /* already has media path */
            rwstrcpy(dstBuffer, "");
        }

        /* Append path */
        rwstrcat(dstBuffer, srcBuffer);

        /* 
         * Convert all /s into \s 
         */
        while ((charToConvert = rwstrchr(dstBuffer, '/')))
        {
            *charToConvert = '\\';
        }
    }

    return(dstBuffer);
}


/*
 *****************************************************************************
 */
void
psPathnameDestroy(RwChar *buffer)
{
    if (buffer)
    {
        RwFree(buffer);
    }
}


/*
 *****************************************************************************
 */
RwChar
psPathGetSeparator(void)
{
    return '\\';
}


/*
 *****************************************************************************
 */
void
psCameraShowRaster(RwCamera *camera)
{
    RwCameraShowRaster(camera, NULL, rwRASTERFLIPWAITVSYNC);
}


/*
 *****************************************************************************
 */
RwBool
psSelectDevice(RwBool useDefault)
{
    if(!useDefault)
    {
        RwInt32     vidMode, numVidModes;
        RwVideoMode vidModemInfo;

        numVidModes = RwEngineGetNumVideoModes();

        /* Add the available video modes to the dialog */
        for (vidMode = 0; vidMode < numVidModes; vidMode++)
        {
            RwEngineGetVideoModeInfo(&vidModemInfo, vidMode);

            if ((vidModemInfo.width == 640) &&
                (vidModemInfo.height == 480) &&
                (vidModemInfo.depth == 32) &&
                (vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE))
            {
                RsGlobal.maximumWidth = vidModemInfo.width;
                RsGlobal.maximumHeight = vidModemInfo.height;

                return (TRUE);
            }
        }
    }
    else
    {
        RwVideoMode vidModemInfo;

        RwEngineGetVideoModeInfo(&vidModemInfo,
                                 RwEngineGetCurrentVideoMode());
        RsGlobal.maximumWidth = vidModemInfo.width;
        RsGlobal.maximumHeight = vidModemInfo.height;
    }

    return (TRUE);
}


/*
 *****************************************************************************
 */
void
padOpen(void)
{
    DWORD                   deviceMask = 0;
    XDEVICE_PREALLOC_TYPE   deviceTypes[] =
    {
       {XDEVICE_TYPE_GAMEPAD, 1}
    };

    XInitDevices(1, deviceTypes);

    /* PAUSE - for XInitDevices before calling XGetDevices */
    {
        volatile RwUInt32 i;

        for (i = 0; i < 0x0FFFFFFF; i++) {}
    }

    /* Blank the GamePads */
    memset(&Pad, 0, sizeof(padStatus));

    /* Get a mask of all currently available devices */
    deviceMask = XGetDevices(XDEVICE_TYPE_GAMEPAD);

    /* Open port 0 */
    if (deviceMask & XDEVICE_PORT0_MASK)
    {
        Pad.device = XInputOpen(XDEVICE_TYPE_GAMEPAD, 0,
                                XDEVICE_NO_SLOT, NULL);
    }
}


/*
 *****************************************************************************
 */
static RwUInt32
remapButtons(WORD dButtons, BYTE *aButtons)
{
    RwUInt32 rsButtons;

    rsButtons = 0;

    rsButtons |= (dButtons & XINPUT_GAMEPAD_START) ? rsPADSTART : 0;
    rsButtons |= (dButtons & XINPUT_GAMEPAD_BACK) ? rsPADSELECT : 0;

    rsButtons |= (dButtons & XINPUT_GAMEPAD_DPAD_UP) ? rsPADDPADUP : 0;
    rsButtons |= (dButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? rsPADDPADDOWN : 0;
    rsButtons |= (dButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? rsPADDPADLEFT : 0;
    rsButtons |= (dButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? rsPADDPADRIGHT : 0;

    /*
     * Convert the analogue buttons to digital
     */

    /* A */
    if (aButtons[XINPUT_GAMEPAD_A] >= 0x80)
    {
        rsButtons |= rsPADBUTTON1;
    }

    /* B */
    if (aButtons[XINPUT_GAMEPAD_B] >= 0x80)
    {
        rsButtons |= rsPADBUTTON2;
    }
    
    /* X */
    if (aButtons[XINPUT_GAMEPAD_X] >= 0x80)
    {
        rsButtons |= rsPADBUTTON3;
    }

    /* Y */
    if (aButtons[XINPUT_GAMEPAD_Y] >= 0x80)
    {
        rsButtons |= rsPADBUTTON4;
    }

    /* Black */
    if (aButtons[XINPUT_GAMEPAD_BLACK] >= 0x80)
    {
        rsButtons |= rsPADBUTTON5;
    }

    /* White */
    if (aButtons[XINPUT_GAMEPAD_WHITE] >= 0x80)
    {
        rsButtons |= rsPADBUTTON6;
    }

    /* Left trigger */
    if (aButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] >= 0x80)
    {
        rsButtons |= rsPADBUTTON7;
    }

    /* Right trigger */
    if (aButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] >= 0x80)
    {
        rsButtons |= rsPADBUTTON8;
    }

    return (rsButtons);
}



/*
 *****************************************************************************
 */
void XboxTestForQuitState(XINPUT_STATE *PadState)
{
	/*
	 *    test whether the left trigger, right trigger, and
	 *    black buttons are depressed
	 */
    if ( ( PadState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]  >= 0x80 ) &&   /* left trigger down  */
		 ( PadState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] >= 0x80 ) &&   /* right trigger down */
		 ( PadState->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK]         >= 0x80 ) )    /* black button down  */
	{
        RsEventHandler(rsQUITAPP, 0);
	}

	return;
}



/*
 *****************************************************************************
 */
void
padUpdate(void)
{
    /* Read the input state for pad0 */
    if (Pad.device)
    {
        XINPUT_STATE        state;
        RsPadButtonStatus   padButtonsDown;
        RsPadButtonStatus   padButtonsUp;
        RwUInt32            buttons;
        RwV2d               delta;
        static RwBool       zeroLeftStick = FALSE;
        static RwBool       zeroRightStick = FALSE;

        /* Get the input state fpr Pad0 */
        XInputGetState(Pad.device, &state);

		/*
		 *    test for quit state
		 *    terminate if found
		 */
		XboxTestForQuitState(&state);


        /*
         * Buttons
         */

        /* Remap the buttons */
        buttons = remapButtons(state.Gamepad.wButtons, state.Gamepad.bAnalogButtons);

        /* Set the padID to 0 */
        padButtonsDown.padID = padButtonsUp.padID = 0;

        /* Find out what buttons were released */
        padButtonsUp.padButtons = Pad.oldButtonState & ~buttons;

        /* Find out what buttons were pressed */
        padButtonsDown.padButtons = ~Pad.oldButtonState & buttons;

        /* If any buttons just went down, or came up, message the app */
        if (padButtonsDown.padButtons)
        {
            RsPadEventHandler(rsPADBUTTONDOWN, &padButtonsDown);
        }
    
        if (padButtonsUp.padButtons)
        {
            RsPadEventHandler(rsPADBUTTONUP, &padButtonsUp);
        }

        /* Save buttons so we know what's changed next time */
        Pad.oldButtonState = buttons;

        /*
         * Sticks
         */

        /* Manage the left analogue sticks */
        delta.x = JOYRANGECONVERT(state.Gamepad.sThumbLX);
        if (RwFabs(delta.x) < JOYDEADZONE)
        {
            delta.x = (RwReal)0.0;
        }

        delta.y = JOYRANGECONVERT(state.Gamepad.sThumbLY);
        if (RwFabs(delta.y) < JOYDEADZONE)
        {
            delta.y = (RwReal)0.0;
        }

        if ((0 == delta.x) && (0 == delta.y))
        {
            if (!zeroLeftStick)
            {
                RsPadEventHandler(rsPADANALOGUELEFT, &delta);
                zeroLeftStick = TRUE;
            }
        }
        else
        {
            RsPadEventHandler(rsPADANALOGUELEFT, &delta);
            zeroLeftStick = FALSE;
        }

	    /* Manage the right analogue sticks */
        delta.x = JOYRANGECONVERT(state.Gamepad.sThumbRX);
        if (RwFabs(delta.x) < JOYDEADZONE)
        {
            delta.x = (RwReal)0.0;
        }

        delta.y = JOYRANGECONVERT(state.Gamepad.sThumbRY);
        if (RwFabs(delta.y) < JOYDEADZONE)
        {
            delta.y = (RwReal)0.0;
        }

        if ((0 == delta.x) && (0 == delta.y))
        {
            if (!zeroRightStick)
            {
                RsPadEventHandler(rsPADANALOGUERIGHT, &delta);
                zeroRightStick = TRUE;
            }
        }
        else
        {
            RsPadEventHandler(rsPADANALOGUERIGHT, &delta);
            zeroRightStick = FALSE;
        }
    }
}


/*
 *****************************************************************************
 */
int
main(void)
{
    RwXboxDeviceConfig  deviceConfig;

    /* These are the default setting just to demonstrate functionality.
     * If no RwXboxDeviceConfig struct is passed these default values
     * are used
     */
    deviceConfig.multiSample = D3DMULTISAMPLE_NONE;
    deviceConfig.zBufferFormat = D3DFMT_D24S8;
    deviceConfig.presentFlags = D3DPRESENTFLAG_10X11PIXELASPECTRATIO;
    deviceConfig.refreshRateInHz = D3DPRESENT_RATE_DEFAULT;

    /*
     * Initialize the platform independent data 
     * This will in turn initialise the platform specific data
     */
    if (RsEventHandler(rsINITIALIZE, NULL) == rsEVENTERROR)
    {
        return FALSE;
    }

    /* Initialize the 3D (RenderWare) components of the app. */
    if (rsEVENTERROR == RsEventHandler(rsRWINITIALIZE, &deviceConfig))
    {
        RsEventHandler(rsTERMINATE, NULL);
        return FALSE;

    }

    /*
     * Open the pad
     */
    padOpen();


    /*
     * Force a camera resize event
     */
    {
       RwRect              r;

        r.x = 0;
        r.y = 0;
        r.w = RsGlobal.maximumWidth;
        r.h = RsGlobal.maximumHeight;

        RsEventHandler(rsCAMERASIZE, &r);
    }

    /*
     * Enter the message processing loop.
     */
    while (!RsGlobal.quit)
    {
        /* Process the pad */
        padUpdate();

        RsEventHandler(rsIDLE, NULL);
    }

    /*
     * Tidy up the 3D (RenderWare) components of the application.
     */
    RsEventHandler(rsRWTERMINATE, NULL);

    /* free the platform dependent data */
    RsEventHandler(rsTERMINATE, NULL);
    
	/*
	 *    return to the dashboard or you get a silly debug message
	 */
	XLaunchNewImage( NULL, NULL );

    return 0;
}

/*
 *****************************************************************************
 */
