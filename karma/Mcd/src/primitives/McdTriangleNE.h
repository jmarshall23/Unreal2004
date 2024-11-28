#ifndef _MCDTRIANGLENE_H
#define _MCDTRIANGLENE_H
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:58 $ - Revision: $Revision: 1.1.10.1 $

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

/**
  @file
  The user-specified triangle. Not part of the geometry.
  Subject to change
*/

#include <McdCTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct McdTriangleNE
{
  MeReal *vertices; /**< pointers to vertices */
  MeVector3 normal;    /**< triangle normal. Subject to change */
  MeVector3 edges[3];
} McdTriangleNE;

/**
    Initialize a McdTriangleNE struct to hold the triangle represented
    by @a vertex1, @a vertex2, @a vertex3. The vertices must be in
    counterclockwise order when seen from the outside of the model.
    Collision detection with other geometries is one-sided, that is
    it is assumed that triangles are part of a model.
    Normal and edges are computed in this call. This part of the API
    is subject to change; for example, the normal might become
    a pointer to a normal that needs to be computed by the user.
*/
void              MEAPI McdTriangleNEInit(McdTriangleNE* t,
                           MeReal* vertex1, MeReal* vertex2, MeReal* vertex3);

/**
    Produce contacts for a model with box geometry with a triangle.
*/
void MEAPI McdBoxTriangleNEIntersect(McdModelID box, McdTriangleNE* t, McdIntersectResult *result);

/**
    Produce contacts for a model with sphere geometry with a triangle.
*/
void MEAPI McdSphereTriangleNEIntersect(McdModelID sphere, McdTriangleNE* t, McdIntersectResult *result);

/**
    Produce contacts for a model with cylinder geometry with a triangle.
*/
void MEAPI McdCylinderTriangleNEIntersect(McdModelID cylinder, McdTriangleNE* t, McdIntersectResult *result);

/**
    Produce contacts for a model with any geometry for which the triangle
    functions are implemented. The return value is 0 if the corresponding
    function is not implemented. See the guide or this header file
    for a listing of implemented functions.
*/
int MEAPI McdTriangleNEIntersect(McdModelID model, McdTriangleNE* t, McdIntersectResult *result);

#ifdef __cplusplus
}
#endif

#endif /* _MCDTRIANGLENE_H */
