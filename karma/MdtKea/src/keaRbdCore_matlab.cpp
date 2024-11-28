/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.5.2.1 $

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
#include "keaStuff.hpp"
#include <MeMath.h>
#include <windows.h>
#ifndef _BUILD_VANILLA
#include "keaCheckCPU_sse.hpp"
#endif
#include <MdtKeaProfile.h>
#include <MdtKea.h>
#include <MeMessage.h>
#include <MeAssert.h>
#include "keaMatlab.h"
#include <malloc.h>
#include <keaInternal.hpp>

inline MeReal getJValue(MeReal *jstore, int row, int col)
{
    return jstore[(row&~3)*12+col*4+(row&3)];
}

MeU8 *packVector(MeU8 *memory, MeReal *vector, int size)
{
    int i;
    for(i=0;i<size;i++)
    {
        *(MeReal *)memory = vector[i];
        memory+=sizeof(MeReal);
    }
    return memory;
}



MeU8 *packGlobals(MeU8 *memory, int nBodies, int nConstraints, int nRows, 
                  MdtKeaParameters *p)
{

    MeMatlabKeaParameters q;
    q.nBodies = nBodies;
    q.nConstraints = nConstraints;
    q.nRows = nRows;
    q.epsilon = p->epsilon;
    q.gamma = p->gamma;
    q.iterations = p->max_iterations;
    q.stepsize = p->stepsize;

    memcpy(memory,&q, sizeof(MeMatlabKeaParameters));
    memory += sizeof(MeMatlabKeaParameters);
    return memory;
}

MeU8 *packConstraint(MeU8 *memory, int body0, int body1, int size, 
                     MeReal *jstore, int startRow)
{
    int i,j;
    *(int *)memory = size;
    memory += sizeof(int);

    *(int *)memory = body0;
    memory += sizeof(int);

    *(int *)memory = body1;
    memory += sizeof(int);

    for(i=0;i<size;i++)
    {
        for(j=0;j<12;j++)
        {
            *(MeReal *)memory = getJValue(jstore,startRow+i,j);
            memory+=sizeof(MeReal);
        }
    }
    return memory;
}



static HANDLE sharedMem = 0,
              serverReady = 0,
              invokeServer = 0,
              clientLive = 0;

static MeU8 *mem = 0;


MeU8 *MdtKeaMatlabSharedSegment()
{
    if(!mem)
    {
        sharedMem = CreateFileMapping(0,0,PAGE_READWRITE,0,10*(1<<20),"MeMatlabSharedSection");
        serverReady = CreateSemaphore(0,0,1,"MeServerReadySemaphore");
        invokeServer = CreateSemaphore(0,0,1,"MeInvokeServerSemaphore");
        clientLive = CreateMutex(0,0,"MeClientLiveMutex");
        WaitForSingleObject(clientLive,INFINITE);
        
        puts("Got heartbeat mutex");
        mem = (MeU8 *)MapViewOfFile(sharedMem,FILE_MAP_ALL_ACCESS,0,0,10*(1<<20));
        puts("Waiting for server start");
        WaitForSingleObject(serverReady,INFINITE);
        puts("server started");
    }

    return mem;
}

/* assumes the bodies are all partitioned s.t. no two constraint partitions use a body */

int getUsedBodies(int *used,int *map,int *JBody,int numConstraints)
{
    int i,usedCt=0;
    
    for(i=0;i<numConstraints;i++)
    {
        int body0 = JBody[2*i],
            body1 = JBody[2*i+1];

        if(body0!=-1 && map[body0]==-1)
        {
            map[body0]=usedCt;
            used[usedCt++]=body0;
        }

        if(body1!=-1 && map[body1]==-1)
        {
            map[body1]=usedCt;
            used[usedCt++]=body1;
        }
    }

    return usedCt;
}


void calculateConstraintForces(MdtKeaConstraints *constraints, 
                               int partitionStartConstraint,
                               int partitionStartRow,
                               int partitionStartStrip,
                               int numConstraints)
{
    int strip = partitionStartStrip;
    int row = partitionStartRow;
    int i,j;

    for(i=partitionStartConstraint;i<partitionStartConstraint + numConstraints;i++)
    {
        MeReal *c = constraints->force+i*16;
        for(j=0;j<constraints->Jsize[i];j++)
        {
            const MeReal lambda = constraints->lambda[row];

            c[0] +=lambda * getJValue(constraints->Jstore,strip,0);
            c[1] +=lambda * getJValue(constraints->Jstore,strip,1);
            c[2] +=lambda * getJValue(constraints->Jstore,strip,2);

            c[4] +=lambda * getJValue(constraints->Jstore,strip,3);
            c[5] +=lambda * getJValue(constraints->Jstore,strip,4);
            c[6] +=lambda * getJValue(constraints->Jstore,strip,5);

            c[8] +=lambda * getJValue(constraints->Jstore,strip,6);
            c[9] +=lambda * getJValue(constraints->Jstore,strip,7);
            c[10] +=lambda * getJValue(constraints->Jstore,strip,8);

            c[12] +=lambda * getJValue(constraints->Jstore,strip,9);
            c[13] +=lambda * getJValue(constraints->Jstore,strip,10);
            c[14] +=lambda * getJValue(constraints->Jstore,strip,11);
            strip++;
            row++;
        }
        strip += 4;
    }
}



void MEAPI MdtKeaAddConstraintForces(MdtKeaConstraints  constraints, 
				                    	   MdtKeaBody *const  blist[],
									       int                num_bodies,
                                           MdtKeaParameters   parameters)
{
    /* No bodies mean no physics, so exit if no bodies */
    if (num_bodies == 0) return;

    int row = 0;
    int i,partition;

    MeU8 *hmem = MdtKeaMatlabSharedSegment();

    int partitionStartStrip = 0;
    int partitionStartConstraint = 0;
    int partitionStartRow = 0;
    
    
    int *used = (int *)_alloca(MeMathCEIL4(num_bodies)*sizeof(int));
    int *map = (int *)_alloca(MeMathCEIL4(num_bodies)*sizeof(int));

    for(i=0;i<num_bodies;i++) map[i] = -1;

    for (partition = 0; partition < constraints.num_partitions; partition++)    
    {
        MeU8 *memory = hmem;

        int i;

        int strip = partitionStartStrip;
        int constraint = partitionStartConstraint;

        int numConstraints = constraints.num_constraints_partition[partition];
        int numRows = constraints.num_rows_exc_padding_partition[partition];

        int usedBodies = getUsedBodies(used,map,
                                       constraints.Jbody+2*partitionStartConstraint,
                                       numConstraints);

        *(int *)memory = 0;     // tag to say it's a constraint solve
        memory += sizeof(int);

        memory = packGlobals(memory,usedBodies,numConstraints,numRows,&parameters);

        for(i=0;i<usedBodies;i++)
        {
            memcpy(memory,blist[used[i]],sizeof(MdtKeaBody));
            memory += sizeof(MdtKeaBody);
        }

        for(i=0;i<numConstraints;i++)
        {
            int size = constraints.Jsize[constraint];
            int index0 = constraints.Jbody[2*constraint];
            int index1 = constraints.Jbody[2*constraint+1];

            memory = packConstraint(memory,
                                    index0 == -1 ? index0 : map[index0],
                                    index1 == -1 ? index1 : map[index1],
                                    size,
                                    constraints.Jstore,
                                    strip);
            constraint++;
            strip += size+4;
        }

        memory = packVector(memory,constraints.lo+partitionStartRow, numRows);
        memory = packVector(memory,constraints.hi+partitionStartRow, numRows);
        memory = packVector(memory,constraints.slipfactor+partitionStartRow, numRows);
        memory = packVector(memory,constraints.xi+partitionStartRow, numRows);
        memory = packVector(memory,constraints.xgamma+partitionStartRow, numRows);
        memory = packVector(memory,constraints.c+partitionStartRow, numRows);

#ifdef ASYNC_DEBUG
        puts("invoking server");
#endif
        ReleaseSemaphore(invokeServer,1,0);
#ifdef ASYNC_DEBUG
        puts("Waiting for server ready");
#endif
        
        WaitForSingleObject(serverReady,INFINITE);

#ifdef ASYNC_DEBUG
        puts("server is done");
#endif
        memory = hmem + sizeof(int) + sizeof(MeMatlabKeaParameters);
 
        for(i=0;i<usedBodies;i++)
        {
            

            MdtKeaBody *b = (MdtKeaBody *)memory;
            MdtKeaBody *u = blist[used[i]];
            u->force[0] = b->force[0];
            u->force[1] = b->force[1];
            u->force[2] = b->force[2];
            u->torque[0] = b->torque[0];
            u->torque[1] = b->torque[1];
            u->torque[2] = b->torque[2];

            memory += sizeof(MdtKeaBody);
        }

        memcpy(constraints.lambda+partitionStartRow,memory,numRows*sizeof(MeReal));

        memory += numRows*sizeof(MeReal);

//        printf("%d bodies, %d rows\n", usedBodies,numRows);

        calculateConstraintForces(&constraints,
                        partitionStartConstraint,
                        partitionStartRow,
                        partitionStartStrip,
                        numConstraints);
        partitionStartStrip += 
            constraints.num_rows_inc_padding_partition[partition];

        partitionStartConstraint += numConstraints;

        partitionStartRow += MeMathCEIL4(numRows);
    }

    for (i=0; i!=num_bodies; i++)
    {
        MdtKeaBody *b = blist[i];
        MeReal *t = b->torque;

        b->accel[0]=b->force[0] * b->invmass;
        b->accel[1]=b->force[1] * b->invmass;
        b->accel[2]=b->force[2] * b->invmass;
        
        // this only works for spherical MoIs, should really do in the world! 
        
        b->accelrot[0] = b->invI0[0]*t[0] + b->invI0[1]*t[1] + b->invI0[2]*t[2];
        b->accelrot[1] = b->invI1[0]*t[0] + b->invI1[1]*t[1] + b->invI1[2]*t[2];
        b->accelrot[2] = b->invI2[0]*t[0] + b->invI2[1]*t[1] + b->invI2[2]*t[2];
      
    }
}


/* crap needed to replace equivalents in keaRbdCore_pc */

int MEAPI MdtKeaMemoryRequired(const int num_rows_exc_padding[],
                               int num_partitions,
                               int max_rows,
                               int max_bodies)
{
    return 0;
}

MeCPUResources MEAPI MdtKeaQueryCPUResources()
{
    return (MeCPUResources)MeUnknown;
}

void MdtFlushCache(int a)
{
}
void MdtSyncDCache(void *a, void *b)
{
}
void MdtInvalidDCache(void *a, void *b)
{
}
