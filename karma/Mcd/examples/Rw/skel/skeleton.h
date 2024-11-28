#ifndef SKELETON_H
#define SKELETON_H

#include <rwcore.h>
#include <rpworld.h>
#ifdef RWTERMINAL
#include "terminal.h"
#endif

#if (defined(_MSC_VER))
#if (_MSC_VER>=1000)

#if (defined(RWDEBUG))

#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif /* _CRTDBG_MAP_ALLOC */

#include <windows.h>
#ifndef UNDER_CE
#include <crtdbg.h>
#endif

#define RSASSERT(_condition) _ASSERTE(_condition)

#endif /* (defined(RWDEBUG)) */

#endif /* (_MSC_VER>=1000) */
#endif /* (defined(_MSC_VER)) */

#if (!defined(RSASSERT))
#define RSASSERT(_condition) /* No op */
#endif /* (!defined(RSASSERT)) */

enum _RsInputDeviceType
{
    rsKEYBOARD,
    rsMOUSE,
    rsPAD,
};
typedef enum _RsInputDeviceType RsInputDeviceType;

enum _RsEventStatus
{
    rsEVENTERROR,
    rsEVENTPROCESSED,
    rsEVENTNOTPROCESSED
};
typedef enum _RsEventStatus RsEventStatus;

enum _RsEvent
{
    rsCAMERASIZE,
    rsCOMMANDLINE,
    rsFILELOAD,
    rsINITDEBUG,
    rsINPUTDEVICEATTACH,
    rsLEFTBUTTONDOWN,
    rsLEFTBUTTONUP,
    rsMOUSEMOVE,
    rsPLUGINATTACH,
    rsREGISTERIMAGELOADER,
    rsRIGHTBUTTONDOWN,
    rsRIGHTBUTTONUP,
    rsRWINITIALISE,
    rsRWTERMINATE,
    rsSELECTDEVICE,
    rsINITIALISE,
    rsTERMINATE,
    rsIDLE,
    rsKEYDOWN,
    rsKEYUP,
    rsQUITAPP,
    rsPADBUTTONDOWN,
    rsPADBUTTONUP,
    rsPADANALOGUELEFT,
    rsPADANALOGUERIGHT
};
typedef enum _RsEvent RsEvent;

typedef RsEventStatus (*RsInputEventHandler)(RsEvent event, void *param);

typedef struct _RsInputDevice RsInputDevice;
struct _RsInputDevice
{
    RsInputDeviceType inputDeviceType;
    RwBool used;
    RsInputEventHandler inputEventHandler;
};

typedef struct _RsGlobalType RsGlobalType;
struct _RsGlobalType
{
    const RwChar *appName;
    RwInt32 maximumWidth;
    RwInt32 maximumHeight;
    RwBool  quit;

    void   *ps; /* platform specific data */

#ifdef RWTERMINAL
    RsTerminal *terminal;
#endif

    RsInputDevice keyboard;
    RsInputDevice mouse;
    RsInputDevice pad;
};

typedef struct _RsMouseStatus RsMouseStatus;
struct _RsMouseStatus
{
    RwV2d   pos;
    RwV2d   delta;
    RwBool  shift;
    RwBool  control;
};

enum _RsKeyCodes
{
    rsESC       = 128,

    rsF1        = 129,
    rsF2        = 130,
    rsF3        = 131,
    rsF4        = 132,
    rsF5        = 133,
    rsF6        = 134,
    rsF7        = 135,
    rsF8        = 136,
    rsF9        = 137,
    rsF10       = 138,
    rsF11       = 139,
    rsF12       = 140,

    rsINS       = 141,
    rsDEL       = 142,
    rsHOME      = 143,
    rsEND       = 144,
    rsPGUP      = 145,
    rsPGDN      = 146,

    rsUP        = 147,
    rsDOWN      = 148,
    rsLEFT      = 149,
    rsRIGHT     = 150,

    rsPADINS    = 151,
    rsPADDEL    = 152,
    rsPADHOME   = 153,
    rsPADEND    = 154,
    rsPADPGUP   = 155,
    rsPADPGDN   = 156,

    rsPADUP     = 157,
    rsPADDOWN   = 158,
    rsPADLEFT   = 159,
    rsPADRIGHT  = 160,

    rsNUMLOCK   = 161,
    rsDIVIDE    = 162,
    rsTIMES     = 163,
    rsMINUS     = 164,
    rsPLUS      = 165,
    rsPADENTER  = 166,
    rsPAD5      = 167,

    rsBACKSP    = 168,
    rsTAB       = 169,
    rsCAPSLK    = 170,
    rsENTER     = 171,
    rsLSHIFT    = 172,
    rsRSHIFT    = 173,
    rsLCTRL     = 174,
    rsRCTRL     = 175,
    rsLALT      = 176,
    rsRALT      = 177,

    rsNULL       = 255
};
typedef enum _RsKeyCodes RsKeyCodes;

typedef struct _RsKeyStatus RsKeyStatus;
struct _RsKeyStatus
{
    RwInt32     keyScanCode;
    RwInt32     keyCharCode;
};

typedef struct _RsPadButtonStatus RsPadButtonStatus;
struct _RsPadButtonStatus
{
    RwInt32     padID;
    RwUInt32    padButtons;
};

enum _RsPadButtons
{
    rsPADDPADLEFT   = 0x00000001,
    rsPADDPADRIGHT  = 0x00000002,
    rsPADDPADUP     = 0x00000004,
    rsPADDPADDOWN   = 0x00000008,
    rsPADSTART      = 0x00000010,
    rsPADSELECT     = 0x00000020,
    rsPADBUTTON1    = 0x00000040,
    rsPADBUTTON2    = 0x00000080,
    rsPADBUTTON3    = 0x00000100,
    rsPADBUTTON4    = 0x00000200,
    rsPADBUTTON5    = 0x00000400,
    rsPADBUTTON6    = 0x00000800,
    rsPADBUTTON7    = 0x00001000,
    rsPADBUTTON8    = 0x00002000,
    rsPADBUTTONA1   = 0x00004000,
    rsPADBUTTONA2   = 0x00008000
};
typedef enum _RsPadButtons RsPadButtons;

extern RsGlobalType RsGlobal;


#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */
extern RsEventStatus            AppEventHandler(RsEvent event, void *param);
extern void                     RsCameraShowRaster(RwCamera *camera);
extern void                     RsErrorMessage(const RwChar *text);
extern RsEventStatus            RsEventHandler(RsEvent event, void *param);
extern RsEventStatus            RsKeyboardEventHandler(RsEvent event, void *param);
extern RsEventStatus            RsMouseEventHandler(RsEvent event, void *param);
extern RsEventStatus            RsPadEventHandler(RsEvent event, void *param);
extern RwBool                   RsInitialise(void);
extern void                     RsMouseSetVisibility(RwBool visible);
extern void                     RsMouseSetPos(RwV2d *pos);
extern RwChar                   RsPathGetSeparator(void);
extern RwChar *                 RsPathnameCreate(const RwChar *srcBuffer);
extern void                     RsPathnameDestroy(RwChar *buffer);
extern RwBool                   RsRegisterImageLoader(void);
extern RwBool                   RsRwInitialise(void *param);
extern void                     RsRwTerminate(void);
extern RwBool                   RsSelectDevice(void);
extern RwBool                   RsInputDeviceAttach(RsInputDeviceType inputDevice, 
                                                    RsInputEventHandler inputEventHandler);
extern void                     RsTerminate(void);
extern RwBool                   RsAlwaysOnTop(RwBool AlwaysOnTop);
extern RwUInt32                 RsTimer(void);
extern void                     RsWarningMessage(const RwChar *text);
extern void                     RsWindowSetText(const RwChar *text);
extern RwUInt8                  RsKeyFromScanCode(RwUInt8 scan, RwBool ShiftKeyDown);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* SKELETON_H */
