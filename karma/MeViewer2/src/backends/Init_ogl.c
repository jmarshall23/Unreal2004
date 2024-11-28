/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/15 12:03:22 $ - Revision: $Revision: 1.44.2.5 $

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

/* OpenGL back-end init */

#ifdef WITH_OPENGL

#include <stdlib.h>
#include <MeVersion.h>
#include <MeString.h>

#include "Init_ogl.h"

struct _oglglobals gOGL;

void PS2_InitPad();


/* This is called as RInit by MeViewer.c */
int OGL_CreateRenderer(RRender *rc)
{
    OGL_InitGlobals(rc);
    OGL_CalibrateTimer();
    OGL_InitGlutCallBacks();

#ifndef PS2
    OGL_CreateMenus();
#endif

    if(gOGL.rc->m_options.m_bDisplayHelpOnConsole)
        OGL_DisplayConsoleHelp();

    return 0;
}


void OGL_InitGlobals(RRender *rc)
{
    int i;

    snprintf(gOGL.m_strWindowTitle, MAX_TITLE_LENGTH - 1
        , "%s (v%s) [OpenGL / GLUT]", ME_PRODUCT_NAME, ME_VERSION_STRING);
    gOGL.m_strWindowTitle[MAX_TITLE_LENGTH - 1] = '\0';

    gOGL.m_bLockAspectRatio = 1;
    gOGL.m_bIsFrameLocked = 1;
    gOGL.m_bIsFullscreen = 0;
    gOGL.m_bQuit = 0;
    gOGL.m_bDoStep = 0;
    gOGL.m_bDisplayFps = 1;
    gOGL.m_nFpsUpdate = 60;
    gOGL.m_width = 640;
    gOGL.m_height = 448;
    gOGL.m_bAllowWireFrame = 1;
    if(rc->m_options.m_bWireFrame != MEFALSE)
        gOGL.m_bForceWireFrame = 1;
    else
        gOGL.m_bForceWireFrame = 0;
    if(rc->m_options.m_bTextures != MEFALSE)
        gOGL.m_bAllowTextures = 1;
    else
        gOGL.m_bAllowTextures = 0;
    gOGL.m_bDisplay2D = 1;
    gOGL.m_bDisplayPSystems = 1;
    gOGL.m_bUseLinearFilter = 1;
    gOGL.rc = rc;

    RMouseDrag.p_rc = &(gOGL.rc);
    RMouseDrag.p_width = &(gOGL.m_width);
    RMouseDrag.p_height = &(gOGL.m_height);
    RMouseDrag.m_allowContextMenu = 0;

    for(i=0; i < 25; i++)
    {
        gOGL.m_glTexNames[i] = 0;
        gOGL.m_TexImages[i].m_pImage = 0;
        gOGL.m_TexImages[i].m_height = 0;
        gOGL.m_TexImages[i].m_width = 0;
    }

    /*
        Check 'frame/MeViewer.c' to see which symbols are bound to
        which positions. More or less:
        0-3: UP, DOWN, LEFT, RIGHT
        4-7: UP2, DOWN2, LEFT2, RIGHT2
        8-13: ACTION0-5 14: MOUSE 15: JOYSTICK
    */

#ifdef PS2
    strncpy(rc->m_ButtonText[0], "D-pad Up", 20);
    strncpy(rc->m_ButtonText[1], "D-pad Down", 20);
    strncpy(rc->m_ButtonText[2], "D-pad Left", 20);
    strncpy(rc->m_ButtonText[3], "D-pad Right", 20);

    strncpy(rc->m_ButtonText[4], "Triangle", 20);       /* UP2 */
    strncpy(rc->m_ButtonText[5], "Cross", 20);          /* DOWN2 */
    strncpy(rc->m_ButtonText[6], "Square", 20);         /* LEFT2 */
    strncpy(rc->m_ButtonText[7], "Circle", 20);         /* RIGHT2 */

    strncpy(rc->m_ButtonText[8], "Select", 20);         /* ACTION0 */
    strncpy(rc->m_ButtonText[9], "Start", 20);          /* ACTION1 */
    strncpy(rc->m_ButtonText[10], "Triangle", 20);      /* ACTION2 */
    strncpy(rc->m_ButtonText[11], "Circle", 20);        /* ACTION3 */
    strncpy(rc->m_ButtonText[12], "Square", 20);        /* ACTION4 */
    strncpy(rc->m_ButtonText[13], "Cross", 20);         /* ACTION5 */

    strncpy(rc->m_ButtonText[14], "R1 + Left Stick", 20);    /* MOUSE */
#else
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
#endif

    /* Todo: What about architectures that don't do texturing? ... */
    rc->m_capabilities = (RMOUSE | RTEXTURES);
}


void OGL_InitGlutCallBacks()
{
    /* GLUT init */
#ifdef PS2
    int argc = 0;
    static char *(argv[]) = { "" };
    glutInit(&argc,argv);
#endif
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( gOGL.m_width, gOGL.m_height );  /* size for PS2 compatibility */
    gOGL.m_glutWindowID = glutCreateWindow(gOGL.m_strWindowTitle);

    glutReshapeFunc(OGL_ReshapeFunc);
    glutDisplayFunc(OGL_DisplayFunc);
    glutKeyboardFunc(OGL_KeyboardFunc);
    glutSpecialFunc(OGL_SpecialKeyFunc);

#ifndef PS2
    glutMouseFunc(OGL_MouseFunc);
    glutMotionFunc(OGL_MotionFunc);
#endif
}


void OGL_CalibrateTimer()
{
    MeProfileTimerMode tmode;
    tmode.counterMode = kMeProfileCounterModeCache;
#ifndef PS2
    tmode.granularity = 0;
#else
    tmode.granularity = 2;
#endif
    tmode.count0Label = 0;
    tmode.count1Label = 0;

    MeProfileStartTiming(tmode, gOGL.rc->m_bProfiling);

    gOGL.m_uiTicksPerSec = (unsigned int)MeProfileGetClockSpeed();
    gOGL.m_uiTicksPerFrame = gOGL.m_uiTicksPerSec / 60;
}


/* Registered as glut keyboard callback
   -- for ASCII key presses only (inc ESC)       */
void OGL_KeyboardFunc(unsigned char key, int x, int y)
{
    switch( key )
    {
    case 'a':
    case 'A':
        RExecuteLeft2Callback(gOGL.rc);
        break;
    case 's':
    case 'S':
        RExecuteDown2Callback(gOGL.rc);
        break;
    case 'd':
    case 'D':
        RExecuteRight2Callback(gOGL.rc);
        break;
    case 'w':
    case 'W':
        RExecuteUp2Callback(gOGL.rc);
        break;
    case 'h':
    case 'H':
        RExecuteActionNCallback(gOGL.rc, 0);
        break;
    case 'm':
    case 'M':
        RExecuteActionNCallback(gOGL.rc, 1);
        break;
    case 27: /* ESC */
        gOGL.m_bQuit = 1;
        break;
    case '+':
        RCameraZoom(gOGL.rc, -0.5f);
        break;
    case '-':
        RCameraZoom(gOGL.rc, 0.5f);
    }
    /* Assignable keys can't go in switch: */
    if (key == gOGL.rc->m_pCallBacks->m_actionKeys[0])
        RExecuteActionNCallback(gOGL.rc, 2);
    else if(key == gOGL.rc->m_pCallBacks->m_actionKeys[1])
        RExecuteActionNCallback(gOGL.rc, 3);
    else if(key == gOGL.rc->m_pCallBacks->m_actionKeys[2])
        RExecuteActionNCallback(gOGL.rc, 4);
    else if(key == gOGL.rc->m_pCallBacks->m_actionKeys[3])
        RExecuteActionNCallback(gOGL.rc, 5);
}

void OGL_ToggleMenuEnable() {

#ifndef PS2
    if(!RMouseDrag.m_allowContextMenu) {
        glutAttachMenu(GLUT_RIGHT_BUTTON);
        RMouseDrag.m_allowContextMenu=1;
    } else {
        glutDetachMenu(GLUT_RIGHT_BUTTON);
        RMouseDrag.m_allowContextMenu=0;
    }
#endif

}

/* Registered as glut special callback
   -- for non-ASCII key presses (Function keys & Arrows)   */
void OGL_SpecialKeyFunc(int key, int x, int y)
{
#ifndef PS2
    int modifiers;
    int bShift, bCtrl, bAlt, bIsModified;

    modifiers = glutGetModifiers();
    bShift = (modifiers & GLUT_ACTIVE_SHIFT) ? 1 : 0;
    bCtrl = (modifiers & GLUT_ACTIVE_CTRL) ? 1 : 0;
    bAlt = (modifiers & GLUT_ACTIVE_ALT) ? 1 : 0;
    bIsModified = bShift || bCtrl || bAlt;
    switch( key )
    {
    case GLUT_KEY_LEFT:
        RExecuteLeftCallback(gOGL.rc);
        break;
    case GLUT_KEY_DOWN:
        RExecuteDownCallback(gOGL.rc);
        break;
    case GLUT_KEY_RIGHT:
        RExecuteRightCallback(gOGL.rc);
        break;
    case GLUT_KEY_UP:
        RExecuteUpCallback(gOGL.rc);
        break;
    case GLUT_KEY_F1:
        if( !bIsModified )
            RRenderToggleUserHelp(gOGL.rc);
        break;
    case GLUT_KEY_F2:
        if(!bIsModified)
            gOGL.m_bForceWireFrame = !gOGL.m_bForceWireFrame;
        else
            if(bShift && !bAlt && !bCtrl)
                gOGL.m_bAllowTextures = !gOGL.m_bAllowTextures;
    case GLUT_KEY_F3:
        if( !bIsModified )
            gOGL.m_bIsFrameLocked = !gOGL.m_bIsFrameLocked;
        else if( bCtrl && !bAlt && !bShift )
        {
            gOGL.m_bDisplayFps = !gOGL.m_bDisplayFps;
            if( !gOGL.m_bDisplayFps && gOGL.rc->m_fpsG )
            {
                RGraphicDelete(gOGL.rc, gOGL.rc->m_fpsG, 1);
                gOGL.rc->m_fpsG = 0;
            }
        }
        break;
    case GLUT_KEY_F4:
        if( bAlt && !bShift && !bCtrl )
            gOGL.m_bQuit = 1;
        break;
    case GLUT_KEY_F8:
        if( !bIsModified )
        {
            gOGL.m_bDoStep = 1;
            gOGL.rc->m_bPause = 1;
        }
        else if( bCtrl && !bAlt && !bShift )
            gOGL.rc->m_bPause = !gOGL.rc->m_bPause;
        break;
    case GLUT_KEY_F9:
        if( !bIsModified )
            OGL_ToggleFullScreen();
        else if( bAlt && !bShift && !bCtrl )
        {
            gOGL.m_bLockAspectRatio = !gOGL.m_bLockAspectRatio;
            OGL_ReshapeFunc(gOGL.m_width, gOGL.m_height);
        }
        break;
    case GLUT_KEY_F12:
        OGL_ToggleMenuEnable();
        break;
    case GLUT_KEY_PAGE_UP:
    {
        MeVector3 newLookAt;
        RCameraGetLookAt(gOGL.rc, newLookAt);
        newLookAt[2] -=0.5f;
        RCameraSetLookAt(gOGL.rc, newLookAt);
    }
    break;
    case GLUT_KEY_PAGE_DOWN:
    {
        MeVector3 newLookAt;
        RCameraGetLookAt(gOGL.rc, newLookAt);
        newLookAt[2] +=0.5f;
        RCameraSetLookAt(gOGL.rc, newLookAt);
    }
    }
#endif
}

/* Glut's mousefunc - called on button state change */
void OGL_MouseFunc(int button, int state, int x, int y)
{
#ifndef PS2
    RMouseButtonWhich this_button;
    RMouseButtonEvent event;
    int modifiers = 0;

    /* Which button is it? */
    switch(button) {
    case GLUT_LEFT_BUTTON:
        this_button = kRLeftButton;
        break;
    case GLUT_MIDDLE_BUTTON:
        this_button = kRMiddleButton;
        break;
    case GLUT_RIGHT_BUTTON:
        this_button = kRRightButton;
        break;
    }

   if(state == GLUT_DOWN)
   {
       if(RMouseDrag.m_button &&
           (RMouseDrag.m_button != this_button) &&
           RMouseDrag.m_meModifiers) /* We've overridden a previously pressed button */
           RExecuteMouseCallback(gOGL.rc,
               x, y, RMouseDrag.m_meModifiers,
               RMouseDrag.m_button, kRNewlyReleased);

       event = kRNewlyPressed;
       modifiers = glutGetModifiers();

       RMouseDrag.m_meModifiers = 0;
       if(modifiers & GLUT_ACTIVE_CTRL)
           RMouseDrag.m_meModifiers |= RCONTROL;
       if(modifiers & GLUT_ACTIVE_SHIFT)
           RMouseDrag.m_meModifiers |= RSHIFT;
       if(modifiers & GLUT_ACTIVE_ALT)
           RMouseDrag.m_meModifiers |= RALT;

       RMouseDrag.m_startx = x;
       RMouseDrag.m_starty = y;
       RMouseDrag.m_camstartDist = gOGL.rc->m_CameraDist;
       RMouseDrag.m_camstartPhi = gOGL.rc->m_CameraPhi;
       RMouseDrag.m_camstartTheta = gOGL.rc->m_CameraTheta;
       RMouseDrag.m_camstartX = x;
       RMouseDrag.m_camstartY = y;
       RCameraGetLookAt(gOGL.rc, RMouseDrag.m_camstartLookAt);

   } else if((state == GLUT_UP) && (this_button == RMouseDrag.m_button))
       event = kRNewlyReleased;

   RMouseDrag.m_button = this_button;

   if(RMouseDrag.m_meModifiers)
       /* Modifiers mean this *isn't* camera stuff, so call the callback. */
       RExecuteMouseCallback(gOGL.rc, x, y, RMouseDrag.m_meModifiers,
           RMouseDrag.m_button, event);

   if(event == kRNewlyReleased) {
       RMouseDrag.m_meModifiers = 0;
       RMouseDrag.m_button = (RMouseButtonWhich)0;
   }

   /* Otherwise, do nothing more - wait for the mouse to move then do
      something to the camera */
#endif
}

/* Glut's active mouse motion callback - called when dragging etc */
void OGL_MotionFunc(int x, int y)
{
    if(!RMouseDrag.m_meModifiers) { /* Do camera stuff */
        switch(RMouseDrag.m_button) {
        case kRLeftButton:
            RCameraRotate(x, y);
            break;
        case kRMiddleButton:
            RCameraZoomDolly(x, y);
            break;
        case kRRightButton:
            RCameraPan(x, y);
        }
    } else        /* Call the callback, but now we're dragging. */
        RExecuteMouseCallback(gOGL.rc, x, y, RMouseDrag.m_meModifiers,
                RMouseDrag.m_button, kRStillPressed);
}


void OGL_CreateMenus()
{
#ifndef PS2
    int rendererMenu;
    int shadingMenu;
    int textureMenu;

    rendererMenu = glutCreateMenu(OGL_HandleRendererMenu);
    glutAddMenuEntry("Toggle Aspect Ratio Lock", 0);
    glutAddMenuEntry("Toggle Frame Rate Lock", 1);
    glutAddMenuEntry("Toggle FPS Display", 2);
    glutAddMenuEntry("Toggle 2D Overlay", 3);
    glutAddMenuEntry("Toggle Particle Systems", 4);

    shadingMenu = glutCreateMenu(OGL_HandleShadingMenu);
    glutAddMenuEntry("Toggle Allow Wireframe", 0);
    glutAddMenuEntry("Toggle Forced Wireframe", 1);

    textureMenu = glutCreateMenu(OGL_HandleTextureMenu);
    glutAddMenuEntry("Toggle Texturing (3D)", 0);
    glutAddMenuEntry("Toggle Bilinear Filtering", 1);

    glutCreateMenu(OGL_HandleRootMenu);
    glutAddMenuEntry("Pause/Play (Ctrl-F8)", 2);
    glutAddMenuEntry("Single Step (F8)", 3);
    glutAddSubMenu("Renderer", rendererMenu);
    glutAddSubMenu("Shading", shadingMenu);
    glutAddSubMenu("Textures", textureMenu);
    glutAddMenuEntry("Toggle Help (F1)", 1);
    glutAddMenuEntry("Exit (ESC)", 0);

/*      */
#endif
}

void OGL_HandleRendererMenu( int value )
{
    switch( value )
    {
    case 0: /* Aspect ratio lock */
        gOGL.m_bLockAspectRatio = !gOGL.m_bLockAspectRatio;
        OGL_ReshapeFunc(gOGL.m_width, gOGL.m_height);
        break;
    case 1: /* Frame rate lock */
        gOGL.m_bIsFrameLocked = !gOGL.m_bIsFrameLocked;
        break;
    case 2: /* FPS display */
        gOGL.m_bDisplayFps = !gOGL.m_bDisplayFps;
        if( !gOGL.m_bDisplayFps && gOGL.rc->m_fpsG )
        {
            RGraphicDelete(gOGL.rc, gOGL.rc->m_fpsG, 1);
            gOGL.rc->m_fpsG = 0;
        }
        break;
    case 3: /* 2D Display */
        gOGL.m_bDisplay2D = !gOGL.m_bDisplay2D;
    case 4: /* Particle System Display */
        gOGL.m_bDisplayPSystems = !gOGL.m_bDisplayPSystems;
    }
}

void OGL_HandleShadingMenu( int value )
{
    switch( value )
    {
    case 0: /* Allow wireframe */
        gOGL.m_bAllowWireFrame = !gOGL.m_bAllowWireFrame;
        break;
    case 1: /* Forced wireframe */
        gOGL.m_bForceWireFrame = !gOGL.m_bForceWireFrame;
        break;
    }
}

void OGL_HandleTextureMenu( int value )
{
    switch( value )
    {
    case 0: /* Enable textures */
        gOGL.m_bAllowTextures = !gOGL.m_bAllowTextures;
        break;
    case 1: /* Toggle bilinear filtering */
        gOGL.m_bUseLinearFilter = !gOGL.m_bUseLinearFilter;
        break;
    }
}

void OGL_HandleRootMenu( int value )
{
    switch( value )
    {
    case 0: /* Exit */
        gOGL.m_bQuit = 1;
        break;
    case 1: /* Help */
        RRenderToggleUserHelp(gOGL.rc);
        break;
    case 2: /* Pause/Play */
        gOGL.rc->m_bPause = !gOGL.rc->m_bPause;
        break;
    case 3: /* Single Step */
        gOGL.m_bDoStep = 1;
        gOGL.rc->m_bPause = 1;
        break;
    }
}

int OGL_LoadTextures()
{
    int i;

#ifndef PS2
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#endif

    glGenTextures(25, gOGL.m_glTexNames);

    for( i=0; i<25; i++ )
    {
        if( gOGL.rc->m_TextureList[i] )
        {
            glBindTexture(GL_TEXTURE_2D, gOGL.m_glTexNames[i]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            RBmpLoad(gOGL.rc, gOGL.rc->m_TextureList[i], &gOGL.m_TexImages[i], 0);
            if( gOGL.m_TexImages[i].m_pImage )
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gOGL.m_TexImages[i].m_width, gOGL.m_TexImages[i].m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, gOGL.m_TexImages[i].m_pImage);
        }
    }

    return 0;
}


void OGL_FreeTextures()
{
    int i;

    glDeleteTextures(25, gOGL.m_glTexNames);

    for(i=0; i<25; i++)
    {
        if(gOGL.m_TexImages[i].m_pImage)
           MeMemoryAPI.destroy(gOGL.m_TexImages[i].m_pImage);
    }
}


void OGL_DisplayConsoleHelp()
{
    MeInfo(0, "------------------MeViewer (OpenGL)---------------------");
    RDisplayBanner();
#ifndef PS2
    MeInfo(0, "Key Controls:");
    MeInfo(0, "\tF1\t\t- Toggles application specific help");
    MeInfo(0, "\tF2\t\t- Toggle Wireframe");
    MeInfo(0, "\tShift F2\t- Toggle Texturing");
    MeInfo(0, "\tF3\t\t- Toggle 60Hz frame rate lock");
    MeInfo(0, "\tCtrl F3\t\t- Toggle fps display");
    MeInfo(0, "\tF8\t\t- Single step");
    MeInfo(0, "\tCtrl F8\t\t- Toggle pause");
    MeInfo(0, "\tF9\t\t- Toggle Fullscreen mode");
    MeInfo(0, "\tAlt F9\t\t- Toggle 640:448 Aspect Ratio lock");
    MeInfo(0, "\tF12\t\t- Toggle right click functionality");
    MeInfo(0, "\tESC\t\t- Quit");
    MeInfo(0, "");
    MeInfo(0, "Mouse Controls:");
    MeInfo(0, "\tLeft drag\t- Rotate Camera");
    MeInfo(0, "\tMiddle drag\t- Dolly (X), Zoom (Y)");
    MeInfo(0, "\tRight drag\t- Pan, or Display GLUT menu (see F12)");
    MeInfo(0, "\tShift + drag\t- Mouse forces (when available)");
#endif
    RDisplayCommandLineHelp();
    MeInfo(0, "--------------------------------------------------------");
    MeInfo(0, "");

}

#endif
