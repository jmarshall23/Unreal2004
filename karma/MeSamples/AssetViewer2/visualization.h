/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:08 $ - Revision: $Revision: 1.2.2.1 $

   This software and its accompanying manuals have been developed
   by Mathengine PLC ("MathEngine") and the copyright and all other
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
#ifndef AV_VISUALIZATION_H
#define AV_VISUALIZATION_H


#include <MePrecision.h>
#include "asestorage.h"

void MEAPI RebuildGraphics();
void MEAPI DestroyGraphics();

/* functions to change state */
void MEAPI ToggleDrawMeshes(MeBool b);
void MEAPI ToggleDrawCollisionGeom(MeBool b);
void MEAPI ToggleDynamicsOrigins(MeBool b);
void MEAPI ToggleMassOrigins(MeBool b);
void MEAPI ToggleDrawBSJoints(MeBool b);
void MEAPI ToggleDrawHinges(MeBool b);
void MEAPI ToggleDrawCarWheelJoints(MeBool b);
void MEAPI ToggleDrawPrismatics(MeBool b);

/* functions to create and destroy graphics */
void MEAPI DrawGraphicMeshes(MeBool b);
void MEAPI DrawCollisionGeom(MeBool b);
void MEAPI DrawDynamicsOrigins(MeBool b);
void MEAPI DrawMassOrigins(MeBool b);
void MEAPI DrawBSJoints(MeBool b);
void MEAPI DrawHinges(MeBool b);
void MEAPI DrawCarWheelJoints(MeBool b);
void MEAPI DrawPrismatics(MeBool b);

void MEAPI ToggleDrawContacts(MeBool b);
void MEAPI SetContactDrawLength(MeReal l);
void MEAPI UpdateConstraintGraphics(void);

/* create ASE graphic */
RGraphic* AVCreateAseGraphic(const char* filename, MeReal scale, MeReal color[4], MeVector3Ptr offset, MeMatrix4Ptr matrix);

#endif