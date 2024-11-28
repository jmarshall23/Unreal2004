/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:40 $ - Revision: $Revision: 1.20.8.4 $

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

#include "RMouseCam.h"
#include <RMenu.h>

void RExecuteUpCallback(RRender* rc) {
    if(rc->m_MenuCurrent)
        RMenuPreviousEntry(rc);
    else
    {
        if(!rc->m_pCallBacks->m_Button[kRButtonUp].m_CallBack)
            return;
        else
            rc->m_pCallBacks->m_Button[kRButtonUp].m_CallBack(rc, rc->m_pCallBacks->m_Button[kRButtonUp].m_Userdata);
    }
}

void RExecuteDownCallback(RRender* rc) {
    if(rc->m_MenuCurrent)
        RMenuNextEntry(rc);
    else
    {
        if(!rc->m_pCallBacks->m_Button[kRButtonDown].m_CallBack)
            return;
        else
            rc->m_pCallBacks->m_Button[kRButtonDown].m_CallBack(rc, rc->m_pCallBacks->m_Button[kRButtonDown].m_Userdata);
    }
}

void RExecuteLeftCallback(RRender* rc) {
    if(rc->m_MenuCurrent)
        RMenuExecute2Entry(rc);
    else
    {
        if(!rc->m_pCallBacks->m_Button[kRButtonLeft].m_CallBack)
            return;
        else
            rc->m_pCallBacks->m_Button[kRButtonLeft].m_CallBack(rc, rc->m_pCallBacks->m_Button[kRButtonLeft].m_Userdata);
    }
}

void RExecuteRightCallback(RRender* rc) {
    if(rc->m_MenuCurrent)
        RMenuExecute1Entry(rc);
    else
    {
        if(!rc->m_pCallBacks->m_Button[kRButtonRight].m_CallBack)
            return;
        else
            rc->m_pCallBacks->m_Button[kRButtonRight].m_CallBack(rc, rc->m_pCallBacks->m_Button[kRButtonRight].m_Userdata);
    }
}

void RExecuteUp2Callback(RRender* rc) {
        if(!rc->m_pCallBacks->m_Button[kRButtonUp2].m_CallBack)
            return;
        else
            rc->m_pCallBacks->m_Button[kRButtonUp2].m_CallBack(rc, rc->m_pCallBacks->m_Button[kRButtonUp2].m_Userdata);
}

void RExecuteDown2Callback(RRender* rc) {
        if(!rc->m_pCallBacks->m_Button[kRButtonDown2].m_CallBack)
            return;
        else
            rc->m_pCallBacks->m_Button[kRButtonDown2].m_CallBack(rc, rc->m_pCallBacks->m_Button[kRButtonDown2].m_Userdata);
}

void RExecuteLeft2Callback(RRender* rc) {
        if(!rc->m_pCallBacks->m_Button[kRButtonLeft2].m_CallBack)
            return;
        else
            rc->m_pCallBacks->m_Button[kRButtonLeft2].m_CallBack(rc, rc->m_pCallBacks->m_Button[kRButtonLeft2].m_Userdata);
}

void RExecuteRight2Callback(RRender* rc) {
        if(!rc->m_pCallBacks->m_Button[kRButtonRight2].m_CallBack)
            return;
        else
            rc->m_pCallBacks->m_Button[kRButtonRight2].m_CallBack(rc, rc->m_pCallBacks->m_Button[kRButtonRight2].m_Userdata);
}

void RExecuteActionNCallback(RRender* rc, unsigned int N) {
    if(N > NUM_ACTION_CALLBACKS) {
        MeWarning(4, "You tried to execute the Action%d callback,"
            "but you only have %d Action callbacks starting from 0 !"
            , N, NUM_ACTION_CALLBACKS);
    }
    else
    {
        if(N == 0)
        {
            RRenderToggleUserHelp(rc);
        }


        if(N == 1 && rc->m_MenuCurrent)
        {
            RRenderHideCurrentMenu(rc);
        }
        else if(N == 1 && !rc->m_MenuCurrent && rc->m_MenuDefault)
        {
            RRenderDisplayDefaultMenu(rc);
        }
        else
        {
            if(!rc->m_pCallBacks->m_Action[N].m_CallBack)
                return;
            else
                rc->m_pCallBacks->m_Action[N].m_CallBack
                (rc, rc->m_pCallBacks->m_Action[N].m_Userdata);
        }
    }
}

void RExecuteJoystickCallback(RRender* rc, MeReal x, MeReal y) {
    if(!rc->m_pCallBacks->m_Joystick.m_CallBack)
        return;
    else
        rc->m_pCallBacks->m_Joystick.m_CallBack(rc
            , x, y
            , rc->m_pCallBacks->m_Joystick.m_Userdata);
}

void RExecuteMouseCallback(
        RRender *         rc, 
        int               x, 
        int               y, 
        int               modifier,
        RMouseButtonWhich button, 
        RMouseButtonEvent event)
{
    if(!rc->m_pCallBacks->m_Mouse.m_CallBack)
        return;
    else 
    {
        AcmeReal portalx, portaly;

#ifdef _XBOX
        portalx = x;
        portaly = y;
#else
        RCalculatePortalCoordinates(
            rc,
            (AcmeReal)*RMouseDrag.p_width,
            (AcmeReal)*RMouseDrag.p_height,
            x,
            y,
            &portalx,
            &portaly);
#endif

        rc->m_pCallBacks->m_Mouse.m_CallBack(rc, (int)portalx, (int)portaly, modifier, button, event,
            rc->m_pCallBacks->m_Mouse.m_Userdata);
    }
}

void RCameraRotate(int x, int y)
{
    AcmeReal theta;
    AcmeReal phi;
    
    theta = RMouseDrag.m_camstartTheta + 3.5f*( (RMouseDrag.m_camstartX - x) / (AcmeReal)(*(RMouseDrag.p_width)) );
    phi = RMouseDrag.m_camstartPhi + 3.5f*( (y - RMouseDrag.m_camstartY) / (AcmeReal)(*(RMouseDrag.p_height)) );
    
    RCameraSetView( *(RMouseDrag.p_rc), (*(RMouseDrag.p_rc))->m_CameraDist, theta, phi );
}

void RCameraZoomDolly(int x, int y)
{
    AcmeReal dist;
    AcmeVector3 newLookAt;

    /* Zoom ... = move in camera Z */

    dist = RMouseDrag.m_camstartDist + 20.0f*( (y - RMouseDrag.m_camstartY) / (AcmeReal)(*(RMouseDrag.p_height)) );

    if( dist < 0.01f )
        dist = 0.01f;

    RCameraSetView( *(RMouseDrag.p_rc)
        , dist
        , (*(RMouseDrag.p_rc))->m_CameraTheta
        , (*(RMouseDrag.p_rc))->m_CameraPhi );

    /* Dolly ... = move in world Z */
    
    newLookAt[0] = RMouseDrag.m_camstartLookAt[0];
    newLookAt[1] = RMouseDrag.m_camstartLookAt[1];
    newLookAt[2] = RMouseDrag.m_camstartLookAt[2] + 20.0f*( (x - RMouseDrag.m_camstartX) / (AcmeReal)(*(RMouseDrag.p_width)) );
    
    RCameraSetLookAt(*(RMouseDrag.p_rc), newLookAt);
}

void RCameraPan(int x, int y) {
    AcmeReal dist;

    /* X */
    dist = 10.0f*( (RMouseDrag.m_camstartX - x) / (AcmeReal)(*(RMouseDrag.p_width)) );
    RMouseDrag.m_camstartX = (AcmeReal)x;

    RCameraPanX(*(RMouseDrag.p_rc), dist);

    /* Y */
    dist = 10.0f*( (y - RMouseDrag.m_camstartY) / (AcmeReal)(*(RMouseDrag.p_height)) );
    RMouseDrag.m_camstartY = (AcmeReal)y;

    RCameraPanY(*(RMouseDrag.p_rc), dist);
}
