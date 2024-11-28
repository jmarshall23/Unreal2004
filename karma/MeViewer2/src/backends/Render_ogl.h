#ifndef _RENDER_OGL_H /* -*- mode: c; -*- */
#define _RENDER_OGL_H


/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:39 $ - Revision: $Revision: 1.10.8.3 $

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
#include "Init_ogl.h"


#ifdef __cplusplus
extern "C"
{
#endif


/* Function Prototypes */

void    OGL_RunApp(RRender*rc, RMainLoopCallBack func, void *userdata);
void    OGL_DisplayFunc(void);
void    OGL_ReshapeFunc(int width, int height);
void    OGL_DrawFrame();
void    OGL_fpsCalc(MeU64 starttime);
void    OGL_glutMainLoop();
void    OGL_ToggleFullScreen(void);
void    OGL_PreMainLoopInit();
void    OGL_PostMainLoopCleanup();
/* void OGL_RotateCamera(int x, int y); */
/* void OGL_ZoomDollyCamera(int x, int y); */
/* void OGL_PanCamera(int x, int y); */
void    OGL_SetWindowTitle(RRender* rc, const char* title);


#ifdef __cplusplus
}
#endif
#endif
#endif
