#ifndef __KEAFUNCTIONS_HPP /* -*- mode: C++; -*- */
#define __KEAFUNCTIONS_HPP

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/26 16:47:09 $ - Revision: $Revision: 1.18.2.2 $

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
#include "keaDebug.h"
#include <keaMemory.hpp>

//Function bag for different platform optimizations
class keaFunctions
{
public:
    virtual void calcJinvMandRHS(
        MeReal                        rhs[],                /* Output */
        MdtKeaJBlockPair              jm[],                 /* Output */
        const MdtKeaJBlockPair        jstore[],             /* Input */
        const MeReal                  xgamma[],             /* Input */
        const MeReal                  c[],                  /* Input */
        const MeReal                  xi[],                 /* Input */
        const MdtKeaInverseMassMatrix invIworld[],          /* Input */
        const MdtKeaBl2BodyRow        bl2body[],            /* Input */
        const int                     jlen[],               /* Input */
        const MdtKeaVelocity          vhmf[],               /* Input */
        int                           num_bodies,           /* Input */
        int                           num_rows_exc_padding, /* Input */
        int                           num_rows_inc_padding, /* Input */
        MeReal                        stepsize,             /* Input */
        MeReal                        gamma)=0;             /* Input */

    virtual void calcIworldandNonInertialForceandVhmf(
        MdtKeaInverseMassMatrix    invIworld[],             /* Output */
        MdtKeaVelocity             vhmf[],                  /* Output */
        const MdtKeaBody *const    blist[],                 /* Input */
        const MdtKeaTransformation tlist[],                 /* Input */
        int                        num_bodies,              /* Input */
        MeReal                     stepsize) = 0;           /* Input */

    virtual void calculateConstraintAndResultantForces(
        MdtKeaBody *const          blist[],                 /* Output/input */
        MdtKeaForcePair            cforces[],               /* Output */
        const MdtKeaJBlockPair     Jstore[],                /* Input */
        const MdtKeaBodyIndexPair  Jbody[],                 /* Input */
        const MeReal               lambda[],                /* Input */
        const MdtKeaBl2BodyRow     bl2body[],               /* Input */
        const MdtKeaBl2CBodyRow    bl2cbody[],              /* Input */
        const int                  jlen[],                  /* Input */
        int                        num_rows_exc_padding,    /* Input */
        int                        num_rows_inc_padding,    /* Input */
        int                        num_constraints,         /* Input */
        int                        num_bodies) = 0;         /* Input */

    virtual void calculateAcceleration(
        MdtKeaBody *const             blist[],              /* input/output */
        const MdtKeaInverseMassMatrix invIworld[],          /* input */
        int                           num_bodies)=0;        /* input */

    virtual void platformInit() = 0;

    virtual void allocateMemory(
        keaTempMemory *   mem,
        MdtKeaConstraints constraints,
        int               num_bodies) = 0;

    void keaCloseDebugDataFile(int file);

    int checkPrintDebugInput(
        const MdtKeaConstraints  constraints,
		const MdtKeaParameters   parameters,
        const MdtKeaBody *const  blist[], 
		int                      num_bodies);


    void initPool(void *  ptr,                              /* Input */
                  int     size);                            /* Input */


    void makejlenandbl2body(
        int                        jlen_12padded[],         /* Output */
        int                        jlen[],                  /* Output */
        MdtKeaBl2BodyRow           bl2body_12padded[],      /* Output */
        MdtKeaBl2BodyRow           bl2body[],               /* Output */
        MdtKeaBl2CBodyRow          bl2cbody[],              /* Output */
        const MdtKeaBodyIndexPair  Jbody[],                 /* Input */
        const int                  Jsize[],                 /* Input */
        const int                  num_rows_inc_padding_partition[],  /* Input */
        const int                  num_rows_exc_padding_partition[],  /* Input */
        const int                  num_constraints_partition[],       /* Input */
        int                        num_constraints,         /* Input */
        int                        num_partitions);         /* Input */
};

//The Vanilla KeaFunctions bag o'tricks
class keaFunctions_Vanilla : public keaFunctions
{
public:
    void calcJinvMandRHS(
        MeReal                        rhs[],                /* Output */
        MdtKeaJBlockPair              jm[],                 /* Output */
        const MdtKeaJBlockPair        jstore[],             /* Input */
        const MeReal                  xgamma[],             /* Input */
        const MeReal                  c[],                  /* Input */
        const MeReal                  xi[],                 /* Input */
        const MdtKeaInverseMassMatrix invIworld[],          /* Input */
        const MdtKeaBl2BodyRow        bl2body[],            /* Input */
        const int                     jlen[],               /* Input */
        const MdtKeaVelocity          vhmf[],               /* Input */
        int                           num_bodies,           /* Input */
        int                           num_rows_exc_padding, /* Input */
        int                           num_rows_inc_padding, /* Input */
        MeReal                        stepsize,             /* Input */
        MeReal                        gamma);               /* Input */

    void calcIworldandNonInertialForceandVhmf(
        MdtKeaInverseMassMatrix    invIworld[],             /* Output */
        MdtKeaVelocity             vhmf[],                  /* Output */
        const MdtKeaBody *const    blist[],                 /* Input */
        const MdtKeaTransformation tlist[],                 /* Input */
        int                        num_bodies,              /* Input */
        MeReal                     stepsize);               /* Input */

    void calculateConstraintAndResultantForces(
        MdtKeaBody *const          blist[],                 /* Output/input */
        MdtKeaForcePair            cforces[],               /* Output */
        const MdtKeaJBlockPair     Jstore[],                /* Input */
        const MdtKeaBodyIndexPair  Jbody[],                 /* Input */
        const MeReal               lambda[],                /* Input */
        const MdtKeaBl2BodyRow     bl2body[],               /* Input */
        const MdtKeaBl2CBodyRow    bl2cbody[],              /* Input */
        const int                  jlen[],                  /* Input */
        int                        num_rows_exc_padding,    /* Input */
        int                        num_rows_inc_padding,    /* Input */
        int                        num_constraints,         /* Input */
        int                        num_bodies);             /* Input */

    void calculateAcceleration(
        MdtKeaBody *const             blist[],              /* input/output */
        const MdtKeaInverseMassMatrix invIworld[],          /* input */
        int                           num_bodies);          /* input */
    void platformInit();

    void allocateMemory(
        keaTempMemory *   mem,
        MdtKeaConstraints constraints,
        int               num_bodies);
};

#ifndef PS2

#ifndef _BUILD_VANILLA
//The P4 Kea solver bag o'tricks
class keaFunctions_SSE : public keaFunctions
{
public:
    void calcJinvMandRHS(
        MeReal                        rhs[],                /* Output */
        MdtKeaJBlockPair              jm[],                 /* Output */
        const MdtKeaJBlockPair        jstore[],             /* Input */
        const MeReal                  xgamma[],             /* Input */
        const MeReal                  c[],                  /* Input */
        const MeReal                  xi[],                 /* Input */
        const MdtKeaInverseMassMatrix invIworld[],          /* Input */
        const MdtKeaBl2BodyRow        bl2body[],            /* Input */
        const int                     jlen[],               /* Input */
        const MdtKeaVelocity          vhmf[],               /* Input */
        int                           num_bodies,           /* Input */
        int                           num_rows_exc_padding, /* Input */
        int                           num_rows_inc_padding, /* Input */
        MeReal                        stepsize,             /* Input */
        MeReal                        gamma);               /* Input */

    void calcIworldandNonInertialForceandVhmf(    
        MdtKeaInverseMassMatrix    invIworld[],             /* Output */
        MdtKeaVelocity             vhmf[],                  /* Output */
        const MdtKeaBody *const    blist[],                 /* Input */
        const MdtKeaTransformation tlist[],                 /* Input */
        int                        num_bodies,              /* Input */
        MeReal                     stepsize);               /* Input */

    void calculateConstraintAndResultantForces(
        MdtKeaBody *const          blist[],                 /* Output/input */
        MdtKeaForcePair            cforces[],               /* Output */
        const MdtKeaJBlockPair     Jstore[],                /* Input */
        const MdtKeaBodyIndexPair  Jbody[],                 /* Input */
        const MeReal               lambda[],                /* Input */
        const MdtKeaBl2BodyRow     bl2body[],               /* Input */
        const MdtKeaBl2CBodyRow    bl2cbody[],              /* Input */
        const int                  jlen[],                  /* Input */
        int                        num_rows_exc_padding,    /* Input */
        int                        num_rows_inc_padding,    /* Input */
        int                        num_constraints,         /* Input */
        int                        num_bodies);             /* Input */

    void calculateAcceleration(
        MdtKeaBody *const             blist[],              /* input/output */
        const MdtKeaInverseMassMatrix invIworld[],          /* input */
        int                           num_bodies);          /* input */

    void platformInit();

    void allocateMemory(
        keaTempMemory *   mem,
        MdtKeaConstraints constraints,
        int               num_bodies);
};
#endif

#else
/* 
 * The PS2 subclass 
*/

class keaFunctions_PS2: public keaFunctions
{
public:
    void calcJinvMandRHS(
        MeReal                        rhs[],                /* Output */
        MdtKeaJBlockPair              jm[],                 /* Output */
        const MdtKeaJBlockPair        jstore[],             /* Input */
        const MeReal                  xgamma[],             /* Input */
        const MeReal                  c[],                  /* Input */
        const MeReal                  xi[],                 /* Input */
        const MdtKeaInverseMassMatrix invIworld[],          /* Input */
        const MdtKeaBl2BodyRow        bl2body[],            /* Input */
        const int                     jlen[],               /* Input */
        const MdtKeaVelocity          vhmf[],               /* Input */
        int                           num_bodies,           /* Input */
        int                           num_rows_exc_padding, /* Input */
        int                           num_rows_inc_padding, /* Input */
        MeReal                        stepsize,             /* Input */
        MeReal                        gamma);               /* Input */

    void calcIworldandNonInertialForceandVhmf(
        MdtKeaInverseMassMatrix    invIworld[],             /* Output */
        MdtKeaVelocity             vhmf[],                  /* Output */
        const MdtKeaBody *const    blist[],                 /* Input */
        const MdtKeaTransformation tlist[],                 /* Input */
        int                        num_bodies,              /* Input */
        MeReal                     stepsize);               /* Input */

    void calculateConstraintAndResultantForces(
        MdtKeaBody *const          blist[],                 /* Output/input */
        MdtKeaForcePair            cforces[],               /* Output */
        const MdtKeaJBlockPair     Jstore[],                /* Input */
        const MdtKeaBodyIndexPair  Jbody[],                 /* Input */
        const MeReal               lambda[],                /* Input */
        const MdtKeaBl2BodyRow     bl2body[],               /* Input */
        const MdtKeaBl2CBodyRow    bl2cbody[],              /* Input */
        const int                  jlen[],                  /* Input */
        int                        num_rows_exc_padding,    /* Input */
        int                        num_rows_inc_padding,    /* Input */
        int                        num_constraints,         /* Input */
        int                        num_bodies);             /* Input */

    void calculateAcceleration(
        MdtKeaBody *const             blist[],              /* input/output */
        const MdtKeaInverseMassMatrix invIworld[],          /* input */
        int                           num_bodies);          /* input */

    void platformInit();

    void allocateMemory(
        keaTempMemory *   mem,
        MdtKeaConstraints constraints,
        int               num_bodies);
};
#endif

#endif
