/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/10 18:22:11 $ - Revision: $Revision: 1.4.2.3 $

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
*/
void keaFunctions_SSE :: calcIworldandNonInertialForceandVhmf(
         MdtKeaInverseMassMatrix    invIworld[],             /* Output */
         MdtKeaVelocity             vhmf[],                  /* Output */
         const MdtKeaBody *const    blist[],                 /* Input */
         const MdtKeaTransformation tlist[],                 /* Input */
         int                        num_bodies,              /* Input */
         MeReal                     stepsize)                /* Input */
{
    int body,i;

#if PRINT_CALCIWORLDNONINERTIALFORCEANDVHMF_INPUT
    printCalcIworldNonInertialForceandVhmfInput(blist,num_bodies);
#endif

#if PROFILE_MDTKEA
    MdtKeaProfileStart("calcIworldandNIForceandVhmf");
#endif

    /* invIworld := makespherical(I) */

    const MdtKeaBody *const* pb         = blist;
    const MdtKeaTransformation *tb      = tlist;
    MeReal *                 pinvIworld = (MeReal* )invIworld;

    for (body = 0; body != num_bodies; body++)
    {
        if((*pb)->flags&MdtKeaBodyFlagIsNonSpherical) 
        { 
            /* invIworld = R^T I^-1 R */ 
            int i,j; 
            MeReal m1[3][3],m2[3][3]; 

            for(i=0;i<3;i++) 
            { 
               for(j=0;j<3;j++) 
                   m1[i][j]=tb->R0[i]*(*pb)->invI0[j]+ 
                            tb->R1[i]*(*pb)->invI1[j]+ 
                            tb->R2[i]*(*pb)->invI2[j]; 
            } 

            for(i=0;i<3;i++) 
            { 
               for(j=0;j<3;j++) 
                   m2[i][j]=m1[i][0]*tb->R0[j]+ 
                            m1[i][1]*tb->R1[j]+ 
                            m1[i][2]*tb->R2[j]; 
            } 

            /* copy invIworld into the right place */ 
            for(i=0;i<3;i++) 
            { 
               pinvIworld[i]   = m2[0][i]; 
               pinvIworld[i+4] = m2[1][i]; 
               pinvIworld[i+8] = m2[2][i]; 
            } 
        } 
        else 
        {
            //MeReal s = (pb->I0[0] + pb->I1[1] + pb->I2[2])/(MeReal)3.0;
            //MeReal t = (pb->invI0[0] + pb->invI1[1] + pb->invI2[2])/(MeReal)3.0;

            /* Alternative way of making the diagonal moment of intertia by Richard T */
            /* This makes Topple look much better */

            MeReal s = (*pb)->I0[0];
            if((*pb)->I1[1] > s) s = (*pb)->I1[1];
            if((*pb)->I2[2] > s) s = (*pb)->I2[2];

            MeReal t = MeRecip(s);

            pinvIworld[0] = pinvIworld[ 5] = pinvIworld[10] = t;
            pinvIworld[1] = pinvIworld[2] = 
            pinvIworld[4] = pinvIworld[6] =
            pinvIworld[8] = pinvIworld[9] = 0.0f;
        }

        pinvIworld[3] = (*pb)->invmass;
        pinvIworld[7] = pinvIworld[11] = 0; 
        if((*pb)->flags&MdtKeaBodyFlagAddCoriolisForce) 
        { 
            int i; 

            /* add coriolis force = omega (R^T I R ) cross omega */ 

            MeReal wrt[3],wrti[3], wrtir[3], coriolis[3];; 
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

            coriolis[0] = wrtir[1]*omega[2]-wrtir[2]*omega[1]; 
            coriolis[1] = wrtir[2]*omega[0]-wrtir[0]*omega[2]; 
            coriolis[2] = wrtir[0]*omega[1]-wrtir[1]*omega[0]; 

            //  JohnH - I don't know why but I needed to cast off const!!! 

            ((MdtKeaBody *)*pb)->torque[0]+=coriolis[0]; 
            ((MdtKeaBody *)*pb)->torque[1]+=coriolis[1]; 
            ((MdtKeaBody *)*pb)->torque[2]+=coriolis[2]; 
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
        for(i=0;i!=3;i++) pvhmf->velocity[i] = hinv*blist[body]->vel[i] + blist[body]->force[i]*invMworld->invmass;
        pvhmf->pad0=0;

        for(i=0;i!=3;i++) pvhmf->angVelocity[i] = hinv*blist[body]->vel[i+4];
        for(i=0;i!=3;i++) pvhmf->angVelocity[i] = pvhmf->angVelocity[i] + blist[body]->torque[0] * invMworld->invI0[i];
        for(i=0;i!=3;i++) pvhmf->angVelocity[i] = pvhmf->angVelocity[i] + blist[body]->torque[1] * invMworld->invI1[i];
        for(i=0;i!=3;i++) pvhmf->angVelocity[i] = pvhmf->angVelocity[i] + blist[body]->torque[2] * invMworld->invI2[i];
        pvhmf->pad1=0;

        pvhmf++;
        invMworld++;
    }

#if PRINT_CALCIWORLDNONINERTIALFORCEANDVHMF_OUTPUT
    printCalcIworldNonInertialForceandVhmfOutput(vhmf,(MeReal *)invIworld,num_bodies);
#endif

#if PROFILE_MDTKEA
    MdtKeaProfileEnd("calcIworldandNIForceandVhmf");
#endif

}
