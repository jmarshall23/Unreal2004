/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/10 18:22:11 $ - Revision: $Revision: 1.18.2.3 $

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

#include <string.h>
#include <MeAssert.h>
#include <stdio.h>
#include "keaCalcConstraintForces.hpp"
#include "keaEeDefs.hpp"
#include "cf_block.hpp"
#include <MeMath.h>
#include <MdtKeaProfile.h>
#include <keaFunctions.hpp>
#include <MdtKeaProfile.h>

/* TODO get it all just right, and you shouldn't have to flush the D$ every time
 * through the loop, which will save a *lot* of those precious cycles */

#define MIN(x,y) (((x)<(y)) ? (x) : (y))

/* Returns the number of constraint forces actually loaded. This will normally
 * be SUM_CFORCES_LEN, but, at the end, will be a bit less */
static int load(Sum_forces_buf* buf);

static const MdtKeaForcePair *      g_cforces;
static const MdtKeaBodyIndexPair *  g_jbody;
static int g_cforces_todo; // (todo actually means to load)
static Sum_forces_spr* g_spr;

static void load_bodies(const MdtKeaBody *const blist[],int num_bodies);
static void store_bodies(MdtKeaBody *const blist[],int num_bodies);

extern void dump_spr(); // todo just for debugging
// just for debugging
void zero_spr();

typedef struct _sf_dmachain
{
    sceDmaTag forces;
    sceDmaTag jbody;
    //sceDmaTag pad1;
    sceDmaTag pad2;
    sceDmaTag pad3;
} Sf_dmachain;

static Sf_dmachain g_dmachain __attribute__ ((aligned(64)))={
    {(SUM_CFORCES_LEN*sizeof(MdtKeaForcePair))/16,0,REF_ID,0,{0,0}}, // forces
    {0,0,REFE_ID,0,{0,0}},                                            // jbody
    //{0,0,0,0,{0,0}},
    {0,0,0,0,{0,0}},
    {0,0,0,0,{0,0}}
};

/* calculateResultantForces
 * ------------------------
 *
 * On Entry:
 *
 * blist           -
 * cforces         -
 * Jbody           - 
 * num_constraints -
 * num_bodies      -
 *
 * 
 *
 * Iterates over constraints
 *
 * Uses a scratchpad double buffer
*/
void calculateResultantForces(MdtKeaBody *const          blist[],                        /* input/output */
                              const MdtKeaForcePair      cforces[],                      /* input */
                              const MdtKeaBodyIndexPair  Jbody[],                        /* input */
                              int                        num_constraints,                /* input */
                              int                        num_bodies)                     /* input */
{
#if PRINT_CALC_RESULTANT_FORCES_INPUT
    printCalcResultantForcesInput(blist,cforces,num_bodies,num_constraints);
#endif
    g_cforces      = cforces;
    g_jbody        = Jbody;
    g_cforces_todo = num_constraints;

    g_spr          = (Sum_forces_spr*)(SPR);

    // Put the bodies into the spr
    load_bodies(blist,num_bodies);

    /* They both start the same, but there's a prolog and epilog. It goes:
     * load 1
     * calculate 1, load 2
     * calculate 2, load 3
     * ... to n
     * calculate n
     * so, you see, the first buffer you load and the first buffer you calculate are both 1
     */
    Sum_forces_buf* load_buf=&(g_spr->dbuf1);
    Sum_forces_buf* calc_buf=&(g_spr->dbuf1);

    // todo what if there isn't as much as two bufferfulls? Well it will just work, I think.

    // Prolog: upload one set of strips, but no calculating

    sceDmaSync(GETCHANTOSPR,0,0);

    int length=load(load_buf);
    int length2;

    load_buf=&(g_spr->dbuf2);

    // Main double-buffer loop. Calculate one while sending the other
    while(g_cforces_todo>0)
    {
        // The following things happen in parallel

        sceDmaSync(GETCHANTOSPR,0,0);

        length2=load(load_buf);

        /* Note that the sum_forces_block actually uses data from the previous
         * double buffer. This means you can't start reloading buffer 0 until
         * buffer 1 has finished calculating. */
        sum_forces_block(calc_buf,length);

        length=length2;

        // Swap double buffer. You can think of a more efficient way to do this later
        if(load_buf==&(g_spr->dbuf1)) load_buf=&(g_spr->dbuf2);
        else load_buf=&(g_spr->dbuf1);

        if(calc_buf==&(g_spr->dbuf1)) calc_buf=&(g_spr->dbuf2);
        else calc_buf=&(g_spr->dbuf1);
    }

    // Epilog
    sum_forces_block(calc_buf,length);

    store_bodies(blist,num_bodies);

#if PRINT_CALC_RESULTANT_FORCES_OUTPUT
    printCalcResultantForcesOutput(blist,num_bodies);
#endif

}

/* load
 * ----
 *
 * dma chain:
 *
 *   {(SUM_CFORCES_LEN*sizeof(Constraint_force))/16,0,REF_ID,0,0,0}, // forces
 *   {0,0,REFE_ID,0,0,0},                                            // jbody
*/
int load(Sum_forces_buf* buf)
{
    /* Set the destination start point in the scratchpad. It should be an offset from
     * the start of the scratchpad in bytes */

    unsigned int dbuf_offset = (unsigned int)buf-SPR;
    const int    rc          = MIN(g_cforces_todo,SUM_CFORCES_LEN);

    MEASSERTALIGNED(g_cforces,16);

    int qwc = MeMathCEIL4(rc*2) / 4;

#if 0
    /* With many partitions, cant guarantee that g_jbody is aligned, so have to copy */

    for(int i=0;i!=rc;i++)
    {
        buf->jbody[i] = g_jbody[i];
    }
#endif

    *((sceDmaTag**)(UNCACHED(&(g_dmachain.forces.next))))  = (sceDmaTag*)DMAADDR(g_cforces);
    *((sceDmaTag**)(UNCACHED(&(g_dmachain.jbody.next ))))  = (sceDmaTag*)DMAADDR(g_jbody);
    *((int*)(UNCACHED_SCEDMATAG(&(g_dmachain.jbody.qwc)))) = qwc;

    DPUT_CHTOSPR_SADR(dbuf_offset);
    asm("sync.l");
    DPUT_CHTOSPR_QWC(0);                        
    asm("sync.l");
    DPUT_CHTOSPR_TADR((u_int)&g_dmachain);
    CPUSYNC
    DPUT_CHTOSPR_CHCR(0x105);
    asm("sync.l");

    //*** just for testing
    sceDmaSync(GETCHANTOSPR,0,0);

    g_jbody        += rc;
    g_cforces      += rc;
    g_cforces_todo -= rc;

    return rc;
}

void load_bodies(const MdtKeaBody *const blist[],int num_bodies)
{
    MEASSERT(num_bodies<=MAXBODIES);
    MdtKeaForce* f=&(g_spr->forces[0]);

    for(int i=0;i<num_bodies;i++){

        __asm__ __volatile__("
        __expression_asm

        force
        torque

        'lqc2  @force, 0x0(%0)
        'lqc2  @torque, 0x10(%0)

        'sqc2  @force, 0x0(%1)
        'sqc2  @torque, 0x10(%1)

        ~force
        ~torque

        __end_expression_asm

        " : : "r" (&(blist[i]->force[0])),
              "r" (&(f[i].force[0])));

/*
        for(int j=0;j<3;j++){
            f[i].force[j]=blist[i].force[j];
            f[i].torque[j]=blist[i].torque[j];
        }
*/
    }
}

void store_bodies(MdtKeaBody *const blist[],int num_bodies)
{
    MdtKeaForce* f=&(g_spr->forces[0]);

    for(int i=0;i<num_bodies;i++){

        __asm__ __volatile__("
        __expression_asm

        force
        torque

        'lqc2  @force, 0x0(%0)
        'lqc2  @torque, 0x10(%0)

        'sqc2  @force, 0x0(%1)
        'sqc2  @torque, 0x10(%1)

        ~force
        ~torque

        __end_expression_asm

        " : : "r" (&(f[i].force[0])),
              "r" (&(blist[i]->force[0])));

/*
        for(int j=0;j<3;j++){
            blist[i].force[j]=f[i].force[j];
            blist[i].torque[j]=f[i].torque[j];
        }
*/
    }
}

// just for debugging
void zero_spr()
{
    unsigned int* spri=(unsigned int*)(SPR);

    for(int i=0;i<4096;i++){
        // spri[i]=0xffffffff;
        spri[i]=0x0;
    }
}

/* 
 * Not even macrocoded!
 * Should be incorporated into calc resultant forces double buffer loop
*/

void keaFunctions_PS2 :: calculateAcceleration(
    MdtKeaBody *const             blist[],                  /* input/output */
    const MdtKeaInverseMassMatrix invIworld[],              /* input */
    int                           num_bodies)               /* input */
{
    int i,j;

#if PROFILE_KEA
    MdtKeaProfileStart("ps2 CalcAccel");
#endif

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

#if PROFILE_KEA
    MdtKeaProfileEnd("ps2 CalcAccel");
#endif
}
