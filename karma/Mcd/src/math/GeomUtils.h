/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

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

#ifndef _GEOMUTILS_H
#define _GEOMUTILS_H

#include <MeMath.h>

#define PARALLEL_THRESH_EPS ((MeReal)0.00025)

/*
    Closest point on a line segment to another point
 */

// closest point on line segment to point
// line p0 + (x * h * d0), -1<=x<=1, point p1
// d0 must be unit length
inline void
PointSegmentClosestPoint(MeVector3 p0, MeVector3 d0, MeReal h, MeVector3 p1, MeVector3 cp)
{
    MeVector3 diff; 
    MeVector3Subtract(diff, p1, p0);
    MeReal t = MeVector3Dot(diff, d0);
    t = MeCLAMP(t, -h, h);
    MeVector3ScaleAndAdd(cp, p0, t, d0);
}


/*
    Closest points on two line segments to each other

    Segments i = 0, 1 are centered at pi, with direction di.  Neither d0 nor d1 need to be
    normalized. Endpoints are at pi+simin*di and pi+simax*di.  simax must be no less than simin.
    Returns paramters of closest points on each segment, s0 and s1.
    If lines are parallel and their projections overlap, then ds0 is set to a nonzero value, such
    that s0 and s0+ds0 are the parameters on segment 0 which mark the borders of the overlap.
    Otherwise, ds0 is set to zero.  In either case, s1 is the parameter on segment 1 which is
    closest to the point on segment 0 with line parameter s0.
    Returns TRUE if the lines are askew, FALSE if they are parallel.

    d0 must be normalized.
 */
MeBool
NSegmentSegment(const MeVector3 p0, const MeVector3 d0, const MeReal s0min, const MeReal s0max,
                const MeVector3 p1, const MeVector3 d1, const MeReal s1min, const MeReal s1max,
                MeReal * const s0, MeReal * const s1, MeReal * const ds0);

// The same as NSegmentSegment, but d1 must also be normalized.
MeBool
NSegmentNSegment(const MeVector3 p0, const MeVector3 d0, const MeReal s0min, const MeReal s0max,
                 const MeVector3 p1, const MeVector3 d1, const MeReal s1min, const MeReal s1max,
                 MeReal * const s0, MeReal * const s1, MeReal * const ds0);


/*
    Find the separation between a segment and a plane (actually a half-space).
    *s = parameter on segment "lowest" relative to plane normal
 */
inline MeReal
SegmentPlaneSep(const MeVector3 ps, const MeVector3 d, const MeReal smin, const MeReal smax,
                const MeVector3 pp, const MeVector3 n, MeReal *s)
{
    // point-plane distance
    const MeReal dn = MeVector3Dot(d, n);
    if (dn > PARALLEL_THRESH_EPS)
    {
        *s = smin;
    } else
    if (dn < -PARALLEL_THRESH_EPS)
    {
        *s = smax;
    } else
    {
        *s = 0;
    }

    MeVector3 disp;
    MeVector3Subtract(disp, ps, pp);

    return MeVector3Dot(disp, n) + (*s)*dn;
}

#endif // #ifndef _GEOMUTILS_H
