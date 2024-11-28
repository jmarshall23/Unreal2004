/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:54 $ - Revision: $Revision: 1.2.2.1 $

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

  -------------------------------------------------------------------

  Note, the following source code was copied from the internet
  on 15 Feb 2002, and it appears to be public domain.  Johnh.
 */

    /*******************************************************
    *                                                      *
    *  volInt.c                                            *
    *                                                      *
    *  This code computes volume integrals needed for      *
    *  determining mass properties of polyhedral bodies.   *
    *                                                      *
    *  For more information, see the accompanying README   *
    *  file, and the paper                                 *
    *                                                      *
    *  Brian Mirtich, "Fast and Accurate Computation of    *
    *  Polyhedral Mass Properties," journal of graphics    *
    *  tools, volume 1, number 1, 1996.                    *
    *                                                      *
    *  This source code is public domain, and may be used  *
    *  in any way, shape or form, free of charge.          *
    *                                                      *
    *  Copyright 1995 by Brian Mirtich                     *
    *                                                      *
    *  mirtich@cs.berkeley.edu                             *
    *  http://www.cs.berkeley.edu/~mirtich                 *
    *                                                      *
    *******************************************************/

/*
	Revision history (from Brian Mirtichs source code volInt.c)

	26 Jan 1996	Program creation.

	 3 Aug 1996	Corrected bug arising when polyhedron density
			is not 1.0.  Changes confined to function main().
			Thanks to Zoran Popovic for catching this one.

	27 May 1997     Corrected sign error in translation of inertia
	                product terms to center of mass frame.  Changes 
			confined to function main().  Thanks to 
			Chris Hecker.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <McdConvexMesh.h>
#include <McdPolygonIntersection.h>

/*
   ============================================================================
   constants -- from the original Mirtich code.
   ============================================================================
*/

#define X 0
#define Y 1
#define Z 2

#define SQR(x) ((x)*(x))
#define CUBE(x) ((x)*(x)*(x))

/*
  ============================================================================
  Compute mass properties

  This function computes the volume integrals of any convex polyhedron.
  (It might work with non-convex shapes, too, but I am not sure about that).

  For notation, let INT(x)dV mean the integral of x over the entire volume
  of the polyhedron.

  The output is ten volume integrals

      T0    = INT(1)dV
      T1[0] = INT(x)dV
      T1[1] = INT(y)dV
      T1[2] = INT(z)dV
      T2[0] = INT(x^2)dV
      T2[1] = INT(y^2)dV
      T2[2] = INT(z^2)dV
      TP[0] = INT(xy)dV
      TP[1] = INT(yz)dV
      TP[2] = INT(zx)dV

  and from these quantities it is easy to compute the volume (T0), center
  of mass (T1/T0), and the inertia tensor (using T2 and TP).

  In Mirtichs original code this function was broken into three parts
  compVolumeIntegrals, compFaceIntegrals, and compProjectionIntegrals.
  ============================================================================
*/

static MeReal 
compVolumeIntegrals(McdConvexHull *p,
                    MeReal T1[3], MeReal T2[3], MeReal TP[3])
{
    int A;   /* alpha */
    int B;   /* beta */
    int C;   /* gamma */

    /* projection integrals */
    MeReal P1, Pa, Pb, Paa, Pab, Pbb, Paaa, Paab, Pabb, Pbbb;

    /* face integrals */
    MeReal Fa, Fb, Fc, Faa, Fbb, Fcc, Faaa, Fbbb, Fccc, Faab, Fbbc, Fcca;

    MeReal T0;      /* T0 is the return value */
    MeReal *n, w;   /* face plane normal and negative distance from origin */
    MeReal k1, k2, k3, k4;
    MeReal a0, a1, da;
    MeReal b0, b1, db;
    MeReal a0_2, a0_3, a0_4, b0_2, b0_3, b0_4;
    MeReal a1_2, a1_3, b1_2, b1_3;
    MeReal C1, Ca, Caa, Caaa, Cb, Cbb, Cbbb;
    MeReal Cab, Kab, Caab, Kaab, Cabb, Kabb;
    int f, numedge;
    int i;

    T0 = T1[X] = T1[Y] = T1[Z] 
       = T2[X] = T2[Y] = T2[Z] 
       = TP[X] = TP[Y] = TP[Z] = 0;

    for (f = 0; f < p->numFace; f++) 
    {
        numedge = McdCnvFaceGetCount(p, f);

        n = p->face[f].normal;
        w = -MeVector3Dot(n, McdCnvFaceGetVertexPosition(p, f, 0));

        C = McdPolygonBestAxis(p->face[f].normal);
        A = (C + 1) % 3;
        B = (A + 1) % 3;

        /*  begin compFaceIntegrals(FACE *f)  */

        /*  begin compProjectionIntegrals(FACE *f)  */

        P1 = Pa = Pb = Paa = Pab = Pbb = Paaa = Paab = Pabb = Pbbb = 0.0;

        for (i = 0; i < numedge; i++) 
        {
            const McdCnvEdge *e = McdCnvFaceGetEdge(p,f,i);

            a0 = p->vertex[e->fromVert].position[A];
            b0 = p->vertex[e->fromVert].position[B];
            a1 = p->vertex[e->toVert].position[A];
            b1 = p->vertex[e->toVert].position[B];
            da = a1 - a0;
            db = b1 - b0;
            a0_2 = a0 * a0; a0_3 = a0_2 * a0; a0_4 = a0_3 * a0;
            b0_2 = b0 * b0; b0_3 = b0_2 * b0; b0_4 = b0_3 * b0;
            a1_2 = a1 * a1; a1_3 = a1_2 * a1; 
            b1_2 = b1 * b1; b1_3 = b1_2 * b1;

            C1 = a1 + a0;
            Ca = a1*C1 + a0_2; Caa = a1*Ca + a0_3; Caaa = a1*Caa + a0_4;
            Cb = b1*(b1 + b0) + b0_2; Cbb = b1*Cb + b0_3; Cbbb = b1*Cbb + b0_4;
            Cab = 3*a1_2 + 2*a1*a0 + a0_2; Kab = a1_2 + 2*a1*a0 + 3*a0_2;
            Caab = a0*Cab + 4*a1_3; Kaab = a1*Kab + 4*a0_3;
            Cabb = 4*b1_3 + 3*b1_2*b0 + 2*b1*b0_2 + b0_3;
            Kabb = b1_3 + 2*b1_2*b0 + 3*b1*b0_2 + 4*b0_3;

            P1 += db*C1;
            Pa += db*Ca;
            Paa += db*Caa;
            Paaa += db*Caaa;
            Pb += da*Cb;
            Pbb += da*Cbb;
            Pbbb += da*Cbbb;
            Pab += db*(b1*Cab + b0*Kab);
            Paab += db*(b1*Caab + b0*Kaab);
            Pabb += da*(a1*Cabb + a0*Kabb);
        }

        P1 /= 2.0;
        Pa /= 6.0;
        Paa /= 12.0;
        Paaa /= 20.0;
        Pb /= -6.0;
        Pbb /= -12.0;
        Pbbb /= -20.0;
        Pab /= 24.0;
        Paab /= 60.0;
        Pabb /= -60.0;

        /*  end of compProjectionIntegrals(FACE *f)  */

        k1 = 1 / n[C]; k2 = k1 * k1; k3 = k2 * k1; k4 = k3 * k1;

        Fa = k1 * Pa;
        Fb = k1 * Pb;
        Fc = -k2 * (n[A]*Pa + n[B]*Pb + w*P1);

        Faa = k1 * Paa;
        Fbb = k1 * Pbb;
        Fcc = k3 * (SQR(n[A])*Paa + 2*n[A]*n[B]*Pab + SQR(n[B])*Pbb
              + w*(2*(n[A]*Pa + n[B]*Pb) + w*P1));

        Faaa = k1 * Paaa;
        Fbbb = k1 * Pbbb;
        Fccc = -k4 * (CUBE(n[A])*Paaa + 3*SQR(n[A])*n[B]*Paab 
               + 3*n[A]*SQR(n[B])*Pabb + CUBE(n[B])*Pbbb
               + 3*w*(SQR(n[A])*Paa + 2*n[A]*n[B]*Pab + SQR(n[B])*Pbb)
               + w*w*(3*(n[A]*Pa + n[B]*Pb) + w*P1));

        Faab = k1 * Paab;
        Fbbc = -k2 * (n[A]*Pabb + n[B]*Pbbb + w*Pbb);
        Fcca = k3 * (SQR(n[A])*Paaa + 2*n[A]*n[B]*Paab + SQR(n[B])*Pabb
             + w*(2*(n[A]*Paa + n[B]*Pab) + w*Pa));

        /*  end of compFaceIntegrals(FACE *f)  */

        T0 += n[X] * ((A == X) ? Fa : ((B == X) ? Fb : Fc));

        T1[A] += n[A] * Faa;
        T1[B] += n[B] * Fbb;
        T1[C] += n[C] * Fcc;
        T2[A] += n[A] * Faaa;
        T2[B] += n[B] * Fbbb;
        T2[C] += n[C] * Fccc;
        TP[A] += n[A] * Faab;
        TP[B] += n[B] * Fbbc;
        TP[C] += n[C] * Fcca;
    }

    T1[X] /= 2; T1[Y] /= 2; T1[Z] /= 2;
    T2[X] /= 3; T2[Y] /= 3; T2[Z] /= 3;
    TP[X] /= 2; TP[Y] /= 2; TP[Z] /= 2;

    return T0;
}


/****************************************************************************
  This computes the center of mass position, inertia tensor, and volume
  of a convex mesh.

  Note:  Although comTM is a matrix, the rotation part is ALWAYS identity.
         comTM[3] is the vector in LRF to the center of mass.

  return 0 always.
*/
MeI16 MEAPI
McdConvexMeshGetMassProperties( McdGeometry *g, MeMatrix4 comTM, MeMatrix3 I, MeReal *volume)
{
    McdConvexMesh *c;
    MeReal T0,T1[3],T2[3],TP[3];
    MeReal density;
    MeVector3Ptr r;

    MEASSERT(McdGeometryGetType(g)==kMcdGeometryTypeConvexMesh);
    c = (McdConvexMesh*)g;

    T0 = compVolumeIntegrals(&c->mHull, T1, T2, TP);

    /* since mass is presumed to be 1, thus density is 1/volume */
    *volume = T0;
    density = 1 / T0;

    /* compute center of mass */
    r = comTM[3];
    MeMatrix4TMMakeIdentity(comTM);
    MeVector3MultiplyScalar(r, T1, density);

    /* compute inertia tensor at the LRF origin */
    I[X][X] = density * (T2[Y] + T2[Z]);
    I[Y][Y] = density * (T2[Z] + T2[X]);
    I[Z][Z] = density * (T2[X] + T2[Y]);
    I[X][Y] = I[Y][X] = - density * TP[X];
    I[Y][Z] = I[Z][Y] = - density * TP[Y];
    I[Z][X] = I[X][Z] = - density * TP[Z];

    /* translate inertia tensor to center of mass */
    I[X][X] -= r[Y]*r[Y] + r[Z]*r[Z];
    I[Y][Y] -= r[Z]*r[Z] + r[X]*r[X];
    I[Z][Z] -= r[X]*r[X] + r[Y]*r[Y];
    I[X][Y] = I[Y][X] += r[X] * r[Y]; 
    I[Y][Z] = I[Z][Y] += r[Y] * r[Z]; 
    I[Z][X] = I[X][Z] += r[Z] * r[X]; 

#if 0
    //================================
    //  For Debugging

    printf("T0 =   %+12.6f (volume) mass=1.0 density=%+12.6f\n", T0, density);
    printf("T1 =   %+12.6f %+12.6f %+12.6f\n", T1[X],T1[Y],T1[Z]);
    printf("T2 =   %+12.6f %+12.6f %+12.6f\n", T2[X], T2[Y], T2[Z]);
    printf("TP =   %+12.6f %+12.6f %+12.6f\n", TP[X], TP[Y], TP[Z]);
    printf("Vertex[0] :      (%+12.6f,%+12.6f,%+12.6f)\n", c->mHull.vertex[0].position[X], c->mHull.vertex[0].position[Y], c->mHull.vertex[0].position[Z]);
    printf("center of mass:  (%+12.6f,%+12.6f,%+12.6f)\n", r[X], r[Y], r[Z]);
    printf("inertia tensor with origin at c.o.m. :\n");
    printf("%+15.6f  %+15.6f  %+15.6f\n", I[X][X], I[X][Y], I[X][Z]);
    printf("%+15.6f  %+15.6f  %+15.6f\n", I[Y][X], I[Y][Y], I[Y][Z]);
    printf("%+15.6f  %+15.6f  %+15.6f\n", I[Z][X], I[Z][Y], I[Z][Z]);
    printf("--------------------------------\n");
#endif

    return 0;
}

