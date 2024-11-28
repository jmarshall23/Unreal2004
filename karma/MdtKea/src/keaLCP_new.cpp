/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/10 18:22:11 $ - Revision: $Revision: 1.7.2.5 $

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "keaLCPSolver.hpp"

#include "keaMatrix.hpp"

#ifndef PS2
#include "keaMatrix_PcSparse.hpp"
#include "keaMatrix_PcSparse_vanilla.hpp"
#endif

#ifndef _BUILD_VANILLA
#include "keaMatrix_PcSparse_SSE.hpp"
#endif
#include "MeMath.h"
#include "keaInternal.hpp"
#include "MeAssert.h"
#include "MdtKeaProfile.h"
#include "keaMemory.hpp"
#include "keaDebug.h"
#include "ReadWriteKeaInputToFile.h"
#include "MeSimpleFile.h"

#ifdef PS2
#include "eeregs.h"
#include "libpc.h"
#endif

extern MdtKeaDebugDataRequest * gDebug;
extern int                      gDebugDataFile;

extern void *poolstack[3];
extern int poolstack_ptr;
extern void *pool_ptr;

extern int file;
extern int partition;
int iteration;

int checkForCycles(
        const int I[],
        const int C[],
        const int hilohere[],
        const int clampedhere[],
        int       iteration,
        int       n)
{
    int i;
    int MASK=(0x7FFFFFFF)>>(30-iteration);

    for(i=0;i<n;i++) 
    {
        if(I[i]==0) 
        {
            //update unclamped interest
            MASK&=~clampedhere[i];
        }
        else {
            MASK&=clampedhere[i];
            //check hilo, updating mask for matching clamps
            if(MASK!=0)
                MASK&=( (C[i]&hilohere[i]) | ((~C[i])&(~hilohere[i])) );
        }
        if(MASK==0)
            break;
    }
    return MASK;
}
/* 
 * keaLCP
 *  
 *     Switch mode:
 *     0 for block murty.
 *     1 for block murty post cycle & common clamp restart thingy...
 *     2 for single index switching rule.
 *   
 */
int keaLCPSolver::solveLCP(
        keaMatrix *    A,
        MeReal *       b, 
        MeReal *       lower,
        MeReal *       upper,
        int            max_iterations,
        MeCPUResources cpuType,
        MeReal         velocityZeroTol)
{
    int iteration;
    int numClamped; 
    int numUnclamped;
    int firstBad;
    int inCycle;
    int someIndicesSwitched;
    int i;

    LOCAL_INT_ARRAY(I, n);
    LOCAL_INT_ARRAY(C, n);
    LOCAL_INT_ARRAY(clampedhere, n);
    LOCAL_INT_ARRAY(hilohere, n);
    LOCAL_INT_ARRAY(clamped, n);
    LOCAL_INT_ARRAY(unclamped, n);
    
    this->velocityZeroTol = velocityZeroTol;
    this->cpuType         = cpuType;
    this->A               = A;

    A->solve(x,   /* Output */
             b ); /* Input  */

    setUpper(upper);
    setLower(lower);

    /* Find the first index of the initial solve that doesnt satisfy the LCP condition */

    firstBad = getFirstBadIndex();

    if (firstBad != n)
    {                
        /*
            Make initial index set - as an attempt to guess the correct solution we
            make all violated indexes clamped and the rest are free.  I = 0 if index
            free, 1 or 2 if clamped at lower or upper bound.
        */
                
        copyXtoInitialSolve();
        getClampIndices(I,C);
                        
        memset(cached, 0, n*sizeof(int));
        memset(clampedValues, 0, c16c12n*sizeof(MeReal));
        
        /*
           First, try block murty
           During our travels, we might get into a cycle
           If we do, find a new starting point by looking at the most common indices pivoted on
           If this doesnt put us into another cycle, carry on
           Else exit this while loop

           On Entry:

           clampedhere          - clamped index set history
           hilohere             - type of clamp history
           max_block_iterations
           block_iterations
           clamped
           unclamped
           I
           C
        */

        iteration           = 0;
        inCycle             = 0;
        someIndicesSwitched = 1;

        /* Hack for testing */

        while(iteration!=max_iterations && !inCycle && someIndicesSwitched)
        {
#if PRINT_LCP_ITERATION
            printf("--iteration %d\n",iteration);
#endif
            keaPushPoolFrame();

            /* Record the current index set in the index set history */
            
            for (i = 0; i < n; i++) 
            {
                clampedhere[i] = (clampedhere[i]<<1)+(I[i]&1);
                hilohere[i]    = (hilohere[i]<<1)+(C[i]&1);
            }
                        
            numClamped = 0;
            numUnclamped = 0;
            
            setClampedValues(
                clamped,       /* Output */
                unclamped,     /* Output */
                &numClamped,   /* Output */
                &numUnclamped, /* Output */
                I,             /* Input  */
                C);            /* Input  */
            
            makeXandW(
                b,             /* */
                unclamped, 
                numUnclamped, 
                clamped, 
                numClamped);
            
            someIndicesSwitched = blockMurtyChooseNewIndices(
                I,            /* Output */ 
                C,            /* Output */
                clamped,      /* Input  */            
                unclamped,    /* Input  */
                numClamped,   /* Input  */
                numUnclamped);/* Input  */

            /* Check to see if we have used this index set before 
               ie check for cycles */

            if(someIndicesSwitched)
            {
                inCycle = checkForCycles(
                    I,           /* Input */
                    C,           /* Input */
                    hilohere,    /* Input */
                    clampedhere, /* Input */
                    iteration,   /* Input */
                    n);          /* Input */
                
                if(inCycle)
                {
                    /* If we are in a cycle, then try to find a new starting point
                       by looking at the most common indices pivoted on.
                       However, this new index set might cause a cycle, so update inCycle */
                    
                    inCycle=commonPivot(
                        inCycle, 
                        I, 
                        C, 
                        clampedhere, 
                        hilohere);                
                }
            }

            keaPopPoolFrame();

            iteration++;
        }
    }

#if PRINT_LCP_OUTPUT
    printLCPOutput(x,MeMathCEIL4(n));
#endif

    return iteration;
}
