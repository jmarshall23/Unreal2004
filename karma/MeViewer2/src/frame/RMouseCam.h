#ifndef _RMOUSECAM_H
#define _RMOUSECAM_H
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:40 $ - Revision: $Revision: 1.12.2.2 $

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

#include <MeViewer.h>

extern struct _rmousedrag
{
    int m_modifiers;
    int m_meModifiers;
    RMouseButtonWhich m_button;
    int m_startx;
    int m_starty;
    AcmeReal m_camstartDist;
    AcmeReal m_camstartTheta;
    AcmeReal m_camstartPhi;
    AcmeReal m_camstartX;
    AcmeReal m_camstartY;
    AcmeVector3 m_camstartLookAt;
    RRender** p_rc;
#ifdef _XBOX
    DWORD* p_width;
    DWORD* p_height;
#else
    int* p_width;
    int* p_height;
#endif
    MeBool m_allowContextMenu;
} RMouseDrag;

#ifdef __cplusplus
extern "C" {
#endif

void RCameraRotate(int x, int y);
void RCameraZoomDolly(int x, int y);
void RCameraPan(int x, int y);
void RExecuteUpCallback(RRender* rc);
void RExecuteDownCallback(RRender* rc);
void RExecuteLeftCallback(RRender* rc);
void RExecuteRightCallback(RRender* rc);
void RExecuteUp2Callback(RRender* rc);
void RExecuteDown2Callback(RRender* rc);
void RExecuteLeft2Callback(RRender* rc);
void RExecuteRight2Callback(RRender* rc);
void RExecuteActionNCallback(RRender* rc, unsigned int N);
void RExecuteJoystickCallback(RRender* rc, MeReal x, MeReal y);
void RExecuteMouseCallback(RRender* rc, int x, int y, int modifier,
        RMouseButtonWhich button, RMouseButtonEvent event);

#ifdef __cplusplus
}
#endif

#endif /* _RMOUSECAM_H */
