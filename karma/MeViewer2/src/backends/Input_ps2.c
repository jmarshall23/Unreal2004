/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/23 09:43:22 $ - Revision: $Revision: 1.8.2.4 $

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

/* Pad input and mouse pointer rendering for PS2*/

#include "Render_ogl.h"
#include <libpad.h>

#ifdef _ME_API_DOUBLE
#define ME_GL_REAL GL_DOUBLE
#else
#define ME_GL_REAL GL_FLOAT
#endif

#define SCREEN_WIDTH (640)
#define SCREEN_HEIGHT (224)

#define PAD_CENTRE (128)
#define PAD_DEAD_OFFSET (60)
#define PAD_ANGLE_DIVISOR (6)
#define PAD_DISTANCE_DIVISOR (6)

#define AUTOREPEAT_FRAMES (5)
#define AUTOREPEAT_MIN (10)

MeU32 paddata;
MeU8  rdata[32];

typedef struct _system
{
    int m_cursorX;
    int m_cursorY;
    int autorepeat[12];
    MeBool dump;
} System;

System gPS2;

void PS2_ReadPad()
{
    static unsigned int camX = 0, camY = 0;
    RMouseButtonWhich button;
    RMouseButtonEvent event;
    RRender *rc = gOGL.rc;

    static AcmeReal mouseVertices[9];
    static AcmeReal mouseNormals[9];
    static AcmeReal mouseColor[4] = { 1.0f,1.0f,0.0f,0.0f };

    if ( scePadGetState( 0, 0 ) != scePadStateStable ) return;

    if(scePadRead(0, 0, rdata))
        paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);
    else
        paddata=0;

    gPS2.dump = 0;

    /* Check dpad */

    if(paddata & SCE_PADLup) {
        if(((!(gPS2.autorepeat[0] % AUTOREPEAT_FRAMES)) && (gPS2.autorepeat[0] > AUTOREPEAT_MIN))
            || gPS2.autorepeat[0] == 0)
            RExecuteUpCallback(rc);
        gPS2.autorepeat[0]++;
    } else
        gPS2.autorepeat[0] = 0;

    if(paddata & SCE_PADLdown) {
        if(((!(gPS2.autorepeat[1] % AUTOREPEAT_FRAMES))&& (gPS2.autorepeat[1] > AUTOREPEAT_MIN))
            || gPS2.autorepeat[1] == 0)
            RExecuteDownCallback(rc);
        gPS2.autorepeat[1]++;
    } else
        gPS2.autorepeat[1] = 0;

    if(paddata & SCE_PADLleft) {
        if(((!(gPS2.autorepeat[2] % AUTOREPEAT_FRAMES))&& (gPS2.autorepeat[2] > AUTOREPEAT_MIN))
            || gPS2.autorepeat[2] == 0)
            RExecuteLeftCallback(rc);
        gPS2.autorepeat[2]++;
    } else
        gPS2.autorepeat[2] = 0;

    if(paddata & SCE_PADLright) {
        if(((!(gPS2.autorepeat[3] % AUTOREPEAT_FRAMES))&& (gPS2.autorepeat[3] > AUTOREPEAT_MIN))
            || gPS2.autorepeat[3] == 0)
            RExecuteRightCallback(rc);
        gPS2.autorepeat[3]++;
    } else
        gPS2.autorepeat[3] = 0;

    /* Check circle, cross, square,triangle */

    /* Check triangle */

    if(paddata & SCE_PADRup) {
        if(((!(gPS2.autorepeat[4] % AUTOREPEAT_FRAMES))&& (gPS2.autorepeat[4] > AUTOREPEAT_MIN))
            || gPS2.autorepeat[4] == 0) {
            RExecuteUp2Callback(rc);
            RExecuteActionNCallback(rc, 2);
        }
        gPS2.autorepeat[4]++;
    } else
        gPS2.autorepeat[4] = 0;

    /* Check cross */

    if(paddata & SCE_PADRdown) {
        if(((!(gPS2.autorepeat[5] % AUTOREPEAT_FRAMES))&& (gPS2.autorepeat[5] > AUTOREPEAT_MIN))
            || gPS2.autorepeat[5] == 0) {
            RExecuteDown2Callback(rc);
            RExecuteActionNCallback(rc, 5);
        }
        gPS2.autorepeat[5]++;
    } else
        gPS2.autorepeat[5] = 0;

    /* Check square */

    if(paddata & SCE_PADRleft) {
        if(((!(gPS2.autorepeat[6] % AUTOREPEAT_FRAMES))&& (gPS2.autorepeat[6] > AUTOREPEAT_MIN))
            || gPS2.autorepeat[6] == 0) {
            RExecuteLeft2Callback(rc);
            RExecuteActionNCallback(rc, 4);
        }
        gPS2.autorepeat[6]++;
    } else
        gPS2.autorepeat[6] = 0;

    /* Check circle */

    if(paddata & SCE_PADRright) {
        if(((!(gPS2.autorepeat[7] % AUTOREPEAT_FRAMES))&& (gPS2.autorepeat[7] > AUTOREPEAT_MIN))
            || gPS2.autorepeat[7] == 0) {
            RExecuteRight2Callback(rc);
            RExecuteActionNCallback(rc, 3);
        }
        gPS2.autorepeat[7]++;
    } else
        gPS2.autorepeat[7] = 0;

    /* Check select */

    if(paddata & SCE_PADselect) 
    {
        if(((!(gPS2.autorepeat[8] % AUTOREPEAT_FRAMES))&& (gPS2.autorepeat[8] > AUTOREPEAT_MIN))
            || gPS2.autorepeat[8] == 0)
        {
            RExecuteActionNCallback(rc, 0);
        }
        gPS2.autorepeat[8]++;
    } 
    else
    {
        gPS2.autorepeat[8] = 0;
    }

    /* Check start */

    if(paddata & SCE_PADstart) {
        if(((!(gPS2.autorepeat[9] % AUTOREPEAT_FRAMES))&& (gPS2.autorepeat[9] > AUTOREPEAT_MIN))
            || gPS2.autorepeat[9] == 0)
            RExecuteActionNCallback(rc, 1);
        gPS2.autorepeat[9]++;
    } else
        gPS2.autorepeat[9] = 0;

    /* Check left shoulder buttons */

    if(paddata & SCE_PADL1) {
        RCameraZoom(rc, -0.1f);
    }

    if(paddata & SCE_PADL2) {
        RCameraZoom(rc, 0.1f);
    }

    /* Check the right analog stick */

    if((rdata[4] < PAD_CENTRE - PAD_DEAD_OFFSET) | (rdata[4] > PAD_CENTRE + PAD_DEAD_OFFSET) |
        (rdata[5] < PAD_CENTRE - PAD_DEAD_OFFSET) | (rdata[5] > PAD_CENTRE + PAD_DEAD_OFFSET)) {
        RMouseDrag.m_camstartDist = rc->m_CameraDist;
        RMouseDrag.m_camstartPhi = rc->m_CameraPhi;
        RMouseDrag.m_camstartTheta = rc->m_CameraTheta;
        RMouseDrag.m_camstartX = 0;
        RMouseDrag.m_camstartY = 0;
        RCameraGetLookAt(rc, RMouseDrag.m_camstartLookAt);
        camX = (-(rdata[4] - PAD_CENTRE))>> 5;
        camY = (-(rdata[5] - PAD_CENTRE))>> 5;

        if(paddata & SCE_PADj)
            RCameraZoomDolly(camX, camY);
        else if(paddata & SCE_PADR2)
            RCameraPan(camX, camY);
        else
            RCameraRotate(camX, camY);
    }
    /* The right stick was the Mouse! Now the left stick is. */

    /* Button state maintenance */

    /* Which button is being pressed now? */
    if(paddata & SCE_PADi)
        button = kRMiddleButton;
    else if(paddata & SCE_PADR1)
        button = kRLeftButton;
    else if(paddata & SCE_PADR2)
        button = kRRightButton;
    else
        button = 0;

    /* New button press? */
    if(button && (button != RMouseDrag.m_button)) {
        /* We've overridden a previously pressed button */
        if(RMouseDrag.m_button)
            RExecuteMouseCallback(rc,
                gPS2.m_cursorX, gPS2.m_cursorY, RMouseDrag.m_meModifiers,
                RMouseDrag.m_button, kRNewlyReleased);
        event = kRNewlyPressed;
    } else

        /* Staying button press? */
        /* Only need to do something if there's movement */
        if(button && (button == RMouseDrag.m_button))
            event = kRStillPressed;
        else
            /* Going button press? */
            if(!button && RMouseDrag.m_button)
                event = kRNewlyReleased;
            else
                event = 0;

    if((rdata[6] < PAD_CENTRE - PAD_DEAD_OFFSET) | (rdata[6] > PAD_CENTRE + PAD_DEAD_OFFSET) |
        (rdata[7] < PAD_CENTRE - PAD_DEAD_OFFSET) | (rdata[7] > PAD_CENTRE + PAD_DEAD_OFFSET)) {
        /* Update the pointer position */
        gPS2.m_cursorX += (rdata[6] - PAD_CENTRE) / 12;
        gPS2.m_cursorY += (rdata[7] - PAD_CENTRE) / 12;

        if(gPS2.m_cursorX < 0)
            gPS2.m_cursorX = 0;
        if(gPS2.m_cursorX > SCREEN_WIDTH)
            gPS2.m_cursorX = SCREEN_WIDTH;

        if(gPS2.m_cursorY < 0)
            gPS2.m_cursorY = 0;
        if(gPS2.m_cursorY > (SCREEN_HEIGHT << 1))
            gPS2.m_cursorY = (SCREEN_HEIGHT << 1);

        if(event == kRStillPressed) {
            if(RMouseDrag.m_meModifiers) {
                RExecuteMouseCallback(rc,
                    gPS2.m_cursorX, gPS2.m_cursorY, RMouseDrag.m_meModifiers,
                    RMouseDrag.m_button, event);
            }
        } /* Still Pressed */
    } /* Movement */

    if(event == kRNewlyPressed) {
        /* Fill in data for this drag */
        RMouseDrag.m_meModifiers = 0;
        if(paddata & SCE_PADR1)
            RMouseDrag.m_meModifiers |= RSHIFT;
        if(paddata & SCE_PADR2)
            RMouseDrag.m_meModifiers |= RCONTROL;
        RMouseDrag.m_startx = gPS2.m_cursorX;
        RMouseDrag.m_starty = gPS2.m_cursorY;

        if(RMouseDrag.m_meModifiers) {
            RExecuteMouseCallback(rc,
                gPS2.m_cursorX, gPS2.m_cursorY, RMouseDrag.m_meModifiers,
                button, event);
        }
    } else if(event == kRNewlyReleased) {
        if(RMouseDrag.m_meModifiers)
            RExecuteMouseCallback(rc,
                gPS2.m_cursorX, gPS2.m_cursorY, RMouseDrag.m_meModifiers,
                RMouseDrag.m_button, event);
    }

    /* And finally synchronize m_button */
    RMouseDrag.m_button = button;

    /* Quick hack - draw the mouse pointer */

    mouseVertices[0] = gPS2.m_cursorX;         mouseVertices[1] = gPS2.m_cursorY;         mouseVertices[2] = 0.0f;
    mouseVertices[3] = gPS2.m_cursorX + 20.0f; mouseVertices[4] = gPS2.m_cursorY + 10.0f; mouseVertices[5] = 0.0f;
    mouseVertices[6] = gPS2.m_cursorX + 10.0f; mouseVertices[7] = gPS2.m_cursorY + 20.0f; mouseVertices[8] = 0.0f;

    mouseNormals[0] = 0.0f; mouseNormals[1] = 0.0f; mouseNormals[2] = -1.0f;
    mouseNormals[3] = 0.0f; mouseNormals[4] = 0.0f; mouseNormals[5] = -1.0f;
    mouseNormals[6] = 0.0f; mouseNormals[7] = 0.0f; mouseNormals[8] = -1.0f;
    

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    APPEND_F_OR_D(glMultMatrix)((MeReal*)gOGL.rc->m_CamMatrix2D);

    glVertexPointer  (3,ME_GL_REAL,0,mouseVertices);
    glNormalPointer  (  ME_GL_REAL,0,mouseNormals);
    glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat *)mouseColor);

    glDisable(GL_TEXTURE_2D);
    glDrawArrays(GL_TRIANGLES, 0, 3);    
}
