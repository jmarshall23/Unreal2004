/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC
 
   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/11/18 17:13:05 $ - Revision: $Revision: 1.11.2.8.4.1 $

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

#include <McdCTypes.h>
#include <McdGeometryTypes.h>

#include <McdGjk.h>
#include <McdGjkMaximumPoint.h>
#include <McdPlaneIntersect.h>
#include <McdPolygonIntersection.h>

/****************************************************************************
  These two arrays are used to efficiently traverse a 4-D binary hypercube.
  There are 16 zero-terminated lists (i = 0..15) and each list contains
  the list of binary numbers that are subsets of i.  The subsets are sorted 
  by number of bits, large to small.  The first item in each list is i.
*/
static const int McdGjk4Di[] = { 
    0, 0, 
    1, 0, 
    2, 0, 
    3, 1, 2, 0, 
    4, 0, 
    5, 1, 4, 0, 
    6, 2, 4, 0, 
    7, 3, 5, 6, 1, 2, 4, 0, 
    8, 0, 
    9, 1, 8, 0, 
    10, 2, 8, 0, 
    11, 3, 9, 10, 1, 2, 8, 0, 
    12, 4, 8, 0, 
    13, 5, 9, 12, 1, 4, 8, 0,
    14, 6, 10, 12, 2, 4, 8, 0, 
    15, 7, 11, 13, 14, 3, 5, 6, 9, 10, 12, 1, 2, 4, 8, 0
};

const int *McdGjkBinarySubset[16] = {
    McdGjk4Di, McdGjk4Di+2, McdGjk4Di+4, McdGjk4Di+6,
    McdGjk4Di+10, McdGjk4Di+12, McdGjk4Di+16,
    McdGjk4Di+20, McdGjk4Di+28, McdGjk4Di+30,
    McdGjk4Di+34, McdGjk4Di+38, McdGjk4Di+46,
    McdGjk4Di+50, McdGjk4Di+58, McdGjk4Di+66 
};

/****************************************************************************
  This returns a random unit vector.  The distibution is not perfectly 
  uniform but it is good enough.
*/
void McdGjkRandomVector(MeVector3 v)
{
    v[0] = MeRealRandomInRange(-1, 1);
    v[1] = MeRealRandomInRange(-1, 1);
    v[2] = MeRealRandomInRange(-1, 1);
    MeVector3Normalize(v);
}


/****************************************************************************
  This initializes the cache.  The cache is a convenient struct used to 
  store temp information that is used throughout the collision detection 
  and contact generation process.
*/
void McdCacheInit(McdCache *c, McdModelPair *p)
{
    MEASSERT(c);

    //  First time initialisation of the cache.

    c->padding = p->model1->mPadding + p->model2->mPadding;
    c->ins1 = &p->model1->mInstance;
    c->ins2 = &p->model2->mInstance;
    c->fat1 = McdGjkFatness(c->ins1);
    c->fat2 = McdGjkFatness(c->ins2);
    c->separation = 1;
    MeVector3Set(c->normal, 0, 1, 0);
    MeVector3Set(c->location, 0, 0, 0);
}

/****************************************************************************
  Hello function to allocate the cache

  NOTE!  This uses a fixed size pool.  If the pool is empty, then
  this will set m_cacheData to null.  Of course, the GJK function must
  handle null appropriately.
*/
MeBool MEAPI McdCacheHello(McdModelPair*p)
{
    MEASSERT(p && p->model1 && p->model1->frame);

    MePool *pool = & p->model1->frame->cachePool;

    if (pool->t == MePoolNULL)          // First time only
        MePoolFixedAPI.init(pool, 100, sizeof(McdCache), MeALIGNTO);
    
    MEASSERT(pool->t == MePoolFIXED);

    p->m_cachedData = MePoolFixedAPI.getStruct(pool);

    if (p->m_cachedData)
    {
        McdCache *c = (McdCache *) p->m_cachedData;
        McdCacheInit(c, p);
    }
    return 1;
}

/****************************************************************************
  Goodbye function to deallocate the cache
*/
void MEAPI McdCacheGoodbye(McdModelPair*p)
{
    MePool *pool = & p->model1->frame->cachePool;

    MEASSERT(pool->t == MePoolFIXED);

    if (p->m_cachedData)
        MePoolFixedAPI.putStruct(pool, p->m_cachedData);

    p->m_cachedData = 0;       // just to be safe
}

/****************************************************************************
  This returns true if the vector in next_i is already in the simplex.
*/
int McdGjkNextIsDuplicate(McdGjkSimplex *s)
{
    int i, b;
    if (!s->inflate && s->next_bit < s->bits && 
         ( ME_ARE_EQUAL_3VEC(s->point[s->next_i].w, s->lastw[0])
        || ME_ARE_EQUAL_3VEC(s->point[s->next_i].w, s->lastw[1])))
        return 1;
    FOR_EACH_BIT(i, b, s->bits)
        if (ME_ARE_EQUAL_3VEC(s->point[s->next_i].w, s->point[i].w))
            return 1;
    return 0;
}

/****************************************************************************
  This computes the lambdas using Cramers rule and multiplies them by
  a set of basis vectors.  
  When sup==0 this returns the shortest vector that connects the origin to 
  the simplex.  When sup!=0 this returns the corresponding location on 
  geometry instance 2 (i.e. the point on ins2 nearest ins1, that we know of).

  This assumes that the bits and delta cache are already computed.
  This returns 1 on success, and 0 on failure.  If zero is returned,
  it means one of the delta values is negative which means that the feature 
  described by "bits" does not contain any point perpendicular to the origin.
  Even when zero is returned, I think v will still be set to a reasonable
  value, i.e. the shortest vector from the origin to the subspace of the feature.

  I have added an attempt at numeric tolerance.  Sometimes the delta values
  differ by more than tolerance order of magnitude.  e.g. one is 2500 and 
  another is -.001.  Because the larger is more than 1e6 times the smaller
  then the smaller is virtually zero and should not cause failure.
*/
int McdGjkComputeVector(MeVector3 v, int bits, int sup, McdGjkSimplex *s)
{
    MeReal d, det = 0;
    int i, b;
    MeReal hi = 0, lo = 0;

    MeVector3Set(v, 0, 0, 0);

    FOR_EACH_BIT(i, b, bits)
    {
        d = s->delta[bits][i];
        if (d < lo) lo = d;
        if (d > hi) hi = d;
        det += d;
        MeVector3MultiplyAdd(v, d, sup ? s->point[i].s2 : s->point[i].w);
    }
    if (det != 0)
        MeVector3Scale(v, 1 / det);

#if 0
    if (!s->inflate && hi<=0) //  when all delta are zero or negative
        return 0;
#endif

    return !(lo < -hi*s->eps);
}

/****************************************************************************
  This is the same as McdGjkComputeVector but the next_bit is also included
  in the calculation.  This allows us to make some performance improvements
  for the case when bits is zero.
*/
int McdGjkComputeNextVector(MeVector3 v, int bits, int sup, McdGjkSimplex *s)
{
    if (!bits)
    {
        if (sup) 
            MeVector3Copy(v, s->point[s->next_i].s2);
        else
            MeVector3Copy(v, s->point[s->next_i].w);
        return 1;
    }
    return McdGjkComputeVector(v, bits | s->next_bit, sup, s);
}

/****************************************************************************
  This looks in the simplex for the next available empty slot.
*/
McdGjkPoint *McdGjkNextAvailablePoint(McdGjkSimplex *s)
{
    for (s->next_i = 0, s->next_bit = 1; s->next_bit & s->bits; s->next_i++)
        s->next_bit <<= 1;

    MEASSERT(s->next_i < 4);

    return s->point + s->next_i;
}

/****************************************************************************
  This computes a vector perpendicular to the triangle in the simplex.
  The simplex must have exactly three points.
  This function is called in cases where the triangle is very near to
  the origin and thus the usual function McdGjkComputeVector is not
  reliable.  

  This returns a unit vector.  As a heuristic, the direction is chosen to 
  be away from the fourth (unused) point of the simplex.

  If a very small number is returned, the result is questionable.
*/

MeReal McdGjkCrossProd(MeVector3 v, McdGjkSimplex *s)
{
    MEASSERT(THREE_BIT(s->bits));

    //  Get p0,p1,p2 points of the triangle, and p3 is the other one.

    McdGjkPoint *p0, *p1, *p2, *p3;
    int i = 0;
    if (s->bits==14) p3 = s->point + (i++);
    p0 = s->point + (i++);
    if (s->bits==13) p3 = s->point + (i++);
    p1 = s->point + (i++);
    if (s->bits==11) p3 = s->point + (i++);
    p2 = s->point + (i++);
    if (s->bits==7) p3 = s->point + (i++);
    MEASSERT(i==4);

    //  Compute cross product

    MeVector3 v1, v2;
    MeVector3Subtract(v1, p0->w, p1->w);
    MeVector3Subtract(v2, p2->w, p1->w);
    MeVector3Cross(v, v1, v2);

    MeReal len = MeVector3Magnitude(v);

    if (len == 0)
    {
        //  If the length of the cross prod is zero...
        MeVector3MakeOrthogonal(v, v1);
    }
    else
    {
        //  Now to determine which way v should point, we employ a heuristic
        //  that the cross prod should point in the opposite direction of
        //  the edge to the extra point p3, i.e. toward outside of the simplex.

        MeVector3Subtract(v1, p3->w, p1->w);
        if (MeVector3Dot(v, v1) > 0)
            MeVector3Scale(v, -1 / len);
        else
            MeVector3Scale(v, 1 / len);
    }
    return len;
}


/****************************************************************************
  This is a magic function.  It removes all the numerical problems from
  the vector v.
  Basically if the v_len is small, then v cannot be trusted and v is 
  recomputed using the geometrical shape of the simplex.  E.g. if the 
  simplex is a triangle, the v is recomputed using the cross product.
*/
void McdGjkAdjustPerpendicular(MeVector3 v, MeReal v_len,  McdGjkSimplex *s)
{
    if (v_len > s->eps)
        return;                // RETURN -- v is trustworthy

    if (!s->bits)
        return;            // RETURN -- any vector will do

    MeVector3 v2;
    MeReal len;

    if (THREE_BIT(s->bits))
    {
        len = McdGjkCrossProd(v2, s);
        if (len > ME_SMALL_EPSILON)
        {
            //  Test is v2 is pointing the wrong way, if so reverse it

            if (MeVector3Dot(v, v2) < 0)
                MeVector3MultiplyScalar(v, v2, -1 / len);
            else
                MeVector3MultiplyScalar(v, v2, 1 / len);

            return;                   // RETURN -- cross prod
        }
        //  If the len is very small, i.e the three points are nearly
        //  colinear, then handle like 1 or 2 bits case.
    }

    //  Find the longest w vector and return a vector orthogonal to it.
    int i, b, k = 0;
    MeReal t, d = 0;
    FOR_EACH_BIT(i, b, s->bits)
    {
        t = MeVector3MagnitudeSqr(s->point[i].w);
        if (t > d) { d = t; k = i; }
    }

    MeVector3MakeOrthogonal(v, s->point[k].w);
}

/****************************************************************************
    This computes the support point in direction v.  
    The sign can be +1 or -1.
*/
MeReal McdGjkComputeSupport(McdGjkPoint *p, const MeVector3 v, 
                            int sign, McdCache *c)
{
    MeVector3 neg_v = { -v[0], -v[1], -v[2] };
    MeVector3 s1;

    MEASSERT(sign*sign == 1);

    if (sign > 0)
    {
        McdGjkMaximumPoint(c->ins1, v, s1);
        McdGjkMaximumPoint(c->ins2, neg_v, p->s2);
    }
    else
    {
        McdGjkMaximumPoint(c->ins1, neg_v, s1);
        McdGjkMaximumPoint(c->ins2, v, p->s2);
    }
    MeVector3Subtract(p->w, s1, p->s2);
    MeVector3Add(p->w, p->w, c->offset);

    return MeVector3Dot(p->w, v);
}

/****************************************************************************
    This finds the next support point to add to the simplex.
    Return the dot product v.w to the new point w which is stored
    in simplex[next_i].
*/
MeReal McdGjkFindNextSupportPoint(MeVector3 v, MeReal v_len, 
                                  McdCache *c, McdGjkSimplex *s, int i)
{
    McdGjkPoint *p = McdGjkNextAvailablePoint(s);

    MeVector3Copy(s->lastw[i & 1], p->w);

    McdGjkAdjustPerpendicular(v, v_len, s);

    MeReal d = McdGjkComputeSupport(p, v, -1, c);

    return d;
}


/****************************************************************************
  This updates the dot product cache after a new vector is added to the 
  next_i position.
*/
void McdGjkUpdateDotCache(McdGjkSimplex *s)
{
    int i, b;

    //  Update the dot product cache for every vector against the new one

    FOR_EACH_BIT(i, b, s->bits | s->next_bit)
        s->dot[s->next_i][i] = 
        s->dot[i][s->next_i] = MeVector3Dot(s->point[i].w, s->point[s->next_i].w);
}

/****************************************************************************
  This updates the determinants of all the delta matrices after a new
  vector is added to the next_i position.
*/
void McdGjkUpdateDeltaCache(McdGjkSimplex *s)
{
    int i,j,k,bi,bj,bk;        // indices and respective bit masks
    MeReal (*dot)[4] = s->dot;
    MeReal (*delta)[4] = s->delta;
    i = s->next_i;
    bi = s->next_bit;

    delta[bi][i] = 1;

    //  Update each pair (i,j)

    FOR_EACH_BIT(j, bj, s->bits)
    {
        delta[bi|bj][i] = dot[j][j] - dot[j][i];
        delta[bi|bj][j] = dot[i][i] - dot[i][j];

        //  Update each triplet (i,j,k) where j < k

        FOR_EACH_BIT(k, bk, s->bits)
        {
            if (bk >= bj)
                break;

            delta[bi|bj|bk][i] = delta[bj|bk][j] * (dot[j][k] - dot[j][i]) +
                                 delta[bj|bk][k] * (dot[k][k] - dot[k][i]);
            delta[bi|bj|bk][j] = delta[bk|bi][k] * (dot[k][k] - dot[k][j]) +
                                 delta[bk|bi][i] * (dot[i][k] - dot[i][j]);
            delta[bi|bj|bk][k] = delta[bi|bj][i] * (dot[i][j] - dot[i][k]) +
                                 delta[bi|bj][j] * (dot[j][j] - dot[j][k]);
        }
    }

    //  If i completes the full simplex, update the full 4 point delta

    if ((bi | s->bits) == 15 && !s->inflate)
    {
        delta[15][0] = delta[14][1] * (dot[1][1] - dot[1][0]) +
                       delta[14][2] * (dot[2][1] - dot[2][0]) +
                       delta[14][3] * (dot[3][1] - dot[3][0]);
        delta[15][1] = delta[13][0] * (dot[0][0] - dot[0][1]) +
                       delta[13][2] * (dot[2][0] - dot[2][1]) +
                       delta[13][3] * (dot[3][0] - dot[3][1]);
        delta[15][2] = delta[11][0] * (dot[0][0] - dot[0][2]) +
                       delta[11][1] * (dot[1][0] - dot[1][2]) +
                       delta[11][3] * (dot[3][0] - dot[3][2]);
        delta[15][3] = delta[ 7][0] * (dot[0][0] - dot[0][3]) +
                       delta[ 7][1] * (dot[1][0] - dot[1][3]) +
                       delta[ 7][2] * (dot[2][0] - dot[2][3]);
    }
}



/****************************************************************************
  GJK Collision detection
  --------------------------
  This function is the bare-bones GJK algorithm.  This does not include
  any pre-check optimisations.

  Because of fatness, there are three possible outcomes of this function.

  1.  If a separating axis is found between the geometries such that the
      distance is greater than the total fatness, this returns 0 immediately.
      The axis location, normal, and separation (positive) is returned
      in the cache.

  2.  If the simplex is found to contain the origin, this returns 1 
      immediately.  The normal and location are not set.

  3.  The third case is that the simplex does not contain the origin, but,
      because of fatness, the two geometries are in contact.  In this case, 
      the GJK algorithm is let run until it finds the minimum distance vector
      between the geometries.  Then this returns 0 and sets the location, 
      normal, and separation (non-positive) of the penetration vector in the 
      cache.

  The caller determines the outcome by looking at the return value and
  the separation.  Only in case 2 does the caller need to use CSO inflation
  to determine the penetration vector.
*/
int McdGjkMain(McdCache *c, McdGjkSimplex *s)
{
    int counter = 1; 
    int ok, is_dup = 0;
    int dup_counter = 0;
    MeReal d, maxd = ME_SMALL_EPSILON;

    //  Add the radii of both geometries

    MeReal fatness = c->fat1 + c->fat2;

    //  Note about choosing initial vector.  You might think (like I did) that
    //  it will be a very clever idea to use the normal direction from the previous
    //  collision test as the initial direction vector.  However, this is bad because
    //  in produces a chaotic system which is very stressful on the GJK and inflation
    //  code because it hits all the weak spots.  The code is much more reliable
    //  by choosing a constant starting v as below.

    MeVector3 v;
    MeReal v_len;

    MeVector3Set(v, .5f, .6f, .6245f);  // arbitrary unit vector
    v_len = 1;

#if MCD_GJK_DEBUG
    if (s->debug > 1)     //  re-entry single step debugging 
    {
        ok = McdGjkComputeVector(v, s->bits, 0, s);
        MEASSERT(ok);
        v_len = MeVector3Normalize(v);
        counter = s->debug;
        maxd = c->normal[0];
        dup_counter = (int) c->normal[1];
    }
    else

#endif

    s->inflate = s->bits = 0; 
    s->eps = c->ins1->mGeometry->frame->mScale * 0.0001f;

    while (v_len > s->eps)
    {
        //  Compute the support vector, w = S(-v) in the space (A-B)
        //  and add it to the simplex. 

        d = McdGjkFindNextSupportPoint(v, v_len, c, s, counter);

        c->separation = d - fatness;

        if (d > maxd)
            maxd = d;

        //  Test GJK isn't stuck in an infinite loop.
        //  This should never happen, so force assertion failure.

        if (++counter > MCDGJK_IMAX)
        {
            c->separation = 1;             // NO CONTACT

#if MCD_GJK_DEBUG
//            MEASSERT(counter < MCDGJK_IMAX);    // force failure
 //           write_binfile("\\temp\\2box.bin",c);
            MeInfo(4, "GJK loop counter is %d **** %g, %g, %g", counter,c->offset[0],c->offset[1],c->offset[2]);
#endif
            return 2;
        }

        //  If the separation is non-negative, return NO CONTACT.
        //  The test NextIsDuplicate prevents a rare situation when GJK
        //  can get stuck in a loop, oscillating between two w vectors.
        //  The test v_len<maxd is intensionally very strict.  Gino says
        //  one should use v_len<(maxd+epsilon) but this is bad because
        //  there are cases when it terminates prematurely.

        is_dup = McdGjkNextIsDuplicate(s);

        if (c->separation > c->padding 
            || v_len < maxd + s->eps
            || is_dup && v_len < 100 * s->eps + fatness)
        {
            MeVector3Copy(c->normal, v);
            MeVector3Normalize(c->normal);

            ok = McdGjkComputeVector(c->location, s->bits, 1, s);
//            MEASSERT(ok);    // TODO - why does this assert sometimes fail?

            MeVector3MultiplyAdd(c->location, c->fat2 + c->separation/2, c->normal);

            return d <= 0;      //  RETURN 
        }

        //  Rarely we get stuck in a loop of duplicate points when
        //  s->bits has less than three bits.  In that case, randomize v
        //  and restart GJK.

        if (is_dup)
            return 2;         /* return error */
/*
        {
            if (++dup_counter > 5)
            {
                s->bits = 0;            // RESTART GJK 
                maxd = ME_SMALL_EPSILON;
                dup_counter = 0;
            }
            McdGjkRandomVector(v);
            v_len = 1;
            continue;
        }
*/

        //  Update the dot product and delta determinants cache.

        McdGjkUpdateDotCache(s);
        McdGjkUpdateDeltaCache(s);

        //  This loop searches the simplex features (first faces, then edges)
        //  to find the one nearest to the origin.

        //  TODO!!  This loop is very inefficient because it looks at all
        //  simplex features and this is wasteful.  For example, if a
        //  the perpendicular point to a face is interior (i.e. all delta
        //  for the face are positive) then we do not need to check any 
        //  sub-feature of that face.  If any delta (or deltas) are zero
        //  then they indicate exactly which features need to be checked.

        int i, bits = 1;
        int v_bits = 0;
        MeReal u_len;
        MeVector3 u;

        for (i = 0; bits; ++i) 
        {
            bits = McdGjkBinarySubset[s->bits][i];

            //  I am not sure why, but when computing face-to-face distance between two
            //  fat shapes sometimes delta[15] will be non-negative and we get a
            //  bogus contact reported.  This test prevents bits==15 if maxd is large.

            if (!i && maxd > ME_SMALL_EPSILON && (bits | s->next_bit) == 15)
                continue;

            if (McdGjkComputeNextVector(u, bits, 0, s))
            {
                u_len = MeVector3Normalize(u);
                if (!v_bits || u_len <= v_len)
                {
                    v_bits = bits;
                    v_len = u_len;
                    MeVector3Copy(v, u);
                }
            }
        }

        s->bits = v_bits | s->next_bit;

        if (s->bits == 15)
        {
            //MEASSERT(maxd == ME_SMALL_EPSILON);
//            if (c->separation > 0 &&
//                c->separation < s->eps)
//                c->separation = 0;
            //MEASSERT(c->separation <= 0);
            break;
        }

#if MCD_GJK_DEBUG         // support single step debugging
        if (s->debug)
        {
            c->separation = v_len;
            c->normal[0] = maxd;
            c->normal[1] = (MeReal) dup_counter;
            s->debug = counter;
            return 2;
        }
#endif
    }

    return 1;                  //  RETURN CONTAINS ORIGIN
}


/****************************************************************************
  This tests if the simplex volume is positive and the origin is interior.
*/
static int McdFatAndHappy(const McdGjkSimplex *s)
{
    return s->bits==15 &&
        s->delta[15][0] > ME_SMALL_EPSILON &&
        s->delta[15][1] > ME_SMALL_EPSILON &&
        s->delta[15][2] > ME_SMALL_EPSILON &&
        s->delta[15][3] > ME_SMALL_EPSILON;
}


/****************************************************************************
  This calls the GJK main routine and tests the outcome.
  If the result is a full simplex, it calls Penetration Depth.
  If anything fails, or if the simplex is degenerate, this 
  moves the geometries apart (or together) and tries again.
*/
int McdGjkTest(McdCache *c, McdGjkSimplex *s)
{
    MeVector3 axis;  
    MeReal axisLen, offsetLen;
    MeVector3 c1, c2;      // bs center points
    MeReal r1, r2;         // bs radii
    int rc, i;

    MeReal originalPadding = c->padding;

    MeVectorSetZero(c->offset, 3);

#if MCD_GJK_DEBUG
    s->debug = 0;
#endif

    for (i = 0; i < 5; ++i)
    {
        rc = McdGjkMain(c, s);

        if (rc == 1 && McdFatAndHappy(s))
            rc = 1 - McdGjkPenetrationDepth(c, s);

        //--------------------------------------------------------
        //  If gjk successful, adjust separation and location

        if (!rc)
        {
            if (i)    // if not the first attempt
            {
                MeReal dot = MeVector3Dot(c->normal, axis);

                //  convexity requires this dot is positive
                MEASSERT(dot > 0);

                MeVector3MultiplyAdd(c->location, -dot * offsetLen / 2, c->normal);
                c->separation -= dot * offsetLen;
                c->padding = originalPadding;
            }
            return 1;         // SUCCESSFUL RETURN
        }

        //--------------------------------------------------------
        //  At this point, either GJK or PD failed.  Thus,
        //  change the offset and try again.

        if (!i)        // first time
        {
            MeVector3 minv, maxv;

            //  Compute direction vector from the center of ins2 to ins1.

            McdGeometryInstanceUpdateAABB(c->ins1, 0, 0);
            McdGeometryInstanceGetAABB(c->ins1, minv, maxv);
            r1 = MeVector3Distance(minv, maxv) / 2;
            MeVector3Add(c1, minv, maxv);
            MeVector3Scale(c1, 0.5f);

            McdGeometryInstanceUpdateAABB(c->ins2, 0, 0);
            McdGeometryInstanceGetAABB(c->ins2, minv, maxv);
            r2 = MeVector3Distance(minv, maxv) / 2;
            MeVector3Add(c2, minv, maxv);
            MeVector3Scale(c2, 0.5f);

//            McdGeometryInstanceGetBSphere(c->ins1, c1, &r1);
//            McdGeometryInstanceGetBSphere(c->ins2, c2, &r2);

            MeVector3Subtract(axis, c1, c2);
            axisLen = MeVector3Normalize(axis);

            if (!axisLen)
                axisLen = r1 + r2;
        }

        if (i == 3)
            offsetLen = r1 + r2;      // last chance
        else
            offsetLen = (i + 1.0f) * (r1 + r2 - axisLen) * 0.25f;
//            offsetLen = (1 - 2*i) * axisLen * 0.05f;

        MeVector3MultiplyScalar(c->offset, axis, offsetLen);

        //  If offsetLen is positive, (the geometries are moved farther
        //  apart), we increase padding so that GJK will compute the 
        //  true min distance vector and not stop too early.

        c->padding = originalPadding;
        if (offsetLen > 0)
            c->padding += offsetLen;
    }

#if MCD_GJK_DEBUG
    MEASSERT(!"McdGjkTest failed.");
    write_binfile("\\temp\\2box.bin",c);
#endif

    //--------------------------------------------------------
    //  At this point we have tried several times with different
    //  offsets the run GJK and they all failed.
    //  As a last resort, just set the contact normal to the axis
    //  and the separation to zero.

    c->separation = 0;
    c->padding = originalPadding;
    MeVector3Copy(c->normal, axis);
    MeVector3ScaleAndAdd(c->location, c2, r2/(r1+r2), axis);

    return 0;           // RETURN MARGINAL SUCCESS
}

/****************************************************************************
    This tests for non-collision using a witness plane algorithm. If
    p->m_cachedData is not NULL, it must point to an McdWitnessCache or a
    struct that contains an McdWitnessCache as its first member.

    This uses the McdGeometryMaximumPointFn on both geometries to test
    if there exists a plane normal to W that separates them.

    Return 1 if the models MIGHT be touching, and return 0 if the models
    are certainly not touching.  In either case, if p->m_cachedData is
    not NULL, the data in McdCache are set.

    Also this sets result->touch, and result->normal.
*/
int MEAPI McdWitnessPlaneTest(McdModelPair *p, McdIntersectResult *result)
{
    McdCache *const c = (McdCache *) p->m_cachedData;
    McdGjkPoint point;

    MeVector3Normalize(c->normal);
    MeVectorSetZero(c->offset, 3);

    c->separation = McdGjkComputeSupport(&point, c->normal, -1, c);  

    if (c->separation > c->padding + c->fat1 + c->fat2)
    {
        MeVector3Copy(c->location, point.s2);
        MeVector3MultiplyAdd(c->location,
          c->fat2 + c->separation/2, c->normal);
        result->touch = 0;
        /* we are sure they don't intersect */
        return 0;
    }

    /* Perhaps they intersect */
    return 1;
}

/****************************************************************************
  This generates contacts after it is determined that two geometries
  are overlapping.

  The cache location, normal, and separation must be set before calling this.

*/
void McdContactGen(McdCache *c, McdIntersectResult *result)
{
    MeVector3 vspace[MCDCONTACTGEN_VSIZE];
    int size1, size2;       // size of each set
    MeReal dist, eps;

    //  Compute signed distance from WRF origin to contact plane

    dist = MeVector3Dot(c->normal, c->location);
    eps = 0.001f * c->ins1->mGeometry->frame->mScale;

    //  Compute a polygon slice on both geometries.  We tend to get better
    //  contacts by slicing each shape slightly off the actually contact
    //  plane.  I.e. get a polygon by slicing ins1 at dist + d.
    //  However, sometimes this strategy will backfire, for instance when
    //  the body is so thin that dist + d is on the other side of it,
    //  thus we try ( +d, +0, -d ) for each model.

    MeReal d = MeFabs(c->separation / 2) + eps;

    int i, j, k, j0, numIntersect = 0;
    MeVector3 *intersect;

    //  Try once with d>0 and if the intersection is empty, 
    //  try several times with different slices.
    //  The "j0" is an optimization: if all the slices of ins2 are empty
    //  then j0 is set to -2 and we don't waste time looking at
    //  different slices of ins1.

    for (i = j0 = 1; i > -2 && j0 > -2 && !numIntersect; --i)
    {
        McdGeometryInstanceGetSlice(c->ins1, c->normal, dist + c->fat1 + d*i,
                MCDCONTACTGEN_VSIZE/3, &size1, vspace);

        if (!size1)
            continue;
        
        for (j = j0; j > -2 && !numIntersect; --j)
        {
            McdGeometryInstanceGetSlice(c->ins2, c->normal, dist - c->fat2 - d*j,
                    MCDCONTACTGEN_VSIZE/2 - size1, &size2, vspace + size1);

            if (!size2)
            {
                if (j0 == j) --j0;
                continue;
            }
            
            //  Make sure all the points are coplanar
            
            for (k = 0; k < size1 + size2; ++k)
                MeVector3MultiplyAdd(vspace[k], dist-MeVector3Dot(vspace[k],c->normal), c->normal);
            
            //  If either size is one point, return it
            
            if (size1==1)
            {
                intersect = vspace;
                numIntersect = 1;
                continue;
            }
            if (size2==1)
            {
                intersect = vspace + size1;
                numIntersect = 1;
                continue;
            }
            
            //  Compute polygon intersection

            intersect = vspace + size1 + size2;

            McdPolygonIntersection(c->normal, dist, 
                        size1, vspace, size2, vspace + size1,
                        &numIntersect, intersect);
        }
    }

#if MCD_GJK_DEBUG         // GJK viewer debugging
    static MeVector3 ds;
    ds[0] = dist + c->fat1 + d*(i+1);
    ds[1] = dist - c->fat2 - d*(j+1);
    result->data = ds;
#endif

    //  If separation is positive, make it zero.  This is important
    //  because positive separation will cause stickiness in Kea,
    //  because of backwards position projection.

    if (c->separation > 0)
        c->separation = 0;

    //  Convert the polygon intersection to contacts

    for (i = 0; i < numIntersect && i < result->contactMaxCount; ++i)
    {
        MeVector3Copy(result->contacts[i].normal, c->normal);
        MeVector3Copy(result->contacts[i].position, intersect[i]);
        result->contacts[i].separation = c->separation;
    }

    result->contactCount = i;
}


/****************************************************************************
  This tests for collision of any two models using GJK.
  If the models are not touching, this returns 0.
  If they are touching, this returns 1 and sets the result.
  The members set in the result are

    - touch = 0 or 1, same as return value
    - contacts = a list of contact points
    - contactCount = how many (not more than contactMaxCount)
    - normal = average normal of all the contacts

  The collision detection may use the McdModelPair member p->m_cachedData
  to cache data and take advantage of time-step coherence.

  Note:  p->userData is reserved for customers, and p->responseData is
  an MdtContactGroupID.
*/
int McdGjkCgIntersect2(McdModelPair* p, McdIntersectResult *result)
{
    McdCache cmem;
    McdCache *c = (McdCache *) p->m_cachedData;
    McdGjkSimplex s;

    //  See if either model is a plane

    if (McdGeometryGetType(p->model1->mInstance.mGeometry) == kMcdGeometryTypePlane)
        return McdPlaneIntersectTest(p->model1,p->model2,result);
    else if (McdGeometryGetType(p->model2->mInstance.mGeometry) == kMcdGeometryTypePlane)
        return McdPlaneIntersectTest(p->model2,p->model1,result);

    //  Test if the cache is missing, if so use a temp cache else call
    //  witness test to see if the normal of the last time step is a
    //  suitable separating plane.

    if (c == 0)
        McdCacheInit(c = &cmem, p);
    else if ( ! McdWitnessPlaneTest(p, result)) 
        return 0;

    //  Call main GJK collision detection (with Penetration Depth inflation).

    int ok = McdGjkTest(c, &s);

    //  Set result flags and counter

    result->touch = (c->separation <= c->padding);
    result->contactCount = 0;
    MeVector3Copy(result->normal, c->normal);

    if (! result->touch 
       || result->contactMaxCount < 1)
        return result->touch;            // RETURN -- NO CONTACTS GENERATED

    McdContactGen(c, result);

    if (result->contactCount==0)
    {

#if MCD_GJK_DEBUG
        //MEASSERT(result->contactCount);  // force failure
        MeInfo(1,"no contacts, generating 1 default");
#endif

        result->contactCount = 1;
        MeVector3Copy(result->contacts[0].normal, c->normal);
        MeVector3Copy(result->contacts[0].position, c->location);
        result->contacts[0].separation = 0;
        result->contacts[0].dims = 0;
    }

    return result->touch;           // RETURN -- ONE OR MORE CONTACTS
}

/****************************************************************************
  This is a dummy to add code profiling to the GJK main function.
*/
int MEAPI
McdGjkCgIntersect(McdModelPair* p, McdIntersectResult *result)
{
    McdProfileStart("McdGjkCgIntersect");
    int t = McdGjkCgIntersect2(p, result);
    McdProfileEnd("McdGjkCgIntersect");
    return t;
}


/*

// DEBUGGING
    int i; if (result->contactCount < 3)
    for (i=0; i<result->contactCount; ++i)
        printf("%d (%g %g %g) (%g %g %g) %g\n",i,result->contacts[i].normal[0],result->contacts[i].normal[1],result->contacts[i].normal[2],result->contacts[i].position[0],result->contacts[i].position[1],result->contacts[i].position[2],result->contacts[i].separation);

  */