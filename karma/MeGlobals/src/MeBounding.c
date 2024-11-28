/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/25 18:15:23 $ - Revision: $Revision: 1.1.2.2 $

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

#include "MeBounding.h"
#include "MeMath.h"

/** 
 *  Calculate a good (about 5% error) bounding sphere for a set of points in 3D.
 */
void MEAPI MeBoundingSphereCalc(MeVector3* points, int numPoints,
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
        min[i] = max[i] = points[0][i];
        minIx[i] = maxIx[i] = 0;
    }

    for(i=1; i<numPoints; i++) 
    {
        for(j=0; j<3; j++)
        {
            if (points[i][j] < min[j]) 
            {
                min[j] = points[i][j];
                minIx[j] = i;
            }
            else if (points[i][j] > max[j]) 
            {
                max[j] = points[i][j];
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
        MeVector3Subtract(diff, points[maxIx[i]], points[minIx[i]]);
        tmpd2 = MeVector3MagnitudeSqr(diff);

        if(tmpd2 > d2)
        {
            d2 = tmpd2;
            MeVector3ScaleAndAdd(centre, points[minIx[i]], (MeReal)0.5, diff);
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

        MeVector3Subtract(cToP, points[i], centre);
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