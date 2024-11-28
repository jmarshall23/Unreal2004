/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/18 12:02:29 $ - Revision: $Revision: 1.45.2.5 $

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

#include <string.h>

#include <MePrecision.h>
#include <MeAssert.h>
#include <MeMath.h>
#include <MeBounding.h>

#include <McdBox.h>
#include <McdSphere.h>
#include <McdConvexMesh.h>
#include <McdGeometryInstance.h>
//#include <MeDebugDraw.h>
#include <MeMessage.h>

#include <McdGjkMaximumPoint.h>

#include <McdConvexMesh.h>

MCD_IMPLEMENT_GEOMETRY_TYPE( McdConvexMesh, "McdConvexMesh", ConvexMesh );


/****************************************************************************
  This does a safe "downcast" from McdGeometryID to McdConvexMesh*
*/
#ifdef _MECHECK
static McdConvexMesh *CAST_ConvexMesh(McdGeometryID g)
{
    MEASSERT(g && McdGeometryGetType(g)==kMcdGeometryTypeConvexMesh);
    return (McdConvexMesh*)g;
}
#else
#   define CAST_ConvexMesh(g) ((McdConvexMesh*)g)
#endif

/****************************************************************************
 *  Calculate a good (about 5% error) bounding sphere for a set of points in 3D.
 */
void MEAPI MeBoundingSphereCalc2(McdCnvVertex* points, int numPoints,
                    MeVector3 centre, MeReal* radius)
{
    MeReal r, r2; /* radius and radius squared */
    MeReal d2;
    MeVector3 min, max; /* AABB */
    int minIx[3], maxIx[3]; /* Extreme points. */
    int i, j;

    if(numPoints == 0)
        return;

    /* First, find AABB, remembering furthest points in each dir. */
    for(i=0; i<3; i++)
    {
        min[i] = max[i] = points[0].position[i];
        minIx[i] = maxIx[i] = 0;
    }

    for(i=1; i<numPoints; i++) 
    {
        for(j=0; j<3; j++)
        {
            if (points[i].position[j] < min[j]) 
            {
                min[j] = points[i].position[j];
                minIx[j] = i;
            }
            else if (points[i].position[j] > max[j]) 
            {
                max[j] = points[i].position[j];
                maxIx[j] = i;
            }
        }
    }

    /*  Now find extreme points furthest apart, 
        and initial centre and radius of sphere. */
    d2 = 0;
    for(i=0; i<3; i++)
    {
        MeVector3 diff;
        MeReal tmpd2;
        MeVector3Subtract(diff, points[maxIx[i]].position, points[minIx[i]].position);
        tmpd2 = MeVector3MagnitudeSqr(diff);

        if(tmpd2 > d2)
        {
            d2 = tmpd2;
            MeVector3ScaleAndAdd(centre, points[minIx[i]].position, (MeReal)0.5, diff);
        }
    }

    r = (MeReal)0.5 * MeSqrt(d2);
    r2 = r * r;


    /*  Now check each point lies within this sphere.
        If not - expand it a bit. */
    for(i=0; i<numPoints; i++) 
    {
        MeVector3 cToP;
        MeReal pr2;

        MeVector3Subtract(cToP, points[i].position, centre);
        pr2 = MeVector3MagnitudeSqr(cToP);

        /* If this point is outside our current bounding sphere.. */
        if(pr2 > r2)
        {
            /* ..expand sphere just enough to include this point. */
            MeReal pr = MeSqrt(pr2);
            r = (MeReal)0.5 * (r + pr);
            r2 = r * r;

            MeVector3MultiplyAdd(centre, (pr-r)/pr, cToP);
        }
    }

    *radius = r;
}

/****************************************************************************
  This create a convex mesh from a the output of QHull

  Caution!! the memory sematics of "poly" is rather strange!!
  Since a McdConvexMesh (geometry) contains an McdConvexHull, it will take
  a _shallow_ copy of poly.  The caller is required to free the poly
  struct BUT the caller must not free any of the arrays that poly points to.

  A good idea is to put poly on the stack, (see McdConvexMeshCreateHull).
*/
McdGeometry* MEAPI
McdConvexMeshCreate(McdFramework *frame, McdConvexHull *poly, MeReal fatness )
{
    McdConvexMesh* c;
    McdGeometry *g;

    c = (McdConvexMesh*)MeMemoryAPI.createAligned( sizeof( McdConvexMesh ), 16 );
    g = (McdGeometry*)c;

    McdGeometryInit(g,frame,kMcdGeometryTypeConvexMesh);

    c->mHull = *poly;
    c->mFatness = fatness;

    MeBoundingSphereCalc2(c->mHull.vertex, c->mHull.numVertex, 
                          c->mBoundingSphereCenter,
                          &c->mBoundingSphereRadius);

    c->mBoundingSphereRadius += fatness;

    return g;
}

/****************************************************************************
  Create new convex polyhedron object from its vertices. Computes
  the convex hull polygons and edges.

  return 0 on failure
*/
McdConvexMeshID MEAPI 
McdConvexMeshCreateHull(McdFramework *frame,
                        const MeVector3 *vertices,
                        int vertexCount, MeReal fatnessRadius )
{
    McdConvexHull poly;
    int ok = McdComputeHull(&poly, vertexCount, vertices);
    if (!ok)
        return 0;
    return McdConvexMeshCreate(frame, &poly, fatnessRadius);
}


/****************************************************************************
    This is deprecated because it's a bad idea.  If you want to change the 
    hull of a geometry, you should create a new geometry.
*/
void MEAPI
McdConvexMeshSetPolyhedron( McdConvexMeshID g, McdConvexHull *poly, const MeReal fatness)
{
    MEASSERT(!"McdConvexMeshSetPolyhedron is deprecated.");
    MeWarning(0, "McdConvexMeshSetPolyhedron is deprecated and does nothing.");
}

/* polymorphic functions */

/****************************************************************************
  Deallocate a convex mesh
*/
void MEAPI
McdConvexMeshDestroy( McdConvexMeshID g)
{
    McdConvexMesh *c = CAST_ConvexMesh(g);
    McdDeallocateHull(&c->mHull);
    McdGeometryDeinit(g);
}

/****************************************************************************
  This computes the bounding box around the convex mesh.
*/
static void getBoundingBox(McdConvexMesh *c, MeMatrix4Ptr tm, 
                           MeVector3 min, MeVector3 max)
{
    MeVector3 localDir;
    int i, h;
    for (i = 0; i < 3; ++i)
    {
        MeVector3Set(localDir, tm[0][i], tm[1][i], tm[2][i]);
        max[i] = McdConvexMeshMaximumPointLocal(c, localDir, 0, MEINFINITY, &h);

        MeVector3Scale(localDir, -1);
        min[i] = McdConvexMeshMaximumPointLocal(c, localDir, 0, MEINFINITY, &h);
    }

    min[0] = tm[3][0] - min[0] - c->mFatness;
    min[1] = tm[3][1] - min[1] - c->mFatness;
    min[2] = tm[3][2] - min[2] - c->mFatness;

    max[0] += tm[3][0] + c->mFatness;
    max[1] += tm[3][1] + c->mFatness;
    max[2] += tm[3][2] + c->mFatness;
}

/****************************************************************************
   This updates the cached AABB. If finalTM is non-NULL, it uses the AABB 
   of the volume swept by the geometry with its current and final positions .
   if tight==True, use slower more accurate algorithm.
*/
void MEAPI
McdConvexMeshUpdateAABB(McdGeometryInstanceID ins, MeMatrix4Ptr finalTM, MeBool tight)
{
    McdConvexMesh *c = CAST_ConvexMesh(McdGeometryInstanceGetGeometry(ins));
    MeMatrix4Ptr tm = McdGeometryInstanceGetTransformPtr(ins);

    if (!tight && !finalTM)
    {
        //  TODO!  use cheaper algorithm here....
    }

    getBoundingBox(c, tm, ins->min, ins->max);

    if (finalTM)
    {
        MeVector3 min,max;
        getBoundingBox(c, finalTM, min, max);
        MeVector3Min(ins->min, ins->min, min);
        MeVector3Max(ins->max, ins->max, max);
    }
}

/****************************************************************************
*/
void MEAPI
McdConvexMeshGetBSphere( McdGeometryID g, MeVector3 center, MeReal *radius )
{
    McdConvexMesh *c = CAST_ConvexMesh(g);

    MeVector3Copy(center, c->mBoundingSphereCenter);
    *radius = c->mBoundingSphereRadius;
}

/****************************************************************************
*/
void MEAPI
McdConvexMeshMaximumPoint(McdGeometryInstanceID ins,
                          MeReal * const inDir, 
                          MeReal * const outPoint)
{
    McdConvexMeshMaximumPointNew(ins, inDir, outPoint);
}

/****************************************************************************
   deprecated
*/
void MEAPI
McdConvexMeshGetXYAABB( McdGeometryID g, MeMatrix4 _tm,
                       MeReal bounds[4])
{
    MEASSERT(!"McdConvexMeshGetXYAABB is deprecated");
}

/****************************************************************************
*/
int MEAPI
McdConvexMeshMeshGetTypeId()
{
    return kMcdGeometryTypeConvexMesh;
}

/****************************************************************************
  This returns the number of vertices (or edges) on a specific
  face of a convex hull.
  return 0 if polyID is out of range.
*/
int MEAPI
McdConvexMeshGetPolygonVertexCount(McdConvexMeshID g, int polyID)
{
    McdConvexMesh *c = CAST_ConvexMesh(g);
    return McdCnvFaceGetCount(&c->mHull, polyID);
}

/****************************************************************************
    Copy polygon vertex position into coords. get direct access to vertex 
*/
const McdConvexHull* MEAPI 
McdConvexMeshGetPolyhedron(McdConvexMeshID g)
{
    return & CAST_ConvexMesh(g)->mHull;
}

/****************************************************************************
  This gets the coordinates of a vertex of a convex.
*/
void MEAPI
McdConvexMeshGetPolygonVertex( McdConvexMeshID m, int polyID, int vertexID, MeReal coords[3])
{
    McdConvexMesh *c = CAST_ConvexMesh(m);
    MeVector3Copy(coords, McdCnvFaceGetVertexPosition(&c->mHull, polyID, vertexID));
}

/****************************************************************************
  This gets a vertex pointer
*/
const MeReal * MEAPI McdConvexMeshGetPolygonVertexPtr( McdConvexMeshID m, int polyID, int vertexID)
{
    McdConvexMesh *c = CAST_ConvexMesh(m);
    return McdCnvFaceGetVertexPosition(&c->mHull, polyID, vertexID);
}

/****************************************************************************
*/
MeReal MEAPI McdConvexMeshGetFatness(McdConvexMeshID g)
{
    McdConvexMesh *c = CAST_ConvexMesh(g);
    return c->mFatness;
}

/****************************************************************************
*/
void MEAPI
McdConvexMeshGetPolygonNormal( McdConvexMeshID m, int polyID, MeVector3 coords)
{
    McdConvexMesh *c = CAST_ConvexMesh(m);
    MeVector3Copy(coords, c->mHull.face[polyID].normal);
}

/****************************************************************************
*/
int MEAPI
McdConvexMeshGetPolygonCount( McdConvexMeshID m)
{
    McdConvexMesh *c = CAST_ConvexMesh(m);
    return c->mHull.numFace;
}

/****************************************************************************
*/
void MEAPI McdConvexMeshDebugDraw(const McdGeometryID g, 
                                  const MeMatrix4 tm, 
                                  const MeReal colour[3])
{
    // do nothing
}
