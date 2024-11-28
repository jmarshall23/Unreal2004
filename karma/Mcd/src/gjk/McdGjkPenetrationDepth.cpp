/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.6.2.7 $

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

#include <McdProfile.h>

#include <MeMath.h>
#include <McdCTypes.h>
#include <McdGeometryTypes.h>
#include <McdGeometryInstance.h>
#include <McdGjk.h>
#include <McdGjkMaximumPoint.h>

/****************************************************************************
  Karma Penetration Depth Computation

  author: John Henckel
  date: 7 Dec 2001

  This is a working document of the design of the penetration depth computation
  in Karma. Good dynamics relies on good contact generation.  The GJK algorithm
  can do collision detection between arbitrary convex shapes.  However, the
  output of GJK does not give contacts. Karma supports several convex
  geometries:  sphere, cylinder, sphyl, plane, box, and convex-mesh; (cone is
  currently on death row). 

  For contact generation we prefer not to have special algorithms for every
  type-pair.  You may observe that although two convex objects may have multiple 
  contact points, all the contact points must be co-planar and all points have 
  the same normal, which is perpendicular to the plane. Our strategy is :

  1. run GJK to test for collision
  2. run CSO inflation to find the minimum penetration depth
  3. compute the contact plane, which is the perpendicular bisector of the 
     minimum penetration depth vector.
  4. get the intersection polygons of each shape with the contact plane
  5. generate contacts representing the perimeter of the polygon intersection

  Not all these steps are required in every case, such as...  

  · In GJK, if two shapes do not overlap, (separation > -e) we stop.  
  · In GJK if two shapes overlap in "fatness" only, only one contact point 
    is required.  We can use GJK to compute the minimum distance between the 
    non-fat shapes and use this for the contact location and normal.  
    P.S.  Actually this is not true.  A fat shape may have a flat face so more
    than one contact may be needed.

  CSO Inflation
  -------------

  The paper "Proximity Queries and Penetration Depth Computation on 3D Game
  Objects" by Gino van den Bergen describes an algorithm to take the output 
  of GJK and compute the minimum penetration depth.

  Given two convex shapes A and B, the CSO (configuration space obstacle) is 
  the shape A+(-B).  In other words, the CSO is the convex hull of the set of 
  vectors (u-v) where u is a point in A and v is a point in B.

  The support mapping called s[A](v) is a function that returns the farthest 
  point on A in the direction of vector v.  
  Notice that s[A+B](v) = s[A](v) + s[B](v). 
  Try to visualize what this means so that you can fully understand its 
  importance.  Also notice that s[-B](-v) = -s[B](v).  
  Therefore, s[A+(-B)](-v) = s[A](-v) - s[B](v).  This is the support formula 
  used in GJK and in CSO inflation.   Given a unit vector, v, 
  let w = s[A](-v) - s[B](v), then w*v is the signed distance from B 
  to A in direction v.

  GJK searches in the direction s[A-B](-v) so that the simplex is driven 
  towards the origin of A-B.  On the other hand, CSO inflation starts 
  with a simplex that contains the origin (the output of GJK) and tries 
  to inflate it in the direction s[A-B](v).  Since it always inflates the 
  nearest face first, the first point it finds on the hull of A-B must be 
  the nearest to the origin.

  Gino's inflation algorithm is as follows.

  1. get simplex output of GJK
  2. handle degenerate input
    a) if simplex has 1 or 2 points, the nearest point to the origin is returned
    b) if simplex has 3 points (not co-linear), use the cross product (or its 
       negative) to look for a fourth point.  If can't find fourth point, the 
       nearest point to the origin is returned
  3. create priority queue and enqueue the four faces of the simplex
  4. begin loop
  5. dequeue the nearest face, with normal v.
  6. let w = s[A-B](v), error = v*(w - v)/|v|.  if error < epsilon, stop
  7. split the face into 6 triangles using w (or fewer if any edge is part of A-B hull).
  8. for each triangle
    a) compute the normal vector from the origin to the triangle.
    b) if normal vector does not exist, discard triangle
    c) else enqueue the triangle
  9. end loop

  Karma implements pretty much this same algorithm, but I have changed steps 
  7 and 8.  Always splitting into 6 triangles is inefficient.  It produces more 
  triangles than necessary and, in most cases, the convergence rate will be slower.  
  It is better to split into 3 triangles at w, and, if w is near the center of the 
  triangle, that's all you need. This generates fewer triangles and better 
  convergence, but one drawback is that it tends to make more elongated triangles. 

  The only time you need to split an edge is when w is near it.  Thus if any new 
  triangle is very slanted such that it does not have a perpendicular, then we add 
  that triangle anyway using the edge normal vector.  These "slanted" triangles 
  are a little different than the regular ones because when we split them we only 
  want to keep two pieces, not three.  The triangle piece along the near edge is 
  redundant. 

  This is the revised algorithm.

  7. split the face into 3 triangles using w, (or 2 if the face is slanted).
  8. for each triangle
    a) compute the normal vector from the origin to the triangle.
    b) if vector does not exist, set the "slanted" triangle flag, compute normal 
       vector from the origin to the nearest edge
    c) enqueue the triangle
*/

/****************************************************************************
  This copies one vector from the simplex, at index i, to the face queue.

  TODO:  change the memoryAPI stuff to a pool.
*/
void McdGjkFaceAddPoint(McdGjkFaceQueue *q, int i)
{
    MEASSERT(q->nump < q->maxp);
    q->point[q->nump] = q->s->point[i];
    q->si[i] = q->nump;
    ++q->nump;
}

/****************************************************************************
  This compares the v_len of two faces.  If f1 < f2 return 1, else 0.
*/
int McdGjkFaceCompare(const void* f1, const void* f2)
{
    return ((McdGjkFace*)f1)->v_len < ((McdGjkFace*)f2)->v_len;
}

/****************************************************************************
  This puts one face into the face queue.  The shortest vector from the 
  origin to the face is v.  The length of v is computed.  The bits is a 
  three bit number indicating which vectors in the simplex represent the 
  corners of the face.   NOTE!!  This assumes that the 3 face points are
  recently added and are not stale.
*/
void McdGjkFaceAdd(McdGjkFaceQueue *q, const MeVector3 v, 
                   int bits, int slant, int depth)
{
    MEASSERT(THREE_BIT(bits));

    McdGjkFace *f = (McdGjkFace *) MePoolxGet(&q->fpool);

    if (!f)
        return;

    //  Add the new face

    f->bits = bits;
    f->depth = depth;
    f->slant = slant;
    MeVector3Copy(f->v, v);
    f->v_len = MeVector3Normalize(f->v);
    memcpy(f->fi, q->si, sizeof f->fi);

    MeHeapPush(&q->face, f);
}


/****************************************************************************
  This gets the face with the smallest vector length (nearest to the origin)
  and returns a pointer to it.  The lastpop is set to this face.
  Return 0 if there aren't any more faces.
*/
McdGjkFace *McdGjkFacePop(McdGjkFaceQueue *q)
{
    MEASSERT(q);
    McdGjkFace *f = (McdGjkFace *) MeHeapPop(&q->face);
    q->lastpop = f;
    return f;
}

/****************************************************************************
  This makes sure the three points of a face are loaded in the simplex.
  If any points are not, this recalculates the dot cache and delta cache.
  This is called usually right after face pop.
*/
void McdGjkFaceLoad(McdGjkFaceQueue *q, McdGjkFace *f)
{
    int i,b, stale_bits = 0;

    FOR_EACH_BIT(i, b, f->bits)
        if (f->fi[i] != q->si[i])     // check if the vertex is not in simplex
            stale_bits |= b;

    q->s->bits = f->bits & ~stale_bits;  // turn off all the stale bits

    FOR_EACH_BIT(i, b, stale_bits)
    {
        q->s->next_i = i;
        q->s->next_bit = b;
        q->s->point[i] = q->point[f->fi[i]];
        q->si[i] = f->fi[i];
        McdGjkUpdateDotCache(q->s);
        McdGjkUpdateDeltaCache(q->s);
        q->s->bits |= b;
    }
    MEASSERT(f->bits == q->s->bits);
}


/****************************************************************************
  This finds the next point to add to the simplex by inflating the 
  current face.
  Return the dot product v.w to the new point w which is stored
  in simplex[next_i].
*/
MeReal McdGjkFindNextInflatePoint(MeVector3 v, MeReal v_len, 
                                  McdCache *c, McdGjkSimplex *s)
{
    MeReal d1, d2;
    McdGjkPoint p1;
    McdGjkPoint *p = McdGjkNextAvailablePoint(s);

    //  When v_len is small, the direction of v is highly contaminated
    //  with numerical errors.  I have not found a good way to deal with
    //  this problem.  The "adjust perpendicular" function uses the cross
    //  product and some other things.

    if (v_len < 10 * s->eps)
        McdGjkCrossProd(v, s);

//    McdGjkAdjustPerpendicular(v, v_len, s);

    d1 = McdGjkComputeSupport(p, v, 1, c);

    //  If v_len is small, then v is suspect.  Try to query -v
    //  and see if we get a much smaller distance.

    if (v_len < 10 * s->eps)
    {
        MeVector3 neg = { -v[0], -v[1], -v[2] };
        p1 = *p;
        d2 = McdGjkComputeSupport(p, neg, 1, c);
        if (d2 < d1 - 10 * s->eps)
        {
            d1 = d2;
            MeVector3Copy(v, neg);    // negate the vector
        }
        else
            *p = p1;    // the original vector is better
    }
    return d1;
}

/****************************************************************************
  This creates the face queue, adding the simplex triangles.
*/
void McdGjkFaceQueueInit(McdGjkFaceQueue *q, McdGjkSimplex *s)
{
    MEASSERT(s->bits == 15);
    q->s = s;
    q->lastpop = 0;

    McdGjkFaceAddPoint(q, 0);
    McdGjkFaceAddPoint(q, 1);
    McdGjkFaceAddPoint(q, 2);
    McdGjkFaceAddPoint(q, 3);

    //  Add each 3-bit subset (face) in simplex to the face queue
    int i, b;
    MeVector3 v;

    for (i = 0; i < 4; ++i)
    {
        b = McdGjkBinarySubset[15][i+1];
        if (McdGjkComputeVector(v, b, 0, s))
            McdGjkFaceAdd(q, v, b, 0, 0);
        else 
        {
            if (b & 1 && McdGjkComputeVector(v, b-1, 0, s))
                McdGjkFaceAdd(q, v, b, 1, 0);
            if (b & 2 && McdGjkComputeVector(v, b-2, 0, s))
                McdGjkFaceAdd(q, v, b, 2, 0);
            if (b & 4 && McdGjkComputeVector(v, b-4, 0, s))
                McdGjkFaceAdd(q, v, b, 4, 0);
            if (b & 8 && McdGjkComputeVector(v, b-8, 0, s))
                McdGjkFaceAdd(q, v, b, 8, 0);
        }
    }
}

#if MCD_GJK_DEBUG
/****************************************************************************
  This is used for single step debugging.  The q data is allocated with
  malloc and preserved in the s.
*/
McdGjkFaceQueue *McdGjkFaceQueueDebugInit(McdGjkSimplex *s)
{
    if (s->q)
        return (McdGjkFaceQueue *) s->q;

    McdGjkPoint *ptmem = (McdGjkPoint *) MeMemoryAPI.create(MCDGJK_PDMAX * sizeof *ptmem);
    McdGjkFace *poolmem = (McdGjkFace *) MeMemoryAPI.create(MCDGJK_PDMAX * sizeof *poolmem);
    void **heapmem = (void**) MeMemoryAPI.create(MCDGJK_PDMAX * sizeof *heapmem);
    McdGjkFaceQueue *q = (McdGjkFaceQueue *) MeMemoryAPI.create(sizeof *q);

    MePoolxInit(&q->fpool, poolmem, sizeof *poolmem, MCDGJK_PDMAX);
    MeHeapInit(&q->face, heapmem, MCDGJK_PDMAX, McdGjkFaceCompare);

    q->maxp = MCDGJK_PDMAX;
    q->nump = 1;              // the zero-th point is reserved
    q->point = ptmem;

    McdGjkFaceQueueInit(q, s);

    return q;
}
#endif // MCD_GJK_DEBUG

/****************************************************************************
  This verifies that the answer returned by GJK pen depth is reasonable.
*/
int McdGjkResultIsReasonable(McdCache *c, McdGjkSimplex *s)
{

#if MCD_GJK_DEBUG
    MeReal len = MeVector3Magnitude(c->normal);

    if (len < 0.999f || len > 1.0001f)
    {
        MeInfo2(6,"len = %g",len);
        return 0;
    }
#endif
//    if (c->separation < -0.2f)
//        printf("sep = %g\n", c->separation);

    MeVector3 s1, s2, neg, v, p;
    MeReal d1, d2, dp;

    //  Find the support v in the normal direction and
    //  the vector p between the centers.
    
    MeVector3MultiplyScalar(neg, c->normal, -1);
    McdGjkMaximumPoint(c->ins1, neg, s1);
    McdGjkMaximumPoint(c->ins2, c->normal, s2);

    MeVector3Subtract(v, s1, s2);
    d1 = MeVector3Dot(v, c->normal);
    d2 = MeVector3Dot(c->normal, c->offset);

    if (MeFabs((d1+d2)-c->separation) > 0.1f)
    {
        //MeInfo2(6,"d1 = %g, d2 = %g, sum = %g, sep = %g",d1,d2,d1+d2,c->separation);
        return 0;
    }
    MeVector3Subtract(p, c->ins1->mTM[3], c->ins2->mTM[3]);
    MeVector3Normalize(p);

    //  The dot of p and v should be at least .2

    dp = MeVector3Dot(c->normal, p);

    if (dp < 0.1f)
    {
        //MeInfo2(6,"dot = %g",dp);
        return 0;
    }
    //  see if the vector p is a better normal than the normal

    MeVector3MultiplyScalar(neg, p, -1);
    McdGjkMaximumPoint(c->ins1, neg, s1);
    McdGjkMaximumPoint(c->ins2, p, s2);

    MeVector3Subtract(v, s1, s2);
    d2 = MeVector3Dot(v, p);

    if (d1 < d2 - 0.01f)
    {
        //MeInfo2(6,"dist_normal = %g, dist_center = %g",d1,d2);
        return 0;
    }
    return 1;          // SUCCESS -- normal looks reasonable
}

#if MCD_GJK_DEBUG
/****************************************************************************
  This writes data for two boxes to a file.  It writes two lines each
  has size of box x,y,z, position x,y,z, and quaternion w,x,y,z.  
*/
void write_binfile(char*fn, McdCache *c)
        //GeometryInstance *ins1, McdGeometryInstance *ins2)
{
    int t1 = McdGeometryGetTypeId(c->ins1->mGeometry);
    int t2 = McdGeometryGetTypeId(c->ins2->mGeometry);
        
    if (t1!=kMcdGeometryTypeBox && t1!=kMcdGeometryTypeConvexMesh ||
        t2!=kMcdGeometryTypeBox && t2!=kMcdGeometryTypeConvexMesh)
    {
        printf("cannot write geometry to binary file\n");
        return;
    }
    FILE *f = fopen(fn,"wb");
    if (!f || ferror(f))
    {
        printf("unable to open %s\n",fn);
        return;
    }

    MeVector3Ptr r;
    MeVector3Ptr v;
    MeVector4 q;

    r = (t1==kMcdGeometryTypeBox) ? 
        ((McdBox*) c->ins1->mGeometry)->mR :
        ((McdConvexMesh*) c->ins1->mGeometry)->mHull.vertex[5].position;

    MeQuaternionFromTM(q,c->ins1->mTM);
    v = c->ins1->mTM[3];
    fwrite(r,sizeof(MeReal),3,f);
    fwrite(v,sizeof(MeReal),3,f);
    fwrite(q,sizeof(MeReal),4,f);

    r = (t2==kMcdGeometryTypeBox) ? 
        ((McdBox*) c->ins2->mGeometry)->mR :
        ((McdConvexMesh*) c->ins2->mGeometry)->mHull.vertex[5].position;

    MeQuaternionFromTM(q,c->ins2->mTM);
    v = c->ins2->mTM[3];
    fwrite(r,sizeof(MeReal),3,f);
    fwrite(v,sizeof(MeReal),3,f);
    fwrite(q,sizeof(MeReal),4,f);

    fwrite(c->offset,sizeof(MeReal),3,f);

    fclose(f);
    printf("wrote binary position data into %s\n",fn);
}
#endif // MCD_GJK_DEBUG

/****************************************************************************
  This uses the result of GJK to compute the minimum penetration depth 
  between the two models.  The penetration depth is a vector between two
  points, one on each model.

  This algorithm is based on the excellent paper by Gino van den Bergen
  "Proximity Queries and Penetration Depth Computation on 3D Game Objects"
  see http://www.win.tue.nl/~gino/solid/ 

  When GJK determines that two convex objects are overlapping, it usually 
  produces a full 4-point simplex.  However, in some special cases, the
  simplex can have fewer.  

  When two spheres overlap, the simplex will have only one point.  
  When a sphere and sphyl overlap, the simplex will have only two points.
  Also, in case the origin of the A-B space is on the boundary of the 
  simplex (e.g. two corners are exactly touching), the simplex may have
  less than four points (but never zero).

  If the simplex size is 1, return the point.  If size is 2, return the 
  closest of the two points.  The contact location and penetration depth is 
  calculated based on fatness of each model.  If simplex size is 3, use the 
  cross product to get another point, and then continue as with 4.

  With a full 4-point simplex, find the distance to the four faces and 
  add them to the priority queue.  We then begin iteration:  take the best
  face off the queue, split it into three faces.  We only cache three
  triangles at a time because thats all that can fit in the simplex.  The
  simplex holds 4 points and all the delta and dot cache between them
  (except delta[15] is not computed for efficiency).

  At each iteration of the search we get the face nearest to the origin.
  We then remove all points from the simplex not on this face.  Then, if
  any of the three points of this face are not in the simplex, we add them.
  The should leave one empty slot, which we use to split the face into 
  three new faces.

  The face.fi is the indices of the face points, and queue.si is the 
  indices of the points in the simplex.  By comparing fi[i] to si[i] we 
  can determine which face points are already in the simplex, and which 
  need to be added.

  The best distance should always increase.  If it doesn't, we stop. 

  NOTE:  This function uses the simplex to do the "expanding polytope algorithm"
  to find the point on the configuration space obstacle (CSO) nearest to the
  origin. As a result the simplex gets messed up, so don't call this function
  again unless you re-run GJK.

  The result penetration vector is returned in the cache
*/
int McdGjkPenetrationDepth(McdCache *c, McdGjkSimplex *s)
{
    McdProfileStart("McdGjkPenetrationDepth");

    s->inflate = 1;       // flag begin inflation (update delta uses this)

    //----------------------------------------
    //  Handle the degenerate cases
    //
    //  NOTE!  Handling of degenerate simplex input turns out
    //  to be very difficult (impossible?)
    //  Therefore just return error and let the McdGjkTest
    //  function try again.

    if (s->bits != 15)
        return 0;

    //----------------------------------------
    //  Allocate large chunks of stack memory

    McdGjkPoint ptmem[MCDGJK_PDMAX];
    McdGjkFace poolmem[MCDGJK_PDMAX];
    void *heapmem[MCDGJK_PDMAX];
    McdGjkFaceQueue qmem;

    //  Declare variables for CSO inflation search

    int i, b, slant, is_dup, depth, ok;
    MeVector3 v;
    MeReal d, min_d = MEINFINITY; 
    McdGjkFace *f, *min_f = 0;
    McdGjkFaceQueue *q = &qmem;

    MePoolxInit(&q->fpool, poolmem, sizeof *poolmem, MCDGJK_PDMAX);
    MeHeapInit(&q->face, heapmem, MCDGJK_PDMAX, McdGjkFaceCompare);

    q->maxp = MCDGJK_PDMAX;
    q->nump = 1;              // the zero-th point is reserved
    q->point = ptmem;

#if MCD_GJK_DEBUG
    if (s->debug) 
    {
        q = McdGjkFaceQueueDebugInit(s);
        if (s->debug > 1) {
            min_d = c->location[0];
            min_f = *(McdGjkFace **)&c->location[1];
        }
    }
    else
#endif

    McdGjkFaceQueueInit(q, s);

    //----------------------------------------
    //  Begin the CSO inflation search

    while (1)
    {
        f = McdGjkFacePop(q);

        //  Check things that should never happen

        ok = 0;
        if (!f)
            break;  
        if (q->nump >= MCDGJK_PDMAX) 
            break;
        ok = 1;

        if (f->v_len > min_d - s->eps && !f->slant)
            break;
        
        //  Ensure the three face points are loaded in the simplex and
        //  recompute the dot and delta cache if necessary.

        McdGjkFaceLoad(q, f);

        //  Compute a new vector to inflate the face

        d = McdGjkFindNextInflatePoint(f->v, f->v_len, c, s);   
        McdGjkFaceAddPoint(q, s->next_i);

        is_dup = McdGjkNextIsDuplicate(s);
        slant = f->slant;
        depth = f->depth + 1;

        if (!slant && d < min_d)
        {
            if (min_f)
                MePoolxPut(&q->fpool, min_f);

            min_d = d;
            min_f = f;

            //  LOOP EXIT CONDITION

            if (f->v_len > min_d - s->eps)
                break;
        }
        else
            MePoolxPut(&q->fpool, f);

        //  Update the dot and delta for the next point, w

        McdGjkUpdateDotCache(s);
        McdGjkUpdateDeltaCache(s);
        s->bits = 15;

        //  Now split a face into 2 or 3 more faces.  The depth is a heuristic
        //  to avoid getting stuck in a useless loop.  For example, when the CSO 
        //  origin is on a simplex edge, there is a tendency for the search to 
        //  inflate around and around the edge forever.

        if (depth < 10 && !is_dup)
        {
            //  Add up to 3 more faces

            for (i = 0; i < 4; ++i)
            {
                b = McdGjkBinarySubset[15][i+1];     // b is 7,11,13,14
                if (b & s->next_bit && (!slant || b & slant))
                {
                    if (McdGjkComputeVector(v, b, 0, s))
                        McdGjkFaceAdd(q, v, b, 0, depth);
                    else if (!slant && McdGjkComputeVector(v, b - s->next_bit, 0, s))
                        McdGjkFaceAdd(q, v, b, s->next_bit, depth);
                }
            }
        }


#if MCD_GJK_DEBUG
        // single-step debugging mode
        if (s->debug)         
        {
            ++s->debug;
            s->q = q;
            c->normal[0] = q->lastpop->v_len;
            c->normal[1] = (MeReal) q->face.used;
            c->normal[2] = (MeReal) q->nump;
            c->location[0] = min_d;
            *(McdGjkFace **)&c->location[1] = min_f;
            return 1;
        }
#endif
    }

    //  Upon leaving the while loop, we have three points in the simplex
    //  representing the face, f, of the CSO nearest to the origin.

    if (min_f && ok)
    {
        f = min_f;
        MeVector3MultiplyScalar(c->normal, f->v, -1);
        c->separation = -min_d - (c->fat1 + c->fat2);
        McdGjkFaceLoad(q, f);
        ok = McdGjkComputeVector(c->location, f->bits, 1, s);
        MeVector3MultiplyAdd(c->location, c->fat2 + c->separation/2, c->normal);
        if (ok)
          ok = McdGjkResultIsReasonable(c, s); 
    }

#if MCD_GJK_DEBUG

    //  see if the result is good
    //  and if not dump the shape data to a binary file.
    
    if (!ok || !min_f) 
    {
//        write_binfile("\\temp\\2box.bin",c);
        MeInfo2(6,"********* Penetration Depth failed ***************");
    }
    if (s->debug) {    // this supports single step debugging
        s->debug = 1;
        s->q = 0;
    }
#endif

    McdProfileEnd("McdGjkPenetrationDepth");

    return ok;
}
