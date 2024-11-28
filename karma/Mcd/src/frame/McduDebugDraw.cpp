/* -*-c++-*-
 *===============================================================
 * File:        McduDebugDraw.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.4.4.1 $
 * $Date: 2002/05/01 15:19:19 $
 *
 *================================================================
 */

#include <MeMath.h>
#include <MeDebugDraw.h>
#include <MeMessage.h>
#include <McdModel.h>
#include <McdGeometry.h>
#include <McdBox.h>
#include <McdSphere.h>
#include <McdCylinder.h>
#include <McdConvexMesh.h>
#include <McdAggregate.h>
#include <McdSpace.h>
#include <McduDebugDraw.h>





MeVector3 boxDraw[12][2] =
{
    {{1, -1, -1},    {1, 1, -1}},
    {{1, 1, -1},     {1, 1, 1}},
    {{1, 1, 1},      {1, -1, 1}},
    {{1, -1, 1},     {1, -1, -1}},

    {{-1, -1, -1},   {-1, 1, -1}},
    {{-1, 1, -1},    {-1, 1, 1}},
    {{-1, 1, 1},     {-1, -1, 1}},
    {{-1, -1, 1},    {-1, -1, -1}},

    {{-1, -1, -1},   {1, -1, -1}},
    {{-1, 1, -1},    {1, 1, -1}},
    {{-1, 1, 1},     {1, 1, 1}},
    {{-1, -1, 1},    {1, -1, 1}}
};

void MEAPI McduDebugDrawAABB(const MeVector3 min, const MeVector3 max, const MeReal colour[3])
{
    int i;
    MeVector3 centre, radius, lv, wv1, wv2;

    MeVector3Add(centre,min,max);
    MeVector3Subtract(radius, min, max);
    MeVector3Scale(centre,(MeReal)0.5);
    MeVector3Scale(radius,(MeReal)0.5);

    for(i=0; i<12; i++)
    {
        MeVector3MultiplyElements(lv,boxDraw[i][0],radius);
        MeVector3Add(wv1,centre,lv);
        MeVector3MultiplyElements(lv,boxDraw[i][1],radius);
        MeVector3Add(wv2,centre,lv);
        MeDebugDrawAPI.line(wv1,wv2,colour[0],colour[1],colour[2]);        
    }
}


/**
    Draws a model, according to the debug flags
*/

void MEAPI McduDebugDrawModel(const McdModelID model, McduDebugDrawFlags flags, const MeReal colour[3])
{
    McdGeometryID geom = McdModelGetGeometry(model);
    MeMatrix4Ptr tm = McdModelGetTransformPtr(model);
    MeVector3 min,max;
    
    if(flags & kMcduDebugDrawAABB)
    {
        McdModelGetAABB(model,min,max);
        McduDebugDrawAABB(min,max,colour);
    }

    if(flags & kMcduDebugDrawDetail)
        McdGeometryDebugDraw(geom, tm, colour);
}

/**
    Draws all the models in space, according to the debug draw flags
*/


void MEAPI McduDebugDrawSpace(const McdSpaceID space, McduDebugDrawFlags flags, const MeReal colour[3])
{
    McdSpaceModelIterator it;
    McdModelID m;    
    
    McdSpaceModelIteratorBegin( space, &it );
    
    while ( McdSpaceGetModel(space, &it, &m )) 
    {
        if(!((flags & kMcduDebugDrawEnabledOnly) && McdSpaceModelIsFrozen(m)))
            McduDebugDrawModel(m,flags,colour);
    }
}





