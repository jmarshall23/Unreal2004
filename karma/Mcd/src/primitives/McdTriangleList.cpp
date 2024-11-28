/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:58 $ - Revision: $Revision: 1.26.2.3 $

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

#include <math.h>
#include <MeMath.h>
#include <MeMemory.h>
#include <lsTransform.h>
#include <McdBox.h>
#include <McdTriangleList.h>
#include <McdCheck.h>
#include <McdGeometryInstance.h>
#include <MeMessage.h>

MCD_IMPLEMENT_GEOMETRY_TYPE( McdTriangleList, "McdTriangleList", TriangleList );

/*----------------------------------------------------------------
 * McdBox implementation
 *----------------------------------------------------------------
 */

/*--- McdtriList type-specific functions ---*/

/**
  McdTriangleLists can be used in two ways.
  Either as a single object with tight bounding box, or with a user
  function that culls a large number of triangles contained in the
  bounding box (such as a terrain), producing just the list of triangles
  potentially colliding with the other model's bounding sphere or
  bounding box (use McdModelGetBSphere, or McdModelGetAABB).

  Create a bounding box of dimension (dx,dy,dz)
*/
McdTriangleListID MEAPI
McdTriangleListCreate(McdFramework *frame, 
                      MeVector3 min, 
                      MeVector3 max, 
                      int maxCount,
                      McdTriangleListFnPtr f )
{
  McdTriangleList *tl;

  if(!f) 
      return 0; //Must have a list generator callback function

  tl = (McdTriangleList *)MeMemoryAPI.createAligned( sizeof( McdTriangleList ),16 );

  McdGeometryInit((McdGeometryID)tl, frame, kMcdGeometryTypeTriangleList);
  McdTriangleListSetBoundingBox((McdGeometryID)tl,min,max);
  tl->triangleListGenerator = f;
  tl->triangleMaxCount = maxCount;  
  return (McdTriangleListID)tl;
}



/**
    Set the bounding box dimension to (dx,dy,dz).
*/

void MEAPI
McdTriangleListSetBoundingBox(McdTriangleListID g,
                              MeVector3 min,
                              MeVector3 max)
{

    McdTriangleList* b = (McdTriangleList*)g;
    MeVector3Add(b->center,min,max);
    MeVector3Scale(b->center,(MeReal)0.5);
    MeVector3Subtract(b->radius,max,min);
    MeVector3Scale(b->radius,(MeReal)0.5);
    MEASSERT(b->radius[0]>(MeReal)0 && b->radius[1]>(MeReal)0 && b->radius[2]>(MeReal)0);

}

/**
    Get the bounding box dimension and write it to (dx,dy,dz)
*/
void MEAPI
McdTriangleListGetBoundingBox( McdTriangleListID g, MeVector3 min, MeVector3 max)
{
  McdTriangleList* b = (McdTriangleList*)g;
  MeVector3Add(max,b->center,b->radius);
  MeVector3Subtract(min,b->center,b->radius);
}


void MEAPI McdTriangleListSetMaxTriangles(McdTriangleListID g, int max)
{
    McdTriangleList* b = (McdTriangleList*)g;
    b->triangleMaxCount = max;
}

int MEAPI McdTriangleListGetMaxTriangles(McdTriangleListID g)
{
    McdTriangleList* b = (McdTriangleList*)g;
    return b->triangleMaxCount;
}


void MEAPI McdTriangleListSetGenerator(McdTriangleListID g,
                                       McdTriangleListFnPtr f)
{
    McdTriangleList* b = (McdTriangleList*)g;
    b->triangleListGenerator = f;
}

McdTriangleListFnPtr MEAPI McdTriangleListGetBoundingBox(McdTriangleListID g)
{
    McdTriangleList* b = (McdTriangleList*)g;
    return b->triangleListGenerator;
}


/*--- McdBox polymorphic functions ---*/

void MEAPI
McdTriangleListDestroy( McdGeometry* g)
{
    if ( g != NULL )
        McdGeometryDeinit(g);
}

void MEAPI
McdTriangleListUpdateAABB(McdGeometryInstanceID ins, MeMatrix4Ptr finalTM, MeBool tight)
{
    McdTriangleList* b = (McdTriangleList*)McdGeometryInstanceGetGeometry(ins);
    MeMatrix4Ptr tm = McdGeometryInstanceGetTransformPtr(ins);
    MeVector3 rFit;
    MeVector3 tCentre;

    MCD_CHECKTRANSFORM(tm, "McdTriangleListUpdateAABB");

    rFit[0] = b->radius[0]*MeFabs(tm[0][0])+
              b->radius[1]*MeFabs(tm[1][0])+
              b->radius[2]*MeFabs(tm[2][0]);
    rFit[1] = b->radius[0]*MeFabs(tm[0][1])+
              b->radius[1]*MeFabs(tm[1][1])+
              b->radius[2]*MeFabs(tm[2][1]);
    rFit[2] = b->radius[0]*MeFabs(tm[0][2])+
              b->radius[1]*MeFabs(tm[1][2])+
              b->radius[2]*MeFabs(tm[2][2]);

    MeMatrix4TMTransform(tCentre,tm,b->center);
    
    MeVector3Subtract(ins->min,tCentre,rFit);
    MeVector3Add(ins->max,tCentre,rFit);


    if(finalTM)
    {
        MeVector3 min,max;
        MeMatrix4TMTransform(tCentre,finalTM,b->center);
        rFit[0] = b->radius[0]*MeFabs(finalTM[0][0])+
                  b->radius[1]*MeFabs(finalTM[1][0])+
                  b->radius[2]*MeFabs(finalTM[2][0]);
        rFit[1] = b->radius[0]*MeFabs(finalTM[0][1])+
                  b->radius[1]*MeFabs(finalTM[1][1])+
                  b->radius[2]*MeFabs(finalTM[2][1]);
        rFit[2] = b->radius[0]*MeFabs(finalTM[0][2])+
                  b->radius[1]*MeFabs(finalTM[1][2])+
                  b->radius[2]*MeFabs(finalTM[2][2]);
        MeVector3Subtract(min,tCentre,rFit);
        MeVector3Add(max,tCentre,rFit);
        MeVector3Min(ins->min,ins->min,min);
        MeVector3Max(ins->max,ins->max,max);
    }
}

void MEAPI
McdTriangleListGetXYAABB( McdGeometry* g, lsTransform *tm, MeReal bounds[4])
{
  MCD_CHECKTRANSFORM(tm, "McdTriangleListGetXYAABB");
  McdTriangleList* b = (McdTriangleList*)g;
  //lsTransform *tm = (lsTransform*)_tm;

    MeReal rFit[2];
    rFit[0] = b->radius[0]*MeFabs(tm->axis(0)[0])+
              b->radius[1]*MeFabs(tm->axis(1)[0])+
              b->radius[2]*MeFabs(tm->axis(2)[0]);
    rFit[1] = b->radius[0]*MeFabs(tm->axis(0)[1])+
              b->radius[1]*MeFabs(tm->axis(1)[1])+
              b->radius[2]*MeFabs(tm->axis(2)[1]);
    bounds[0] = tm->t()[0]-rFit[0];
    bounds[1] = tm->t()[0]+rFit[0];
    bounds[2] = tm->t()[1]-rFit[1];
    bounds[3] = tm->t()[1]+rFit[1];
}

void MEAPI
McdTriangleListGetBSphere( McdGeometry* g, MeVector3 center, MeReal *radius )
{
  McdTriangleList *b = (McdTriangleList*)g;

  MeVector3Copy(center,b->center);
  *radius = MeVector3Magnitude(b->radius);
}

extern MeReal localSignZ( const MeReal x );

//MeReal localSignZ( const MeReal x )
//{
//  if (x == 0) return 0;
//  return x < 0 ? -(MeReal) 1.0 : (MeReal) 1.0;
//}

void MEAPI
McdTriangleListMaximumPoint(McdGeometryInstanceID ins,
          MeReal * const inDir, MeReal * const outPoint)
{
    static int warn = 1;

    if(warn)
        MeWarning(0, "McdTriangleListMaximumPoint: Don't know how to get maximum point!");
    warn = 0;
}


/**
   Unit mass assumed.  The inertia for a box about the x-axis
   can be shown to be:
   I = 1/3 M (ry^2 + rz^2), ri's are the radii of the box.
   Cross terms are zero when the origin is at the CM -> relTM = I
*/
MeI16 MEAPI
McdTriangleListGetMassProperties( McdGeometry *g,
            MeMatrix4 relTM,
            MeMatrix3 m,
            MeReal *volume)
{
    static int warn = 1;

    if(warn)
        MeWarning(0, "McdTriangleListGetMassProperties: Don't know how to get mass props!");
    warn = 0;
    return 0;
}

void* MEAPI McdTriangleListGetUserData( McdTriangleListID g )
{
  return ((McdTriangleList*)g)->userData;

}

void MEAPI McdTriangleListSetUserData( McdTriangleListID g, void* data)
{
  ((McdTriangleList*)g)->userData = data;
}

void MEAPI McdTriangleListDebugDraw(const McdGeometryID geom, const MeMatrix4 tm, const MeReal colour[3])
{
    static int warn = 1;
    if(warn)
        MeWarning(0, "McdTriangleListDebugDraw: Don't know how to draw a TriangleList!");
    warn = 0;
}


