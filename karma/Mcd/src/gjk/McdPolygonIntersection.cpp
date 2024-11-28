/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.3.2.3 $

   This software and its accompanyng manuals have been developed
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

#include <MeMath.h>
#include <McdPolygonIntersection.h>


/****************************************************************************
  return true if a==(xv,yv) within tolerance
*/
int McdEqual2D(const MeVector3 a, MeReal xv, MeReal yv, int x, int y)
{
    MeReal u = a[x]-xv, v = a[y]-yv;
    return u < ME_MEDIUM_EPSILON 
        && u > -ME_MEDIUM_EPSILON 
        && v < ME_MEDIUM_EPSILON 
        && v > -ME_MEDIUM_EPSILON;
}
/****************************************************************************
  r = a - b
*/
void McdSubtract2D(MeVector3 r, const MeVector3 a, const MeVector3 b, int x, int y)
{
    r[x] = a[x] - b[x];
    r[y] = a[y] - b[y];
}
/****************************************************************************
  r = a + b
*/
void McdAdd2D(MeVector3 r, const MeVector3 a, const MeVector3 b, int x, int y)
{
    r[x] = a[x] + b[x];
    r[y] = a[y] + b[y];
}
/****************************************************************************
  return a x b
*/
MeReal McdCross2D(const MeVector3 a, const MeVector3 b, int x, int y)
{
    return a[x]*b[y] - b[x]*a[y];
}

/****************************************************************************
  One of these is called by qsort.  
  I had to make one for each axis.
*/
typedef int (*McdPolyPointCompareFn)(const void*,const void*);

int McdPolyPointCompare0(const void *p1, const void *p2) 
{   
    MeReal t = ((MeReal*)p1)[0] - ((MeReal*)p2)[0];
    return t < 0 ? -1 : t > 0 ? 1 : 0;
}
int McdPolyPointCompare1(const void *p1, const void *p2) 
{   
    MeReal t = ((MeReal*)p1)[1] - ((MeReal*)p2)[1];
    return t < 0 ? -1 : t > 0 ? 1 : 0;
}
int McdPolyPointCompare2(const void *p1, const void *p2) 
{   
    MeReal t = ((MeReal*)p1)[2] - ((MeReal*)p2)[2];
    return t < 0 ? -1 : t > 0 ? 1 : 0;
}


/****************************************************************************
  This chooses a coordinate axis x=0, y=1, z=2 that is most parallel to
  the given normal.  
*/
int McdPolygonBestAxis(const MeVector3 normal)
{
    MeVector3 temp = { MeFabs(normal[0]), MeFabs(normal[1]), MeFabs(normal[2]) };
    int axis;
    if (temp[0] > temp[1])
        axis = temp[0] > temp[2] ? 0 : 2;
    else
        axis = temp[1] > temp[2] ? 1 : 2;
    return axis;
}

/****************************************************************************
  This sorts polygon vertices. 

  First compute the center of the polygon, then compute the angle of each
  vertex against the middle using cross prod.  Store the angle
  in the axis coordinate.  Then sort by angle.
*/
void McdPolygonSort(int numpoly, MeVector3 *poly, int axis)
{
    int i, x, y;
    MeVector3 v, mid = {0,0,0};

    if (numpoly < 3)
        return;

    x = NextMod3(axis);
    y = NextMod3(x);

    //  Find poly midpoint
    for (i = 0; i < numpoly; ++i)
        McdAdd2D(mid, mid, poly[i], x, y);

    mid[x] *= (MeReal) 1/numpoly;
    mid[y] *= (MeReal) 1/numpoly;

    for (i = 0; i < numpoly; ++i)
    {
        McdSubtract2D(v, poly[i], mid, x, y);
        poly[i][axis] = MeAtan2(v[y], v[x]);
    }

    McdPolyPointCompareFn cmp[] = { McdPolyPointCompare0, McdPolyPointCompare1, McdPolyPointCompare2 };

    qsort(poly, numpoly, sizeof *poly, cmp[axis]);
}

/****************************************************************************
  This appends one 2D point to an array of 3D vectors.
  It test if the new point is the same as the last point, or the first 
  point in polyOut.  Return 1 if it is the same as the first.
*/
int McdAddPoint(int *numOut, MeVector3 *polyOut, 
                MeReal xv, MeReal yv, int x, int y)
{
    int i = *numOut;

    if (i > 0 && McdEqual2D(polyOut[i-1], xv, yv, x, y))
        return 0;

    if (i > 1 && McdEqual2D(polyOut[0], xv, yv, x, y))
        return 1;

    polyOut[i][x] = xv;
    polyOut[i][y] = yv;
    ++*numOut;
    return 0;
}


/****************************************************************************
  This tests if a point is inside (or on an edge) of a polygon.
  The polygon must be sorted.  Return true or false.
*/
int McdPolygonContainsPoint(int numpoly, MeVector3 *poly, 
                            MeVector3 pt, int axis)
{
    int x, y, i, j;
    MeVector3 a, b;

    x = NextMod3(axis);
    y = NextMod3(x);

    for (i = 0; i < numpoly; ++i)
    {
        //  if the cross product of the poly-to-point vector and the
        //  edge vector is negative, the point is outside the edge.

        j = NextModN(i, numpoly);

        McdSubtract2D(a, pt,      poly[i], x, y);
        McdSubtract2D(b, poly[j], poly[i], x, y);

        if (McdCross2D(a, b, x, y) > 0)
            return 0;
    }
    return 1;
}


/****************************************************************************
  This computes the intersection of two polygons.
  It uses the sickle algorithm, see O'Rourke 82 at

      http://www.cs.smith.edu/~orourke/books/compgeom.html
      http://citeseer.nj.nec.com/context/201691/0

  The normal and distance determines the projection plane.  All the points
  in poly1 and poly2 are assumed to lie on (or close to) this plane.

  This function messes up poly1 and poly2, they are flattened to
  a coordinate plane and sorted using qsort.

  The caller MUST allocate sufficient space for polyOut.  The maximum
  size required is 2*min(numpoly1, numpoly2).
*/
void McdPolygonIntersection(const MeVector3 normal, MeReal dist,
                            int numpoly1, MeVector3 *poly1, 
                            int numpoly2, MeVector3 *poly2, 
                            int *numOut, MeVector3 *polyOut)
{
    //  check parms

    MEASSERT(poly1 && poly2 && numOut && polyOut);
    *numOut = 0;

    //  determine best axis

    int axis = McdPolygonBestAxis(normal);

    //  sort each poly 

    McdPolygonSort(numpoly1, poly1, axis);
    McdPolygonSort(numpoly2, poly2, axis);

    //  special case for polygons with only one point

    if (numpoly1==1 && McdPolygonContainsPoint(numpoly2, poly2, poly1[0], axis))
        MeVector3Copy(polyOut[(*numOut)++], poly1[0]);
    else if (numpoly2==1 && McdPolygonContainsPoint(numpoly1, poly1, poly2[0], axis))
        MeVector3Copy(polyOut[(*numOut)++], poly2[0]);

    if (numpoly1 < 2 || numpoly2 < 2)
        return;


    //------------------------------------------------------------------
    //  begin intersection walk

    int s1, s2, e1, e2, neg1, neg2;
    int x, y, inside, change, i, numboth;
    MeVector3 a1, a2, b1, b2, b3;
    MeReal cross, sign1, sign2, ps1, ps2;

    x = NextMod3(axis);
    y = NextMod3(x);

    s1 = s2 = 0;        // start of each edge
    e1 = e2 = 1;        // end of edge
    change = 0;         // which poly should advance, 1 or 2
    inside = 0;         // which poly is inside the other, 1 or 2
    ps1 = ps2 = 0;      // the previous value of sign1,2
    neg1 = neg2 = 0;    // true if sign1,2 was ever negative

    numboth = numpoly1 + numpoly2;

    for (i = 0; i <= 2*numboth; ++i)
    {
        //  a1, a2 are the sides of the two polygons
        //  b1, b2 are the bridge vectors between the sides

        if (change!=2) 
            McdSubtract2D(a1, poly1[e1], poly1[s1], x, y);
        if (change!=1) 
            McdSubtract2D(a2, poly2[e2], poly2[s2], x, y);
        McdSubtract2D(b1, poly2[e2], poly1[s1], x, y);
        McdSubtract2D(b2, poly1[e1], poly2[s2], x, y);

        cross = McdCross2D(a1, a2, x, y);
        sign1 = McdCross2D(a1, b1, x, y);
        sign2 = McdCross2D(a2, b2, x, y);

        //  look for a change in sign1 or sign2 which indicates a cross over

        if (change && (ps1*sign1 <= 0 || ps2*sign2 <= 0) 
            && !ME_IS_ZERO(cross))
        {
            MeReal k1, k2;
            McdSubtract2D(b3, poly1[s1], poly2[s2], x, y);

            k1 = McdCross2D(a2, b3, x, y) / cross;
            k2 = McdCross2D(a1, b3, x, y) / cross;

            //  Test if k1 and k2 are in the range [0, 1]

            if (k1 > -ME_SMALL_EPSILON && k1 < 1 + ME_SMALL_EPSILON && 
                k2 > -ME_SMALL_EPSILON && k2 < 1 + ME_SMALL_EPSILON )
            {
                if (McdAddPoint(numOut, polyOut, 
                        poly1[s1][x] + k1*a1[x],
                        poly1[s1][y] + k1*a1[y], x, y))
                    break;

                inside = cross < 0 ? 1 : 2;
            }
        }

        //  The following verifies that the "inside" flag is set correctly.
        //  If all goes well this shouldn't be necessary, but just in case.

        if (inside==1 && sign2<0 && sign1>0)   
            inside = 2;
        if (inside==2 && sign1<0 && sign2>0)   
            inside = 1;
        if (inside && sign1<0 && sign2<0)   
            inside = 0;

        //  These are the advance rules
        if (cross > 0)
            change = sign1 > 0 ? 1 : 2;
        else
            change = sign2 > 0 ? 2 : 1;

        //  Special rule for colinear edges
        if (!sign1 && !sign2)
            change = inside==1 ? 2 : 1;

        if (change==1)
        {
            s1 = e1;
            e1 = NextModN(e1, numpoly1);
            if (inside==change &&
                McdAddPoint(numOut, polyOut, poly1[s1][x], poly1[s1][y], x, y))
                break;
        }
        else
        {
            s2 = e2;
            e2 = NextModN(e2, numpoly2);
            if (inside==change &&
                McdAddPoint(numOut, polyOut, poly2[s2][x], poly2[s2][y], x, y))
                break;
        }
        ps1 = sign1;
        ps2 = sign2;
        neg1 |= sign1 < 0;
        neg2 |= sign2 < 0;

        //  If we've gone all the way around either poly then break.
        //  Look at the neg1,2 flag to see if either is completely inside the other.

        if (!*numOut && i > numboth)
        {
            if (!neg2)
                memcpy(polyOut, poly1, (*numOut = numpoly1)*sizeof *polyOut);
            else if (!neg1)
                memcpy(polyOut, poly2, (*numOut = numpoly2)*sizeof *polyOut);
            break;
        }
    }

    //  Compute the missing coordinate for the polyOut by projecting 
    //  back onto the contact plane.

    for (i = 0; i < *numOut; ++i)
        polyOut[i][axis] = (dist - normal[x]*polyOut[i][x] - normal[y]*polyOut[i][y]) / normal[axis];

}
