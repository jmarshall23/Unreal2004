/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

    $Date: 2002/04/19 18:30:20 $ $Revision: 1.174.2.14 $

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <MeProfile.h>

#if 0
#undef WITH_D3D
#endif

#include "RMouseCam.h"

#ifdef _XBOX
#include <xtl.h>
#include "Render_xbox.h"
#endif

#ifdef WITH_D3D
#include "MeViewer_d3d.h"
#endif

#ifdef WITH_OPENGL
#include "Init_ogl.h"
#include "Render_ogl.h"
#endif

#ifdef WITH_NULL
#include "Render_null.h"
#endif

#ifdef WITH_BENCHMARK
#include "Render_benchmark.h"
#endif

#include <MeViewer.h>
#include <MeMemory.h>
#include <RGeometryUtils.h>
#include <MeCommandLine.h>
#include <MeVersion.h>
#include <MeString.h>

/* define SLOW_GRAPHICS if your machine's rendering is too jerky */
/* #if 0 */
#ifdef WITH_PS2
#define SLOW_GRAPHICS
#endif

#ifdef WIN32
/* if VC++ 6.0 or greater use delayed loading */

#if _MSC_VER >= 1200 && (defined _ME_OGL_DELAYLOAD || defined _ME_D3D_DELAYLOAD)
#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* necessary before delayimp.h */
#include <delayimp.h>
#define ME_DEFINE_DELAYLOAD_HOOK /* Undefine this if you want your own Error Hook */
#pragma comment(lib,"delayimp.lib")
#endif /* VC++ >= 6 and Delay Loading requested */

# ifndef _NO_RENDER_AUTO_LIB_INCLUDE

#   ifdef WITH_OPENGL /* Link with OpenGl libraries */
#     pragma comment(lib,"opengl32.lib")
#     pragma comment(lib,"glut32.lib")
#     if _MSC_VER >= 1200 && defined _ME_OGL_DELAYLOAD
#        pragma comment(linker,"/delayload:opengl32.dll")
#        pragma comment(linker,"/delayload:glut32.dll")
#     endif /* Delay Loading requested */
#   endif /* OGL requested */

#   ifdef WITH_D3D /* Link with Direct3D libraries */
#     pragma comment(lib,"dxguid.lib")
#     pragma comment(lib,"DDRAW.LIB")
#     if _MSC_VER >= 1200 && defined _ME_D3D_DELAYLOAD
#        pragma comment(linker,"/delayload:DDRAW.dll")
#     endif /* Delay Loading requested */
#   endif /* D3D requested */
#  endif /* _NO_RENDER_AUTO_LIB_INCLUDE */

#endif /* Win32 */

/*
   Redzones for drawing e.g. text in case our display device does not show
   the whole 640x448 viewport! These can usefully be set in
   platform-specific headers.
*/
#ifndef X_MINVISIBLE

#ifdef _XBOX
#define X_MINVISIBLE (65)
#else
#define X_MINVISIBLE (0)
#endif

#endif

/* To make performance bar visible, and line up ticks with bar etc. */
#define X_PERFBAROFFSET X_MINVISIBLE

#ifndef X_MAXVISIBLE

#ifdef _XBOX
#define X_MAXVISIBLE (640)
#else
#define X_MAXVISIBLE (640)
#endif

#endif

#ifndef Y_MINVISIBLE

#ifdef _XBOX
#define Y_MINVISIBLE (20)
#else
#define Y_MINVISIBLE (0)
#endif

#endif

#ifndef Y_MAXVISIBLE

#ifdef _XBOX
#define Y_MAXVISIBLE (448-20)
#else
#define Y_MAXVISIBLE (448)
#endif

#endif

#ifdef SLOW_GRAPHICS
#ifndef SPHERE_RES
#define SPHERE_RES (8)
#endif
#ifndef CYL_RES
#define CYL_RES (8)
#endif
#ifndef CONE_RES
#define CONE_RES (8)
#endif
#ifndef TORUS_RES
#define TORUS_RES (8)
#endif
#else
#ifndef SPHERE_RES
#define SPHERE_RES (12)
#endif
#ifndef CYL_RES
#define CYL_RES (12)
#endif
#ifndef CONE_RES
#define CONE_RES (12)
#endif
#ifndef TORUS_RES
#define TORUS_RES (12)
#endif
#endif

/* Alignment should not be required anymore */
/*#ifndef PS2*/
#undef MEASSERTALIGNED
#define MEASSERTALIGNED(x,y)    ((void) 0)
/*#endif */

struct _rmousedrag RMouseDrag;

int      (*RInit) (RRender *) = 0;
void     (*RRun)(RRender* rc, RMainLoopCallBack func, void *userdata) = 0;
void     (*RRenderSetWindowTitle)(RRender* rc, const char* title) = 0;

RGraphic * MEAPI RGraphicDomeCreate(RRender *rc,
    AcmeReal radius, int tileU, int tileV,
    const float color[4], MeMatrix4Ptr matrix);

char *R_LastObjectFilename = 0;
char *R_ObjectFileBuffer = 0;
int  R_ObjectFileLength = 0;

int Rtimeout = 0;
MeProfileLogModes_enum Rprofiling = kMeProfileDontLog;

int R_CharacterWidths[10][10] = {
    { 6, 6, 6, 10, 10, 14, 12, 2, 6, 6 },      /* Space ! " # $ % & ' ( ) */
    { 6, 10, 4, 6, 2, 6, 10, 8, 10, 10 },       /* * + , - . / 0 1 2 3 */
    { 10, 10, 10, 10, 10, 10, 4, 4, 10, 10 },   /* 4 5 6 7 8 9 : ; < = */
    { 10, 10, 18, 12, 12, 12, 13, 12, 10, 14 }, /* > ? @ A B C D E F G */
    { 12, 5, 8, 12, 10, 15, 13, 14, 12, 14 },   /* H I J K L M N O P Q */
    { 14, 12, 12, 13, 12, 18, 12, 12, 11, 6 },  /* R S T U V W X Y Z [ */
    { 6, 4, 8, 10, 4, 10, 10, 10, 10, 10 },     /* \ ] ^ _ ` a b c d e */
    { 6, 10, 10, 4, 4, 10, 4, 15, 10, 10 },     /* f g h i j k l m n o */
    { 10, 10, 6, 9, 6, 10, 10, 14, 10, 10 },    /* p q r s t u v w x y */
    { 10, 6, 4, 6, 10, 0, 0, 0, 0, 0 }          /* z { | } ~ */
};


#ifdef ME_DEFINE_DELAYLOAD_HOOK

/*
  This is the failure hook
*/

FARPROC WINAPI
DelayErrorHook (unsigned int dliNotify, PDelayLoadInfo dli)
{
    int i = 0;

    char buff [256];

    LPVOID lpMsgBuf;

    FormatMessage
        ( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
        , NULL
        , dli->dwLastError
        , MAKELANGID(LANG_NEUTRAL
        , SUBLANG_DEFAULT)
        , (LPTSTR) &lpMsgBuf
        , 0
        , NULL);


    snprintf (buff, sizeof (buff) / sizeof (buff[0])

        , "There was an error loading %s\n%s"

            "\n"

            "You might like to select an alternative renderer using the arguments:\n"

            "\n"

            "    -d3d (for direct 3d)\n"

            "    -gl (for open gl)"

        , dli->szDll, lpMsgBuf);
    LocalFree (lpMsgBuf);



    buff [sizeof (buff) / sizeof (buff[0]) - 1] = '\0';


    MessageBox
        ( 0
        , buff
        , "Fatal Error"
        , MB_OK | MB_TASKMODAL);

    return 0;                   /* Throw an exception */
} /* DelayErrorHook (unsigned, PDelayLoadInfo) */

#endif /* ME_DEFINE_DELAYLOAD_HOOK */

void MEAPI RDisplayBanner(void)
{
    MeInfo(0, "");
    MeInfo(0, ME_PRODUCT_NAME          " (v " ME_VERSION_STRING ")");
    MeInfo(0, ME_COPYRIGHT_LINE1"");
    MeInfo(0, "");
}

void MEAPI RDisplayCommandLineHelp(void)
{
    MeInfo(0, "");
    MeInfo(0, "Command line options:");
#ifdef WITH_OPENGL
    MeInfo(0, "\t-gl\t\t- Run using OpenGL");
#endif
#ifdef WITH_D3D
    MeInfo(0, "\t-d3d\t\t- Run using Direct3D");
#endif
#ifdef WITH_NULL
    MeInfo(0, "\t-null\t\t- Run with no rendering");
#endif
#ifdef WITH_BENCHMARK
    MeInfo(0, "\t-bench\t\t- Run in benchmark mode");
#endif
    MeInfo(0, "\t-timeout n\t- Timeout in n frames");
    MeInfo(0, "\t-profile n\t- Profile for n frames");
    MeInfo(0, "\t-wireframe\t- Run in wireframe mode");
    MeInfo(0, "\t-notextures\t- Run without textures");

}


/**
    Parses command-line parameters

    @param roptions The RRenderOptions struct to place the results in.
    @param options The full set of options passed in to our program.
    @param eat Should the Renderer eat the options it recognises?
*/
void MEAPI RInternalParseRenderType
(RRender* rc, MeCommandLineOptions *options, MeCommandLineOptions *overrides, MeBool eat)
{
    MeCommandLineOptions* current;

    if(!options)
        MeFatalError(0,"RInternalParseRenderType: You must pass a valid MeCommandLineOptions* to RInternalParseRenderType.");
    if(!rc)
        MeFatalError(0,"RInternalParseRenderType: You must pass a valid RRender* to RInternalParseRenderType.");

    /* Just in case no renderer is specified */
    rc->m_options.m_renderType = kRDefault;
    rc->m_options.m_bDisplayHelpOnConsole = MEFALSE;
        rc->m_options.m_bCalibrateTimer = 1;
    /* Check for relevant command line arguments */

    for(current = options; current; current = options==current ? overrides : 0) {

        /* Timeout? */
        if(!Rtimeout)
            Rtimeout = MeCommandLineOptionsGetNumeric(current, "-timeout", eat);

        /* Profile? */
        if(MeCommandLineOptionsCheckFor(current, "-profile", 0))
        {
            MeInfo(0, "Profiling is switched on.");
            Rprofiling = kMeProfileLogTotals;
            Rtimeout = MeCommandLineOptionsGetNumeric(current,
                "-profile", eat);
        }

        /* Always dump the basic help out ... */
        rc->m_options.m_bDisplayHelpOnConsole = !MEFALSE;

        /* Renderer type */

        if(MeCommandLineOptionsCheckFor(current, "-default", eat))
            rc->m_options.m_renderType = kRDefault;

        if(MeCommandLineOptionsCheckFor(current, "-gl", eat))
            rc->m_options.m_renderType = kROpenGL;

        if(MeCommandLineOptionsCheckFor(current, "-d3d", eat))
            rc->m_options.m_renderType = kRD3D;

        if(MeCommandLineOptionsCheckFor(current, "-ps2", eat))
            rc->m_options.m_renderType = kRPs2;

        if(MeCommandLineOptionsCheckFor(current, "-ps2hig", eat))
            rc->m_options.m_renderType = kRPs2HiG;

        if(MeCommandLineOptionsCheckFor(current, "-null", eat))
            rc->m_options.m_renderType = kRNull;

        if(MeCommandLineOptionsCheckFor(current, "-bench", eat))
            rc->m_options.m_renderType = kRBenchmark;

        /* Wireframe, Textures */

        if(MeCommandLineOptionsCheckFor(current, "-wireframe", eat))
            rc->m_options.m_bWireFrame = !MEFALSE;
        else
            rc->m_options.m_bWireFrame = MEFALSE;

        if(MeCommandLineOptionsCheckFor(current, "-notextures", eat))
            rc->m_options.m_bTextures = MEFALSE;
        else
            rc->m_options.m_bTextures = !MEFALSE;
    }

    if(Rtimeout)
        MeInfo(0, "Will timeout after %d frames.", Rtimeout);
}

/**
    Creates a new render context.

    @param options input from the command line
        @param overrideoptions used for overriding command line parameters
        @param eat specifies whether command line options should be eaten
        @param pWnd Used for Direct3D only. Instead of the renderer
        creating a window the user can pass one in. The user must also provide
        their own resources. If this method is used you should not call RRun()
        but should call Render_D3D() directly. This parameter should be NULL
        if you want the renderer to create its own window or you are not using
        Direct3D.

        The function RRenderContextCreate() is the same but without the pWnd
        argument.
    Calls RInit in platform-dependent code.
    Return value is zero if creation or RInit fail.
*/
RRender * MEAPI RRenderContextCreate2(
    MeCommandLineOptions* options
    , MeCommandLineOptions* overrideoptions
    , MeBool eat,void *pWnd,RCreateCallback cb)
{
    int i;
    RRender *rc = 0;

#if defined WIN32 && _MSC_VER >= 1200 \
        && (defined _ME_OGL_DELAYLOAD || defined _ME_D3D_DELAYLOAD)
    /*
      Set the failure handle for delay-loading
     */
    __pfnDliFailureHook = DelayErrorHook;
#endif

    rc = (RRender*)MeMemoryAPI.create(sizeof(RRender));

    if( !rc )
        return 0;

    /* We allow user code to force behaviour in certain situations, e.g. for
       testing we sometimes want to force the renderer to be null. */
    RInternalParseRenderType(rc, options, overrideoptions, eat);

#ifdef WITH_D3D
        rc->D3D = NULL;
#endif
        rc->m_sphereRes = SPHERE_RES;
    rc->m_AspectRatio = (AcmeReal)(640.0f/448.0f);
    rc->m_Fov = ME_PI/6;

    rc->m_bUseAmbientLight = 1;
    rc->m_rgbAmbientLightColor[0] = 0.2f;
    rc->m_rgbAmbientLightColor[1] = 0.2f;
    rc->m_rgbAmbientLightColor[2] = 0.2f;

    rc->m_DirectLight1.m_bUseLight = 1;
    rc->m_DirectLight1.m_Direction[0] = 0.86f;
    rc->m_DirectLight1.m_Direction[1] = 0.26f;
    rc->m_DirectLight1.m_Direction[2] = 0.43f;
    rc->m_DirectLight1.m_rgbAmbient[0] = 0.7f;
    rc->m_DirectLight1.m_rgbAmbient[1] = 0.7f;
    rc->m_DirectLight1.m_rgbAmbient[2] = 0.7f;
    rc->m_DirectLight1.m_rgbDiffuse[0] = 0.6f;
    rc->m_DirectLight1.m_rgbDiffuse[1] = 0.6f;
    rc->m_DirectLight1.m_rgbDiffuse[2] = 0.6f;
    rc->m_DirectLight1.m_rgbSpecular[0] = 0.5f;
    rc->m_DirectLight1.m_rgbSpecular[1] = 0.5f;
    rc->m_DirectLight1.m_rgbSpecular[2] = 0.5f;
    rc->m_bForceDirectLight1Update = 1;

    rc->m_DirectLight2.m_bUseLight = 0;
    rc->m_DirectLight2.m_Direction[0] = 1;
    rc->m_DirectLight2.m_Direction[1] = 0;
    rc->m_DirectLight2.m_Direction[2] = 0;
    rc->m_DirectLight2.m_rgbAmbient[0] = 0.2f;
    rc->m_DirectLight2.m_rgbAmbient[1] = 0.2f;
    rc->m_DirectLight2.m_rgbAmbient[2] = 0.2f;
    rc->m_DirectLight2.m_rgbDiffuse[0] = 0.7f;
    rc->m_DirectLight2.m_rgbDiffuse[1] = 0.7f;
    rc->m_DirectLight2.m_rgbDiffuse[2] = 0.7f;
    rc->m_DirectLight2.m_rgbSpecular[0] = 0.5f;
    rc->m_DirectLight2.m_rgbSpecular[1] = 0.5f;
    rc->m_DirectLight2.m_rgbSpecular[2] = 0.5f;
    rc->m_bForceDirectLight2Update = 1;

    rc->m_PointLight.m_bUseLight = 0;
    rc->m_PointLight.m_Position[0] = 0;
    rc->m_PointLight.m_Position[1] = 0;
    rc->m_PointLight.m_Position[2] = 0;
    rc->m_PointLight.m_rgbAmbient[0] = 0.2f;
    rc->m_PointLight.m_rgbAmbient[1] = 0.2f;
    rc->m_PointLight.m_rgbAmbient[2] = 0.2f;
    rc->m_PointLight.m_rgbDiffuse[0] = 0.7f;
    rc->m_PointLight.m_rgbDiffuse[1] = 0.7f;
    rc->m_PointLight.m_rgbDiffuse[2] = 0.7f;
    rc->m_PointLight.m_rgbSpecular[0] = 0.5f;
    rc->m_PointLight.m_rgbSpecular[1] = 0.5f;
    rc->m_PointLight.m_rgbSpecular[2] = 0.5f;
    rc->m_PointLight.m_AttenuationConstant = 1.0f;
    rc->m_PointLight.m_AttenuationLinear = 0.0f;
    rc->m_PointLight.m_AttenuationQuadratic = 0.0f;
    rc->m_bForcePointLightUpdate = 1;

    rc->m_backgroundColour[0] = 0;
    rc->m_backgroundColour[1] = 0;
    rc->m_backgroundColour[2] = 0;

    rc->m_pRG_First = 0;
    rc->m_pRG_First2D = 0;
    rc->m_pPS_First = 0;
    rc->m_pNL_First = 0;

    rc->m_bQuitNextFrame = 0;
    rc->m_bPause = 0;

    rc->m_isHelpDisplayed = 1;
    rc->m_UserHelpG = 0;
    rc->m_fpsG = 0;
    rc->m_PausedG = 0;

    rc->m_PerformanceBar = 0;

    rc->m_nTimeout = (unsigned)Rtimeout;
    rc->m_bProfiling = Rprofiling;

    for( i = 0; i < 16; i++ )
        rc->m_ButtonText[i][0] = '\0';

    /* Default Text for variable substitution in RGraphicTextCreate */
    strncat(rc->m_ButtonText[0], "Up", 20);          /* $UP */
    strncat(rc->m_ButtonText[1], "Down", 20);        /* $DOWN */
    strncat(rc->m_ButtonText[2], "Left", 20);        /* $LEFT */
    strncat(rc->m_ButtonText[3], "Right", 20);       /* $RIGHT */

    strncat(rc->m_ButtonText[4], "Up2", 20);         /* $UP2 */
    strncat(rc->m_ButtonText[5], "Down2", 20);       /* $DOWN2 */
    strncat(rc->m_ButtonText[6], "Left2", 20);       /* $LEFT2 */
    strncat(rc->m_ButtonText[7], "Right2", 20);      /* $RIGHT2 */

    strncat(rc->m_ButtonText[8], "Action0", 20);     /* $ACTION0 */
    strncat(rc->m_ButtonText[9], "Action1", 20);     /* $ACTION1 */
    strncat(rc->m_ButtonText[10], "Action2", 20);     /* $ACTION2 */
    strncat(rc->m_ButtonText[11], "Action3", 20);     /* $ACTION3 */
    strncat(rc->m_ButtonText[12], "Action4", 20);     /* $ACTION4 */
    strncat(rc->m_ButtonText[13], "Action5", 20);     /* $ACTION5 */

    strncat(rc->m_ButtonText[14], "Modifier + Mouse",20); /* $MOUSE */
    strncat(rc->m_ButtonText[15], "Joystick", 20);    /* $JOYSTICK */

    /* AppName is empty unless you set it yourself */
    rc->m_AppName[0] = '\0';

    RRenderUpdateProjectionMatrices(rc);

    /* set camera to origin looking forward (+z) */
    rc->m_CameraLookAt[0] = 0;
    rc->m_CameraLookAt[1] = 0;
    rc->m_CameraLookAt[2] = 1;

    rc->m_CameraPhi = rc->m_CameraTheta = 0;
    rc->m_CameraDist = 15;

    RCameraUpdate(rc);

    for( i = 0; i < 25; i++ )
        rc->m_TextureList[i] = 0;

    /* set up callbacks */
    rc->m_pCallBacks = (RRenderCallBacks*)MeMemoryAPI.createZeroed(sizeof(RRenderCallBacks));
    if( rc->m_pCallBacks == 0 )
    {
        RRenderContextDestroy(rc);
        return 0;
    }

    /* Default keys for actions 3-6 */
    if(NUM_ACTION_CALLBACKS >= 6) {
        rc->m_pCallBacks->m_actionKeys[0] = '\r';
        rc->m_pCallBacks->m_actionKeys[1] = ' ';
        rc->m_pCallBacks->m_actionKeys[2] = 'o';
        rc->m_pCallBacks->m_actionKeys[3] = 'p';
    }

    rc->m_MenuCurrent = 0;
    rc->m_MenuDefault = 0;

    rc->m_Skydome = 0;
    MEASSERTALIGNED(rc->m_SkydomeMatrix, 16);
    MeMatrix4TMMakeIdentity(rc->m_SkydomeMatrix);

    rc->m_lines = 0;
    rc->m_numLines = 0;
    rc->m_usedLines = 0;

        /* call callback so that params can be changed */
        if (cb) cb(rc);

    if (rc->m_options.m_renderType == kRDefault)
    {
#if   (defined _XBOX) 
        rc->m_options.m_renderType = kRXbox;
#elif (defined WITH_D3D)
        rc->m_options.m_renderType = kRD3D;
#elif (defined WITH_OPENGL)
        rc->m_options.m_renderType = kROpenGL;
#elif (defined WITH_PS2)
        rc->m_options.m_renderType = kRPs2;
#elif (defined WITH_PS2HIG)
        rc->m_options.m_renderType = kRPs2HiG;
#elif (defined WITH_NULL)
        rc->m_options.m_renderType = kRNull;
#elif (defined WITH_BENCHMARK)
        rc->m_options.m_renderType = kRBenchmark;
#endif
    }

    switch( rc->m_options.m_renderType )
    {
    case kRXbox:
#ifndef _XBOX
        MeInfo(0,"Xbox renderer unavailable (_XBOX) undefined.");
        RRenderContextDestroy(rc);
        return 0;
#else
        RRun = Xbox_RunApp;
        RRenderSetWindowTitle = Xbox_SetWindowTitle;
        Xbox_CreateRenderer(rc,pWnd);
#endif
        break;

    case kRD3D:
#ifndef WITH_D3D
        MeInfo(0,"D3D renderer unavailable.");
        RRenderContextDestroy(rc);
        return 0;
#else
        RRun = D3D_RunApp;
        RRenderSetWindowTitle = D3D_SetWindowTitle;
        if( D3D_CreateRenderer(rc,pWnd) )
        {
            MeInfo(0,"D3D render init failed, trying OpenGL renderer.");
            RRun = OGL_RunApp;
            RRenderSetWindowTitle = OGL_SetWindowTitle;
            if( OGL_CreateRenderer(rc) )
            {
                ME_REPORT(MeWarning(1, "OpenGL Renderer initialisation failed."));
                RRenderContextDestroy(rc);
                return 0;
            }
        }

#endif
        break;

    case kRPs2HiG:
#ifndef WITH_PS2HIG
        MeInfo(0,"PS2 HiG renderer unavailable.");
        RRenderContextDestroy(rc);
        return 0;
#else
        RRun = Ps2HiG_RunApp;
        RRenderSetWindowTitle = Ps2HiG_SetWindowTitle;
        if( Ps2HiG_CreateRenderer(rc) )
        {
            ME_REPORT(MeWarning(1, "Renderer initialisation failed."));
            RRenderContextDestroy(rc);
            return 0;
        }
#endif
        break;


    case kRNull:
#ifndef WITH_NULL
        MeInfo(0,"Null renderer unavailable.");
        RRenderContextDestroy(rc);
        return 0;
#else
        MeInfo(1,"Using Null renderer.");
        RRun = Null_RunApp;
        RRenderSetWindowTitle = Null_SetWindowTitle;
        if( Null_CreateRenderer(rc) )
        {
            ME_REPORT(MeWarning(1, "Renderer initialisation failed."));
            RRenderContextDestroy(rc);
            return 0;
        }
#endif
        break;


    case kRBenchmark:
#ifndef WITH_BENCHMARK
        MeInfo(0,"Benchmark renderer unavailable.");
        RRenderContextDestroy(rc);
        return 0;
#else
        MeInfo(1,"Using Benchmark renderer.");
        RRun = Bench_RunApp;
        RRenderSetWindowTitle = Bench_SetWindowTitle;
        if( Bench_CreateRenderer(rc) )
        {
            ME_REPORT(MeWarning(1, "Renderer initialisation failed."));
            RRenderContextDestroy(rc);
            return 0;
        }
#endif
        break;


    case kROpenGL:
#ifndef WITH_OPENGL
        MeInfo(0,"OpenGL renderer unavailable.");
        RRenderContextDestroy(rc);
        return 0;
#else
        RRun = OGL_RunApp;
        RRenderSetWindowTitle = OGL_SetWindowTitle;
        if( OGL_CreateRenderer(rc) )
        {
            ME_REPORT(MeWarning(1, "Renderer initialisation failed."));
            RRenderContextDestroy(rc);
            return 0;
        }
#endif
        break;

    default:
        MeInfo(1,"Chosen renderer does not exist.");
        RRenderContextDestroy(rc);
        return 0;
    }

    return rc;
}

/**
    Cleans up and frees a Render Context.

    @param rc The render context to destroy.
*/
int MEAPI RRenderContextDestroy( RRender *rc )
{
    int i;
#ifdef WITH_D3D
        if (rc->D3D)
                MeMemoryAPI.destroy(rc->D3D);
#endif
    /* make sure user help is in list */
    if( rc->m_UserHelpG && !rc->m_isHelpDisplayed )
        RGraphicAddToList(rc, rc->m_UserHelpG, 1);

    if (rc->m_PausedG) {
        RGraphicDelete(rc,rc->m_PausedG,1);
    }

    /* step through object lists calling RGraphicDelete */
    while(rc->m_pRG_First){ RGraphicDelete(rc, rc->m_pRG_First, 0); }
    while(rc->m_pRG_First2D){ RGraphicDelete(rc, rc->m_pRG_First2D, 1); }
    while(rc->m_pNL_First) { RNLineDelete(rc, rc->m_pNL_First); }
    
    /* destroy any lines that were in the pool but not drawn. */
    for(i=rc->m_usedLines; i<rc->m_numLines; i++)
    {
        RNLineDestroy(rc->m_lines[i]);
    }

    i=0;
    while(rc->m_TextureList[i]){ MeMemoryAPI.destroy(rc->m_TextureList[i++]);}
    while(rc->m_pPS_First)
    {
        RParticleSystem *curr = rc->m_pPS_First;
        RParticleSystemRemoveFromList(rc, curr);
        RParticleSystemDestroy(curr);
    }

    if( rc->m_PerformanceBar )
        MeMemoryAPI.destroy(rc->m_PerformanceBar);

    MeMemoryAPI.destroy(rc->m_pCallBacks);

    if(rc->m_lines)
        MeMemoryAPI.destroy(rc->m_lines);

    MeMemoryAPI.destroy(rc);

    if(R_LastObjectFilename)
        MeMemoryAPI.destroy(R_LastObjectFilename);

    if(R_ObjectFileBuffer)
        MeMemoryAPI.destroy(R_ObjectFileBuffer);

    MeInfo(1, "Render Context destroyed");

    return 0;
}

/**
    Tells the back-end to quit next frame.

    @param rc The render context to quit.
*/
void MEAPI RRenderQuit( RRender *rc )
{
    rc->m_bQuitNextFrame = 1;
}

void MEAPI RRenderUpdateProjectionMatrices( RRender *rc )
{
    /*
      set default projection matrix
      {1/aspect_ratio * cot(FOV),0,0,0,
      0,cot(FOV),0,0,
      0,0,far/(far-near),1,
      0,0,-(far/(far-near))*near,0}

      default gives a near z of 0.05 and far z of 100, FOV 60 deg(y)

      wfgg 20001211: Moved near z to 0.30 to reduce `flimmering'.
    */
    MeMatrix4SetZero(rc->m_ProjMatrix);

    rc->m_ProjMatrix[0][0] = 1/rc->m_AspectRatio * 1/MeTan(rc->m_Fov);
    rc->m_ProjMatrix[1][1] = 1/MeTan(rc->m_Fov);
    rc->m_ProjMatrix[2][2] = FARZ / ( FARZ - NEARZ );
    rc->m_ProjMatrix[2][3] = 1;
    rc->m_ProjMatrix[3][2] = -(FARZ / ( FARZ - NEARZ )) * NEARZ;

    MeMatrix4SetZero(rc->m_CamMatrix2D);

    rc->m_CamMatrix2D[0][0] = (NEARZ + OFFSET_2D) / (320.0f * rc->m_ProjMatrix[0][0]);
    rc->m_CamMatrix2D[1][1] = -(NEARZ + OFFSET_2D) / (224.0f * rc->m_ProjMatrix[1][1]);
    rc->m_CamMatrix2D[2][2] = 1.0f;
    rc->m_CamMatrix2D[3][0] = -(NEARZ + OFFSET_2D) / rc->m_ProjMatrix[0][0];
    rc->m_CamMatrix2D[3][1] = (NEARZ + OFFSET_2D) / rc->m_ProjMatrix[1][1];
    rc->m_CamMatrix2D[3][2] = (NEARZ + OFFSET_2D);
    rc->m_CamMatrix2D[3][3] = 1.0f;
}


/**
    Create default skydome with given texture.
    Will remain centered on the camera.

    @param rc RRenderContext to create skydome in.
    @param name Filename of texture to use for skydome.
    @param tileU Number of times to repeat texture around dome.
    @param tileV Number of times to repeat texture top-to-bottom.
*/
void MEAPI RRenderSkydomeCreate(RRender *const rc,
    const char * name, int tileU, int tileV)
{
    float white[4] = {1, 1, 1, 1};

    rc->m_Skydome = RGraphicDomeCreate(rc,
        FARZ - 10, tileU, tileV, white, rc->m_SkydomeMatrix);
    RGraphicSetTexture(rc, rc->m_Skydome, name);

    rc->m_Skydome->m_pObject->m_ColorEmissive[0] = 1;
    rc->m_Skydome->m_pObject->m_ColorEmissive[1] = 1;
    rc->m_Skydome->m_pObject->m_ColorEmissive[2] = 1;
    rc->m_Skydome->m_pObject->m_ColorEmissive[3] = 1;
}

/**
    Set the pause state of the renderer.
*/
void MEAPI RRenderSetPause( RRender *rc, int p)
{
    rc->m_bPause = p;
}


/**
    Sets the transform pointer for an RGraphic.

    @param g The RGraphic in question.
    @param matrix The transformation matrix to assign to g.
*/
void MEAPI RGraphicSetTransformPtr(RGraphic *g, MeMatrix4Ptr matrix)
{
    /* ToDo: Ensure it's really an affine transformation matrix */
    MEASSERTALIGNED(matrix, 16);
    g->m_pLWMatrix = matrix;
}

/**
    Sets the color of an RGraphic.

    @param g The RGraphic in question.
    @param color The RGBA colour to assign to the RGraphic. N.B. This sets
    the ambient and diffuse components of the colour; the emissive and
    specular components are zeroed.
*/

void  MEAPI RGraphicSetColor(RGraphic *g, const float color[4])
{
    float emissive[] = {0,0,0,1};
    float specular[] = {0,0,0,1};
    int i;
    for (i=0; i<4; i++) {
        g->m_pObject->m_ColorAmbient[i] = color[i];
        g->m_pObject->m_ColorDiffuse[i] = color[i];
        g->m_pObject->m_ColorEmissive[i] = emissive[i];
        g->m_pObject->m_ColorSpecular[i] = specular[i];
    }
}

/**
    Turns wireframe on or off for a graphic.
*/
void MEAPI RGraphicSetWireframe(RGraphic *g, MeBool b)
{
    g->m_pObject->m_bIsWireFrame = b;
}

/**
    Returns wireframe state.
*/
MeBool MEAPI RGraphicIsWireframe(RGraphic *g)
{
        return g->m_pObject->m_bIsWireFrame;
}

/**
    Gets the color of an RGraphic.

    @param g The RGraphic in question.
    @param color The returned RGBA colour of the RGraphic.
*/

void  MEAPI RGraphicGetColor(RGraphic *g, float color[4])
{
    int i;
    for (i=0; i<4; i++)
        color[i] = g->m_pObject->m_ColorAmbient[i];
}

/**
    Updates RObjectHeader matrices from RGraphic matrix pointers
    Also used for timeout and pause graphic.

    Called each frame by the back-end.

    @param rc The render context all of whose RGraphics' matrices are to be updated.
*/
void MEAPI RRenderUpdateGraphicMatrices( RRender *rc )
{
    static unsigned Rframecount = 0;
    static int pausedisplayed = 0;
    RGraphic *current;

    /* See if we have to timeout */
    Rframecount++;
    if( rc->m_nTimeout && Rframecount >= rc->m_nTimeout ) {
        MeInfo(0, "Timeout reached after %d frames. Quitting...",
               Rframecount);
        RRenderQuit(rc);
    }

    if( rc->m_Skydome )
    {
        AcmeVector3 pos;
        RCameraGetPosition(rc, pos);
        rc->m_SkydomeMatrix[3][0] = pos[0];
        rc->m_SkydomeMatrix[3][1] = pos[1];
        rc->m_SkydomeMatrix[3][2] = pos[2];
    }
    
    /* Copy matrix from m_pLWMatrix to m_pObject->m_Matrix for all objects */
    
    for(current = rc->m_pRG_First; current; current = current->m_pNext) {
        if( current->m_pLWMatrix ) {
            /*             MEASSERTALIGNED(current->m_pObject->m_Matrix, 16); */
            /*             MEASSERTALIGNED(current->m_pLWMatrix, 16); */
            /*             MeMatrix4CopyAligned( current->m_pObject->m_Matrix, current->m_pLWMatrix); */
            MeMatrix4Copy( current->m_pObject->m_Matrix, current->m_pLWMatrix);
        }
    }
    
    for(current = rc->m_pRG_First2D; current; current = current->m_pNext) {
        if( current->m_pLWMatrix ) {
            /*             MeInfo(5, "matrix at %x\n", current->m_pObject->m_Matrix); */
            /*             MEASSERTALIGNED(current->m_pObject->m_Matrix, 16); */
            /*             MEASSERTALIGNED(current->m_pLWMatrix, 16); */
            /*             MeMatrix4CopyAligned(current->m_pObject->m_Matrix, current->m_pLWMatrix); */
            MeMatrix4Copy(current->m_pObject->m_Matrix, current->m_pLWMatrix);
        }
    }
    
    /* Native lines */
    {
        RNativeLine *curLine;
        for(curLine = rc->m_pNL_First; curLine; curLine = curLine->m_next)
        {
            if( curLine->m_pMatrix )
            {
                MeMatrix4Copy( curLine->m_matrix, curLine->m_pMatrix);
            }
        }
    }
    

    /* Display pause graphic */
    if( !rc->m_PausedG )
    {
        const float textcol[4] = {0.6f,0.6f,0.6f,1.0f};
        rc->m_PausedG = RGraphicTextCreate(rc, "Paused", 575, 0, textcol);
        RGraphicRemoveFromList(rc, rc->m_PausedG, 1);
    }
    if( rc->m_bPause ) {
        if( !pausedisplayed )
        {
            RGraphicAddToList(rc, rc->m_PausedG, 1);
            pausedisplayed = 1;
        }
    }
    else if( pausedisplayed )
    {
        RGraphicRemoveFromList(rc, rc->m_PausedG, 1);
        pausedisplayed = 0;
    }
}


/**
    Allocates memory for RGraphic

    @param numVertices specifies how many vertices in RGraphic. Should be multiple of 3.
    @return A pointer to the resulting RGraphic, or 0 for failure.

    Fills in numVertices and pointers in RGraphic
*/
RGraphic* MEAPI RGraphicCreateEmpty( int numVertices )
{
    RGraphic *rg = (RGraphic *)MeMemoryAPI.create(sizeof(RGraphic));
    if( !rg )
        return 0;

    rg->m_pNext = 0;
    rg->m_nMaxNumVertices = numVertices;
    rg->m_pObject = (RObjectHeader *)MeMemoryAPI.createZeroed((sizeof(RObjectHeader)+(sizeof(RObjectVertex)*numVertices)) );

    rg->m_pVertices = (RObjectVertex *)( ((char *)rg->m_pObject) + sizeof(RObjectHeader));

#ifdef _XBOX

    D3DDevice_CreateVertexBuffer( numVertices*sizeof(CUSTOMVERTEX),
                                                  D3DUSAGE_WRITEONLY, 
                                                  D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &rg->m_pVertexBuffer);

#endif

#ifdef PS2
    rg->m_pVertexArray   = (AcmeReal *)MeMemoryAPI.createZeroed(numVertices*3*sizeof(AcmeReal));
    rg->m_pNormalArray   = (AcmeReal *)MeMemoryAPI.createZeroed(numVertices*3*sizeof(AcmeReal));
    rg->m_pTexCoordArray = (AcmeReal *)MeMemoryAPI.createZeroed(numVertices*2*sizeof(AcmeReal));
#endif

    rg->m_pObject->m_nNumVertices = numVertices;

    return rg;
}


/**
    Puts RGraphic into render-list in a given render context.

    @param rc The render context containing the list.
    @param rg The graphic to add to the list.
    @param is2D determines whether @param rg is added to 3D or 2D list.

*/
void MEAPI RGraphicAddToList(RRender *rc, RGraphic *rg, int is2D )
{
    /* put objects with alpha < 1 at end, others at beginnning */
    RGraphic *curRG;

#ifdef WITH_OPENGL
    if(  rc->m_options.m_renderType == kROpenGL )
        RGraphicCalcOGLDisplayList(rg);
#endif

    if( is2D )
        curRG = rc->m_pRG_First2D;
    else
        curRG = rc->m_pRG_First;

    if( curRG == 0 ) /* empty list */
    {
        if( is2D )
            rc->m_pRG_First2D = rg;
        else
            rc->m_pRG_First = rg;
        return;
    }

    if( rg->m_pObject->m_ColorAmbient[3] < 1.0f || rg->m_pObject->m_ColorDiffuse[3] < 1.0f || rg->m_pObject->m_ColorSpecular[3] < 1.0f)
    {
        while( curRG->m_pNext ){ curRG = curRG->m_pNext; }
        curRG->m_pNext = rg;
    }
    else
    {
        if( is2D)
        {
            rg->m_pNext = rc->m_pRG_First2D;
            rc->m_pRG_First2D = rg;
        }
        else
        {
            rg->m_pNext = rc->m_pRG_First;
            rc->m_pRG_First = rg;
        }
    }
    return;
}


/**
    Removes RGraphic from render-list.

    @param rc The render context containing the list.
    @param rg The graphic to remove.
    @param is2D specifies whether to look in 2D or 3D list
*/
void MEAPI RGraphicRemoveFromList(RRender *rc, RGraphic *rg, int is2D)
{
    RGraphic *current;
    RGraphic *next;

    if( is2D )
        current = rc->m_pRG_First2D;
    else
        current = rc->m_pRG_First;

    if(current) /* not an empty list */
    {
        next = current->m_pNext;
        if( current == rg ) /* rg is first in list */
        {
            if( is2D )
                rc->m_pRG_First2D = next;
            else
                rc->m_pRG_First = next;
        }
        else
        {
            while(next)
            {
                if(next == rg)
                {
                    current->m_pNext = next->m_pNext;
                    break;
                }
                current = next;
                next = current->m_pNext;
            }
        }
    }
}


/**
    Frees memory allocated for RGraphic

    @param rg The graphic to destroy.

    N.B. This also frees memory allocated for the associated RObjectHeader and vertex list.
*/
void MEAPI RGraphicDestroy(RGraphic *rg)
{
    MeMemoryAPI.destroy(rg->m_pObject);
    MeMemoryAPI.destroy(rg);
}


/**
    Removes RGraphic from list and then frees memory (so no need to call RGraphicDestroy afterwards).

    @param rc The render context that the graphic is associated with.
    @param rg the graphic to delete.
    @param is2D Is the graphic in the 2D list?
*/
void MEAPI RGraphicDelete(RRender *rc, RGraphic *rg, int is2D)
{
    RGraphicRemoveFromList(rc, rg, is2D);
    RGraphicDestroy(rg);
}


/**
    Calculates camera position

    @param rc the render context whose camera is to be interrogated.
    @param pos is filled in with position of camera in world co-ordinates
*/
void MEAPI RCameraGetPosition(RRender *rc, AcmeVector3 pos)
{
    pos[0] = rc->m_CameraLookAt[0] +
        rc->m_CameraDist*(AcmeReal)MeCos(rc->m_CameraPhi)*(AcmeReal)MeSin(rc->m_CameraTheta);
    pos[1] = rc->m_CameraLookAt[1] +
        rc->m_CameraDist*(AcmeReal)MeSin(rc->m_CameraPhi);
    pos[2] = rc->m_CameraLookAt[2] -
        rc->m_CameraDist*(AcmeReal)MeCos(rc->m_CameraPhi)*(AcmeReal)MeCos(rc->m_CameraTheta);
/*     MeVector3Copy(pos, rc->m_CamMatrix[3]); */
}

/** Returns a unit vector in the direction the camera is looking. */
void MEAPI RCameraGetLookDir(RRender *rc, AcmeVector3 dir)
{
    dir[0] = rc->m_CamMatrix[0][2];
    dir[1] = rc->m_CamMatrix[1][2];
    dir[2] = rc->m_CamMatrix[2][2];
}


/**
    Calculates camera 'up' vector in world space.

    @param rc The render context whose camera is to be interrogated.
    @param up Current up vector, calculated from angle and elevation.
*/
void MEAPI RCameraGetUp(RRender *rc, AcmeVector3 up)
{
    up[0] = -(AcmeReal)MeSin(rc->m_CameraPhi) * (AcmeReal)MeSin(rc->m_CameraTheta);
    up[1] = (AcmeReal)MeCos(rc->m_CameraPhi);
    up[2] = (AcmeReal)MeSin(rc->m_CameraPhi) * (AcmeReal)MeCos(rc->m_CameraTheta);
}

/**
    Retrieves camera look-at point

    @param rc The render context whose camera is to be interrogated.
    @param camlookat is used as destination for array of 3 AcmeReals (x,y,z).
*/
void MEAPI RCameraGetLookAt(RRender *rc, AcmeVector3 camlookat)
{
    camlookat[0] = rc->m_CameraLookAt[0];
    camlookat[1] = rc->m_CameraLookAt[1];
    camlookat[2] = rc->m_CameraLookAt[2];
}

/**
    Returns camera distance from lookat.
*/
AcmeReal MEAPI RCameraGetDistance(RRender *rc)
{
    return rc->m_CameraDist;
}

/**
    Returns elevation of camera.
*/
AcmeReal MEAPI RCameraGetElevation(RRender *rc)
{
    return rc->m_CameraPhi;
}

/**
    Sets elevation of camera.
*/
void MEAPI RCameraSetElevation(RRender *rc, const MeReal elevation)
{
    rc->m_CameraPhi = elevation;
    RCameraUpdate(rc);
    rc->m_dirty |= MEV_CAMERA;
}

/**
    Returns elevation of camera.
*/
AcmeReal MEAPI RCameraGetRotation(RRender *rc)
{
    return rc->m_CameraTheta;
}

/**
   Sets rotation of camera.
*/
void MEAPI RCameraSetRotation(RRender *rc, const MeReal rotation)
{
    rc->m_CameraTheta = rotation;
    RCameraUpdate(rc);
    rc->m_dirty |= MEV_CAMERA;
}

/**
   Set distance of camera from lookat point
*/
void MEAPI RCameraSetOffset(RRender* rc, const MeReal offset) {
    rc->m_CameraDist = offset;
    RCameraUpdate(rc);
    rc->m_dirty |= MEV_CAMERA;
}

/**
    Updates Camera Matrix

    Creates camera matrix from look-at and relative position

    @param rc the render context whose camera is to be manipulated.

    N.B. Reads from m_theta, m_phi, m_CameraLookAt. Writes to
    m_CamMatrix.

*/
void MEAPI RCameraUpdate(RRender *rc)
{
    AcmeReal campos[3];
    AcmeReal up[3];
    AcmeReal right[3];
    AcmeReal view[3];
    AcmeReal length;
    AcmeReal trans[3];

    rc->m_dirty |= MEV_CAMERA;

    /* get camera position */
    RCameraGetPosition(rc, campos);

    /* work out camera view */
    view[0] = rc->m_CameraLookAt[0] - campos[0];
    view[1] = rc->m_CameraLookAt[1] - campos[1];
    view[2] = rc->m_CameraLookAt[2] - campos[2];

    /* normalise view */
    length = (AcmeReal)MeSqrt(view[0]*view[0] + view[1]*view[1] + view[2]*view[2]);
    view[0] /= length;
    view[1] /= length;
    view[2] /= length;

#if 0
    /* calculate up-vector (0,1,0)-view.y*view */
    up[0] = 0 - view[1] * view[0];
    up[1] = 1 - view[1] * view[1];
    up[2] = 0 - view[1] * view[2];
#else
    RCameraGetUp(rc, up);
#endif

    /* normalise up */
    length = (AcmeReal)MeSqrt(up[0]*up[0] + up[1]*up[1] + up[2]*up[2]);
    up[0] /= length;
    up[1] /= length;
    up[2] /= length;

    /* right = up X view */
    right[0] = up[1]*view[2] - up[2]*view[1];
    right[1] = up[2]*view[0] - up[0]*view[2];
    right[2] = up[0]*view[1] - up[1]*view[0];

    /* calculate translations */
    trans[0] = -(campos[0]*right[0] + campos[1]*right[1] + campos[2]*right[2]);
    trans[1] = -(campos[0]*up[0] + campos[1]*up[1] + campos[2]*up[2]);
    trans[2] = -(campos[0]*view[0] + campos[1]*view[1] + campos[2]*view[2]);

    rc->m_CamMatrix[0][0]  = right[0];
    rc->m_CamMatrix[0][1]  = up[0];
    rc->m_CamMatrix[0][2]  = view[0];
    rc->m_CamMatrix[0][3]  = 0;
    rc->m_CamMatrix[1][0]  = right[1];
    rc->m_CamMatrix[1][1]  = up[1];
    rc->m_CamMatrix[1][2]  = view[1];
    rc->m_CamMatrix[1][3]  = 0;
    rc->m_CamMatrix[2][0]  = right[2];
    rc->m_CamMatrix[2][1]  = up[2];
    rc->m_CamMatrix[2][2] = view[2];
    rc->m_CamMatrix[2][3] = 0;
    rc->m_CamMatrix[3][0] = trans[0];
    rc->m_CamMatrix[3][1] = trans[1];
    rc->m_CamMatrix[3][2] = trans[2];
    rc->m_CamMatrix[3][3] = 1;
}

/**
   Create camera matrix from lookat, position
*/
void MEAPI RCameraSetLookAtAndPosition(RRender *rc, const MeVector3 lookAt, const MeVector3 position)
{
    const MeVector3 y = {0, 1, 0};
    MeVector3 up;
    AcmeReal right[3];
    MeVector3 view, viewtemp;
    AcmeReal trans[3];

    /* work out camera view */
    MeVector3Subtract (view, lookAt, position);

    /* Resynchronize the angles and distance in rc... */

    MeVector3Copy(rc->m_CameraLookAt, lookAt);
    rc->m_CameraDist = MeVector3Magnitude(view);

    rc->m_CameraPhi = MeAsin(view[1]/rc->m_CameraDist);
    rc->m_CameraTheta = MeAsin(view[0]/(rc->m_CameraDist * MeCos(rc->m_CameraPhi)));

    if(view[2] < 0) {
        if(rc->m_CameraTheta > 0)
            rc->m_CameraTheta = ME_PI - rc->m_CameraTheta;
        else
            rc->m_CameraTheta = -ME_PI - rc->m_CameraTheta;
    }

    /* normalise view */
    MeVector3Normalize(view);

    /* calculate up-vector (0,1,0)-view.y*view */
    MeVector3Copy(viewtemp, view);
    MeVector3Scale(viewtemp, viewtemp[1]);
    MeVector3Subtract(up, y, viewtemp);
    MeVector3Normalize(up);

    /* right = up X view */
    MeVector3Cross(right, up, view);

    /* calculate translations */
    trans[0] = -(position[0]*right[0] + position[1]*right[1] + position[2]*right[2]);
    trans[1] = -(position[0]*up[0] + position[1]*up[1] + position[2]*up[2]);
    trans[2] = -(position[0]*view[0] + position[1]*view[1] + position[2]*view[2]);

    rc->m_CamMatrix[0][0]  = right[0];
    rc->m_CamMatrix[0][1]  = up[0];
    rc->m_CamMatrix[0][2]  = view[0];
    rc->m_CamMatrix[0][3]  = 0;
    rc->m_CamMatrix[1][0]  = right[1];
    rc->m_CamMatrix[1][1]  = up[1];
    rc->m_CamMatrix[1][2]  = view[1];
    rc->m_CamMatrix[1][3]  = 0;
    rc->m_CamMatrix[2][0]  = right[2];
    rc->m_CamMatrix[2][1]  = up[2];
    rc->m_CamMatrix[2][2] = view[2];
    rc->m_CamMatrix[2][3] = 0;
    rc->m_CamMatrix[3][0] = trans[0];
    rc->m_CamMatrix[3][1] = trans[1];
    rc->m_CamMatrix[3][2] = trans[2];
    rc->m_CamMatrix[3][3] = 1;

}


/**
    Sets camera look-at point

    @param rc the render context whose camera is to be interrogated.
    @param lookAt The x,y,z co-ordinates of the new point to look at.
*/
void MEAPI RCameraSetLookAt( RRender *rc, const AcmeVector3 lookAt)
{
    rc->m_CameraLookAt[0] = lookAt[0];
    rc->m_CameraLookAt[1] = lookAt[1];
    rc->m_CameraLookAt[2] = lookAt[2];
    RCameraUpdate(rc);
}


/**
    Sets camera view

    @param rc the render context whose camera is to be manipulated.
    @param dist The distance from the lookAt point to locate the camera.
    @param theta specifies angle in radians
    @param phi specifies elevation in radians
    @sa RCameraSetLookAt
*/
void MEAPI RCameraSetView( RRender *rc, AcmeReal dist, AcmeReal theta, AcmeReal phi )
{
    rc->m_CameraDist = dist;
    rc->m_CameraTheta = theta;
    rc->m_CameraPhi = phi;

#if 0
    if( rc->m_CameraPhi < -((ME_PI /2)-0.1f) )
        rc->m_CameraPhi = -((ME_PI /2)-0.1f);
    if( rc->m_CameraPhi > ((ME_PI /2)-0.1f) )
        rc->m_CameraPhi = ((ME_PI /2)-0.1f);
#else
    if( rc->m_CameraPhi > ME_PI * 2 )
        rc->m_CameraPhi -= ME_PI * 2;
    if( rc->m_CameraPhi < ME_PI * 2 )
        rc->m_CameraPhi += ME_PI * 2;
#endif

    if( rc->m_CameraTheta < -ME_PI )
        rc->m_CameraTheta += 2*ME_PI;
    if( rc->m_CameraTheta > ME_PI )
        rc->m_CameraTheta -= 2*ME_PI;

    RCameraUpdate(rc);
}


/**
    Sets the camera matrix directly.
*/
void MEAPI RCameraSetMatrix( RRender *rc, MeMatrix4Ptr m )
{
    MEASSERTALIGNED(rc->m_CamMatrix, 16);
/*     MeMatrix4CopyAligned(rc->m_CamMatrix,m); */
     MeMatrix4Copy(rc->m_CamMatrix,m);
}

/**
    Sets the field of view.
*/
void MEAPI RCameraSetFov( RRender *rc, AcmeReal fov )
{
    rc->m_Fov = fov;
    RRenderUpdateProjectionMatrices(rc);
}

/**
    Changes camera distance from look-at point.

    @param rc The render context whose camera is to be manipulated.
    @param dist is added to current camera distance.
    Will not allow camera to get < 0.01f from look-at
*/
void MEAPI RCameraZoom( RRender *rc, AcmeReal dist )
{
    rc->m_CameraDist += dist;

    if(rc->m_CameraDist < (NEARZ+0.05f) )
        rc->m_CameraDist = (NEARZ+0.05f);

    RCameraUpdate(rc);
}


/**
    Pans the camera in screen X direction

    @param rc the render context whose camera is to be interrogated.
    @param dist specifies distance to be moved
*/
void MEAPI RCameraPanX( RRender *rc, AcmeReal dist )
{
    /* Move dist in the cam_matrix->right direction */
    rc->m_CameraLookAt[0] += dist * rc->m_CamMatrix[0][0];
    rc->m_CameraLookAt[1] += dist * rc->m_CamMatrix[1][0];
    rc->m_CameraLookAt[2] += dist * rc->m_CamMatrix[2][0];

    RCameraUpdate(rc);
}


/**
    Pans the camera in screen Y direction

    @param rc the render context whose camera is to be interrogated.
    @param dist specifies distance to be moved
*/
void MEAPI RCameraPanY( RRender *rc, AcmeReal dist )
{
    /* Move dist in the cam_matrix->up direction */
    rc->m_CameraLookAt[0] += dist * rc->m_CamMatrix[0][1];
    rc->m_CameraLookAt[1] += dist * rc->m_CamMatrix[1][1];
    rc->m_CameraLookAt[2] += dist * rc->m_CamMatrix[2][1];

    RCameraUpdate(rc);
}


/**
    Pans the camera in screen Z direction

    @param rc the render context whose camera is to be interrogated.
    @param dist specifies distance to be moved
*/
void MEAPI RCameraPanZ( RRender *rc, AcmeReal dist )
{
    /* Move dist in the cam_matrix->view direction */
    rc->m_CameraLookAt[0] += dist * rc->m_CamMatrix[0][2];
    rc->m_CameraLookAt[1] += dist * rc->m_CamMatrix[1][2];
    rc->m_CameraLookAt[2] += dist * rc->m_CamMatrix[2][2];

    RCameraUpdate(rc);
}


/**
    Rotates the camera angle

    @param rc the render context whose camera is to be interrogated.
    @param d_theta specifies angle in radians to be moved
*/
void MEAPI RCameraRotateAngle( RRender *rc, AcmeReal d_theta )
{
    rc->m_CameraTheta += d_theta;
    RCameraUpdate(rc);
}


/**
    Rotates the camera elevation

    @param rc the render context whose camera is to be interrogated.
    @param d_phi specifies angle in radians to be moved
*/
void MEAPI RCameraRotateElevation( RRender *rc, AcmeReal d_phi )
{
    rc->m_CameraPhi += d_phi;
    RCameraUpdate(rc);
}

/* ********
   Callbacks
   ******** */

/**
    Sets the call-back for "Up"

    @param rc The render context whose callback is to be set.
    @param func The callback to assign to this button.
*/
void MEAPI RRenderSetUpCallBackWithText( RRender *rc, RButtonPressCallBack func,
                                 void* userdata , char* text) {
    rc->m_pCallBacks->m_Button[kRButtonUp].m_CallBack = func;
    rc->m_pCallBacks->m_Button[kRButtonUp].m_Userdata = userdata;
    rc->m_pCallBacks->m_Button[kRButtonUp].m_Text     = text;
}


/**
    Sets the call-back for "Down"

    @param rc The render context whose callback is to be set.
    @param func The callback to assign to this button.
*/
void MEAPI RRenderSetDownCallBackWithText( RRender *rc, RButtonPressCallBack func,
                                   void* userdata , char* text) {
    rc->m_pCallBacks->m_Button[kRButtonDown].m_CallBack = func;
    rc->m_pCallBacks->m_Button[kRButtonDown].m_Userdata = userdata;
    rc->m_pCallBacks->m_Button[kRButtonDown].m_Text     = text;
}


/**
    Sets the call-back for "Left"

    @param rc The render context whose callback is to be set.
    @param func The callback to assign to this button.
*/
void MEAPI RRenderSetLeftCallBackWithText( RRender *rc, RButtonPressCallBack func,
                                   void* userdata , char* text) {
    rc->m_pCallBacks->m_Button[kRButtonLeft].m_CallBack = func;
    rc->m_pCallBacks->m_Button[kRButtonLeft].m_Userdata = userdata;
    rc->m_pCallBacks->m_Button[kRButtonLeft].m_Text     = text;
}


/**
    Sets the call-back for "Right"

    @param rc The render context whose callback is to be set.
    @param func The callback to assign to this button.
*/
void MEAPI RRenderSetRightCallBackWithText( RRender *rc, RButtonPressCallBack func,
                                    void* userdata , char* text) {
    rc->m_pCallBacks->m_Button[kRButtonRight].m_CallBack = func;
    rc->m_pCallBacks->m_Button[kRButtonRight].m_Userdata = userdata;
    rc->m_pCallBacks->m_Button[kRButtonRight].m_Text     = text;
}


/**
    Sets the call-back for "Up2"

    @param rc The render context whose callback is to be set.
    @param func The callback to assign to this button.
*/
void MEAPI RRenderSetUp2CallBackWithText( RRender *rc, RButtonPressCallBack func,
                                  void* userdata , char* text) {
    rc->m_pCallBacks->m_Button[kRButtonUp2].m_CallBack = func;
    rc->m_pCallBacks->m_Button[kRButtonUp2].m_Userdata = userdata;
    rc->m_pCallBacks->m_Button[kRButtonUp2].m_Text     = text;
}


/**
    Sets the call-back for "Down2"

    @param rc The render context whose callback is to be set.
    @param func The callback to assign to this button.
*/
void MEAPI RRenderSetDown2CallBackWithText( RRender *rc, RButtonPressCallBack func,
                                    void* userdata , char* text) {
    rc->m_pCallBacks->m_Button[kRButtonDown2].m_CallBack = func;
    rc->m_pCallBacks->m_Button[kRButtonDown2].m_Userdata = userdata;
    rc->m_pCallBacks->m_Button[kRButtonDown2].m_Text     = text;
}


/**
    Sets the call-back for "Left2"

    @param rc The render context whose callback is to be set.
    @param func The callback to assign to this button.
*/
void MEAPI RRenderSetLeft2CallBackWithText( RRender *rc, RButtonPressCallBack func,
                                    void* userdata , char* text) {
    rc->m_pCallBacks->m_Button[kRButtonLeft2].m_CallBack = func;
    rc->m_pCallBacks->m_Button[kRButtonLeft2].m_Userdata = userdata;
    rc->m_pCallBacks->m_Button[kRButtonLeft2].m_Text     = text;
}


/**
    Sets the call-back for "Right2"

    @param rc The render context whose callback is to be set.
    @param func The callback to assign to this button.
*/
void MEAPI RRenderSetRight2CallBackWithText( RRender *rc, RButtonPressCallBack func,
                                     void* userdata , char* text) {
    rc->m_pCallBacks->m_Button[kRButtonRight2].m_CallBack = func;
    rc->m_pCallBacks->m_Button[kRButtonRight2].m_Userdata = userdata;
    rc->m_pCallBacks->m_Button[kRButtonRight2].m_Text     = text;
}

/**
    Sets the mouse call-back

    @param rc The render context whose callback is to be set.
    @param func The callback to assign to this button.
*/
void MEAPI RRenderSetMouseCallBackWithText( RRender *rc, RMouseCallBack func,
                                    void* userdata , char* text) {
    rc->m_pCallBacks->m_Mouse.m_CallBack = func;
    rc->m_pCallBacks->m_Mouse.m_Userdata = userdata;
    rc->m_pCallBacks->m_Mouse.m_Text     = text;
}

/**
    Sets the Joystick call-back

    @param rc The render context whose callback is to be set.
    @param func The callback to assign to this button.
*/
void MEAPI RRenderSetJoystickCallBackWithText( RRender *rc, RJoystickCallBack func,
                                       void* userdata , char* text) {
    rc->m_pCallBacks->m_Joystick.m_CallBack = func;
    rc->m_pCallBacks->m_Joystick.m_Userdata = userdata;
    rc->m_pCallBacks->m_Joystick.m_Text     = text;
}

/**
   Sets the call-back for the Nth Action

   @param rc The render context whose callback is to be set.
   @param N The number of the callback which is to be set. The standard
   version of the renderer allows values from 0 to 5.
   @param func The function to call when the specified action key is
   pressed.

   @param userdata

*/
void MEAPI RRenderSetActionNCallBackWithText( RRender *rc, int N, RButtonPressCallBack func,
                                      void* userdata, char *text)
{
    if(N > NUM_ACTION_CALLBACKS)
        MeWarning(0, "You tried to set and Action Callback that doesn't exist."
            "There are only %d Action Callbacks.\n", NUM_ACTION_CALLBACKS);
    else {
        rc->m_pCallBacks->m_Action[N].m_CallBack = func;
        rc->m_pCallBacks->m_Action[N].m_Userdata = userdata;
        rc->m_pCallBacks->m_Action[N].m_Text     = text;
    }
}

/**
   Assigns a key to a given Action Callback

   @param rc The render context whose callback key is to be assigned.
   @param n The number of the callback to assign the key to. With the
   standard renderer, this must be in the range 2-5.
   @param key The key character to assign to the nth action callback.
*/
void MEAPI RRenderSetActionNKey(RRender* rc, const unsigned int n, const char key) {
    if(n < 2 || n >= NUM_ACTION_CALLBACKS)
        MeWarning(0, "You tried to assign a key to Action %d, an action you can't assign a key to."
            "You can only assign keys to Actions 2, 3, 4 and 5.\n", n);
    else
        rc->m_pCallBacks->m_actionKeys[n-2] = key;
}

/**
    Fills R_ObjectFileBuffer

    @param filename The name of the file to load.

    Makes sure that R_ObjectFileBuffer contains the contents of
    the specified file.
    R_ObjectFileBuffer is null terminated.

    Currently checks if *filename was the last file loaded.
*/
void MEAPI RGraphicFillObjectBuffer( char *filename )
{
    /* Check whether new object is same as the last.
       Could be modified to allow more sophisticated file caching */

    int fh; /* file handle */
    int len;

    if(R_LastObjectFilename)
        if( strcmp(R_LastObjectFilename, filename) == 0 )
        {
/*            MeInfo(1,"Creating instance of last object (%s).\n", filename);*/
            return;
        }

    /* If we get here, then this is a new type of object */
    if(R_LastObjectFilename)
        MeMemoryAPI.destroy(R_LastObjectFilename);

    R_LastObjectFilename = (char *)MeMemoryAPI.create(strlen(filename)+1);
    strcpy(R_LastObjectFilename, filename);

    fh = MeOpenWithSearch( filename, kMeOpenModeRDONLY );
    if( (fh<0) )
    {
        ME_REPORT(MeInfo(0,"Couldn't open file: %s", filename ));
        R_ObjectFileLength = 0;
        return;
    }

    len = MeLseek(fh, 0, kMeSeekEND);

    if( R_ObjectFileBuffer )
    {
        MeMemoryAPI.destroy(R_ObjectFileBuffer);
        R_ObjectFileBuffer = 0;
    }
    R_ObjectFileBuffer = (char *)MeMemoryAPI.create(len+1);
    if( !R_ObjectFileBuffer )
    {
        ME_REPORT(MeWarning(1,"MeMemoryAPI.create failed for file buffer."));
        R_ObjectFileLength = 0;
        return;
    }

    if( MeLseek(fh, 0, kMeSeekSET) == -1 )
        ME_REPORT(MeWarning(1,"MeSeek failed."));
    else if( MeRead(fh, R_ObjectFileBuffer, len) == -1 )
        ME_REPORT(MeWarning(1,"MeRead failed."));

    if(MeClose(fh) < 0)
        ME_REPORT(MeWarning(1,"Failed to close file handle. Continuing anyway..."));

    R_ObjectFileBuffer[len] = '\0';
    R_ObjectFileLength = len;
}

/**
    Creates new RGraphic and fills in vertices from file.

    N.B. RObjectHeader is not filled in.

    @param filename The geometry file to attempt to load.
    @return A pointer to the resulting RGraphic, or 0 for failure.
*/
RGraphic * MEAPI RGraphicLoadMEG( char *filename )
{
#if 0
    int len;
    char *buf;
    char numbuf[20];
    int vertexCount;
    int i,j;
    RGraphic *rg = 0;
    AcmeReal vertex[8];
    int vertexIndex;
    int vertexAddedCount;
    RObjectVertex *next_free_vertex;

    RGraphicFillObjectBuffer( filename );
    buf = R_ObjectFileBuffer;
    len = R_ObjectFileLength;

    if( !buf || !len )
    {
        MeFatalError(0, "Loading of Object Buffer failed. Cannot create graphic, so, cannot continue!\n"
            "Ensure that the graphic file to be loaded is present in the Resources directory.");
        return rg;
    }

    j = -1;
    for( i=0; i<len; i++ )
    {
        if(buf[i] == ':' && j == -1)
            j = i;
        if(buf[i] == ' ' && j != -1)
            break;
    }
    if( i == len )
        ME_REPORT(MeWarning(1,"Couldn't find vertex count."));
    else
    {
        memcpy(&numbuf, &buf[j+1], i-j-1);
        numbuf[(i-j-1)] = '\0';
        vertexCount = atoi(numbuf);
        if(vertexCount%3)
            ME_REPORT(MeWarning(1,"Vertex count is not a multiple of three."));
        else
        { /* we can now create a new RGraphic */
            rg = RGraphicCreateEmpty(vertexCount);
            if( rg )
            {
                /* time to fill in co-ords */
                j = -1;
                vertexIndex = 0;
                vertexAddedCount = 0;
                next_free_vertex = rg->m_pVertices;
                while( i < len )
                { /* walk through buffer */
                    if( buf[i] == ':' && j == -1 )
                        j = i;
                    if( buf[i] == ' ' && j != -1 )
                    {
                        /* found a number between ':' and ' ' */
                        memcpy(&numbuf, &buf[j+1], i-j-1);
                        numbuf[(i-j-1)] = '\0';
                        vertex[vertexIndex] = (AcmeReal)atof(numbuf);
                        vertexIndex++;
                        if( vertexIndex == 8 )
                        {
                            next_free_vertex->m_X = vertex[0];
                            next_free_vertex->m_Y = vertex[1];
                            next_free_vertex->m_Z = vertex[2];
                            next_free_vertex->m_NX = vertex[3];
                            next_free_vertex->m_NY = vertex[4];
                            next_free_vertex->m_NZ = vertex[5];
                            next_free_vertex->m_U = vertex[6];
                            next_free_vertex->m_V = vertex[7];
                            next_free_vertex++;

                            vertexAddedCount++;

                            vertexIndex = 0;
                        }
                        j = -1;
                    }
                    i++;
                    if( vertexAddedCount == vertexCount )
                        break;
                }
                if( i > len-1 && vertexAddedCount != vertexCount )
                {
                    ME_REPORT(MeWarning(1,"End of buffer reached, without all vertices found."));
                    RGraphicDestroy(rg);
                    rg = 0;
                }
            }
            else
                ME_REPORT(MeWarning(1,"Couldn't create new RGraphic."));
        }
    }
    if(rg) {
        return rg;
    } else {
        MeFatalError(0, "Graphic could not be created (see above for possible reasons).\n"
            "Ensure that the graphic file to be loaded is present in the Resources directory.");
        return 0; /* Remove warning */
    }
#endif
    return 0;
}


/**
    Creates a RGraphic from object file.

    A new RGraphic is created using the parameters passed
    @param filename specifies object geometry file (.meg) to load.
    @param xScale specifies the x scaling factor
    @param yScale specifies the y scaling factor
    @param zScale specifies the z scaling factor
    @param color specifies object RGBA color, which sets the ambient and the
    diffuse components of the object's colour. The emissive and specular
    components are initialized to zero.
    @param matrix is the pointer to the associated transformation matrix
    @param is2D indicates if object is to be put in 2D render-list
    @param bKeepAspectRatio indicates if the aspect ratio is to be preserved when object is normalized
    @return A pointer to the resulting RGraphic, or 0 for failure.
*/
RGraphic *MEAPI RGraphicCreate(RRender *rc, char *filename,
    AcmeReal xScale, AcmeReal yScale, AcmeReal zScale,
    const float color[4], MeMatrix4Ptr matrix, int is2D, int bKeepAspectRatio)
{
    RGraphic *rg;

    MEASSERTALIGNED(matrix, 16);

    rg = RGraphicLoadMEG(filename);
    if( !rg )
        return 0;

    /* fill in header... */
    rg->m_pObject->m_nTextureID = -1; /* no texture */

    RGraphicSetColor(rg,color);

    rg->m_pObject->m_bIsWireFrame = 0; /* default to solid */

    rg->m_pLWMatrix = matrix;

    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    if( !is2D )
        RGraphicNormalize(rg, bKeepAspectRatio);
    RGraphicScale(rg, xScale, yScale, zScale);

    RGraphicAddToList(rc, rg, is2D);

    return rg;
}


/**
    Scales an RGraphic

    @param rg The RGraphic in question.
    @param xScale The multiplier for the x co-ordinate of each vertex.
    @param yScale The multiplier for the y co-ordinate of each vertex.
    @param zScale The multiplier for the z co-ordinate of each vertex.

    N.B. The RGraphic must be centered on 0,0,0 in model space.
*/
void MEAPI RGraphicScale(RGraphic *rg, AcmeReal xScale, AcmeReal yScale, AcmeReal zScale)
{
    int i;
    RObjectVertex *next_vertex = rg->m_pVertices;

    for( i = 0; i < rg->m_pObject->m_nNumVertices; i++ )
    {
        next_vertex->m_X = next_vertex->m_X * xScale;
        next_vertex->m_Y = next_vertex->m_Y * yScale;
        next_vertex->m_Z = next_vertex->m_Z * zScale;
        next_vertex++;
    }
}


/**
    Creates a Square

    @param rc The render context into whose 3D list the resulting RGraphic is put.
    @param side is the length of the side of the square
    @param color is the RGBA color of the object
    @param matrix is a pointer to the object's transformation matrix
    @return A pointer to the resulting RGraphic, or 0 for failure.
*/
RGraphic * MEAPI RGraphicSquareCreate(RRender *rc, AcmeReal side, const float color[4], MeMatrix4Ptr matrix)
{
    RObjectVertex* rv;
    RGraphic* rg = RGraphicCreateEmpty(6);

    MEASSERTALIGNED(matrix, 16);

    if ( !rg )
        return 0;
    rv = rg->m_pVertices;

    /* Fill in vertices */

    rv->m_X = -(side/2);
    rv->m_Y = (side/2);
    rv->m_Z = 0;
    rv->m_NX = 0;
    rv->m_NY = 0;
    rv->m_NZ = -1;
    rv->m_U = 0;
    rv->m_V = 0;
    rv++;

    rv->m_X = -(side/2);
    rv->m_Y = -(side/2);
    rv->m_Z = 0;
    rv->m_NX = 0;
    rv->m_NY = 0;
    rv->m_NZ = -1;
    rv->m_U = 0;
    rv->m_V = 1;
    rv++;

    rv->m_X = (side/2);
    rv->m_Y = (side/2);
    rv->m_Z = 0;
    rv->m_NX = 0;
    rv->m_NY = 0;
    rv->m_NZ = -1;
    rv->m_U = 1;
    rv->m_V = 0;
    rv++;

    rv->m_X = -(side/2);
    rv->m_Y = -(side/2);
    rv->m_Z = 0;
    rv->m_NX = 0;
    rv->m_NY = 0;
    rv->m_NZ = -1;
    rv->m_U = 0;
    rv->m_V = 1;
    rv++;

    rv->m_X = (side/2);
    rv->m_Y = (side/2);
    rv->m_Z = 0;
    rv->m_NX = 0;
    rv->m_NY = 0;
    rv->m_NZ = -1;
    rv->m_U = 1;
    rv->m_V = 0;
    rv++;

    rv->m_X = (side/2);
    rv->m_Y = -(side/2);
    rv->m_Z = 0;
    rv->m_NX = 0;
    rv->m_NY = 0;
    rv->m_NZ = -1;
    rv->m_U = 1;
    rv->m_V = 1;

    rg->m_pObject->m_nTextureID = -1; /* no texture */

    RGraphicSetColor(rg,color);

    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;

    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    RGraphicAddToList(rc, rg, 0);

    return rg;
}


/**
    Creates a Box

    @param rc The render context into whose 3D list the resulting RGraphic is put.
    @param width is the width (x) of the box
    @param height is the height (y) of the box
    @param depth is the depth (z) of the box
    @param color is the RGBA color of the object
    @param matrix is a pointer to the object's transformation matrix
    @return A pointer to the resulting RGraphic, or 0 for failure.
*/
RGraphic * MEAPI RGraphicBoxCreate(RRender *rc, AcmeReal width, AcmeReal height, AcmeReal depth, const float color[4], MeMatrix4Ptr matrix)
{
    /* return RGraphicCreate(rc, "cube.meg", 0.5f*width, 0.5f*height, 0.5f*depth, color, matrix, 0, 0); */

    RGraphic *rg;
    int numVertices = RCalculateBoxVertexCount();

/*     MEASSERTALIGNED(matrix, 16); */

    rg = RGraphicCreateEmpty(numVertices);
    if ( !rg )
        return 0;

    RCalculateBoxGeometry(rg, width, height, depth);

    rg->m_pObject->m_nTextureID = -1; /* no texture */

    RGraphicSetColor(rg,color);

    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;

    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    RGraphicAddToList(rc, rg, 0);

    return rg;
}


/**
    Creates a Sphere

    @param rc The render context into whose 3D list the resulting RGraphic is put.
    @param radius is the radius of the sphere
    @param color is the RGBA color of the object
    @param matrix is a pointer to the object's transformation matrix
    @return A pointer to the resulting RGraphic, or 0 for failure.
*/
RGraphic * MEAPI RGraphicSphereCreate(RRender *rc, AcmeReal radius, const float color[4], MeMatrix4Ptr matrix)
{
    RGraphic *rg;
    int sides = RRenderGetSphereResolution(rc);
    int rings = RRenderGetSphereResolution(rc);
    int numVertices = RCalculateSphereVertexCount(sides, rings);

/*     MEASSERTALIGNED(matrix, 16); */

    rg = RGraphicCreateEmpty(numVertices);
    if ( !rg )
        return 0;

    RCalculateSphereGeometry(rg, radius, sides, rings);

    rg->m_pObject->m_nTextureID = -1; /* no texture */

    RGraphicSetColor(rg,color);

    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;

    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    RGraphicAddToList(rc, rg, 0);

    return rg;
}

RGraphic * MEAPI RGraphicSphylCreate(RRender *rc, AcmeReal radius, AcmeReal height,
                                     const float color[4], MeMatrix4Ptr matrix)
{
    RGraphic *rg;
    int sides = RRenderGetSphereResolution(rc);
    int rings = (int)((MeReal)0.5 * RRenderGetSphereResolution(rc));
    int numVertices = RCalculateSphylVertexCount(sides, rings);

    MEASSERTALIGNED(matrix, 16);

    rg = RGraphicCreateEmpty(numVertices);
    if ( !rg )
        return 0;

    RCalculateSphylGeometry(rg, radius, height, sides, rings);

    rg->m_pObject->m_nTextureID = -1; /* no texture */

    RGraphicSetColor(rg,color);

    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;

    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    RGraphicAddToList(rc, rg, 0);

    return rg;
}

/**
        Sets the resolution of a sphere.
*/
void MEAPI RRenderSetSphereResolution(RRender* r, int res)
{
        r->m_sphereRes = res;
}

/**
        Returns the resolution of a sphere.
*/
int MEAPI RRenderGetSphereResolution(RRender* r)
{
        return r->m_sphereRes;
}

/**
    Creates a Cylinder

    @param rc The render context into whose 3D list the resulting RGraphic is put.
    @param radius is the radius of the cylinder
    @param height is length (z) of the cylinder
    @param color is the RGBA color of the object
    @param matrix is a pointer to the object's transformation matrix
    @return A pointer to the resulting RGraphic, or 0 for failure.
*/
RGraphic * MEAPI RGraphicCylinderCreate(RRender *rc, AcmeReal radius, AcmeReal height, const float color[4], MeMatrix4Ptr matrix)
{
    int sides = CYL_RES;
    MEASSERTALIGNED(matrix, 16);
    /* return RGraphicCreate(rc, "cylinder.meg", radius, radius, 0.5f*height, color, matrix, 0, 0); */
    return RGraphicFrustumCreate(rc, radius, radius, -height*0.5f, height*0.5f, sides, color, matrix);
}


/**
    Creates a Cone

    @param rc The render context into whose 3D list the resulting RGraphic is put.
    @param radius is the radius of the base of the cone
    @param height is length (z) of the cone
    @param color is the RGBA color of the object
    @param matrix is a pointer to the object's transformation matrix
    @return A pointer to the resulting RGraphic, or 0 for failure.
*/
RGraphic * MEAPI
RGraphicConeCreate(RRender *rc, AcmeReal radius,
                   AcmeReal upper_height, AcmeReal lower_height,
                   const float color[4], MeMatrix4Ptr matrix)
{
    int sides = CONE_RES*2;

#if 0
    /* This is consistent with cone definition in Mcd */
    return RGraphicFrustumCreate(rc, 0, radius, upper_height, -lower_height,
                                 sides, color, matrix);
#else
    RGraphic *rg;
    int numVertices = RCalculateConeVertexCount(sides);

    MEASSERTALIGNED(matrix, 16);

    rg = RGraphicCreateEmpty(numVertices);
    if ( !rg )
        return 0;

    RCalculateConeGeometry(rg, radius, -lower_height, upper_height, sides);

    rg->m_pObject->m_nTextureID = -1; /* no texture */

    RGraphicSetColor(rg,color);

    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;

    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    RGraphicAddToList(rc, rg, 0);

    return rg;
#endif
}


/**
    Creates a Torus

    @param rc The render context into whose 3D list the resulting RGraphic is put.
    @param radius is the outer radius of the torus
    @param color is the RGBA color of the object
    @param matrix is a pointer to the object's transformation matrix
    @return A pointer to the resulting RGraphic, or 0 for failure.
*/
RGraphic * MEAPI
RGraphicTorusCreate(RRender *rc, AcmeReal innerRadius, AcmeReal outerRadius,
                    const float color[4], MeMatrix4Ptr matrix)
{
    /* int sides = TORUS_RES * 3/4;
       int rings = TORUS_RES * 5/4; */
    int rings = TORUS_RES * 3/4;
    int sides = TORUS_RES * 5/4;
    RGraphic *rg;
    int numVertices = RCalculateTorusVertexCount(sides, rings);

    MEASSERTALIGNED(matrix, 16);

    rg = RGraphicCreateEmpty(numVertices);
    if ( !rg )
        return 0;

    RCalculateTorusGeometry(rg, innerRadius, outerRadius, sides, rings);

    rg->m_pObject->m_nTextureID = -1; /* no texture */

    RGraphicSetColor(rg,color);

    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;

    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    RGraphicAddToList(rc, rg, 0);

    return rg;
}

/**
   Creates an arbitary frustum.

   @param rc The render context into whose 3D list the resulting RGraphic is placed.
   @param bottomRadius The radius of the approximation to a circle that forms the bottom of the frustum.
   @param topRadius The radius of the approximation to a circle that forms the top of the frustum.
   @param bottom The z co-ordinate, in the frustum's reference frame, of the bottom of the frustum.
   @param top The z co-ordinate, in the frustum's reference frame, of the top of the frustum.
   @param sides The number of sides of the regular polygon at each end of the frustum.
   @param color The RGBA colour of the graphic.
   @param matrix A pointer to the local-world transform for this graphic.
   @return A pointer to the resulting RGraphic, or 0 for failure.
*/
RGraphic * MEAPI RGraphicFrustumCreate(RRender *rc, AcmeReal bottomRadius,
                               AcmeReal topRadius, AcmeReal bottom,
                               AcmeReal top, int sides,
                               const float color[4], MeMatrix4Ptr matrix)
{
    RGraphic *rg;
    int numVertices = RCalculateFrustumVertexCount(sides);

    MEASSERTALIGNED(matrix, 16);

    rg = RGraphicCreateEmpty(numVertices);
    if ( !rg )
        return 0;

    RCalculateFrustumGeometry(rg, bottomRadius, topRadius, bottom, top, sides);

    rg->m_pObject->m_nTextureID = -1; /* no texture */

    RGraphicSetColor(rg,color);

    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;

    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    RGraphicAddToList(rc, rg, 0);

    return rg;
}

/**
   Loads in ASE file and creates a graphics object for it.
   Material information from the ASE file is ignored (although texture co-ords
   are used). The overall color and texture are set in the same way as all other
   RGraphics.

   @param rc The render context into whose 3D list the resulting RGraphic is placed.
   @param filename The ASE file to be loaded.
   @param scaleFactor Amount to scale graphics file by on load.
   @param color The RGBA colour of the graphic.
   @param matrix A pointer to the local-world transform for this graphic.
   @return A pointer to the resulting RGraphic, or 0 for failure.
*/
RGraphic * MEAPI RGraphicLoadASE(RRender *rc, char* filename,
                                 AcmeReal xScale, AcmeReal yScale, AcmeReal zScale,
                                 const float color[4], MeMatrix4Ptr matrix)
{
    RGraphic *rg;
    int numVertices;
    MeASEObject *object = MeASEObjectLoadParts(filename, xScale, yScale, zScale, 1);

    MEASSERTALIGNED(matrix, 16);

    if ( !object )
        return 0;

    numVertices = RCalculateASEVertexCount(object, 0);

    rg = RGraphicCreateEmpty(numVertices);

    if ( !rg )
        return 0;

    RLoadASEGeometry(rg, object, 0);

    MeASEObjectDestroy(object);

    rg->m_pObject->m_nTextureID = -1; /* no texture */

    RGraphicSetColor(rg,color);

    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;

    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    RGraphicAddToList(rc, rg, 0);

    return rg;
}

RGraphic * MEAPI RGraphicLoadASEwithOffset(RRender *rc, char* filename,
                                 AcmeReal xScale, AcmeReal yScale, AcmeReal zScale,
                                 const float color[4], MeVector3Ptr offset, MeMatrix4Ptr matrix)
{
    RGraphic *rg;

    rg = RGraphicLoadASE(rc, filename, xScale, yScale, zScale, color, matrix);
    if(rg)
    {
        RObjectVertex *vtx = rg->m_pVertices;
        int i;
        for(i=0; i<rg->m_pObject->m_nNumVertices; i++)
        {
            vtx->m_X += offset[0];
            vtx->m_Y += offset[1];
            vtx->m_Z += offset[2];
            vtx++;
        }
    }
    return rg;
}

RGraphic * MEAPI RGraphicLoadASEwithOffsetNoMcd(RRender *rc, char* filename,
                                           AcmeReal xScale, AcmeReal yScale, AcmeReal zScale,
                                           const float color[4], MeVector3Ptr offset, MeMatrix4Ptr matrix)
{
    RGraphic *rg;
    
    rg = RGraphicLoadASENoMCD(rc, filename, xScale, yScale, zScale, color, matrix);
    if(rg)
    {
        RObjectVertex *vtx = rg->m_pVertices;
        int i;
        for(i=0; i<rg->m_pObject->m_nNumVertices; i++)
        {
            vtx->m_X += offset[0];
            vtx->m_Y += offset[1];
            vtx->m_Z += offset[2];
            vtx++;
        }
    }
    return rg;
}

/**
   Loads in ASE file and creates a graphics object for it.

   This function varies from RGraphicLoadASE because it does not load parts
   of the ASE whose names begin 'MCDBX','MCDSP','MCDCY' or 'MCDCX'. These can
   be used to indicate collision geometry, and should not be loaded to render.

   Material information from the ASE file is ignored (although texture co-ords
   are used). The overall color and texture are set in the same way as all other
   RGraphics.

   @see RGraphicLoadASE
*/
RGraphic * MEAPI RGraphicLoadASENoMCD(RRender *rc, char* filename,
                                 AcmeReal xScale, AcmeReal yScale, AcmeReal zScale,
                                 const float color[4], MeMatrix4Ptr matrix)
{
    RGraphic *rg;
    int numVertices;
    MeASEObject *object = MeASEObjectLoadParts(filename, xScale, yScale, zScale, 1);

    MEASSERTALIGNED(matrix, 16);

    if ( !object )
        return 0;

    numVertices = RCalculateASEVertexCount(object, 1);

    rg = RGraphicCreateEmpty(numVertices);

    if ( !rg )
        return 0;

    RLoadASEGeometry(rg, object, 1);

    MeASEObjectDestroy(object);

    rg->m_pObject->m_nTextureID = -1; /* no texture */

    RGraphicSetColor(rg,color);

    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;

    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    RGraphicAddToList(rc, rg, 0);

    return rg;
}


RGraphic * MEAPI RGraphicLoadASEwithOffsetNoMcdFromStream(RRender *rc, MeStream str,
                                                AcmeReal xScale, AcmeReal yScale, AcmeReal zScale,
                                                const float color[4], MeVector3Ptr offset, MeMatrix4Ptr matrix)
{
    RGraphic *rg;
    int numVertices;
    MeASEObject *object = MeASEObjectLoadPartsFromStream(str, xScale, yScale, zScale, 1);
    
    MEASSERTALIGNED(matrix, 16);
    
    if ( !object )
        return 0;
    
    numVertices = RCalculateASEVertexCount(object, 1);
    
    rg = RGraphicCreateEmpty(numVertices);
    
    if ( !rg )
        return 0;
    
    RLoadASEGeometry(rg, object, 1);
    
    MeASEObjectDestroy(object);
    
    rg->m_pObject->m_nTextureID = -1; /* no texture */
    
    RGraphicSetColor(rg,color);
    
    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;
    
    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);
    
    { /* Apply offset */
        RObjectVertex *vtx = rg->m_pVertices;
        int i;
        for(i=0; i<rg->m_pObject->m_nNumVertices; i++)
        {
            vtx->m_X += offset[0];
            vtx->m_Y += offset[1];
            vtx->m_Z += offset[2];
            vtx++;
        }
    }

    RGraphicAddToList(rc, rg, 0);
    
    return rg;
}


RGraphic * MEAPI RGraphicDomeCreate(RRender *rc,
    AcmeReal radius, int tileU, int tileV,
    const float color[4], MeMatrix4Ptr matrix)
{
    const int sides = SPHERE_RES;
    const int rings = SPHERE_RES;

    RGraphic *rg;

    const int numVertices = RCalculateDomeVertexCount(sides, rings);

    MeDebug(16,"RGraphicDomeCreate: numVertices %d",numVertices);

    MEASSERTALIGNED(matrix, 16);

    rg = RGraphicCreateEmpty(numVertices);

    if (rg == 0)
    {
        MeWarning(0,"RGraphicDomeCreate: %s","RGraphicCreateEmpty failed");
        return 0;
    }

    RCalculateDomeGeometry(rg, radius, sides, rings, tileU, tileV);

    rg->m_pObject->m_nTextureID = -1; /* no texture */

    RGraphicSetColor(rg,color);

    rg->m_pObject->m_bIsWireFrame = 0;
    rg->m_pLWMatrix = matrix;

    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    RGraphicAddToList(rc, rg, 0);

    return rg;
}

/**
    Creates a Line

    @param rc The render context into whose 3D list the resulting RGraphic is put.
    @param origin is the start location of the line (x,y,z)
    @param end is the end location of the line (x,y,z)
    @param color is the RGBA color of the object
    @param matrix is a pointer to the object's transformation matrix
    @return A pointer to the resulting RGraphic, or 0 for failure.
*/
RGraphic * MEAPI RGraphicLineCreate(RRender *rc, AcmeReal *origin, AcmeReal *end, const float color[4], MeMatrix4Ptr matrix)
{
    RGraphic *rg = RGraphicCreateEmpty(6);
    if ( !rg )
        return 0;

    MEASSERTALIGNED(matrix, 16);

    rg->m_pObject->m_nTextureID = -1; /* no texture */

    RGraphicSetColor(rg,color);

    rg->m_pObject->m_bIsWireFrame = 1;
    rg->m_pLWMatrix = matrix;

    /* let matrix default to identity */
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    /* Fill in RGraphic vertices and move line around.... */
    RGraphicLineMoveEnds( rg, origin, end );

    RGraphicAddToList(rc, rg, 0);

    return rg;
}

/**
   Moves the ends of an RGraphic which is a line.

   @param lineG A pointer to the RGraphic representing a line.
   @param origin A three-vector containing the co-ordinates of the start of the line.
   @param end A three-vector containing the co-ordinates of the end of the line.
   @return 0 for success, 1 for failure (an MeWarning will be printed before returning in this case).
*/
int MEAPI RGraphicLineMoveEnds( RGraphic *lineG, AcmeReal *origin, AcmeReal *end )
{
    AcmeReal x,y,z,w = 0.005f;   /* w is line width (formerly 0.05f) */
    RObjectVertex *next_vertex;
    int i;
    AcmeReal direct[3];
    AcmeReal centre[3];
    AcmeReal length;

    direct[0] = end[0] - origin[0];
    direct[1] = end[1] - origin[1];
    direct[2] = end[2] - origin[2];
    length = (AcmeReal)MeSqrt( (direct[0]*direct[0]) + (direct[1]*direct[1]) + (direct[2]*direct[2]) );

    if(length>0)
    {
        direct[0] /= length;
        direct[1] /= length;
        direct[2] /= length;
    }

    centre[0] = 0.5f * (origin[0] + end[0]);
    centre[1] = 0.5f * (origin[1] + end[1]);
    centre[2] = 0.5f * (origin[2] + end[2]);

    /* Check that lineG is (probably a line) */
    if( !lineG )
    {
        ME_REPORT(MeWarning(1, "RGraphicLineMoveEnds: lineG is not valid"));
        return 1;
    }
    if( lineG->m_nMaxNumVertices < 6 )
    {
        ME_REPORT(MeWarning(1, "RGraphicLineMoveEnds: lineG is not big enough to be a line"));
        return 1;
    }
    if( lineG->m_pObject->m_nNumVertices != 6 ) /* and from above lineG->m_nMaxNumVertices >= 6 */
    {
        ME_REPORT(MeWarning(1, "RGraphicLineMoveEnds: lineG does not have 6 vertices"));
        /* Could still be used, though.... */
        /* probably best to get rid of this and return 1 ;-) */
        lineG->m_pObject->m_nNumVertices = 6;
    }

    /* Build this...
    x:-0.5 y:0 z:-0.05  nx:0 ny:0 nz:1 u:0 v:0 /
    x:-0.5 y:0 z:0.05   nx:0 ny:0 nz:1 u:0 v:0 /
    x:0.5  y:0 z:0.05   nx:0 ny:0 nz:1 u:0 v:0 /
    x:-0.5 y:0 z:-0.05  nx:0 ny:0 nz:1 u:0 v:0 /
    x:0.5  y:0 z:0.05   nx:0 ny:0 nz:1 u:0 v:0 /
    x:0.5  y:0 z:-0.05  nx:0 ny:0 nz:1 u:0 v:0 /
    */
    next_vertex = lineG->m_pVertices;
    for( i = 0; i < 6; i++ )
    {
        next_vertex->m_Y = 0.0f;
        next_vertex->m_NX = 0.0f;
        next_vertex->m_NY = 0.0f;
        next_vertex->m_NZ = 1.0f;
        next_vertex->m_U = 0.0f;
        next_vertex->m_V = 0.0f;
        next_vertex++;
    }
    next_vertex = lineG->m_pVertices;
    next_vertex->m_X = -0.5f * length;
    next_vertex->m_Z = -w;
    next_vertex++;
    next_vertex->m_X = -0.5f * length;
    next_vertex->m_Z = w;
    next_vertex++;
    next_vertex->m_X = 0.5f * length;
    next_vertex->m_Z = w;
    next_vertex++;
    next_vertex->m_X = -0.5f * length;
    next_vertex->m_Z = -w;
    next_vertex++;
    next_vertex->m_X = 0.5f * length;
    next_vertex->m_Z = w;
    next_vertex++;
    next_vertex->m_X = 0.5f * length;
    next_vertex->m_Z = -w;


    next_vertex = lineG->m_pVertices;
    for( i = 0; i < lineG->m_pObject->m_nNumVertices; i++ )
    {
        x = next_vertex->m_X;
        y = next_vertex->m_Y;
        z = next_vertex->m_Z;

        next_vertex->m_X = x*direct[0] + centre[0];
        next_vertex->m_Y = x*direct[1] + y + centre[1];
        next_vertex->m_Z = x*direct[2] + z + centre[2];

        next_vertex++;


        x = next_vertex->m_X;
        y = next_vertex->m_Y;
        z = next_vertex->m_Z;

        next_vertex->m_X = x*direct[0] + centre[0];
        next_vertex->m_Y = x*direct[1] + y + centre[1];
        next_vertex->m_Z = x*direct[2] + z + centre[2];

        next_vertex++;
        i++;
    }

    return 0; /* success */
}


/**
    Sets the texture of a RGraphic

    @param filename specifies the name of the texture. N.B. The filename
    should not include the extension
    @return is the Texture ID, or -1 if call failed.
*/
int MEAPI RGraphicSetTexture(RRender *rc, RGraphic *rg, const char *filename)
{
    if( !rg )
        return -1;
    rg->m_pObject->m_nTextureID = RRenderTextureCreate(rc, filename);
    return rg->m_pObject->m_nTextureID;
}


/**
    Creates a Texture ID for a filename

    Returns an ID for a texture filename.
    Returns -1 if all IDs are allocated.

    @param rc The render context into which the texture will be loaded.
    @param filename The name of the texture file to attempt to load.
*/
int MEAPI RRenderTextureCreate(RRender *rc, const char *filename)
{
    /* check table, set ID */
    int i;
    int ret_id;

    /* See if texture is already in list */
    ret_id = -1;
    for( i = 0; i < 25 ; i++ )
    {
        if( rc->m_TextureList[i] )
            if( strcmp(rc->m_TextureList[i], filename) == 0 )
            {
                ret_id = i;
                break;
            }
    }

    /* if texture has not been used before, add it to list */
    if( ret_id == -1 )
    {
        for( i = 0; i < 25; i++ )
            if( !rc->m_TextureList[i] )
            {
                rc->m_TextureList[i] = (char*) MeMemoryAPI.create(strlen(filename)+1);
                strcpy(rc->m_TextureList[i],filename);
                /* rc->m_TextureList[i] = filename; */
                ret_id = i;
                break;
            }
    }

    /* return value will be -1 if texture setting failed */
    return ret_id;
}

/**
    Creates new RParticleSystem
    called by RParticleSystemCreate

    Allocates memory for new particle system.
    System is not added to a list.
    Require a TextureID, not filename.

    @param numParticles The number of particles in the new system.
    @param positions A pointer to the start of the list of positions of each
    particle in the system.
    @param tex_id The texture ID of the texture to be applied to each particle.
    @param color The RGBA colour to set the ambient and diffuse components
    of the particles' graphics to.
    @param size The size of triangle used to represent each particle.
    @return A pointer to the newly created particle system, or 0 for failure.
*/
RParticleSystem * MEAPI RParticleSystemNew( int numParticles, MeVector3 *positions, int tex_id, const float color[4], AcmeReal size )
{
    RParticleSystem *ps = (RParticleSystem*)MeMemoryAPI.create(sizeof(RParticleSystem));

    if( !ps )
        return 0;

    ps->m_Positions = positions;
    ps->m_nNumParticles = numParticles;
    ps->m_nTextureID = tex_id;
    ps->m_TriangleSize = size;

    ps->m_pNextSystem = 0;

    ps->m_rgbDiffuse[0] = ps->m_rgbEmissive[0] = ps->m_rgbSpecular[0] = 0.0f;
    ps->m_rgbDiffuse[1] = ps->m_rgbEmissive[1] = ps->m_rgbSpecular[1] = 0.0f;
    ps->m_rgbDiffuse[2] = ps->m_rgbEmissive[2] = ps->m_rgbSpecular[2] = 0.0f;
    ps->m_rgbDiffuse[3] = ps->m_rgbEmissive[3] = ps->m_rgbSpecular[3] = color[3];

    ps->m_rgbAmbient[0] = color[0];
    ps->m_rgbAmbient[1] = color[1];
    ps->m_rgbAmbient[2] = color[2];
    ps->m_rgbAmbient[3] = color[3];

    return ps;
}


/**
    Adds a RParticleSystem to render-list

    @param rc The renderer to whose 3D list the particle system will be added.
    @param ps The particle system to add.
*/
void MEAPI RParticleSystemAddToList( RRender *rc, RParticleSystem *ps )
{
    if( !rc->m_pPS_First ) /*empty list*/
        rc->m_pPS_First = ps;
    else
    {
        RParticleSystem *curr;
        curr = rc->m_pPS_First;
        while( curr->m_pNextSystem )
            curr = curr->m_pNextSystem;
        curr->m_pNextSystem = ps;
    }
}


/**
    Removes a RParticleSystem from render-list

    @param rc The render context from whose 3D list the particle system is to be removed.
    @param ps The particle system to remove.
*/
void MEAPI RParticleSystemRemoveFromList( RRender *rc, RParticleSystem *ps )
{
    RParticleSystem *curr = rc->m_pPS_First;

    if( curr == ps ) /* first in list */
    {
        rc->m_pPS_First = curr->m_pNextSystem;
        return;
    }

    while( curr )
    {
        if( curr->m_pNextSystem == ps )
        {
            curr->m_pNextSystem = curr->m_pNextSystem->m_pNextSystem;
            return;
        }
        curr = curr->m_pNextSystem;
    }
}


/**
    Frees memory associated with a RParticleSystem

    @param ps The particle system to destroy.
*/
void MEAPI RParticleSystemDestroy( RParticleSystem *ps )
{
    if( ps )
        MeMemoryAPI.destroy(ps);
}


/**
    Creates a new RParticleSystem

    Gets a texture ID, calls RParticleSystemNew and adds to render list.

    @param rc The render context to whose 3d list the particle system will be added.
    @param numParticles The number of particles in the system.
    @param positions specifies array of 4-vectors of particle positions
    @param tex_filename The name of the texture to load and apply to each particle in the system.
    @param color The colour to set the ambient and diffuse components of the particles' colour to.
    @param tri_size The size of the triangle that will represent each particle for rendering purposes.
*/

RParticleSystem * MEAPI RParticleSystemCreate( RRender *rc, int numParticles, MeVector3 *positions, char *tex_filename, const float color[4], AcmeReal tri_size )
{
    RParticleSystem *ps;
    int tex_id = RRenderTextureCreate(rc, tex_filename);

    ps = RParticleSystemNew(numParticles, positions, tex_id, color, tri_size);
    if( ps )
        RParticleSystemAddToList(rc, ps);

    return ps;
}


/**
    Toggles the display of user helpr

    Called by platform-specific back-end.
    Toggles the pause state of associated render context.

    @param rc The render context for which to toggle the display of the help
    text.
*/
void MEAPI RRenderToggleUserHelp( RRender *const rc )
{
    /* Toggles the display of help */
    if( rc->m_UserHelpG && (rc->m_MenuCurrent == NULL))
    {
        if( rc->m_isHelpDisplayed )
        {
            RGraphicRemoveFromList(rc, rc->m_UserHelpG, 1);
        }
        else
        {
            RGraphicAddToList(rc, rc->m_UserHelpG, 1);
        }

        rc->m_isHelpDisplayed = !rc->m_isHelpDisplayed;
   }
}

/**
    Builds the RGraphic representing user help.

    @param help is an array of null-terminated strings
    @param arraySize is the number of elements in the array
*/
void MEAPI RRenderCreateUserHelp( RRender *const rc,
     const char *const help[], const int arraySize )
{
    int length;
    int i;
    char *text;
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    const char *h = "$ACTION0 - toggle help\n";
/*    const char *id = " - " ME_PRODUCT_NAME
        " (v" ME_VERSION_STRING ") \nwww.mathengine.com\n"
              ME_COPYRIGHT_LINE1 "\n\n";*/
    const char *const id =  "\n"
        ME_COPYRIGHT_LINE1 "\nwww.mathengine.com\n\n";

    length = 0;
    for( i = 0; i < arraySize; i++ )
        length += strlen(help[i]) + 1;
    length += strlen(h) + 1;
    length += strlen(id) + 1;
    length += strlen(rc->m_AppName) + 1;
    length++;

    for( i = 0; i < kRButtons; i++ )
    {
        if (rc->m_pCallBacks->m_Button[i].m_Text)
        {
            length += strlen(rc->m_ButtonText[i]);
            length += strlen(" - ");
            length += strlen(rc->m_pCallBacks->m_Button[i].m_Text);
            length += strlen("\n");
        }
    }
    if (rc->m_pCallBacks->m_Mouse.m_Text)
    {
        length += strlen(rc->m_ButtonText[14]);
        length += strlen(" - ");
        length += strlen(rc->m_pCallBacks->m_Mouse.m_Text);
        length += strlen("\n");
    }
    if (rc->m_pCallBacks->m_Joystick.m_Text)
    {
        length += strlen(rc->m_ButtonText[15]);
        length += strlen(" - ");
        length += strlen(rc->m_pCallBacks->m_Joystick.m_Text);
        length += strlen("\n");
    }
    for( i = 0; i < NUM_ACTION_CALLBACKS; i++ )
    {
        if (rc->m_pCallBacks->m_Action[i].m_Text)
        {
            length += strlen(rc->m_ButtonText[kRButtons+i]);
            length += strlen(" - ");
            length += strlen(rc->m_pCallBacks->m_Action[i].m_Text);
            length += strlen("\n");
        }
    }

    text = (char *)MeMemoryAPI.create(length);
    text[0] = '\0';

    strcat(text, rc->m_AppName);
    strcat(text,id);
    for( i = 0; i < arraySize; i++ )
    {
        strcat(text, help[i]);
        strcat(text, "\n");
    }
    strcat(text,h);

    for( i = 0; i < kRButtons; i++ )
    {
        if (rc->m_pCallBacks->m_Button[i].m_Text)
        {
            strcat(text, rc->m_ButtonText[i]);
            strcat(text, " - ");
            strcat(text, rc->m_pCallBacks->m_Button[i].m_Text);
            strcat(text, "\n");
        }
    }
    if (rc->m_pCallBacks->m_Mouse.m_Text)
    {
        strcat(text, rc->m_ButtonText[14]);
        strcat(text, " - ");
        strcat(text, rc->m_pCallBacks->m_Mouse.m_Text);
        strcat(text, "\n");
    }
    if (rc->m_pCallBacks->m_Joystick.m_Text)
    {
        strcat(text, rc->m_ButtonText[15]);
        strcat(text, " - ");
        strcat(text, rc->m_pCallBacks->m_Joystick.m_Text);
        strcat(text, "\n");
    }
    for( i = 0; i < NUM_ACTION_CALLBACKS; i++ )
    {
        if (rc->m_pCallBacks->m_Action[i].m_Text)
        {
            strcat(text, rc->m_ButtonText[8+i]);
            strcat(text, " - ");
            strcat(text, rc->m_pCallBacks->m_Action[i].m_Text);
            strcat(text, "\n");
        }
    }

    rc->m_UserHelpG = RGraphicTextCreate(rc, text, X_MINVISIBLE, Y_MINVISIBLE, color);

    MeMemoryAPI.destroy(text);

    rc->m_isHelpDisplayed = 0;
    RGraphicRemoveFromList(rc, rc->m_UserHelpG, 1);
}


/**
    Creates a RGraphic representing a ground-plane.

    The plane is a square in x,z.

    @param rc The render context into whose 3D list the resulting RGraphic is put.
    @param side_length is the length of the side of the square
    @param triangles_per_side specifies the number of triangles per side of the square
    @param y_position sets the y position of the square
*/
RGraphic * MEAPI RGraphicGroundPlaneCreate(RRender *rc, AcmeReal side_length, int triangles_per_side, const float color[4], AcmeReal y_position)
{
    RGraphic *rg = 0;
    RObjectVertex *vtx;
    int i,j;
    AcmeReal x, z, u, v, d_xz, d_uv;

#ifdef PS2
#ifdef WITH_OPENGL
    /*
        There are clipping problems with large triangles with 'ps2gl'. A very
        temporary palliative is to make sure that there are enough triangles
        so that they are small.
    */
    if (side_length/triangles_per_side >= 3)
        triangles_per_side = side_length/3;

    /*
        However we don't want too many triangles, as it makes the 'Tank'
        demo fail to start because of another limitation in 'ps2gl'.
        The number below has been derived from experiments with 'Tank'.
    */

    if (triangles_per_side > 100)
        triangles_per_side = 100;
#endif
#endif

    rg = RGraphicCreateEmpty(2*6*triangles_per_side*triangles_per_side);
    if( !rg )
        return 0;

    rg->m_pLWMatrix = 0;

    MEASSERTALIGNED(rg->m_pObject->m_Matrix, 16);

    MeMatrix4TMMakeIdentity(rg->m_pObject->m_Matrix);

    rg->m_pObject->m_nTextureID = -1;

    RGraphicSetColor(rg,color);

    d_xz = side_length / triangles_per_side;
    d_uv = 1.0f / (AcmeReal)triangles_per_side;

    v = 0.0f;
    z = (side_length/2.0f);
    vtx = rg->m_pVertices;
    /* Upper Plane */
    for( i=0; i < triangles_per_side; i++ )
    {
        x = -(side_length/2.0f);
        u = 0.0f;
        for( j=0; j < triangles_per_side; j++ )
        {
            /* create square */
            vtx->m_NX = 0.0f;
            vtx->m_NY = 1.0f;
            vtx->m_NZ = 0.0f;
            vtx->m_U  = u;
            vtx->m_V  = v;
            vtx->m_X  = x;
            vtx->m_Y  = y_position;
            vtx->m_Z  = z;
            vtx++;
            vtx->m_NX = 0.0f;
            vtx->m_NY = 1.0f;
            vtx->m_NZ = 0.0f;
            vtx->m_U  = u;
            vtx->m_V  = v + d_uv;
            vtx->m_X  = x;
            vtx->m_Y  = y_position;
            vtx->m_Z  = z - d_xz;
            vtx++;
            vtx->m_NX = 0.0f;
            vtx->m_NY = 1.0f;
            vtx->m_NZ = 0.0f;
            vtx->m_U  = u + d_uv;
            vtx->m_V  = v + d_uv;
            vtx->m_X  = x + d_xz;
            vtx->m_Y  = y_position;
            vtx->m_Z  = z - d_xz;
            vtx++;
            vtx->m_NX = 0.0f;
            vtx->m_NY = 1.0f;
            vtx->m_NZ = 0.0f;
            vtx->m_U  = u;
            vtx->m_V  = v;
            vtx->m_X  = x;
            vtx->m_Y  = y_position;
            vtx->m_Z  = z;
            vtx++;
            vtx->m_NX = 0.0f;
            vtx->m_NY = 1.0f;
            vtx->m_NZ = 0.0f;
            vtx->m_U  = u + d_uv;
            vtx->m_V  = v + d_uv;
            vtx->m_X  = x + d_xz;
            vtx->m_Y  = y_position;
            vtx->m_Z  = z - d_xz;
            vtx++;
            vtx->m_NX = 0.0f;
            vtx->m_NY = 1.0f;
            vtx->m_NZ = 0.0f;
            vtx->m_U  = u + d_uv;
            vtx->m_V  = v;
            vtx->m_X  = x + d_xz;
            vtx->m_Y  = y_position;
            vtx->m_Z  = z;
            vtx++;

            x += d_xz;
            u += d_uv;
        }
        z -= d_xz;
        v += d_uv;
    }

    /* Lower Plane */
#define PLANE_THICKNESS (0.4f)
    v = 0.0f;
    z = (side_length/2.0f);
    for( i=0; i < triangles_per_side; i++ )
    {
        x = -(side_length/2.0f);
        u = 0.0f;
        for( j=0; j < triangles_per_side; j++ )
        {
            /* create square */
            vtx->m_NX = 0.0f;
            vtx->m_NY = -1.0f;
            vtx->m_NZ = 0.0f;
            vtx->m_U  = u;
            vtx->m_V  = v;
            vtx->m_X  = x;
            vtx->m_Y  = y_position - PLANE_THICKNESS;
            vtx->m_Z  = z;
            vtx++;
            vtx->m_NX = 0.0f;
            vtx->m_NY = -1.0f;
            vtx->m_NZ = 0.0f;
            vtx->m_U  = u;
            vtx->m_V  = v + d_uv;
            vtx->m_X  = x;
            vtx->m_Y  = y_position - PLANE_THICKNESS;
            vtx->m_Z  = z - d_xz;
            vtx++;
            vtx->m_NX = 0.0f;
            vtx->m_NY = -1.0f;
            vtx->m_NZ = 0.0f;
            vtx->m_U  = u + d_uv;
            vtx->m_V  = v + d_uv;
            vtx->m_X  = x + d_xz;
            vtx->m_Y  = y_position - PLANE_THICKNESS;
            vtx->m_Z  = z - d_xz;
            vtx++;
            vtx->m_NX = 0.0f;
            vtx->m_NY = -1.0f;
            vtx->m_NZ = 0.0f;
            vtx->m_U  = u;
            vtx->m_V  = v;
            vtx->m_X  = x;
            vtx->m_Y  = y_position - PLANE_THICKNESS;
            vtx->m_Z  = z;
            vtx++;
            vtx->m_NX = 0.0f;
            vtx->m_NY = -1.0f;
            vtx->m_NZ = 0.0f;
            vtx->m_U  = u + d_uv;
            vtx->m_V  = v + d_uv;
            vtx->m_X  = x + d_xz;
            vtx->m_Y  = y_position - PLANE_THICKNESS;
            vtx->m_Z  = z - d_xz;
            vtx++;
            vtx->m_NX = 0.0f;
            vtx->m_NY = -1.0f;
            vtx->m_NZ = 0.0f;
            vtx->m_U  = u + d_uv;
            vtx->m_V  = v;
            vtx->m_X  = x + d_xz;
            vtx->m_Y  = y_position - PLANE_THICKNESS;
            vtx->m_Z  = z;
            vtx++;

            x += d_xz;
            u += d_uv;
        }
        z -= d_xz;
        v += d_uv;
    }

    RGraphicAddToList(rc, rg, 0);
    return rg;
}


/**
    Creates a RGraphic representing text

    The RGraphic is placed in the 2D list.
    It uses "font" as the texture.

    @param rc The render context into whose 2D list the resulting RGraphic is placed.
    @param text_in The text to display, N.B This is parsed first, allowing for variable substitution to take place in RParseText.
    @param orig_x The x co-ordinate of the left edge of the text.
    @param orig_y The y co-ordinate of the top edge of the text.
    @param color The RGBA colour of the text.
    @return A pointer to the resulting RGraphic, or 0 for failure.
    @sa RParseText
*/
RGraphic * MEAPI RGraphicTextCreate(RRender *rc, char *text_in, AcmeReal orig_x, AcmeReal orig_y, const float color[4])
{
    int len, charcount;
    int i;
    RGraphic *rg;
    RObjectVertex *vtx;
    char letter;
    char *text;
    AcmeReal u, v, u_width, width, v_height, height, x, y;
    int row, col;
    int textsize;

    AcmeReal origX = MeMAX(orig_x, X_MINVISIBLE);
    AcmeReal origY = MeMAX(orig_y, Y_MINVISIBLE);

    textsize = strlen(text_in) + 200; /* Allow 200 characters for variable substitution */
    text = (char *)MeMemoryAPI.create(textsize);

    RParseText(rc, text_in, text, textsize);

    len = strlen(text);
    charcount = len;
    for(i = 0; i <= len; i++) {
        if( text[i] == '\n' )
            charcount --;
    }
    rg = RGraphicCreateEmpty(charcount*6);
    if( !rg )
        return 0;

    /* Set object matrix to identity and disable associated L->W matrix */
    rg->m_pLWMatrix = 0;

    MEASSERTALIGNED(rg->m_pObject->m_Matrix, 16);

    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);

    rg->m_pObject->m_bIsWireFrame = 0;

    RGraphicSetColor(rg,color);

    /* Set texture to font. Destroy object if failed. */
    rg->m_pObject->m_nTextureID = RRenderTextureCreate(rc, "font");
    if( rg->m_pObject->m_nTextureID == -1 )
    {
        MeMemoryAPI.destroy(text);
        RGraphicDestroy(rg);
        return 0;
    }

    /* Set vertices... */
    x = origX;
    y = origY;
    /*height = (AcmeReal)24.0f;*/
        height = (AcmeReal)20.0f;
        v_height = (AcmeReal)24.0f/256.0f;
    vtx = rg->m_pVertices;
    for( i = 0; i < len; i++ )
    {
        letter = text[i];
        if( letter == '\n' )
        {
            y += height;
            x = origX;
        }
        else
        {
            if( letter < 32 || letter > 126 )
                letter = 32;                    /* if not in font, then use a space */
            letter -= 32;
            row = (int)letter / 10;
            col = (int)letter % 10;
            v = (AcmeReal)row * 24.0f / 256.0f;
            u = (AcmeReal)col * 24.0f / 256.0f;
            u_width = (AcmeReal)R_CharacterWidths[row][col] / 256.0f;
            /*width = (AcmeReal)R_CharacterWidths[row][col];*/
                        width = (AcmeReal)R_CharacterWidths[row][col]/1.2f;
            vtx->m_NX = 0.0f;
            vtx->m_NY = 0.0f;
            vtx->m_NZ = -1.0f;
            vtx->m_Z = 0.0f;
            vtx->m_X = x;
            vtx->m_Y = y;
            vtx->m_U = u;
            vtx->m_V = v;
            vtx++;

            vtx->m_NX = 0.0f;
            vtx->m_NY = 0.0f;
            vtx->m_NZ = -1.0f;
            vtx->m_Z = 0.0f;
            vtx->m_X = x;
            vtx->m_Y = y + height;
            vtx->m_U = u;
            vtx->m_V = v + v_height;
            vtx++;

            vtx->m_NX = 0.0f;
            vtx->m_NY = 0.0f;
            vtx->m_NZ = -1.0f;
            vtx->m_Z = 0.0f;
            vtx->m_X = x + width;
            vtx->m_Y = y;
            vtx->m_U = u + u_width;
            vtx->m_V = v;
            vtx++;

            vtx->m_NX = 0.0f;
            vtx->m_NY = 0.0f;
            vtx->m_NZ = -1.0f;
            vtx->m_Z = 0.0f;
            vtx->m_X = x;
            vtx->m_Y = y + height;
            vtx->m_U = u;
            vtx->m_V = v + v_height;
            vtx++;

            vtx->m_NX = 0.0f;
            vtx->m_NY = 0.0f;
            vtx->m_NZ = -1.0f;
            vtx->m_Z = 0.0f;
            vtx->m_X = x + width;
            vtx->m_Y = y;
            vtx->m_U = u + u_width;
            vtx->m_V = v;
            vtx++;

            vtx->m_NX = 0.0f;
            vtx->m_NY = 0.0f;
            vtx->m_NZ = -1.0f;
            vtx->m_Z = 0.0f;
            vtx->m_X = x + width;
            vtx->m_Y = y + height;
            vtx->m_U = u + u_width;
            vtx->m_V = v + v_height;
            vtx++;

            x += width;
        }
    }

    MeMemoryAPI.destroy(text);

    RGraphicAddToList(rc, rg, 1);
    return rg;
}


/**
    Parses text, substituting text for variables.

    Variables defined by $ followed by capitals or numbers are substituted
    by text in RRender.

    @param rc Source for strings to be substituted.
    @param text_in Input string containing variables to be substituted.
    @param text_out Output string returned with text substituted for variables.
    @param outbuffersize Amount of memory you allocated that is pointed to by
    text_out, i.e. maximum size of output string.
*/
void MEAPI RParseText(RRender *rc, char *text_in, char *text_out, int outbuffersize)
{
    unsigned int i,j,k;
    int buttonID;
    int bInVar = 0;
    char tokenbuf[20];

    memset(text_out, 0, outbuffersize);

    j=0;
    k=0;
    for( i = 0; i < strlen(text_in); i++ )
    {
        if( strlen(text_out) >= (unsigned int)outbuffersize )
        {
            text_out[outbuffersize-1] = '\0';
            return;
        }
        if( text_in[i] == '$' )
        {
            if( !bInVar )
                bInVar = 1;
        }
        else
        {
            if( !bInVar )
            {
                text_out[j] = text_in[i];
                j++;
            }
            else
            {
                /* in a token */
                if( ((text_in[i] > 90 || text_in[i] < 65) && (text_in[i] > 57 || text_in[i] < 48) ) || k == 19 )
                    /* end of token (must be upper case or number, max length 19+'\0')*/
                {
                    tokenbuf[k] = '\0';

                    buttonID = -1;
                    /* token held in buffer - substitute */
                    if( !strcmp(tokenbuf, "UP") )
                        buttonID = 0;
                    else if( !strcmp(tokenbuf, "DOWN") )
                        buttonID = 1;
                    else if( !strcmp(tokenbuf, "LEFT") )
                        buttonID = 2;
                    else if( !strcmp(tokenbuf, "RIGHT") )
                        buttonID = 3;
                    else if( !strcmp(tokenbuf, "UP2") )
                        buttonID = 4;
                    else if( !strcmp(tokenbuf, "DOWN2") )
                        buttonID = 5;
                    else if( !strcmp(tokenbuf, "LEFT2") )
                        buttonID = 6;
                    else if( !strcmp(tokenbuf, "RIGHT2") )
                        buttonID = 7;
                    else if( !strcmp(tokenbuf, "ACTION0") )
                        buttonID = 8;
                    else if( !strcmp(tokenbuf, "ACTION1") )
                        buttonID = 9;
                    else if( !strcmp(tokenbuf, "ACTION2") )
                        buttonID = 10;
                    else if( !strcmp(tokenbuf, "ACTION3") )
                        buttonID = 11;
                    else if( !strcmp(tokenbuf, "ACTION4") )
                        buttonID = 12;
                    else if( !strcmp(tokenbuf, "ACTION5") )
                        buttonID = 13;
                    else if( !strcmp(tokenbuf,"MOUSE") )
                        buttonID = 14;
                    else if( !strcmp(tokenbuf, "JOYSTICK") )
                        buttonID = 15;

                    if( buttonID != -1 )
                    {
                        strcat(text_out, rc->m_ButtonText[buttonID]);
                        j += strlen(rc->m_ButtonText[buttonID]);
                    }
                    else
                    {
                        if( !strcmp(tokenbuf, "APPNAME") )
                        {
                            strcat(text_out, rc->m_AppName);
                            j += strlen( rc->m_AppName );
                        }
                        else
                        {
                            strcat(text_out, "<unknown>");
                            j += 9;
                        }
                    }

                    i--; /* let the character after token be processed */
                    k=0;
                    bInVar = 0;
                }
                else
                {
                    tokenbuf[k] = text_in[i];
                    k++;
                }
            }
        }

    }
}


/**
    Creates a RGraphic rectangle

    The RGraphic is put in the 2D list.

    @param rc The render context into whose 2D list the resulting RGraphic is put.
    @param orig_x The x co-ordinate of the left edge of the rectangle.
    @param orig_x The y co-ordinate of the top edge of the rectangle.
    @param width The x dimension of the rectangle.
    @param height The y dimension of the rectangle.
    @return A pointer to the resulting RGraphic, or 0 for failure.
*/
RGraphic * MEAPI RGraphic2DRectangleCreate( RRender *rc, AcmeReal orig_x, AcmeReal orig_y, AcmeReal width, AcmeReal height, const float color[4])
{
    RGraphic * rg = RGraphicCreateEmpty(6);
    RObjectVertex* rv = rg->m_pVertices;

    MEASSERTALIGNED(rg->m_pObject->m_Matrix, 16);

    rv->m_X = -(width/2);
    rv->m_Y = (height/2);
    rv->m_Z = 0;
    rv->m_NX = 0;
    rv->m_NY = 0;
    rv->m_NZ = -1;
    rv->m_U = 0;
    rv->m_V = 0;
    rv++;

    rv->m_X = -(width/2);
    rv->m_Y = -(height/2);
    rv->m_Z = 0;
    rv->m_NX = 0;
    rv->m_NY = 0;
    rv->m_NZ = -1;
    rv->m_U = 0;
    rv->m_V = 1;
    rv++;

    rv->m_X = (width/2);
    rv->m_Y = (height/2);
    rv->m_Z = 0;
    rv->m_NX = 0;
    rv->m_NY = 0;
    rv->m_NZ = -1;
    rv->m_U = 1;
    rv->m_V = 0;
    rv++;

    rv->m_X = -(width/2);
    rv->m_Y = -(height/2);
    rv->m_Z = 0;
    rv->m_NX = 0;
    rv->m_NY = 0;
    rv->m_NZ = -1;
    rv->m_U = 0;
    rv->m_V = 1;
    rv++;

    rv->m_X = (width/2);
    rv->m_Y = (height/2);
    rv->m_Z = 0;
    rv->m_NX = 0;
    rv->m_NY = 0;
    rv->m_NZ = -1;
    rv->m_U = 1;
    rv->m_V = 0;
    rv++;

    rv->m_X = (width/2);
    rv->m_Y = -(height/2);
    rv->m_Z = 0;
    rv->m_NX = 0;
    rv->m_NY = 0;
    rv->m_NZ = -1;
    rv->m_U = 1;
    rv->m_V = 1;

    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);
    rg->m_pObject->m_nTextureID = -1; /* no texture */
    RGraphicSetColor(rg, color);
    rg->m_pObject->m_bIsWireFrame = 0; /* default to solid */
    rg->m_pLWMatrix = 0;
    RGraphicAddToList(rc, rg, 1);

    if (rg)
    {
        rg->m_pObject->m_Matrix[3][0] = orig_x + 0.5f*width;
        rg->m_pObject->m_Matrix[3][1] = orig_y + 0.5f*height;
    }
    return rg;
}

void MEAPI RGraphicTranslateVertices(RRender * rc,RGraphic *rg, MeVector3Ptr offset)
{
    RObjectVertex* vtx = rg->m_pVertices;
    int i;

    for(i=0; i < rg->m_pObject->m_nNumVertices; i++)
    {
        vtx[i].m_X += offset[0];
        vtx[i].m_Y += offset[1];
        vtx[i].m_Z += offset[2];        
    }

#ifdef WITH_OPENGL
    if(  rc->m_options.m_renderType == kROpenGL )
        RGraphicCalcOGLDisplayList(rg);
#endif

}

void MEAPI RGraphicTranslate2D(RRender* rc, RGraphic* rg, AcmeReal dX, AcmeReal dY) {
    RObjectVertex* rv = rg->m_pVertices;
    int i;
    for(i=0; i != rg->m_pObject->m_nNumVertices; i++, rv++)
    {
        rv->m_X += dX;
        rv->m_Y += dY;

    }
}

#define TICK_MINY (Y_MAXVISIBLE - 33.0f)
#define TICK_MAXY (TICK_MINY + 4.0f)

RGraphic* RPerformanceBarCreateTicks( RRender *rc )
{
    RGraphic *rg;
    RObjectVertex *vtx;
    float scalecol[4] = {1.0f, 1.0f, 1.0f, 0.7f};


    rg = RGraphicCreateEmpty(15);
    if( !rg )
        return 0;

    vtx = rg->m_pVertices;

    vtx->m_X    = 3.0f; vtx->m_Y        = TICK_MINY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;
    vtx->m_X    = 0.5f; vtx->m_Y        = TICK_MAXY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;
    vtx->m_X    = 5.5f; vtx->m_Y        = TICK_MAXY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;
    
    vtx->m_X    = 67.0f; vtx->m_Y       = TICK_MINY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;
    vtx->m_X    = 64.5f; vtx->m_Y       = TICK_MAXY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;
    vtx->m_X    = 69.5f; vtx->m_Y       = TICK_MAXY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;

    vtx->m_X    = 131.0f; vtx->m_Y      = TICK_MINY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;
    vtx->m_X    = 128.5f; vtx->m_Y      = TICK_MAXY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;
    vtx->m_X    = 133.5f; vtx->m_Y      = TICK_MAXY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;

    vtx->m_X    = 195.0f; vtx->m_Y      = TICK_MINY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;
    vtx->m_X    = 192.5f; vtx->m_Y      = TICK_MAXY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;
    vtx->m_X    = 197.5f; vtx->m_Y      = TICK_MAXY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;

    vtx->m_X    = 259.0f; vtx->m_Y      = TICK_MINY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;
    vtx->m_X    = 256.5f; vtx->m_Y      = TICK_MAXY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;
    vtx->m_X    = 261.5f; vtx->m_Y      = TICK_MAXY;   vtx->m_Z = 0.0f;
    vtx->m_NX   = 0.0f; vtx->m_NY       = 0.0f;     vtx->m_NZ   = -1.0f;
    vtx->m_U    = 0.0f; vtx->m_V= 0.0f;  vtx++;

    rg->m_pObject->m_nTextureID = -1; /* no texture */
    RGraphicSetColor(rg,scalecol);
    rg->m_pObject->m_bIsWireFrame = 0; /* default to solid */
    rg->m_pLWMatrix = 0;
    MeMatrix4TMMakeIdentity((MeMatrix4Ptr)rg->m_pObject->m_Matrix);
    RGraphicAddToList(rc, rg, 1);
    
    return rg;
}

/**
    Sets up the performance bar

    @param rc The render context whose performance bar is to be created.
    @return A pointer to the newly created performance bar, or 0 for failure.
*/
RPerformanceBar * MEAPI RPerformanceBarCreate( RRender *rc )
{
    float dyncol[4] = {1.0f, 0.0f, 0.0f, 0.7f};
    float colcol[4] = {0.0f, 1.0f, 0.0f, 0.7f};
    float rencol[4] = {0.0f, 0.0f, 1.0f, 0.7f};
    float idlecol[4] = {0.7f, 0.7f, 0.7f, 0.7f};
    float scalecol[4] = {1.0f, 1.0f, 1.0f, 0.7f};

    if( rc->m_PerformanceBar )
        return rc->m_PerformanceBar;

    rc->m_PerformanceBar = (RPerformanceBar *)MeMemoryAPI.create(sizeof(RPerformanceBar));
    if( !rc->m_PerformanceBar )
        return 0;

    rc->m_PerformanceBar->m_ColBar = RGraphic2DRectangleCreate(rc, 0.0f + X_PERFBAROFFSET,Y_MAXVISIBLE - 38.0f,1.0f,5.0f, colcol);
    rc->m_PerformanceBar->m_DynBar = RGraphic2DRectangleCreate(rc, 0.0f + X_PERFBAROFFSET,Y_MAXVISIBLE - 38.0f,1.0f,5.0f, dyncol);
    rc->m_PerformanceBar->m_RenBar = RGraphic2DRectangleCreate(rc, 0.0f + X_PERFBAROFFSET,Y_MAXVISIBLE - 38.0f,1.0f,5.0f, rencol);
    rc->m_PerformanceBar->m_IdleBar = RGraphic2DRectangleCreate(rc, 0.0f + X_PERFBAROFFSET,Y_MAXVISIBLE - 38.0f,1.0f,5.0f, idlecol);

    rc->m_PerformanceBar->m_Ticks = RPerformanceBarCreateTicks(rc);
    RGraphicTranslate2D(rc, rc->m_PerformanceBar->m_Ticks, X_PERFBAROFFSET, 0);

    dyncol[3] = colcol[3] = rencol[3] = scalecol[3] = 1.0f;

    rc->m_PerformanceBar->m_ColText = RGraphicTextCreate(rc, "Collision", 0.0f + X_PERFBAROFFSET, Y_MAXVISIBLE - 68.0f, colcol );
    rc->m_PerformanceBar->m_DynText = RGraphicTextCreate(rc, "Dynamics", 75.0f + X_PERFBAROFFSET, Y_MAXVISIBLE - 68.0f, dyncol );
    rc->m_PerformanceBar->m_RenText = RGraphicTextCreate(rc, "Rendering", 165.0f + X_PERFBAROFFSET, Y_MAXVISIBLE - 68.0f, rencol );

    rc->m_PerformanceBar->m_Scale0 = RGraphicTextCreate(rc, "0ms", 0.0f + X_PERFBAROFFSET, Y_MAXVISIBLE - 27.0f, scalecol);
    rc->m_PerformanceBar->m_Scale4 = RGraphicTextCreate(rc, "4ms", 64.0f + X_PERFBAROFFSET, Y_MAXVISIBLE - 27.0f, scalecol);
    rc->m_PerformanceBar->m_Scale8 = RGraphicTextCreate(rc, "8ms", 128.0f + X_PERFBAROFFSET, Y_MAXVISIBLE - 27.0f, scalecol);
    rc->m_PerformanceBar->m_Scale12 = RGraphicTextCreate(rc, "12ms", 192.0f + X_PERFBAROFFSET, Y_MAXVISIBLE - 27.0f, scalecol);
    rc->m_PerformanceBar->m_Scale16 = RGraphicTextCreate(rc, "16ms", 256.0f + X_PERFBAROFFSET, Y_MAXVISIBLE - 27.0f, scalecol);

    return rc->m_PerformanceBar;
}


/**
    Updates the performance bar.

    This is called by the platform-specific back-end

    @param rc The render context whose performance bar is to be updated.
    @param coltime The time taken by the collision detection update in the last frame.
    @param dyntime The time taken by the dynamics simulation update in the last frame.
    @param rentime The time taken by the rendering in the last frame.
    @param idletime The amount of idle time in the last frame (e.g. waiting for VSync)
*/
void MEAPI RPerformanceBarUpdate( RRender *rc, AcmeReal coltime, AcmeReal dyntime, AcmeReal rentime, AcmeReal idletime )
{
    AcmeReal colstart, colend;
    AcmeReal dynstart, dynend;
    AcmeReal renstart, renend;
    AcmeReal idlestart, idleend;
    AcmeReal *colmatrix;
    AcmeReal *dynmatrix;
    AcmeReal *renmatrix;
#if 0
    RObjectVertex *vtx;
    AcmeReal *idlematrix;
#endif

    if(!rc->m_PerformanceBar)
        return;

    colstart = 0.0f;
    colend = colstart + 16.0f * coltime;
    dynstart = colend;
    dynend = dynstart + 16.0f * dyntime;
    renstart = dynend;
    renend = renstart + 16.0f * rentime;
    idlestart = renend;
    idleend = idlestart + 16.0f * idletime;

    colmatrix = (AcmeReal*)rc->m_PerformanceBar->m_ColBar->m_pObject->m_Matrix;
    dynmatrix = (AcmeReal*)rc->m_PerformanceBar->m_DynBar->m_pObject->m_Matrix;
    renmatrix = (AcmeReal*)rc->m_PerformanceBar->m_RenBar->m_pObject->m_Matrix;

    colmatrix[12] = X_PERFBAROFFSET + coltime * 8.0f;
    colmatrix[0]  = coltime * 16.0f;

    dynmatrix[12] = X_PERFBAROFFSET + coltime * 16.0f + dyntime * 8.0f;
    dynmatrix[0]  = dyntime * 16.0f;

    renmatrix[12] = X_PERFBAROFFSET + coltime * 16.0f + dyntime * 16.0f + rentime * 8.0f;
    renmatrix[0]  = rentime * 16.0f;

#if 0
    vtx = rc->m_PerformanceBar->m_ColBar->m_pVertices;
    vtx->m_X = colstart;
    vtx++;
    vtx->m_X = colstart;
    vtx++;
    vtx->m_X = colend;
    vtx++;
    vtx->m_X = colstart;
    vtx++;
    vtx->m_X = colend;
    vtx++;
    vtx->m_X = colend;

    vtx = rc->m_PerformanceBar->m_DynBar->m_pVertices;
    vtx->m_X = dynstart;
    vtx++;
    vtx->m_X = dynstart;
    vtx++;
    vtx->m_X = dynend;
    vtx++;
    vtx->m_X = dynstart;
    vtx++;
    vtx->m_X = dynend;
    vtx++;
    vtx->m_X = dynend;

    vtx = rc->m_PerformanceBar->m_RenBar->m_pVertices;
    vtx->m_X = renstart;
    vtx++;
    vtx->m_X = renstart;
    vtx++;
    vtx->m_X = renend;
    vtx++;
    vtx->m_X = renstart;
    vtx++;
    vtx->m_X = renend;
    vtx++;
    vtx->m_X = renend;

    vtx = rc->m_PerformanceBar->m_IdleBar->m_pVertices;
    vtx->m_X = idlestart;
    vtx++;
    vtx->m_X = idlestart;
    vtx++;
    vtx->m_X = idleend;
    vtx++;
    vtx->m_X = idlestart;
    vtx++;
    vtx->m_X = idleend;
    vtx++;
    vtx->m_X = idleend;
#endif
}


/**
   Creates the FPS RGraphic

   The RGraphic is added to the 2D list
   This is called by the platform-specific back-end

   @param rc The render context in which the frame rate will be displayed.
   @param fps The current frame rate.
*/
void MEAPI RRenderDisplayFps( RRender *rc, AcmeReal fps )
{
    float fpsCol[4] = {1.0f, 1.0f, 1.0f, 0.7f};
    char numbuf[6];
    char buf[10];

    if( rc->m_fpsG )
        RGraphicDelete(rc, rc->m_fpsG, 1);

    buf[0] = '\0';
    snprintf( numbuf, 6, "%.3g", fps);
    if( numbuf[strlen(numbuf)-1] == '.' )
        numbuf[strlen(numbuf)-1] = '\0';
    strcat(buf, numbuf);
    strcat(buf, "fps");

    rc->m_fpsG = RGraphicTextCreate(rc, buf, X_MAXVISIBLE-80, Y_MAXVISIBLE-27, fpsCol);
}


/**
    Makes sure RGraphic lies between -1, 1 on all axes
    and is centered on 0,0,0

    @param rg The RGraphicx in question.
    @param bKeepAspectRatio specifies whether to maintain
    aspect ratio when normalizing.
*/
void MEAPI RGraphicNormalize( RGraphic *rg, int bKeepAspectRatio )
{
    RObjectVertex *vtx;
    int i;
    AcmeReal minx, maxx;
    AcmeReal miny, maxy;
    AcmeReal minz, maxz;
    AcmeReal transx, transy, transz;
    AcmeReal scalex, scaley, scalez;
    AcmeReal scale;

    minx = miny = minz = maxx = maxy = maxz = 0.0f;
    vtx = rg->m_pVertices;
    for(i = 0; i < rg->m_pObject->m_nNumVertices; i++ )
    {
        maxx = (vtx->m_X > maxx) ? vtx->m_X : maxx;
        maxy = (vtx->m_Y > maxy) ? vtx->m_Y : maxy;
        maxz = (vtx->m_Z > maxz) ? vtx->m_Z : maxz;
        minx = (vtx->m_X < minx) ? vtx->m_X : minx;
        miny = (vtx->m_Y < miny) ? vtx->m_Y : miny;
        minz = (vtx->m_Z < minz) ? vtx->m_Z : minz;
        vtx++;
    }

    if(maxx > 1.00001f || maxy > 1.00001f || maxz > 1.00001f || minx < -1.00001f || miny < -1.00001f || minz < -1.00001f)
    {
        transx = -(maxx + minx)/2.0f;
        transy = -(maxy + miny)/2.0f;
        transz = -(maxz + minz)/2.0f;

        scalex = 2.0f / (maxx - minx);
        scaley = 2.0f / (maxy - miny);
        scalez = 2.0f / (maxz - minz);

        if( bKeepAspectRatio )
        {
            scale = 1.0f;
            if( scalex < 1.0f || scaley < 1.0f || scalez < 1.0f )
            {
                /* find minimum */
                scale = (scalex > scaley) ? scaley : scalex;
                scale = (scale > scalez) ? scalez : scale;
            }
            else
            {
                /*find maximum */
                scale = (scalex > scaley) ? scalex : scaley;
                scale = (scale > scalez) ? scale : scalez;
            }
            scalex = scaley = scalez = scale;
        }

        MeInfo(1,"Normalize:\n %2.03f < x < %2.03f\n %2.03f < y < %2.03f\n %2.03f < z < %2.03f",
            minx,maxx,miny,maxy,minz,maxz );
        MeInfo(1,"Translation required is (%2.03f, %2.03f, %2.03f)", transx, transy, transz);
        MeInfo(1,"Scaling is (%2.03f, %2.03f, %2.03f)", scalex, scaley, scalez);

        /* do the translation & scaling */
        vtx = rg->m_pVertices;
        for( i = 0; i < rg->m_pObject->m_nNumVertices; i++ )
        {
            vtx->m_X = (vtx->m_X + transx)*scalex;
            vtx->m_Y = (vtx->m_Y + transy)*scaley;
            vtx->m_Z = (vtx->m_Z + transz)*scalez;
            vtx++;
        }
    } /* end: if(requires normalisation) */

 return;  /* it's all for show */
}


/**
    Creates a geometry file from a RGraphic.

    @param rg The graphic to be saved.
    @param filename The name under which to save the geometry.
*/
int MEAPI RGraphicSave( RGraphic *rg, char *filename )
{
    char* buf;
    char numbuf[20];
    int bufsize;
    int i;
    int fh; /* file handle */
    RObjectVertex *vtx;

    bufsize = rg->m_pObject->m_nNumVertices * 160;

    buf = (char *)MeMemoryAPI.create(bufsize);
    if( !buf )
        return 1;

    MeInfo(1,"Preparing data to create %s\nPlease wait...", filename);

    buf[0] = '\0';
    strcat(buf, "vc:");
    snprintf(numbuf, 20, "%.10d", rg->m_pObject->m_nNumVertices);
    strcat(buf, numbuf);
    strcat(buf, " /\n\n");

    vtx = rg->m_pVertices;
    for( i = 0; i < rg->m_pObject->m_nNumVertices; i++ )
    {
        strcat(buf, "x:");
        snprintf(numbuf, 20,"%.8g", vtx->m_X);
        strcat(buf, numbuf);
        strcat(buf, " y:");
        snprintf(numbuf, 20,"%.8g", vtx->m_Y);
        strcat(buf, numbuf);
        strcat(buf, " z:");
        snprintf(numbuf, 20,"%.8g", vtx->m_Z);
        strcat(buf, numbuf);
        strcat(buf, " nx:");
        snprintf(numbuf, 20,"%.8g", vtx->m_NX);
        strcat(buf, numbuf);
        strcat(buf, " ny:");
        snprintf(numbuf, 20,"%.8g", vtx->m_NY);
        strcat(buf, numbuf);
        strcat(buf, " nz:");
        snprintf(numbuf, 20,"%.8g", vtx->m_NZ);
        strcat(buf, numbuf);
        strcat(buf, " u:");
        snprintf(numbuf, 20,"%.8g", vtx->m_U);
        strcat(buf, numbuf);
        strcat(buf, " v:");
        snprintf(numbuf, 20,"%.8g", vtx->m_V);
        strcat(buf, numbuf);
        strcat(buf, " /\n");
        if( i%3 == 2 )
            strcat(buf, "\n");
        vtx++;
    }
    strcat(buf, "\n");

    MeInfo(1,"Created buffer. Writing file.");

    fh = MeOpen( filename, kMeOpenModeWRONLY );
    if( fh )
    {
        MeWrite(fh, buf, strlen(buf));
        MeClose(fh);
        MeInfo(1,"Done.");
    }
    else
        ME_REPORT(MeWarning(1,"Couldn't create file."));

    MeMemoryAPI.destroy (buf);
    return 0;
}


/**
    Loads a 128*128*24 or 256*256*24 .bmp file

    Fills in RImage struct, and creates 32bit bitmap at m_pImage of RImage
    This allocate memory that must be freed later.

    @param rc The current render context (ignored).
    @param filename The name of the file to attempt to load.
    @param p_image Returned as pointer to the block of memory containing the
    image after loading. N.B. Do not allocate memory yourself and pass a
    pointer in - this will be ignored since the memory is allocated inside
    this function.
    @param bRequireBGR Output is in format BGRA rather than the standard RGBA.
*/
int MEAPI RBmpLoad(RRender* rc, char *filename, RImage *p_image, int bRequireBGR)
{
    int fh;
    unsigned int len, lenread;
    char *fullname = (char *)MeMemoryAPI.create( strlen(filename) + 55 );
    unsigned char *fileBuffer;
    MeU32 bitmap_offset;
    MeU32 compression_type;
    MeU16 bpp;
    MeU32 height, width;
    MeU32 col;
    int pixelcount;
    int i,j;
    int bSkipLoad = 0;
    unsigned char r,g,b,a,tmp;

    MeU32 headerLength;

    fullname[0]='\0';
    strcat(fullname, filename);
    strcat(fullname,".bmp");

    fh = MeOpenWithSearch( fullname, kMeOpenModeRDBINARY );
    if( fh < 0)
    {
        ME_REPORT(MeWarning(1,"Couldn't open file: %s", fullname ));
        MeMemoryAPI.destroy(fullname);
        return 1;
    }
    MeMemoryAPI.destroy(fullname);

    len = MeLseek(fh, 0, kMeSeekEND);

    fileBuffer = (unsigned char *)MeMemoryAPI.create(len);
    if( !fileBuffer )
    {
        ME_REPORT(MeWarning(1,"MeMemoryAPI.create failed for file buffer."));
        return 1;
    }

    if( MeLseek(fh, 0, kMeSeekSET) == -1 )
        ME_REPORT(MeWarning(1,"MeSeek failed."));

    else if( (lenread = MeRead(fh, fileBuffer, len)) != len )
        ME_REPORT(MeInfo(0,"MeRead failed.(len %d, lenread %d)", len, lenread));

    headerLength =(((MeU32)fileBuffer[17] << 24) & 0xff)
        |  (((MeU32)fileBuffer[16] << 16) & 0xff)
        |  (((MeU32)fileBuffer[15] << 8) & 0xff)
        |  ((MeU32)fileBuffer[14] & 0xff);
    bitmap_offset = headerLength + 14;

    if(12==headerLength) {
        /* OS/2 header style :) */
        bpp = *(MeU16*)(&fileBuffer[24]);
        compression_type = 0;
    } else {
        /* Windows style */
        bpp = ((((MeU16)fileBuffer[29] & 0xff) << 8 ))
        | (((MeU16)fileBuffer[28] & 0xff));
        compression_type = ((((MeU32)fileBuffer[33] & 0xff) << 24) )
            |  ((((MeU32)fileBuffer[32]& 0xff) << 16) )
            |  ((((MeU32)fileBuffer[31] & 0xff) << 8))
            |  (((MeU32)fileBuffer[30]& 0xff));
        width = ((((MeU32)fileBuffer[21] & 0xff) << 24))
            |  ((((MeU32)fileBuffer[20] & 0xff) << 16))
            |  ((((MeU32)fileBuffer[19] & 0xff) << 8))
            |  (((MeU32)fileBuffer[18]& 0xff) );
        height =  ((((MeU32)fileBuffer[25]  & 0xff) << 24))
            | ((((MeU32)fileBuffer[24] & 0xff) << 16))
            | ((((MeU32)fileBuffer[23] & 0xff) << 8))
            | (((MeU32)fileBuffer[22] & 0xff));
    }

    if( bpp != 24 )
    {
        ME_REPORT(MeWarning(1,"%s.bmp is not 24bpp and so cannot be loaded.", filename));
        bSkipLoad = 1;
    }
    if( compression_type != 0 ) /* BI_RGB */
    {
        ME_REPORT(MeWarning(1,"%s.bmp is in a compressed format and will not be loaded.", filename));
        bSkipLoad = 1;
    }

    if( (width != height)
        || ((width != 128) && (width != 256)) )
    {
        ME_REPORT(MeWarning(1,"%s.bmp is not 128x128 or 256x256 pixels and will not be loaded.", filename));
        bSkipLoad = 1;
    }

    pixelcount = width * height;

    if( !bSkipLoad )
    {
        p_image->m_height = height;
        p_image->m_width = width;
        p_image->m_pImage = (unsigned char*)MeMemoryAPI.createZeroed(pixelcount * 4); /* for 32bit out */

        j = (pixelcount - width)*4;
        col = 0;
        for( i = 0; i < pixelcount; i++ )
        {
            b = (unsigned char)fileBuffer[bitmap_offset + i*3];
            g = (unsigned char)fileBuffer[bitmap_offset + i*3 + 1];
            r = (unsigned char)fileBuffer[bitmap_offset + i*3 + 2];
            a = (unsigned char)((r+g+b)/3);
            if( bRequireBGR )
            {
                tmp = b;
                b = r;
                r = tmp;
            }

            p_image->m_pImage[j++] = r;
            p_image->m_pImage[j++] = g;
            p_image->m_pImage[j++] = b;
            p_image->m_pImage[j++] = a;
            col ++;
            if( col == width )
            {
                col = 0;
                j -= width * 8;
            }
        }
    }

    if(MeClose(fh))
        ME_REPORT(MeWarning(1,"Failed to close file handle. Continuing anyway..."));

    MeMemoryAPI.destroy(fileBuffer);

    return 0;
}

void MEAPI RLightSwitchOn(RRender* rc, RRenderLight light) {
    switch(light) {
    case kRAmbient:
        rc->m_bUseAmbientLight = !MEFALSE;
        rc->m_dirty |= MEV_AMBLIGHT;
        break;
    case kRDirect1:
        rc->m_DirectLight1.m_bUseLight = !MEFALSE;
        rc->m_dirty |= MEV_DIRLIGHT1;
        break;
    case kRDirect2:
        rc->m_DirectLight2.m_bUseLight = !MEFALSE;
        rc->m_dirty |= MEV_DIRLIGHT2;
        break;
    case kRPoint:
        rc->m_PointLight.m_bUseLight = !MEFALSE;
        rc->m_dirty |= MEV_POINTLIGHT;
        break;
    }
}

void MEAPI RLightSwitchOff(RRender* rc, RRenderLight light) {

    switch(light) {
    case kRAmbient:
        rc->m_bUseAmbientLight = MEFALSE;
        rc->m_dirty |= MEV_AMBLIGHT;
        break;
    case kRDirect1:
        rc->m_DirectLight1.m_bUseLight = MEFALSE;
        rc->m_dirty |= MEV_DIRLIGHT1;
        break;
    case kRDirect2:
        rc->m_DirectLight2.m_bUseLight = MEFALSE;
        rc->m_dirty |= MEV_DIRLIGHT2;
        break;
    case kRPoint:
        rc->m_PointLight.m_bUseLight = MEFALSE;
        rc->m_dirty |= MEV_POINTLIGHT;
        break;
    }
}

void MEAPI RRenderSetAppName(RRender* rc, const char* appname) {
    strncpy(rc->m_AppName, appname, sizeof(rc->m_AppName));
    rc->m_AppName[sizeof(rc->m_AppName)-1] = '\0';
}

/**
        Sets the background colour.
*/
void MEAPI RRenderSetBackgroundColor(RRender* rc,  const float color[4])
{
    rc->m_backgroundColour[0] = color[0];
    rc->m_backgroundColour[1] = color[1];
    rc->m_backgroundColour[2] = color[2];
}

/* ****************** DEBUG LINES ********************** */

/** Add a new line to be drawn. */
void MEAPI RLineAdd(RRender*const  rc,
                    const AcmeVector3 start, const AcmeVector3 end,
                    const AcmeReal r, const AcmeReal g, const AcmeReal b)
{
    static const AcmeReal zero3[3] = {0, 0, 0};
    static float white[4] = {1, 1, 1, 1};
    float color[4];
    RNativeLine* newLine;

    /* If we have no spare lines, increase pool of them. */
    if(rc->m_numLines == rc->m_usedLines)
    {
        int newSize;

        if(rc->m_numLines == 0) /* If pool is empty, create. */
        {
            newSize = 1;
            rc->m_lines = MeMemoryAPI.create(newSize * sizeof(RGraphic*));
        }
        else /* Otherwise, resize (double) */
        {
            newSize = rc->m_numLines * 2;
            rc->m_lines = MeMemoryAPI.resize(rc->m_lines, newSize * sizeof(RGraphic*));
        }

        /* Create line graphics, but dont put into list yet. */
        while(rc->m_numLines < newSize)
        {
            rc->m_lines[rc->m_numLines] = RNLineCreate(rc, zero3, zero3, white, 0);
            RNLineRemoveFromList(rc, rc->m_lines[rc->m_numLines]);

            (rc->m_numLines)++;
        }
    }

    /* We should now have a spare line! */
    MEASSERT(rc->m_numLines > rc->m_usedLines);

    newLine = rc->m_lines[rc->m_usedLines];

    RNLineAddToList(rc, newLine);

    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = 1;

    RNLineSetColor(newLine, color);
    RNLineMoveEnds(newLine, start, end);

    (rc->m_usedLines)++;
}

/** Remove all lines currently being drawn. */
void MEAPI RLineRemoveAll(RRender* rc)
{
    int i;

    /* Remove all lines from draw list. */
    for(i=0; i<rc->m_usedLines; i++)
    {
        RNLineRemoveFromList(rc, rc->m_lines[i]);
    }

    rc->m_usedLines = 0;
}
/* ***************************************************** */

/**
        Converts window coordinates to renderer coordinates.
        portalx and portaly are out parameters.
*/
void MEAPI RCalculatePortalCoordinates(RRender* rc,AcmeReal width, AcmeReal height,int x,int y,AcmeReal* portalx,AcmeReal* portaly)
{
        AcmeReal windowRatio = width/height;
        AcmeReal portalscale;

        /* if window is correct ratio, just scale */
        if(ME_ARE_EQUAL(windowRatio, rc->m_AspectRatio))
        {
                MeReal portalscaleX = 640/width;
                MeReal portalscaleY = 448/height;

                *portalx = (AcmeReal)x * portalscaleX;
                *portaly = (AcmeReal)y * portalscaleY;
        }
        /* if x is too long */
        else if(windowRatio > rc->m_AspectRatio)
        {
                AcmeReal deadx;
                portalscale = 448/height;
                *portaly = (AcmeReal)y * portalscale;

                deadx = width*(AcmeReal)0.5 - 320/portalscale;

                *portalx = ((AcmeReal)x - deadx) * portalscale;
        }
        /* if y is too long */
        else
        {
                AcmeReal deady;
                portalscale = 640/(AcmeReal)*RMouseDrag.p_width;
                *portalx = (AcmeReal)x * portalscale;

                deady = height*(AcmeReal)0.5 - 224/portalscale;

                *portaly = ((AcmeReal)y - deady) * portalscale;
        }

}

/**
        Sets up the projection and 2D matrices to make the view orthographic.
*/
void MEAPI RCameraSetOrthographicView(RRender* rc, AcmeReal zoomFactor, AcmeReal viewDepth)
{
    MeMatrix4SetZero(rc->m_ProjMatrix);

    rc->m_ProjMatrix[0][0] = 1/rc->m_AspectRatio * 2 / zoomFactor;
    rc->m_ProjMatrix[1][1] = 2 / zoomFactor;
    rc->m_ProjMatrix[2][2] = 1 / viewDepth;

    rc->m_ProjMatrix[3][3] = 1;

    MeMatrix4SetZero(rc->m_CamMatrix2D);

    rc->m_CamMatrix2D[0][0] = zoomFactor/448;
    rc->m_CamMatrix2D[1][1] = -zoomFactor/448;
    rc->m_CamMatrix2D[2][2] = 1.0f;

    rc->m_CamMatrix2D[3][0] = rc->m_AspectRatio * -(0.5f * zoomFactor);
    rc->m_CamMatrix2D[3][1] = (0.5f * zoomFactor);
    rc->m_CamMatrix2D[3][2] = 0;

    rc->m_CamMatrix2D[3][3] = 1.0f;
}

/**
        Orthographic view from the front.
*/
void MEAPI RCameraSetFrontView(RRender* rc, AcmeReal zoomFactor, AcmeReal viewDepth)
{
        RCameraSetOrthographicView(rc,zoomFactor,viewDepth);
        RCameraSetElevation(rc,0);
        RCameraSetRotation(rc,0);
}

/**
        Orthographic view from the back.
*/
void MEAPI RCameraSetBackView(RRender* rc, AcmeReal zoomFactor, AcmeReal viewDepth)
{
        RCameraSetOrthographicView(rc,zoomFactor,viewDepth);
        RCameraSetElevation(rc,0);
        RCameraSetRotation(rc,ME_PI);
}

/**
        Orthographic view from the left.
*/
void MEAPI RCameraSetLeftView(RRender* rc, AcmeReal zoomFactor, AcmeReal viewDepth)
{
        RCameraSetOrthographicView(rc,zoomFactor,viewDepth);
        RCameraSetElevation(rc,0);
        RCameraSetRotation(rc,3*ME_PI/2);
}

/**
        Orthographic view from the right.
*/
void MEAPI RCameraSetRightView(RRender* rc, AcmeReal zoomFactor, AcmeReal viewDepth)
{
        RCameraSetOrthographicView(rc,zoomFactor,viewDepth);
        RCameraSetElevation(rc,0);
        RCameraSetRotation(rc,ME_PI/2);
}

/**
        Orthographic view from the top.
*/
void MEAPI RCameraSetTopView(RRender* rc, AcmeReal zoomFactor, AcmeReal viewDepth)
{
        RCameraSetOrthographicView(rc,zoomFactor,viewDepth);
        RCameraSetElevation(rc,ME_PI/2);
        RCameraSetRotation(rc,0);
}

/**
        Orthographic view from the bottom.
*/
void MEAPI RCameraSetBottomView(RRender* rc, AcmeReal zoomFactor, AcmeReal viewDepth)
{
        RCameraSetOrthographicView(rc,zoomFactor,viewDepth);
        RCameraSetElevation(rc,3*ME_PI/2);
        RCameraSetRotation(rc,0);
}


/** 
    Combine two RGraphics, defining the relative transform of the second to the first.
    A new RGraphic is returned, and added to the render list.
*/

RGraphic* MEAPI RGraphicCombine(RRender* rc, RGraphic* a, RGraphic* b, MeMatrix4Ptr transform)
{
    RGraphic* rg;
    RObjectVertex* vtx;
    RObjectVertex* vtxa;
    int i;
    MeVector4 pretrans, posttrans;

    rg = RGraphicCreateEmpty(a->m_pObject->m_nNumVertices + b->m_pObject->m_nNumVertices);

    rg->m_pLWMatrix = a->m_pLWMatrix;
    memcpy(rg->m_pObject, a->m_pObject, sizeof(RObjectHeader));
    rg->m_pObject->m_nNumVertices = a->m_pObject->m_nNumVertices + b->m_pObject->m_nNumVertices;
    
    vtx = rg->m_pVertices;
    vtxa = a->m_pVertices;

    //copy a's vertices
    for( i=0; i<a->m_pObject->m_nNumVertices; i++)
    {
        vtx->m_X  = vtxa->m_X;
        vtx->m_Y  = vtxa->m_Y;
        vtx->m_Z  = vtxa->m_Z;
        vtx->m_NX = vtxa->m_NX;
        vtx->m_NY = vtxa->m_NY;
        vtx->m_NZ = vtxa->m_NZ;
        vtx->m_U  = vtxa->m_U;
        vtx->m_V  = vtxa->m_V;
        vtx++;
        vtxa++;
    }

    //transform b's vertices and put them in rg
    vtxa = b->m_pVertices;
    for( i=0; i<b->m_pObject->m_nNumVertices; i++)
    {
        // x,y,z
        pretrans[0] = vtxa->m_X;
        pretrans[1] = vtxa->m_Y;
        pretrans[2] = vtxa->m_Z;
        pretrans[3] = 1;
        MeMatrix4MultiplyVector(posttrans, transform, pretrans);
        vtx->m_X = posttrans[0];
        vtx->m_Y = posttrans[1];
        vtx->m_Z = posttrans[2];

        // normals
        pretrans[0] = vtxa->m_NX;
        pretrans[1] = vtxa->m_NY;
        pretrans[2] = vtxa->m_NZ;
        pretrans[3] = 0;
        MeMatrix4MultiplyVector(posttrans, transform, pretrans);
        vtx->m_NX = posttrans[0];
        vtx->m_NY = posttrans[1];
        vtx->m_NZ = posttrans[2];

        // u,v
        vtx->m_U = vtxa->m_U;
        vtx->m_V = vtxa->m_V;

        vtx++;
        vtxa++;
    }

    RGraphicAddToList(rc, rg, 0);

    return rg;
}


/* --------------- NATIVE LINES -------------------*/

void MEAPI RNLineAddToList(RRender *rc, RNativeLine *line)
{
    RNativeLine *curNL;

    curNL = rc->m_pNL_First;

    if( curNL == 0 ) /* empty list */
    {
        rc->m_pNL_First = line;
        return;
    }

    while( curNL->m_next ){ curNL = curNL->m_next; }
    curNL->m_next = line;

    return;
}


void MEAPI RNLineRemoveFromList(RRender *rc, RNativeLine *line)
{
    RNativeLine *current;
    RNativeLine *next;

    current = rc->m_pNL_First;

    if(current) /* not an empty list */
    {
        next = current->m_next;
        if( current == line ) /* line is first in list */
        {
            rc->m_pNL_First = next;
        }
        else
        {
            while(next)
            {
                if(next == line)
                {
                    current->m_next = next->m_next;
                    break;
                }
                current = next;
                next = current->m_next;
            }
        }
    }

    line->m_next = 0;
}


void MEAPI RNLineDestroy(RNativeLine *line)
{
    if( line->m_vectors->m_refCount == 0 )
        MeMemoryAPI.destroy(line->m_vectors);
    else
        line->m_vectors->m_refCount--;
    
    MeMemoryAPI.destroy(line);
}


void MEAPI RNLineDelete(RRender *rc, RNativeLine *line)
{
    RNLineRemoveFromList(rc, line);
    RNLineDestroy(line);
    line = 0;
}

RNativeLine* MEAPI RNLineCreate( RRender *const rc,
    const AcmeReal start[3], const AcmeReal end[3],
    const AcmeReal color[4], const MeVector4 *const matrix)
{
    RNativeLine *line = 0;
    RNativeLineVectors *vectors = 0;

    line = MeMemoryAPI.createZeroed(sizeof(struct RNativeLine));
    if( !line )
        return 0;

    vectors = MeMemoryAPI.createZeroed(sizeof(RNativeLineVectors));
    if( !vectors )
    {
        MeMemoryAPI.destroy(line);
        return 0;
    }

    vectors->m_refCount = 0;
    vectors->m_start[0] = start[0];
    vectors->m_start[1] = start[1];
    vectors->m_start[2] = start[2];
    vectors->m_end[0] = end[0];
    vectors->m_end[1] = end[1];
    vectors->m_end[2] = end[2];
    vectors->m_d3dpadding[3] = (AcmeReal)-1.0;
    vectors->m_d3dpadding2[3] = (AcmeReal)-1.0;
    

    line->m_color[0] = color[0];
    line->m_color[1] = color[1];
    line->m_color[2] = color[2];
    line->m_color[3] = color[3];
    
    line->m_next = 0;
    line->m_vectors = vectors;

    line->m_pMatrix = matrix;
    
    MeMatrix4TMMakeIdentity(line->m_matrix);
    
    RNLineAddToList(rc, line);
    
    return line;
}

void MEAPI RNLineSetColor( RNativeLine* line, AcmeReal color[4] )
{
    int i;
    
    if( !line )
        return;
    
    for( i=0; i<4; i++)
        line->m_color[i] = color[i];
}

void MEAPI RNLineMoveEnds( RNativeLine*const line, const AcmeReal start[3], const AcmeReal end[3] )
{
    int i;

    if( !line )
        return;

    for( i=0; i<3; i++)
    {
        line->m_vectors->m_start[i] = start[i];
        line->m_vectors->m_end[i] = end[i];
    }
}

