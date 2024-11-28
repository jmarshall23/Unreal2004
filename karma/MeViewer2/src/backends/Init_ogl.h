#ifndef _INIT_OGL_H /* -*- mode: c; -*- */
#define _INIT_OGL_H

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:39 $ - Revision: $Revision: 1.13.8.3 $

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

#ifdef WITH_OPENGL

#include <MeViewer.h>
#include "RMouseCam.h"
#include "Render_ogl.h"

/*
    If you do not already have glut support on your system, then a copy is
    supplied with the MathEngine SDK installer.  Re-run the installer and
    select the GLUT option.  (Alternatively, you may wish to compile without
    OpenGL support by undefining WITH_OPENGL in your project settings.)
*/
# include <GL/glut.h>

# if defined LINUX && defined FX
#   include <GL/glx.h>
# endif

#ifdef __cplusplus
extern "C"
{
#endif

/* FixMe: Not yet implemented: */
MeU32 RGetCapabilities_ogl();

#define MAX_TITLE_LENGTH 100
struct _oglglobals
{
    int                  m_glutWindowID;
    char                 m_strWindowTitle[MAX_TITLE_LENGTH];
    int                  m_bLockAspectRatio;
    int                  m_bIsFrameLocked;
    unsigned int       m_uiTicksPerFrame;
    unsigned int       m_uiTicksPerSec;
    int                  m_bQuit;
    int                  m_bDoStep;
    int                  m_bDisplayFps;
    int                  m_nFpsUpdate;
    int                  m_width;
    int                  m_height;
    int                  m_bIsFullscreen;
    int                  m_bAllowWireFrame;
    int                  m_bForceWireFrame;
    int                  m_bAllowTextures;
    int                  m_bDisplay2D;
    int                  m_bDisplayPSystems;
    int                  m_bUseLinearFilter;

    GLuint               m_glTexNames[25];
    RImage               m_TexImages[25];

    RMainLoopCallBack    callbackfunc;
    void                 *userdata; /* callback userdata */
    RRender              *rc;
};

extern struct _oglglobals gOGL;

/* Function Prototypes */

int     OGL_CreateRenderer(RRender *rc);
void    OGL_InitGlobals(RRender *rc);
void    OGL_InitGlutCallBacks();
void    OGL_CalibrateTimer();
void    OGL_KeyboardFunc(unsigned char key, int x, int y);
void    OGL_SpecialKeyFunc(int key, int x, int y);
void    OGL_MouseFunc(int button, int state, int x, int y);
void    OGL_MotionFunc(int x, int y);
void    OGL_CreateMenus();
void    OGL_HandleRendererMenu( int value );
void    OGL_HandleShadingMenu( int value );
void    OGL_HandleTextureMenu( int value );
void    OGL_HandleRootMenu( int value );
int     OGL_LoadTextures();
void    OGL_FreeTextures();
void    OGL_DisplayConsoleHelp();


#ifdef __cplusplus
}
#endif
#endif
#endif
