/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   $Date: 2002/11/05 11:08:02 $ $Revision: 1.14.2.13.4.1 $

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

/*
  "It all comes from here, the stench and the fear."
*/

#include <stdio.h>
#include <math.h>
#include <string.h>

#include <MeMath.h>
#include <MdtKeaProfile.h>

#include "keaStuff.hpp"

#include "keaCalcIworldandNonInertialForceandVhmf.h"
#include "keaMakejlenandbl2body.hpp"

#include "keaMatrix.hpp"
#include "keaMatrix_PcSparse.hpp"

#include <keaMatrix_tester.hpp>
#include <keaInternal.hpp>
#include <keaDebug.h>

#if (defined PS2)
#   include <keaMatrix_ps244smalldense.hpp>
#   include <keaMatrix_ps2smalldense.hpp>
#   include <keaMatrix_ps2sparse.hpp>
#endif

#if (defined _BUILD_VANILLA)
#   include <keaMatrix_PcSparse_vanilla.hpp>
#elif (defined WIN32)
#   include "keaCheckCPU_sse.hpp"
#   include <keaMatrix_PcSparse_sse.hpp>
#   include <keaMatrix_PcSparse_vanilla.hpp>
#elif (defined _XBOX)
#   include <keaMatrix_PcSparse_sse.hpp>
#else
#   include <keaMatrix_PcSparse_vanilla.hpp>
#endif

#include "keaLCPSolver.hpp"

#include "keaFunctions.hpp"
#include "keaInternal.hpp"
#include <MeProfile.h>
#include <MdtKea.h>
#include <MeMessage.h>
#include <MeAssert.h>
#include "keaDebug.h"
#include "keaMemory.hpp"
#include "ReadWriteKeaInputToFile.h"
#include "MeSimpleFile.h"

/*
 * Memory pool management
 */

void                   *poolstack[3];
int                     poolstack_ptr;
/* The address of the next free address in the pool */
void                   *pool_ptr;
/* The address of the end of the pool */
void                   *pool_max;

/*
 * Globals used for debugging
 */

int                     gDebugDataFile;
int                     gPartition;
MdtKeaDebugDataRequest  *gDebug;

#if TEST_KEAMATRIX
keaMatrix_pcSparse_vanilla correctMatrix;
keaMatrix_tester           matrixComparator;
#endif

void MEAPI MdtKeaAddConstraintForces(
               MdtKeaConstraints            pconstraints, 
               MdtKeaBody *                 blist[],
               const MdtKeaTransformation   tlist[],
               int                          num_bodies,
               MdtKeaParameters             parameters)
{
    int i;
    keaTempMemory        mem;
    MdtKeaConstraints    constraints;
    int *                jlen_12padded;                    
    MdtKeaBl2CBodyRow *  bl2body_12padded;                 
    MdtKeaJBlockPair *   jm;                   
    MeReal *             rhs;                  

    /*
     * keaFuncs is optimised for each platform
     *
     * The factory decides which optimised version to use for the
     * current platform
     */
        
#if (defined PS2)
    keaFunctions_PS2            ps2Functions;
    keaFunctions *const         keaFuncs = &ps2Functions;
#endif

#if (defined _BUILD_VANILLA)
    keaFunctions_Vanilla        vanillaFunctions;
    keaFunctions *const         keaFuncs = &vanillaFunctions;
#elif (defined WIN32)
    keaFunctions_SSE            sseFunctions;
    keaFunctions_Vanilla        vanillaFunctions;

    keaFunctions *const         keaFuncs =
        (parameters.cpu_resources == (MeCPUResources) MeSSE)
        ? (keaFunctions *) &sseFunctions
        : (keaFunctions *) &vanillaFunctions;
#elif (defined _XBOX)
    keaFunctions_SSE            sseFunctions;
    keaFunctions *const         keaFuncs = &sseFunctions;
#else
    keaFunctions_Vanilla        vanillaFunctions;
#   if (!defined PS2)
        keaFunctions *const     keaFuncs = &vanillaFunctions;
#   endif
#endif

    MdtKeaProfileStart("init");

    /* 
     * No bodies mean no physics, so exit if no bodies 
    */

    if (num_bodies == 0) return;

    gDebug = &parameters.debug;

#ifdef _MECHECK
    for(i=0; i<pconstraints.num_rows_exc_padding; i++)
    {
        if(pconstraints.slipfactor[i] < 0)
        {
            MeWarning(0, "MdtKeaAddConstraintForces: Negative slipfactor.");
            pconstraints.slipfactor[i] = 0;
        }
    }
#endif

    /*
     * Check input for null pointers
     * If the user has requested debug data logging then open a file
     * If user has requested that input is read from a file, then read it
     * Print input if user requested it
     */

    gDebugDataFile = keaFuncs->checkPrintDebugInput(
        pconstraints,
        parameters,
        blist, 
        num_bodies);

    /* 
     * Do platform specific initialisation
     * On PS2 we need to kill toSPR, fromSPR and vif0 interrupts to make
     * it work with renderware
     */
    keaFuncs->platformInit();

    /*
     * All kea memory allocation is done from a pool
     * Initialise this pool
     */

    keaFuncs->initPool(parameters.memory_pool, parameters.memory_pool_size);

    /*
     * Allocate memory for rhs, vhmf, JM, invIworld etc     
     * (Also sets flags on pointers on PS2 to force use of uncached
     * accelerated memory)
     */

    keaFuncs->allocateMemory(&mem,pconstraints,num_bodies);

    MdtKeaProfileEnd("init");

    /*
     * Make invIworld and vhmf
     */

    keaFuncs->calcIworldandNonInertialForceandVhmf(
        mem.invIworld,                   /* Output       */
        mem.vhmf,                        /* Output       */
        blist,                           /* Output/Input */
        tlist,                           /* Input        */
        num_bodies,                      /* Input        */
        parameters.stepsize);            /* Input        */

    /*
     *  Make the tables required to manipulate J quickly
     *  These tables are:
     *  bl2body
     *  jlen
     *  bl2cony
     *
     *  Their use is explained in 'An Optimised Representation of the Kea 
     *  Jacobian Matrix' by R.Tonge
    */

    keaFuncs->makejlenandbl2body(
        mem.jlen_12padded,                            /* Output */
        mem.jlen,                                     /* Output */
        mem.bl2body_12padded,                         /* Output */
        mem.bl2body,                                  /* Output */
        mem.bl2cbody,                                 /* Output */
        pconstraints.Jbody,                           /* Input */
        pconstraints.Jsize,                           /* Input */
        pconstraints.num_rows_inc_padding_partition,  /* Input */
        pconstraints.num_rows_exc_padding_partition,  /* Input */
        pconstraints.num_constraints_partition,       /* Input */
        pconstraints.num_constraints,                 /* Input */
        pconstraints.num_partitions);                 /* Input */

    /* 
     * Given J and inverse(M), calculate J * inverse(M)
     * Given c, xi, xgamma, gamma, J and vhmf, calculate rhs
    */

    keaFuncs->calcJinvMandRHS(
        mem.rhs,                                                /* Output */
        mem.jm,                                                 /* Output */
        pconstraints.Jstore,                                    /* Input */
        pconstraints.xgamma,                                    /* Input */
        pconstraints.c,                                         /* Input */
        pconstraints.xi,                                        /* Input */
        mem.invIworld,                                          /* Input */
        mem.bl2body,                                            /* Input */
        mem.jlen,                                               /* Input */
        mem.vhmf,                                               /* Input */
        num_bodies,                                             /* Input */
        MeMathCEIL4(pconstraints.num_rows_exc_padding),         /* Input */
        pconstraints.num_rows_inc_padding,                      /* Input */
        parameters.stepsize,                                    /* Input */
        parameters.gamma);                                      /* Input */

    /*
     *  Given JM and J calculate A := JM*transpose(J)
     *  Calculate Achol := cholesky(A)
     *  Given (A, Achol, rhs, lo, hi) , solve LCP
    */

    constraints      = pconstraints;
    jlen_12padded    = mem.jlen_12padded;
    bl2body_12padded = mem.bl2body_12padded;
    jm               = mem.jm;
    rhs              = mem.rhs;

    for (gPartition = 0;
         gPartition != constraints.num_partitions;
         gPartition++)
    {
        if (constraints.num_constraints_partition[gPartition] > 0)
        {
            if(parameters.debug.writeKeaInterData && 
               gDebug->frame==gDebug->badFrame)
            {
                writeIntToFile(gDebugDataFile,"partition",gPartition);
            }

            keaPushPoolFrame();


            int num_constraints        = 
                constraints.num_constraints_partition[gPartition];
            int num_rows               = 
                constraints.num_rows_exc_padding_partition[gPartition];

            int ceil4_num_rows         = MeMathCEIL4(num_rows);
            int ceil12_num_rows        = MeMathCEIL12(num_rows);
            int num_strips             = ceil4_num_rows / 4;
            int ceil3_num_strips       = ceil12_num_rows / 4;


            /*
               Select the keaLCPMatrix subclass that is appropriate for this 
               platform and this partition's size
               Then, ask it to allocate memory for the given number of rows
               Then, ask it to initialise it's matrix given jm, j and epsilon
               Then, ask it to set its Achol matrix to the cholesky 
               factorisation of A
            */

            {                
                keaLCPSolver kSolver;

#if (defined PS2)
                keaMatrix_ps244smalldense   ps244DenseAMatrix;
                keaMatrix_ps2smalldense     ps2SmallDenseAMatrix;
                keaMatrix_ps2sparse         ps2SparseAMatrix;
#   if (!defined _BUILD_VANILLA)
                keaMatrix *const            A =
                    (num_rows <= 4) ? (keaMatrix *) &ps244DenseAMatrix
                    : (num_rows <= 36) ? (keaMatrix *) &ps2SmallDenseAMatrix
                    : (keaMatrix *) &ps2SparseAMatrix;
#   endif
#endif

#if (defined _BUILD_VANILLA)
                keaMatrix_pcSparse_vanilla  vanillaAMatrix;
                keaMatrix *const            A = &vanillaAMatrix;
#elif (defined WIN32)
                keaMatrix_pcSparse_SSE      sseAMatrix;
                keaMatrix_pcSparse_vanilla  vanillaAMatrix;
                keaMatrix *const            A =
                    (parameters.cpu_resources == (MeCPUResources) MeSSE)
                    ? (keaMatrix *) &sseAMatrix
                    : (keaMatrix *) &vanillaAMatrix;
#elif (defined _XBOX)
                keaMatrix_pcSparse_SSE      sseAMatrix;
                keaMatrix *const            A = &sseAMatrix;
#else
                keaMatrix_pcSparse_vanilla  vanillaAMatrix;
#   if (!defined PS2)
                keaMatrix *const            A = &vanillaAMatrix;
#   endif
#endif
             
#if TEST_KEAMATRIX
                /* If we are in debug mode, run whatever matrix subclass
                   the factory chose in parallel with the pc vanilla matrix
                   and compare the results
                */
                matrixComparator.suspect=A;
                matrixComparator.correct=&correctMatrix;
                A=&matrixComparator;
#endif
                A->allocate(num_rows);
                kSolver.allocate(num_rows);
                
                A->makeFromJMJT((MeReal *)jm,     /* Input */
                    (MeReal *)constraints.Jstore, /* Input */
                    jlen_12padded,                /* Input */
                    (int *)bl2body_12padded,      /* Input */
                    constraints.slipfactor,       /* Input */
                    parameters.epsilon,           /* Input */
                    MeRecip(parameters.stepsize));/* Input */                                

                A->factorize();                                

                /* On PS2, we need to write Achol back to memory if it is in vumem */
                
                A->writebackMatrixChol();
                
                /*
                Solve the LCP given by KeaLCPMatrix A, rhs, lo and hi, 
                placing the result in lambda
                */
                
                MdtKeaProfileStart("LCP");
                
                kSolver.solveLCP(
                    A,
                    rhs, 
                    constraints.lo, 
                    constraints.hi,
                    parameters.max_iterations,
                    parameters.cpu_resources,
                    parameters.velocityZeroTol);
                /*
                 *  lambda := A->x
                */

                for(i=0;i!=ceil4_num_rows;i++)
                {
                    constraints.lambda[i] = kSolver.x[i];
                }
            }

            MdtKeaProfileEnd("LCP");

            jm      = jm + 
                      constraints.num_rows_inc_padding_partition[gPartition]/2 
                      * MdtKeaMAXBODYCONSTRAINT/4;

            rhs               = rhs              + num_strips * 4;
            jlen_12padded     = jlen_12padded    + ceil3_num_strips;
            bl2body_12padded  = bl2body_12padded + ceil3_num_strips;

            constraints.Jstore     += 
                constraints.num_rows_inc_padding_partition[gPartition]/4;

            constraints.Jbody      += num_constraints;
            constraints.xi         += ceil4_num_rows;
            constraints.c          += ceil4_num_rows;
            constraints.lo         += ceil4_num_rows;
            constraints.hi         += ceil4_num_rows;
            constraints.slipfactor += ceil4_num_rows;
            constraints.xgamma     += ceil4_num_rows;
            constraints.lambda     += ceil4_num_rows;
            constraints.force      += num_constraints;
            constraints.Jsize      += num_constraints;
            constraints.Jofs       += num_constraints;

            keaPopPoolFrame();
        }
    }

    /*
     *  Given lambda and J, calculate transpose(J)*lambda to get the constraint 
     *  forces. Store in pconstraints.force.
     *  Then add the constraint forces to the resultant forces of there corresponding bodies
     *  Note that on entry, the resultant force is initialised to the external force
    */

    keaFuncs->calculateConstraintAndResultantForces(
        blist,                                      /* Output/input */
        pconstraints.force,                         /* Output */
        pconstraints.Jstore,                        /* Input */
        pconstraints.Jbody,                         /* Input */
        pconstraints.lambda,                        /* Input */
        mem.bl2body,                                /* Input */
        mem.bl2cbody,                               /* Input */
        mem.jlen,                                   /* Input */
        pconstraints.num_rows_exc_padding,          /* Input */
        pconstraints.num_rows_inc_padding,          /* Input */
        pconstraints.num_constraints,               /* Input */
        num_bodies);                                /* Input */

    /*
     * For each body, calculate acceleration from resultant force and inverse 
     * mass matrix
    */

    keaFuncs->calculateAcceleration(
        blist,                      /* input/output */
        mem.invIworld,              /* input */
        num_bodies);                /* input */

    /*
     * If debug data logging enabled, close the debug info file
    */

    keaFuncs->keaCloseDebugDataFile(gDebugDataFile);
}

MeCPUResources MEAPI MdtKeaQueryCPUResources()
{
#if (defined PS2 || defined NGC)
    return (MeCPUResources)MeUnknown;
#elif (defined _BUILD_VANILLA)
    return (MeCPUResources)MeUnknown;
#elif (defined WIN32 || defined _XBOX)
    MeInfo(0,"Autodetecting CPU for SSE");

    CPUResources::DiscoverSIMDAvailablilty();

    if(CPUResources::KNIAvailable())
    {
        MeInfo(0,"Using SSE Optimizations");
        return (MeCPUResources)MeSSE;
    }

    MeInfo(0,"Using x86 Optimizations");
    return (MeCPUResources)MeX86;
#else
#   error MdtKeaQueryCPUResources: unsupported platform
#endif
}

