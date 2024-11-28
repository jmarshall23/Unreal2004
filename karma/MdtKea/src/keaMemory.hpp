#ifndef _KEAMEMORY_HPP
#define _KEAMEMORY_HPP
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.13.2.1 $

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

void keaPushPoolFrame();
void keaPopPoolFrame();

void keaInitPool(void *ptr, int size);
void *keaPoolAlloc(int size,const char *name);

typedef struct {
    MdtKeaInverseMassMatrix * invIworld;
    MdtKeaVelocity *          vhmf;

    MdtKeaJBlockPair *   jm;
    MeReal *             rhs;

    /* PS2 JinvMJT requires jlen and bl2body partitions to start on a 12 elt boundary */
    int *                jlen_12padded;
    MdtKeaBl2BodyRow *   bl2body_12padded;

    int *                jlen;
    MdtKeaBl2BodyRow *   bl2body;
    MdtKeaBl2CBodyRow *  bl2cbody;

} keaTempMemory;

void allocateMemory(keaTempMemory *   mem,
                    MdtKeaConstraints constraints,
                    int               num_bodies);

#endif
