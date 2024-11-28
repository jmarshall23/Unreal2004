/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.14.2.1 $

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

#include <MdtKea.h>
#include <MeMath.h>
#include <MdtKeaProfile.h>
#include "keaInternal.hpp"
#include "keaDebug.h"
#include <keaFunctions.hpp>

/* calcIworldandNonInertialForceandVhmf
 * ------------------------------------
 *
 * This function holds all the code that loops over bodies that is not the integrator
 *
 * On Entry:
 *
 * invIworld        - an uninitialised array of size 12*num_bodies MeReals
 * tlist            - an array of 4*4 matrices in column major order. Array is of length num_bodies
 * blist[i].invmass - inverse mass of body i (for all i, 0<=i<num_bodies )
 * blist[i].invI0   - a 3*4 symmetric matrix. element 1,4 is the inverse mass. elements 2,4 and 3,4 are uninitialised (for all i, 0<=i<num_bodies )
 * blist[i].I0      - a 3*4 symmetric matrix. elements 1,4 2,4 and 3,4 are uninitialised (for all i, 0<=i<num_bodies )
 * blist[i].vel     - a 4 element vector, first 3 elements are the velocity of body i, 4th is uninitialised (for all i, 0<=i<num_bodies )
 * blist[i].velrot  - a 4 element vector, first 3 elements are the rotational velocity of body i, 4th is uninitialised (for all i, 0<=i<num_bodies )
 * blist[i].force   - a 4 element vector, first 3 elements are the force of body i, 4th is uninitialised (for all i, 0<=i<num_bodies )
 * blist[i].torque  - a 4 element vector, first 3 elements are the torque of body i, 4th is uninitialised (for all i, 0<=i<num_bodies )
 * num_bodies       - the number of bodies
 * stepsize         - the timestep
 *
 * On Exit:
 *
 * Iworld[i]        = transpose(tlist[i])*matrix_part(blist[i].I0)*tlist[i]                   (for all i, 0<=i<num_bodies )
 * invIworld[i]     = transpose(tlist[i])*matrix_part(blist[i].invI0)*tlist[i]                (for all i, 0<=i<num_bodies )
 *                    element 1,4 of invIworld[i] = blist[i].invmass
 * blist[i].torque  = blist[i].torque-cross(blist[i].velrot[i],matrix_part(Iworld[i])*velrot) (for all i, 0<=i<num_bodies )
 *                    ,if blist[i].flags & MdtKeaBodyFlagIsConstrained
 *                  = blist[i].torque
 *                    , otherwise
 *
 * Where matrix_part |  a0  a1  a2  a3 | = |  a0  a1  a2 |
 *                   |  a4  a5  a6  a7 |   |  a4  a5  a6 |
 *                   |  a8  a9 a10 a11 |   |  a8  a9 a10 |
 *
 * Cache status
 * ------------
 * invIworld - uncached accelerated pointer
 * vhmf      - uncached accelerated pointer
*/

MeReal constants[4] __attribute__ ((aligned(64)));

void keaFunctions_PS2 :: calcIworldandNonInertialForceandVhmf(
         MdtKeaInverseMassMatrix    invIworld[],             /* Output */
         MdtKeaVelocity             vhmf[],                  /* Output */
         const MdtKeaBody *const    blist[],                 /* Input */
         const MdtKeaTransformation tlist[],                 /* Input */
         int                        num_bodies,              /* Input */
         MeReal                     stepsize)                /* Input */
{
    int body;
    
#if PRINT_CALCIWORLDNONINERTIALFORCEANDVHMF_INPUT
    printCalcIworldNonInertialForceandVhmfInput(blist,num_bodies);
#endif

#if PROFILE_KEA
    MdtKeaProfileStart("ps2 calcIworldandNIForceandVhmf");
#endif


    const MdtKeaBody * const* pb       = blist;
    const MdtKeaTransformation *tb     = tlist;

    MeReal *                 pinvIworld = (MeReal* )invIworld;

    for (body = 0; body != num_bodies; body++)
    {        
        if((*pb)->flags&MdtKeaBodyFlagIsNonSpherical)
        {
            /* invIworld = R^T I^-1 R */

	    __asm__ __volatile__("
	    __expression_asm

	    i0
	    i1
	    i2
	    iw0
	    iw1
	    iw2
	    tm0
	    tm1
	    tm2

	    'lqc2  @i0, 0x0(%0)
	    'lqc2  @i1, 0x10(%0)
	    'lqc2  @i2, 0x20(%0)

	    'lqc2  @tm0, 0x0(%1)
	    'lqc2  @tm1, 0x10(%1)
	    'lqc2  @tm2, 0x20(%1)

	    # i = Rt * invIbody

	    ACC.xyz = tm0 * i0.x
	    ACC.xyz = ACC + tm1 * i0.y
	    i0.xyz  = ACC + tm2 * i0.z

	    ACC.xyz = tm0 * i1.x
	    ACC.xyz = ACC + tm1 * i1.y
	    i1.xyz  = ACC + tm2 * i1.z

	    ACC.xyz = tm0 * i2.x
	    ACC.xyz = ACC + tm1 * i2.y
	    i2.xyz  = ACC + tm2 * i2.z

	    # iw = i * R

	    ACC.xyz = tm0 * i0.x
	    ACC.xyz = ACC + tm1 * i1.x
	    iw0.xyz = ACC + tm2 * i2.x

	    # Keep the mass
	    iw0.w = i0.w + K.x

	    ACC.xyz = tm0 * i0.y
	    ACC.xyz = ACC + tm1 * i1.y
	    iw1.xyz = ACC + tm2 * i2.y

	    ACC.xyz = tm0 * i0.z
	    ACC.xyz = ACC + tm1 * i1.z
	    iw2.xyz = ACC + tm2 * i2.z

	    'sqc2  @iw0, 0x0(%2)
	    'sqc2  @iw1, 0x10(%2)
	    'sqc2  @iw2, 0x20(%2)

	    ~i0
	    ~i1
	    ~i2
	    ~iw0
	    ~iw1
	    ~iw2
	    ~tm0
	    ~tm1
	    ~tm2

	    __end_expression_asm

	    " : : "r" ((*pb)->invI0), "r" (tb->R0), "r" (pinvIworld));

        }
        else
        {
            //MeReal s = (pb->I0[0] + pb->I1[1] + pb->I2[2])/(MeReal)3.0;
            //MeReal t = (pb->invI0[0] + pb->invI1[1] + pb->invI2[2])/(MeReal)3.0;
            
            /* Alternative way of making the diagonal moment of inertia by Richard T */
            /* This makes Topple look much better */
            /* invIworld := makespherical(I) */

            MeReal s = (*pb)->I0[0];
            if((*pb)->I1[1] > s) s = (*pb)->I1[1];
            if((*pb)->I2[2] > s) s = (*pb)->I2[2];
            
            MeReal t = MeRecip(s);
            
            pinvIworld[0] = pinvIworld[5] = pinvIworld[10] = t;
        
            pinvIworld[1] = pinvIworld[2] = 
            pinvIworld[4] = pinvIworld[6] = 
            pinvIworld[8] = pinvIworld[9] = 0.0f;
        }

        pinvIworld[3] = (*pb)->invmass;
        pinvIworld[7] = pinvIworld[11] = 0;
        
        if((*pb)->flags&MdtKeaBodyFlagAddCoriolisForce)
        {


            /* add coriolis force = omega (R^T I R ) cross omega */

	    __asm__ __volatile__("
	    __expression_asm

	    nif
	    angvel
	    tm0
	    tm1
	    tm2
	    i0
	    i1
	    i2
	    ii0
	    ii1
	    ii2
	    tmp
	    torque

	    'lqc2  @angvel, 0x0(%0)
	    'lqc2  @tm0, 0x0(%1)
	    'lqc2  @tm1, 0x10(%1)
	    'lqc2  @tm2, 0x20(%1)
	    'lqc2  @i0, 0x0(%2)
	    'lqc2  @i1, 0x10(%2)
	    'lqc2  @i2, 0x20(%2)
	    'lqc2  @torque, 0x0(%3)

	    # Rt * Ibody * R

	    ACC.xyz = tm0 * i0.x
	    ACC.xyz = ACC + tm1 * i0.y
	    i0.xyz  = ACC + tm2 * i0.z

	    ACC.xyz = tm0 * i1.x
	    ACC.xyz = ACC + tm1 * i1.y
	    i1.xyz  = ACC + tm2 * i1.z

	    ACC.xyz = tm0 * i2.x
	    ACC.xyz = ACC + tm1 * i2.y
	    i2.xyz  = ACC + tm2 * i2.z

	    ACC.xyz = tm0 * i0.x
	    ACC.xyz = ACC + tm1 * i1.x
	    ii0.xyz = ACC + tm2 * i2.x

	    ACC.xyz = tm0 * i0.y
	    ACC.xyz = ACC + tm1 * i1.y
	    ii1.xyz = ACC + tm2 * i2.y

	    ACC.xyz = tm0 * i0.z
	    ACC.xyz = ACC + tm1 * i1.z
	    ii2.xyz = ACC + tm2 * i2.z

	    # tmp = angvel * (that)

	    ACC = ii0 * angvel.x
	    ACC = ACC + ii1 * angvel.y
	    tmp = ACC + ii2 * angvel.z

	    # Cross-product tmp and angvel

	    'vopmula.xyz  @ACC, @tmp, @angvel
	    'vopmsub.xyz  @nif, @angvel, @tmp

            #  'sqc2  @nif, 0x0(%3)

	    torque = torque + nif
	    'sqc2  @torque, 0x0(%3)

	    ~nif
	    ~angvel
	    ~tm0
	    ~tm1
	    ~tm2
	    ~i0
	    ~i1
	    ~i2
	    ~ii0
	    ~ii1
	    ~ii2
	    ~torque
	    ~tmp

	    __end_expression_asm
            " : : "r" ((*pb)->velrot), "r" (tb->R0), "r" ((*pb)->I0), "r" ((*pb)->torque));

    /*
            MeReal wrt[3],wrti[3], wrtir[3];
            const MeReal *omega=(*pb)->velrot;


            wrt[0] = omega[0]*tb->R0[0] + omega[1]*tb->R0[1] + omega[2]*tb->R0[2];
            wrt[1] = omega[0]*tb->R1[0] + omega[1]*tb->R1[1] + omega[2]*tb->R1[2];
            wrt[2] = omega[0]*tb->R2[0] + omega[1]*tb->R2[1] + omega[2]*tb->R2[2];

            for(i=0;i<3;i++)
            {
                wrti[i] = wrt[0]*(*pb)->I0[i] 
                        + wrt[1]*(*pb)->I1[i] 
                        + wrt[2]*(*pb)->I2[i];
            }

            for(i=0;i<3;i++)
            {
                wrtir[i] = wrti[0]*tb->R0[i] 
                         + wrti[1]*tb->R1[i] 
                         + wrti[2]*tb->R2[i];
            }

            (*pb)->torque[0]+=wrtir[1]*omega[2]-wrtir[2]*omega[1];
            (*pb)->torque[1]+=wrtir[2]*omega[0]-wrtir[0]*omega[2];
            (*pb)->torque[2]+=wrtir[0]*omega[1]-wrtir[1]*omega[0];
    */









        }

		tb++;
        pb++;
        pinvIworld += sizeof(MdtKeaInverseMassMatrix) / sizeof(MeReal);
    }

    /* Calculate vhmf:=v/h+inverse(M)*fe */

    MdtKeaInverseMassMatrix *  invMworld = (MdtKeaInverseMassMatrix *)invIworld;
    MeReal                     hinv      = MeRecip(stepsize);
    MdtKeaVelocity *           pvhmf     = vhmf;

    for(body=0;body!=num_bodies;body++)
    {
	__asm__ __volatile__("
	__expression_asm

	vhmf0
	vhmf1
	vel
	angvel
	# hm has hinv in x field
	hm
	invM0
	invM1
	invM2

	force
	torque

        'mfc1   $8,%4
        'qmtc2  $8,@hm

	'lqc2  @vel, 0x0(%0)
	'lqc2  @angvel, 0x10(%0)
	'lqc2  @invM0, 0x0(%1)
	'lqc2  @invM1, 0x10(%1)
	'lqc2  @invM2, 0x20(%1)

	'lqc2  @force,0x0(%3)
	'lqc2  @torque,0x10(%3)

	ACC = vel * hm.x
	vhmf0 = ACC + force * invM0.w

	# Taking advantage of the fact that invM is symmetric here, so we
	# don't have to do nasty dot products

	ACC = angvel * hm.x
	ACC = ACC + invM0 * torque.x
	ACC = ACC + invM1 * torque.y
	vhmf1 = ACC + invM2 * torque.z

        # Better make sure that's zero; it was before.
	vhmf1.w = K * K.x

	'sqc2  @vhmf0, 0X0(%2)
	'sqc2  @vhmf1, 0X10(%2)

	~vhmf0
	~vhmf1
	~vel
	~angvel
	~hm
	~invM0
	~invM1
	~invM2
	~force
	~torque

	__end_expression_asm

	" : : "r" (blist[body]->vel),
              "r" (invMworld),
              "r" (pvhmf),
              "r" (blist[body]->force),
              "f" (hinv)
             : "$8");

        pvhmf++;
        invMworld++;
    }

#if PRINT_CALCIWORLDNONINERTIALFORCEANDVHMF_OUTPUT
    printCalcIworldNonInertialForceandVhmfOutput(vhmf,(MeReal *)invIworld,num_bodies);
#endif

#if PROFILE_KEA
    MdtKeaProfileEnd("ps2 calcIworldandNIForceandVhmf");
#endif
}
