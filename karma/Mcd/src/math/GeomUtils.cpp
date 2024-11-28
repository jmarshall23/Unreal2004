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

#include <GeomUtils.h>
#include <McdQHullTypes.h>

// closest intersection points between two lines
// line p0 + (s * d0), line p1 + (t * d1)
// d0 & d1 must by unit length
// returns TRUE if intersection is "proper," i.e. lines are not parallel.  Returns false otherwise,
// but intersection points are still given
MeBool
LineDistance(MeVector3 p0, MeVector3 d0, MeVector3 p1, MeVector3 d1, MeReal *s, MeReal *t, MeReal eps)
{
    MeVector3 diff;
    MeReal b, d, det;

    MeVector3Subtract(diff, p1, p0);
    b = MeVector3Dot(d0, d1);
    d = MeVector3Dot(d0, diff);
//    det = MeFabs(1 - (b*b)); // = |d0 x d1|^2 >= 0
    det = 1 - b*b; // since d0 & d1 are normalized, b*b <= 1.  In the case of roundoff making det < 0, the
                   // proper branch of the following conditional will still be used.

    if(det > eps)
    {
        MeReal e = MeVector3Dot(d1, diff);
        *s = (d-b*e)/det;
        // *t = (*s)*b-e = ((d-b*e)/det)*b - e = (b*d - b*b*e - e + b*b*e)/(1-b*b) = (b*d-e)/det
        *t = (*s)*b-e; 
        return true;
    }
    else // lines are parallel
    {
        *s = d;
        *t = 0;
        return false;
    }
}

// If num/den is outside the range [sMin, sMax], *s is clamped to the closest of that
// range's extrema, and TRUE is returned.  Otherwise, *s is unchaged, and FALSE is returned.
// N.B. den must be positive
inline MeBool
ClampFraction(MeReal * const s, const MeReal num, const MeReal den,
              MeReal sMin, const MeReal sMax)
{
    if (num < sMin*den)
    {
        *s = sMin;
    } else
    if (num > sMax*den)
    {
        *s = sMax;
    } else
    {
        return false;
    }
    return true;
}

// Sets *s = val and returns TRUE, unless val is outside the range [sMin, sMax].  In that case,
// *s is clamped to the closest of that range's extrema and FALSE is returned.
inline MeBool
LoudClamp(MeReal * const s, const MeReal val, MeReal sMin, const MeReal sMax)
{
    if (val < sMin)
    {
        *s = sMin;
    } else
    if (val > sMax)
    {
        *s = sMax;
    } else
    {
        *s = val;
        return false;
    }
    return true;
}

MeBool
NSegmentSegment(const MeVector3 p0, const MeVector3 d0, const MeReal s0min, const MeReal s0max,
                const MeVector3 p1, const MeVector3 d1, const MeReal s1min, const MeReal s1max,
                MeReal * const s0, MeReal * const s1, MeReal * const ds0)
{
    MEASSERT( s0max >= s0min && s1max >= s1min );

    MeVector3 r;
    MeVector3Subtract(r, p1, p0);

    const MeReal d1d1 = MeVector3Dot(d1, d1);
    const MeReal d0d1 = MeVector3Dot(d0, d1);

    const MeReal det = d1d1 - d0d1*d0d1;

    const MeReal d0r = MeVector3Dot(d0, r);
    const MeReal d1r = MeVector3Dot(d1, r);

    if (det > PARALLEL_THRESH_EPS*d1d1)
    {
        const MeReal s0numInf = d0r*d1d1-d1r*d0d1;
        if (ClampFraction(s0, s0numInf, det, s0min, s0max))
        {
            // s0 got clamped.  s1 is calculated from point-line formula
            const MeReal s1num = (*s0)*d0d1-d1r;
            if (ClampFraction(s1, s1num, d1d1, s1min, s1max))
            {
                // Since s1 got clamped, we must re-calculate s0.  This is provably
                // the last iteration of this process.
                const MeReal s0num = (*s1)*d0d1+d0r;
                *s0 = MeCLAMP(s0num, s0min, s0max);
            } else
            {
                // s1 is not yet calculated, because it won't be clamped.  Find it now.
                *s1 = s1num/d1d1;
            }
        } else
        {
            // s0 is not yet calculated, because it won't be clamped (yet).
            // See if s1 will get clamped
            const MeReal s1numInf = d0r*d0d1-d1r;
            if (ClampFraction(s1, s1numInf, det, s1min, s1max))
            {
                // s1 got clamped.  calculate s0 from the point-line formula
                const MeReal s0num = (*s1)*d0d1+d0r;
                *s0 = MeCLAMP(s0num, s0min, s0max);
            } else
            {
                // Neither s0 or s1 will be clamped.  Find their values by the
                // infinite-line formula.
                const MeReal invDet = MeRecip(det);
                *s0 = s0numInf*invDet;
                *s1 = s1numInf*invDet;
            }
        }
        *ds0 = 0;
        return true;
    } else
    {
        // lines are parallel.  Find transform between coordinate systems.
        MeReal s1SgnScaleMin;
        MeReal s1SgnScaleMax;
        if (d0d1 > 0)
        {
            s1SgnScaleMin = s1min;
            s1SgnScaleMax = s1max;
        } else
        {
            s1SgnScaleMin = s1max;
            s1SgnScaleMax = s1min;
        }
        // the endpoints of segment 1 in segment 0's coordinate system
        const MeReal s0min1 = d0r+s1SgnScaleMin*d0d1;
        const MeReal s0max1 = d0r+s1SgnScaleMax*d0d1;
        // overlap the segments.
        if (s0max1 <= s0min)
        {
            // They don't overlap.  Closest point to segment 0 is the min. parameter.
            *s0 = s0min;
            *ds0 = 0;
            *s1 = s1SgnScaleMax;
        } else
        if (s0min1 >= s0max)
        {
            // They don't overlap.  Closest point to segment 0 is the max. parameter.
            *s0 = s0max;
            *ds0 = 0;
            *s1 = s1SgnScaleMin;
        } else
        {
            // They overlap.
            *s0 = MeMAX(s0min, s0min1);
            *ds0 = MeMIN(s0max, s0max1)-(*s0);
            *s1 = ((*s0)*d0d1 - d1r)/d1d1;
        }
        return false;
    }
}

MeBool
NSegmentNSegment(const MeVector3 p0, const MeVector3 d0, const MeReal s0min, const MeReal s0max,
                 const MeVector3 p1, const MeVector3 d1, const MeReal s1min, const MeReal s1max,
                 MeReal * const s0, MeReal * const s1, MeReal * const ds0)
{
    MEASSERT( s0max >= s0min && s1max >= s1min );

    MeVector3 r;
    MeVector3Subtract(r, p1, p0);

    const MeReal d0d1 = MeVector3Dot(d0, d1);

    const MeReal det = 1 - d0d1*d0d1;

    const MeReal d0r = MeVector3Dot(d0, r);
    const MeReal d1r = MeVector3Dot(d1, r);

    if (det > PARALLEL_THRESH_EPS)
    {
        const MeReal s0numInf = d0r-d1r*d0d1;
        if (ClampFraction(s0, s0numInf, det, s0min, s0max))
        {
            // s0 got clamped.  s1 is calculated from point-line formula
            const MeReal s1num = (*s0)*d0d1-d1r;
            if (LoudClamp(s1, s1num, s1min, s1max))
            {
                // Since s1 got clamped, we must re-calculate s0.  This is provably
                // the last iteration of this process.
                const MeReal s0num = (*s1)*d0d1+d0r;
                *s0 = MeCLAMP(s0num, s0min, s0max);
            }
        } else
        {
            // s0 is not yet calculated, because it won't be clamped (yet).
            // See if s1 will get clamped
            const MeReal s1numInf = d0r*d0d1-d1r;
            if (ClampFraction(s1, s1numInf, det, s1min, s1max))
            {
                // s1 got clamped.  calculate s0 from the point-line formula
                const MeReal s0num = (*s1)*d0d1+d0r;
                *s0 = MeCLAMP(s0num, s0min, s0max);
            } else
            {
                // Neither s0 or s1 will be clamped.  Find their values by the
                // infinite-line formula.
                const MeReal invDet = MeRecip(det);
                *s0 = s0numInf*invDet;
                *s1 = s1numInf*invDet;
            }
        }
        *ds0 = 0;
        return true;
    } else
    {
        // lines are parallel.  Find transform between coordinate systems.
        MeReal s1SgnScaleMin;
        MeReal s1SgnScaleMax;
        if (d0d1 > 0)
        {
            s1SgnScaleMin = s1min;
            s1SgnScaleMax = s1max;
        } else
        {
            s1SgnScaleMin = s1max;
            s1SgnScaleMax = s1min;
        }
        // the endpoints of segment 1 in segment 0's coordinate system
        const MeReal s0min1 = d0r+s1SgnScaleMin*d0d1;
        const MeReal s0max1 = d0r+s1SgnScaleMax*d0d1;
        // overlap the segments.
        if (s0max1 <= s0min)
        {
            // They don't overlap.  Closest point to segment 0 is the min. parameter.
            *s0 = s0min;
            *ds0 = 0;
            *s1 = s1SgnScaleMax;
        } else
        if (s0min1 >= s0max)
        {
            // They don't overlap.  Closest point to segment 0 is the max. parameter.
            *s0 = s0max;
            *ds0 = 0;
            *s1 = s1SgnScaleMin;
        } else
        {
            // They overlap.
            *s0 = MeMAX(s0min, s0min1);
            *ds0 = MeMIN(s0max, s0max1)-(*s0);
            *s1 = (*s0)*d0d1 - d1r;
        }
        return false;
    }
}

