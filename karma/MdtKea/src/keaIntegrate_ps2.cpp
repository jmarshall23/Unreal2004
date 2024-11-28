/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.23.2.2 $

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
#include <MdtKeaProfile.h>
#include "keaDebug.h"
#include "keaIntegrate.hpp"
#include <stdio.h>

/* integrate
 * ---------
 * On Entry:
 *
 * tlist                 - Array of 4*4 transformation matrices in column major order. Array length is num_bodies
 * blist[i].invmass      -
 * blist[i].flags        -
 * blist[i].force        -
 * blist[i].torque       -
 * blist[i].vel          -
 * blist[i].velrot       -
 * blist[i].qrot         -
 * blist[i].accel        -
 * blist[i].accelrot     -
 * blist[i].fastspinaxis -
 * invIworld             - Array of 3*4 symmetric matrices. Array length is num_bodies
 * parameters.stepsize   - timestep
 * num_bodies            - the number of bodies
 *
 * On Exit:
 *
 * tlist[i] (rotation part) =
 * tlist[i] (position part) =
 * blist[i].vel             =
 * blist[i].velrot          =
 * blist[i].qrot            =
 * blist[i].accel           =
 * blist[i].accelrot        =
 *
*/
void MEAPI MdtKeaIntegrateSystem(MdtKeaBody *const      blist[],                /* Input, output */
                                 MdtKeaTransformation * tlist,                  /* Output */
                                 int                    num_bodies,             /* Input */
                                 MdtKeaParameters       parameters              /* Input */
                                )
{
    int i;
    MdtKeaBody *pb;
    int *pt;

    MeVector4 stepsizevec;

#if PROFILE_KEA
    MdtKeaProfileStart("ps2 integrate");
#endif

    /*
       Print integrator input for debugging purposes
     */

#if PRINT_INTEGRATOR_INPUT

    printf("Integrator input:\n");
    for (i = 0; i != num_bodies; i++)
    {
#if PRINT_INTEGRATOR_INPUT_ACCEL
        printf("accel=\n");
        for (int k = 0; k != 8; k++)
            printf("%12.6f ", blist[i]->accel[k]);
        printf("\n");
#endif

		/* Note, forces are not actually used by integrator */

	    printf("body %3d: ",i);
		for(k=0;k!=3;k++) printf("% 8.2f ",blist[i]->force[k]); 
		for(k=0;k!=3;k++) printf("% 8.2f ",blist[i]->torque[k]); 
		printf("\n");

#if PRINT_INTEGRATOR_INPUT_TRANSFORMATIONS
        printMat44((MeReal *)(tlist+i));
        printf("\n");
#endif
        /*
        printf("%12.6f\n", b[i].invmass);

        printf("invIworld=\n");
        for (k = 0; k != 4; k++)
            printf("%12.6f ", invIworld[i * 12 + 4 * 0 + k]);
        printf("\n");
        for (k = 0; k != 4; k++)
            printf("%12.6f ", invIworld[i * 12 + 4 * 1 + k]);
        printf("\n");
        for (k = 0; k != 4; k++)
            printf("%12.6f ", invIworld[i * 12 + 4 * 2 + k]);
        printf("\n");
        */
    }
#endif

    stepsizevec[0] = parameters.stepsize;

    static MeVector4 oldConstantRegister __attribute__ ((aligned(16))) =
        { 0.0f, 0.5f, -1.0f, 1.0f };
    static unsigned int sineconstants[4] __attribute__ ((aligned(16))) =
        { 0xbe2aaaa4, 0x3c08873e, 0xb94fb21f, 0x362e9c14 };

    MeVector4 debugvalues[8] __attribute__ ((aligned(16)));

    for (i = 0, pt = (int *) tlist; i != num_bodies;
        i++, pt +=
        sizeof (MdtKeaTransformation) / 4)
    {
        pb = blist[i];

        __asm__ __volatile__(
            "
                 __expression_asm
            
                stepsize
                inversemass
            #    force
            #    invi0
            #    invi1
            #    invi2
            #    torque
                rotvel
                vel
                pos
            
                acceleration
                rotaccel
            #    temp3
            #    temp4
            #    temp5
            
                'lqc2  @vel,         0x90(%4)
            #    'lqc2  @force,       0x10(%4)
                'lqc2  @inversemass, 0x00(%4)
            #    'lqc2  @torque,      0x20(%4)
            #    'lqc2  @invi0,       0x00(%5)
            #    'lqc2  @invi1,       0x10(%5)
            #    'lqc2  @invi2,       0x20(%5)
                'lqc2  @acceleration, 0xc0(%4)
                'lqc2  @rotaccel,     0xd0(%4)
                'lqc2  @stepsize,    0(%0)
                'lqc2  @rotvel,      0xa0(%4)
                'lqc2  @pos,         0x30(%3)
            
            # (1) calculate new velocity
            
                ACC = vel + K.x
                #acceleration = force * inversemass.z
            #    ~force
            
            # While waiting, start to calculate rotational acceleration
            
                #temp3.xyz = torque * invi0
                #temp4.xyz = torque * invi1
                #temp5.xyz = torque * invi2
                vel = ACC + acceleration * stepsize.x
                ~acceleration
            
            #    ~invi0
            #    ~invi1
            #    ~invi2
            #    ~torque
            
            
            # (2) Then, calculate new rotational velocity
            # [No dependency on previous calc]
            
            #    rotaccel.x = temp3.x    + temp3.y
            #    rotaccel.y = temp4.y    + temp4.x
            #    rotaccel.z = temp5.z    + temp5.x
            # stall cycle in rotaccel calc.
            #    'sqc2  @acceleration,0xc0(%4)
            #    rotaccel.x = rotaccel.x + temp3.z
            #    rotaccel.y = rotaccel.y + temp4.z
            #    rotaccel.z = rotaccel.z + temp5.y
                ACC = rotvel + K.x
            #    ~temp3
            #    ~temp4
            #    ~temp5
            #    ~acceleration
            
            
                quat
                oldconst
            
            # Load the quaternion - qrot
                'lqc2  @quat, 0xb0(%4)
            # Load the old style (EB2000) constant register
                'lqc2  @oldconst, 0x00(%2)
            
                rotvel = ACC + rotaccel * stepsize.x
            
            # (3) Update the position [Depends on (1)]
            
                ACC = pos + K.x
                pos.xyz = ACC + vel * stepsize.x
            
            # At this point: Add <<dodgy hack>> (seems reasonable
            # though) for fast-spinning objects
            # Also, another <<dodgy hack>> to change quat representation
            # by wfgg.
                newstylequat
            # This latter <<dodgy hack>> will be bubbled out
            # after GDC I suppose.
            
            # if (b->fastSpin)  (This is not likely in general case)
                'lw $8, 0x00(%5)
                'andi $8,$8,1
            # while waiting for load
                'sqc2 @vel, 0x90(%4)
                'sqc2 @rotvel, 0xa0(%4)
            #    'sqc2 @rotaccel, 0xd0(%4)
                ~rotaccel
            
                'beql $8, $0, 0f
            
            # 'Reference' implementation in keaMathStuff.cpp
            # keaDoFiniteRotation is the original function's name
            
            #######
                rot
                fastSpinAxis
                wlen
                h
                tmp
                nw
                S
                wlen1
                X
                t1
            #######
            
                'lqc2 @fastSpinAxis, 0x00(%6)
                'vmr32 @newstylequat, @quat
                'lqc2 @S, 0x00(%1)
                rot = rotvel * fastSpinAxis
                wlen = fastSpinAxis * fastSpinAxis
                ACC.xyzw = K + K.w
            
                rot.x = rot.x + rot.y
                wlen.x = wlen.x + wlen.y
            
            
                rot.x = rot.x + rot.z
                wlen.x = wlen.x + wlen.z
            
            
                h.x = rot.x * stepsize.x
                Q = | wlen.x
            
            
               
                'vwaitq
                wlen.x = K.x + Q
             
            
                Q = K.w /  wlen.x
                X.x = h.x * wlen.x
            
             
                X.x = X.x * oldconst.y
            
                'vwaitq
                wlen1.x = K.x + Q
                t1.x = X * X
            
            
                nw.xyz = fastSpinAxis.xyz * wlen1.x
            #
            ######
                ~h
                ~wlen
                ~wlen1
                t2
                t3
                t4
                s
                q0nw
            ######
            #
                t2.x = t1 * t1
                ACC.x = ACC.x + t1.x * S.x
                q0nw.xyz = nw.xyz * newstylequat.w
                fastSpinAxis.xyz = fastSpinAxis * rot.x
                t3.x = t2 * t1
                t4.x = t2 * t2
                ACC.x = ACC.x + t2.x * S.y
                rotvel = rotvel - fastSpinAxis
                ACC.x = ACC.x + t3.x * S.z
                ACC.x = ACC.x + t4.x * S.w
                s.x = ACC.x + K.x * K.x
                'vopmula ACC, @newstylequat, @nw
            
            
                s.x = s.x * X.x
            #
            ######
                ~X
                ~t1
                ~t2
                ~t3
                ~t4
                ~S
                c
                newq
                dot
            ######
            #
                'vopmsub @tmp, @nw, @newstylequat
            
            
                c.x = s.x * s.x
                tmp.xyz = q0nw - tmp
            
            
                c.w = K.w - c.x
                ACC.xyz = tmp.xyz * s.x
            
            
                Q = | c.w
                dot.xyz = newstylequat * nw
            
            
                 dot.xyz = dot.xyz * s.x
            
            
                 dot.x = dot.x + dot.y
            
            
                'vwaitq
                dot.x = dot.x + dot.z
                c.x = K.x + Q
            
            
            
                newq.w = newstylequat.w * c.x
                newstylequat.xyz = ACC.xyz + newstylequat.xyz * c.x
            
            
                newq.w = newq.w - dot.x
            #
            ######
                ~dot
                ~nw
                ~q0nw
                ~tmp
                ~s
                ~c
            ######
            #
              
                newstylequat.w = newq.w + K.x
            
            
            
                'vmr32 @newstylequat, @newstylequat
                'vmr32 @newstylequat, @newstylequat
                'vmr32 @quat, @newstylequat
            #
            ######
                ~newq
                ~rot
                ~fastSpinAxis
                ~newstylequat
            ######
            #
                '0:
            ###############################################################
            
            # Register allocation now carried out by eac ...
            # Register allocation:             vf01 -> temp, quat''
            #                  vf02 -> quat
            #                  vf03 -> quat'
            #                  vf04 -> quat''
            #                  vf05 -> qmul
            #                  vf06 -> qmul'
            #                  vf07 -> qmul''
            #                  vf08 -> ans
            #                  vf09 -> ans'
            #                  vf10 -> ans''
            #                  the rest are temps :¬j
            
            # **Create dq** (vf31) from q(vf02) and W(@rotvel)
            # ... col1-3 are vf27-9
            
                temp1
                temp2
                temp3
                temp4
                temp5
            
                pos.w = K.w + K.x
                temp3 = quat * rotvel.z
                temp2 = quat * rotvel.y
                temp1 = quat * rotvel.x
                'sqc2 @pos, 0x30(%3)
            
                temp4.z = temp2.z + temp3.w
                temp4.w = temp2.w + temp1.x
                temp4.x = temp2.x + temp3.y
                temp4.y = temp2.y - temp1.z
            
            
            
                temp5.y = temp1.y + temp4.z
                temp5.z = temp3.z - temp4.w
                temp5.w = temp1.w - temp4.x
            
                temp5.x = temp3.x - temp4.y
            
            
            
                'vmr32.xyzw @temp1, @temp5
            
            
                ACC = quat + K.x
                temp1 = temp1 * oldconst.y
            
            
            
                temp1.xyz = temp1.xyz * oldconst.z
            
            
            
                quat = ACC + temp1 * stepsize.x
            
            
              
                temp2 = quat * quat
            
             
                temp1.x = temp2.x + temp2.y
                temp1.z = temp2.z + temp2.w
            
            
            
                temp1.x = temp1.x + temp1.z
            
                Q = K.w / | temp1.x
            
             
            
            
                 'vwaitq 
                quat = quat * Q
            
                ~temp5
                ~temp4
                ~temp3
                ~temp2
                ~temp1
            
            ###########
            
                q
            # to new q-rep
                'vmr32 @q, @quat
            # Normalized Quaternion is now stored again.
                'sqc2 @quat, 0xb0(%4)
                #start dilip's qtor
                ~quat
                q2minqv2
                twoq
                twoq0qv
                v0
                v1
                v2
                m0
                m1
                m2
                one
            #
            #####
                v2 = K - K
                m0 = K - K
                twoq = q + q
                m1 = K - K
                one = v2 + K.w
                v2.z = v2 + K.w
                ACC = twoq * q
                q2minqv2.w = ACC - K * K.w
                twoq0qv = q * twoq.w
                m2 = K - K
                'vmr32 @v1, @v2
                'vopmula ACC, @twoq0qv, @v2
                ACC = ACC + q * twoq.z
                ACC.z = ACC + one * q2minqv2.w
                'vopmsub @m2, @v2, @twoq0qv
                'vmr32 @v0, @v1
                'vopmula ACC, @twoq0qv, @v1
                ACC = ACC + q * twoq.y
                ACC.y = ACC + one * q2minqv2.w
                'vopmsub @m1, @v1, @twoq0qv
                'vopmula ACC, @twoq0qv, @v0
                ACC = ACC + q * twoq.x
                ACC.x = ACC + one * q2minqv2.w
                'vopmsub @m0, @v0, @twoq0qv
                'sqc2 @m2, 0x20(%3)
                'sqc2 @m1, 0x10(%3)
            #
                'sqc2 @m0, 0x0(%3)
                ##### 30 cycles! I should have rtfm
            #
                ~q2minqv2
                ~twoq
                ~twoq0qv
                ~v0
                ~v1
                ~v2
                ~m0
                ~m1
                ~m2
                ~one
                ~q
            
                ~oldconst
                ~stepsize
                ~inversemass
            #    ~force
            #    ~invi0
            #    ~invi1
            #    ~invi2
            #    ~torque
                ~rotvel
                ~vel
                ~pos
                __end_expression_asm
"
            : :
            "r" (&(stepsizevec)),                          /* %0 */
            "r" (&(sineconstants[0])),                     /* %1 */
            "r" (&oldConstantRegister[0]),                 /* %2 */
            "r" (pt),                                      /* %3 */
            "r" (pb),                                      /* %4 */
            "r" (&( pb->flags)),                           /* %5 */
            "r" (&( pb->fastSpinAxis[0])),                 /* %6 */
            "r" (&(debugvalues[0][0]))                     /* %7 */
            :
            "$8"
        );
    }
    /*
       Print output data for debugging purposes
     */

#if PRINT_INTEGRATOR_OUTPUT
    MdtKeaBody *b = (MdtKeaBody *) blist;
    MdtKeaTransformation *t = (MdtKeaTransformation *) tlist;
    int k;

    for (i = 0; i != num_bodies; i++)
    {
        printf("force= ");
        for (k = 0; k != 8; k++)
            printf("%12.6f ", b[i].force[k]);
        printf("\n");

        printf("transformation=\n");
        for (k = 0; k != 4; k++)
            printf("%12.6f ", t[i].R0[k]);
        printf("\n");
        for (k = 0; k != 4; k++)
            printf("%12.6f ", t[i].R1[k]);
        printf("\n");
        for (k = 0; k != 4; k++)
            printf("%12.6f ", t[i].R2[k]);
        printf("\n");
        for (k = 0; k != 4; k++)
            printf("%12.6f ", t[i].pos[k]);
        printf("\n");
    }
#endif

#if PROFILE_KEA
    MdtKeaProfileEnd("ps2 integrate");
#endif

}
