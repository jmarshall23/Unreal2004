/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.5.2.2 $

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

#include <McdGjkMaximumPoint.h>

 /****************************************************************************
  This file contains functions for "support" i.e. finding the extreme
  point in a particular direction.  Also fatness which is a fixed
  radius added to the skeletal inside of a rounded shape.

  John Henckel  1/2002
*/


/****************************************************************************
  This gets the fatness of the geometry instance.  Presently only convex
  mesh have "fatness" however we need to change this someday.
*/
MeReal McdGjkFatness(McdGeometryInstanceID ins)
{
    switch (McdGeometryGetTypeId(ins->mGeometry))
    {
    case kMcdGeometryTypeConvexMesh:
        {
            McdConvexMesh *cnv = (McdConvexMesh *) ins->mGeometry;
            return cnv->mFatness;
        }
    case kMcdGeometryTypeSphere:
        {
            McdSphere *s = (McdSphere *) ins->mGeometry;
            return s->mRadius;
        }
    case kMcdGeometryTypeSphyl:
        {
            McdSphyl *s = (McdSphyl *) ins->mGeometry;
            return s->mRadius;
        }
    }
    return 0;     // default fatness is zero
}

/****************************************************************************
  This replaces the default one because the default returns edges.
*/
void
McdBoxMaximumPointNew(McdGeometryInstanceID ins,
                      const MeVector3 inDir, MeVector3 outPoint)
{
    McdBox *b = (McdBox*)McdGeometryInstanceGetGeometry(ins);
    MeMatrix4Ptr tm = McdGeometryInstanceGetTransformPtr(ins);
    MeVector3 n;
    MeMatrix4TMInverseRotate(n, tm, inDir);
    MeVector3 c = { (n[0]<0?-1:1)*b->mR[0],
                    (n[1]<0?-1:1)*b->mR[1],
                    (n[2]<0?-1:1)*b->mR[2] };
    MeMatrix4TMTransform(outPoint, tm, c);
}

/****************************************************************************
  This returns the farthest point in a given direction.
  The inDir is a direction in local reference frame.  
  outIndex is the index into the McdConvexHull vertices.
  hint is an index that might be a good place to start looking.
  Stop on the first point that has a dotprod > minDist.

  This uses a greedy hill climbing algorithm.  There might be some problems
  with precision here.  I haven't used any epsilons, and I wonder if perhaps
  the greedy search might wander around and around a maxima without ever
  reaching it.  It doesn't seem like it could.  It might be a good idea
  to search the edges in a random order... maybe.

  The dot product of inDir with vertex[outIndex] is returned.
*/
MeReal
McdConvexMeshMaximumPointLocal(McdConvexMesh *conv,
                               const MeVector3 inDir, 
                               int hint,
                               MeReal minDist,
                               int *outIndex)
{
    MEASSERT(conv && outIndex);

    int i, j, k, t, m, next, prev;
    int n = conv->mHull.numVertex;
    McdCnvVertex *vert = conv->mHull.vertex;
    MeReal d1, d2;

    next = (hint < 0 || hint >= n) ? 0 : hint;
    d1 = MeVector3Dot(inDir, vert[next].position);
    i = -1;

    //  Walk over the convex edges always going to the best neighbor vertex.
    for (t = -2; t < n && i != next; ++t)
    {
        //  find any vertex with distance greater than minDist
        if (d1 > minDist)
            break;

        //  Look at all the vertices around "i", if any are better, set "next".
        prev = i;
        i = next;
        m = McdCnvVertexGetCount(&conv->mHull, i);

        for (j = 0; j < m; ++j)
        {
            k = McdCnvVertexGetNeighbor(&conv->mHull, i, j);

            if (k == prev) continue;    // don't go back where we came from

            d2 = MeVector3Dot(inDir, vert[k].position);

            if (d2 > d1)
            {
                d1 = d2;
                next = k;
            }
        }
    }
    MEASSERT(t < n);      // check exceeded max iterations

    *outIndex = next;
    return d1;
}

/****************************************************************************
  This returns the farthest point in a given direction.
  The inDir is a direction in WRF.  outPoint is a vertex of the ConvexMesh.
*/
MeReal MEAPI
McdConvexMeshMaximumPointNew(McdGeometryInstanceID ins,
                             const MeVector3 inDir, 
                             MeVector3 outPoint)
{
    McdConvexMesh *conv = (McdConvexMesh*)McdGeometryInstanceGetGeometry(ins);
    MeMatrix4Ptr tm = McdGeometryInstanceGetTransformPtr(ins);
    MeVector3 temp;
    MeReal d;
    int i;

    //  transform the direction into instance reference frame
    MeMatrix4TMInverseRotate(temp, tm, inDir);

    //  Find index of maximum point
    d = McdConvexMeshMaximumPointLocal(conv, temp, 0, MEINFINITY, &i);

    //  transform the max point to WRF
    MeMatrix4TMTransform(outPoint, tm, conv->mHull.vertex[i].position);

    return d;
}


/*
    This computes the farthest point on a geometry instance in a given
    direction.

    Usually this just calls McdGeometryInstanceMaximumPoint, but I may
    want to add some optimizations, etc for ConvexMesh, Sphere, and
    Sphyl.
*/
void McdGjkMaximumPoint(McdGeometryInstanceID ins, 
                        const MeVector3 v, MeVector3 out)
{
    switch (McdGeometryGetTypeId(ins->mGeometry))
    {
    case kMcdGeometryTypeSphere:
        MeVector3Copy(out, ins->mTM[3]);
        break;

    case kMcdGeometryTypeSphyl:
        McdGeometryInstanceMaximumPoint(ins, (MeReal*) v, out);
        MeVector3MultiplyAdd(out, -McdGjkFatness(ins), v);
        break;

    case kMcdGeometryTypeBox:
        McdBoxMaximumPointNew(ins, v, out);
        break;

    case kMcdGeometryTypeConvexMesh:
        McdConvexMeshMaximumPointNew(ins, v, out);
        break;

    default:
        McdGeometryInstanceMaximumPoint(ins, (MeReal*) v, out);
    }
}
