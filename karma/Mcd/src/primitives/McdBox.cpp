/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:57 $ - Revision: $Revision: 1.38.2.1 $

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
#include <McdGeometryTypes.h>
#include <McdGeometry.h>
#include <McdGeometryInstance.h>
#include <McdBox.h>
#include <McdCheck.h>
#include <MeMath.h>
#include <lsTransform.h>
#include <MeDebugDraw.h>
#include <MeMemory.h>

MCD_IMPLEMENT_GEOMETRY_TYPE( McdBox, "McdBox" , Box);

/*----------------------------------------------------------------
 * McdBox implementation
 *----------------------------------------------------------------
 */

/*--- McdBox type-specific functions ---*/

/**
    Create a box of dimensions (dx,dy,dz)
*/
McdBoxID MEAPI
McdBoxCreate(McdFramework *frame, MeReal dx, MeReal dy, MeReal dz )
{
  McdBoxID b;

  b = (McdBoxID)MeMemoryAPI.createAligned( sizeof(McdBox), 16 );
  if (!b) return 0;
  McdGeometryInit((McdGeometryID)b, frame, McdBoxGetTypeId() );
  McdBoxSetDimensions( b,dx,dy,dz);

  return b;
}

/**
    Set the box dimensions to (dx,dy,dz)
*/
void MEAPI
McdBoxSetDimensions(McdGeometryID g,
            MeReal dx, MeReal dy, MeReal dz )
{
  McdBox* b = (McdBox*)g;

  b->mR[0] = MeReal(0.5) * dx;
  b->mR[1] = MeReal(0.5) * dy;
  b->mR[2] = MeReal(0.5) * dz;
  b->mRadius = (MeReal)MeSqrt( b->mR[0] * b->mR[0] +
                   b->mR[1] * b->mR[1] +
                   b->mR[2] * b->mR[2] );
}

/**
    Get the box dimensions and write them to (dx,dy,dz)
*/
void MEAPI
McdBoxGetDimensions( McdGeometryID g, MeReal *dx, MeReal *dy, MeReal *dz )
{
  McdBox* b = (McdBox*)g;
  (*dx) = 2 * b->mR[0];
  (*dy) = 2 * b->mR[1];
  (*dz) = 2 * b->mR[2];
}

/**
    Return the box bounding sphere's radius
*/
MeReal MEAPI
McdBoxGetBSphereRadius( McdGeometryID g )
{
  return ((McdBox*)g) -> mRadius;
}

MeReal* MEAPI
McdBoxGetRadii( McdBoxID g)
{
  McdBox* b = (McdBox*)g;
  return b->mR;
}


/*--- McdBox polymorphic functions ---*/

void MEAPI
McdBoxDestroy( McdGeometry* g)
{
    MCD_CHECKGEOMETRY(g, "McdBoxDestroy");
    McdGeometryDeinit(g);
}

void MEAPI
McdBoxUpdateAABB(McdGeometryInstanceID ins, MeMatrix4 finalTM, MeBool tight)
{
    McdBox* b = (McdBox*)McdGeometryInstanceGetGeometry(ins);
    MeMatrix4Ptr tm = McdGeometryInstanceGetTransformPtr(ins);
    MCD_CHECKTRANSFORM(tm, "McdBoxUpdateAABB");
    
    MeVector3 rFit;
    rFit[0] = b->mR[0]*MeFabs(tm[0][0])+
              b->mR[1]*MeFabs(tm[1][0])+
              b->mR[2]*MeFabs(tm[2][0]);
    rFit[1] = b->mR[0]*MeFabs(tm[0][1])+
              b->mR[1]*MeFabs(tm[1][1])+
              b->mR[2]*MeFabs(tm[2][1]);
    rFit[2] = b->mR[0]*MeFabs(tm[0][2])+
              b->mR[1]*MeFabs(tm[1][2])+
              b->mR[2]*MeFabs(tm[2][2]);
    
    MeVector3Subtract(ins->min, (MeVector3Ptr)(tm[3]), rFit);
    MeVector3Add(ins->max, (MeVector3Ptr)(tm[3]), rFit);

    if(finalTM)
    {
        MeVector3 min,max;
        rFit[0] = b->mR[0]*MeFabs(finalTM[0][0])+
                  b->mR[1]*MeFabs(finalTM[1][0])+
                  b->mR[2]*MeFabs(finalTM[2][0]);
        rFit[1] = b->mR[0]*MeFabs(finalTM[0][1])+
                  b->mR[1]*MeFabs(finalTM[1][1])+
                  b->mR[2]*MeFabs(finalTM[2][1]);
        rFit[2] = b->mR[0]*MeFabs(finalTM[0][2])+
                  b->mR[1]*MeFabs(finalTM[1][2])+
                  b->mR[2]*MeFabs(finalTM[2][2]);

        MeVector3Subtract(min, (MeVector3Ptr)(finalTM[3]), rFit);
        MeVector3Add(max, (MeVector3Ptr)(finalTM[3]), rFit);
        MeVector3Min(ins->min,ins->min,min);
        MeVector3Max(ins->max,ins->max,max);
    }
}

void MEAPI
McdBoxGetXYAABB( McdGeometry* g, lsTransform *tm, MeReal bounds[4])
{
  MCD_CHECKTRANSFORM(tm, "McdBoxGetXYAABB");
  McdBox* b = (McdBox*)g;
  //lsTransform *tm = (lsTransform*)_tm;

    MeReal rFit[2];
    rFit[0] = b->mR[0]*MeFabs(tm->axis(0)[0])+
              b->mR[1]*MeFabs(tm->axis(1)[0])+
              b->mR[2]*MeFabs(tm->axis(2)[0]);
    rFit[1] = b->mR[0]*MeFabs(tm->axis(0)[1])+
              b->mR[1]*MeFabs(tm->axis(1)[1])+
              b->mR[2]*MeFabs(tm->axis(2)[1]);
    bounds[0] = tm->t()[0]-rFit[0];
    bounds[1] = tm->t()[0]+rFit[0];
    bounds[2] = tm->t()[1]-rFit[1];
    bounds[3] = tm->t()[1]+rFit[1];
}

void MEAPI
McdBoxGetBSphere( McdGeometry* g, MeVector3 center, MeReal *radius )
{
  McdBox *b = (McdBox*)g;

  center[0] = center[1] = center[2] = (MeReal)(0.);
  *radius = b->mRadius;
}

MeReal localSignZ( const MeReal x )
{
  if (x == 0) return 0;
  return x < 0 ? -(MeReal) 1.0 : (MeReal) 1.0;
}

void MEAPI
McdBoxMaximumPoint(McdGeometryInstanceID ins,
          MeReal * const inDir, MeReal * const outPoint)
{
    McdBox *b = (McdBox*)McdGeometryInstanceGetGeometry(ins);
    lsTransform *tm = (lsTransform*)McdGeometryInstanceGetTransformPtr(ins);
    lsVec3 n;
    tm->inverseTransformWithoutTranslate(*(lsVec3*)inDir,&n);
    // const lsVec3 sR(SignZ(n[0])*b->mR[0],SignZ(n[1])*b->mR[1],SignZ(n[2])*b->mR[2]);
    const lsVec3 sR(localSignZ(n[0])*b->mR[0],localSignZ(n[1])*b->mR[1],localSignZ(n[2])*b->mR[2]);
    tm->transform(sR,(lsVec3*)outPoint);
}


/**
   Unit mass assumed.  The inertia for a box about the x-axis
   can be shown to be:
   I = 1/3 M (ry^2 + rz^2), ri's are the radii of the box.
   Cross terms are zero when the origin is at the CM -> relTM = I
*/
MeI16 MEAPI
McdBoxGetMassProperties( McdGeometry *g,
            MeMatrix4 relTM,
            MeMatrix3 m,
            MeReal *volume)
{
  McdBox* b = (McdBox*)g;
  MeReal Box_coef = (MeReal)(1/3.0), rx, rz, ry;
  rx = b->mR[0];
  ry = b->mR[1];
  rz = b->mR[2];
  MeReal mass = 1.0;

  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++)
      m[i][j] = 0.0;

  m[0][0] = Box_coef * mass * (ry * ry + rz * rz);
  m[1][1] = Box_coef * mass * (rx * rx + rz * rz);
  m[2][2] = Box_coef * mass * (rx * rx + ry * ry);

  MeMatrix4TMMakeIdentity( relTM );

  *volume = 8 * rx * ry * rz;

  return 0;
}


void MEAPI
McdBoxMaximumPointLocal( McdBoxID g, MeReal* inDir, MeReal* outPoint)
{
  McdBox *b = (McdBox*)g;
  outPoint[0] = localSignZ(inDir[0])*b->mR[0],
  outPoint[1] = localSignZ(inDir[1])*b->mR[1],
  outPoint[2] = localSignZ(inDir[2])*b->mR[2];
}

void MEAPI
McdBoxGetXYAABB( McdBoxID g, MeMatrix4 _tm, MeReal bounds[4])
{
  McdBox* b = (McdBox*)g;
  lsTransform *tm = (lsTransform*)_tm;
  MCD_CHECKTRANSFORM(tm, "McdBoxGetXYAABB");

    MeReal rFit[2];
    rFit[0] = b->mR[0]*MeFabs(tm->axis(0)[0])+
              b->mR[1]*MeFabs(tm->axis(1)[0])+
              b->mR[2]*MeFabs(tm->axis(2)[0]);
    rFit[1] = b->mR[0]*MeFabs(tm->axis(0)[1])+
              b->mR[1]*MeFabs(tm->axis(1)[1])+
              b->mR[2]*MeFabs(tm->axis(2)[1]);
    bounds[0] = tm->t()[0]-rFit[0];
    bounds[1] = tm->t()[0]+rFit[0];
    bounds[2] = tm->t()[1]-rFit[1];
    bounds[3] = tm->t()[1]+rFit[1];
}



/**
Draw a box for debug purposes. Assumes MeDebugDraw line function has been set up
*/

void MEAPI McdBoxDebugDraw(const McdGeometryID geom, const MeMatrix4 tm, const MeReal colour[3])
{
    static const MeVector3 boxDraw[12][2] =
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

    if(McdGeometryGetTypeId(geom) != kMcdGeometryTypeBox)
        return;

    McdBoxID box = (McdBoxID)geom;
    int i;
    MeVector3 rads;
    MeVector3 wv1,wv2,lv;

    McdBoxGetDimensions(box, &rads[0], &rads[1], &rads[2]);
    MeVector3Scale(rads, 0.5); /* change from lengths to radii */
    
    for(i=0; i<12; i++)
    {
        MeVector3MultiplyElements(lv,boxDraw[i][0],rads);
        MeMatrix4TMTransform(wv1, tm, lv);

        MeVector3MultiplyElements(lv,boxDraw[i][1],rads);
        MeMatrix4TMTransform(wv2, tm, lv);

        MeDebugDrawAPI.line(wv1,wv2,colour[1],colour[2],colour[3]);        
    }
}
