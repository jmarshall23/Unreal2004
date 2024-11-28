/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/11/04 16:04:26 $ - Revision: $Revision: 1.2.2.1.4.1 $

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

#include <keaFunctions.hpp>
#include <MdtKeaProfile.h>

void keaFunctions_Vanilla :: calculateAcceleration(
    MdtKeaBody *const             blist[],                  /* input/output */
    const MdtKeaInverseMassMatrix invIworld[],              /* input */
    int                           num_bodies)               /* input */
{
    int i,j;

    MdtKeaProfileStart("calcAcceleration");
    for (i=0; i!=num_bodies; i++)
    {
      for (j=0; j<3; j++)
                  blist[i]->accel[j]=blist[i]->force[j] * blist[i]->invmass;

      MeReal *invIw = (MeReal*)(invIworld + i);

      for(j=0; j<3; j++) blist[i]->accelrot[j]=
        (blist[i]->torque[0]*invIw[j*4+0] +
         blist[i]->torque[1]*invIw[j*4+1] +
         blist[i]->torque[2]*invIw[j*4+2]);
    }

    MdtKeaProfileEnd("calcAcceleration");
    
}

