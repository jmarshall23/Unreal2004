#ifndef _NEWJMBLOCK_HPP
#define _NEWJMBLOCK_HPP
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.8.2.1 $

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

#include <libdma.h>
#include <MdtKea.h>

#define NSTRIPS (3)
#define MAXBLOCKS (8)

#define J_LEN         (NSTRIPS*MAXBLOCKS*6*4)
#define NEWM_LEN      (NSTRIPS*MAXBLOCKS*3*4)
#define VHMF_LEN      (NSTRIPS*MAXBLOCKS*2*4)
#define J_M_VHMF_LEN  (J_LEN + NEWM_LEN + VHMF_LEN)
#define J_BL2BODY_LEN (NSTRIPS*MAXBLOCKS)
#define XI_LEN        (NSTRIPS*4)            // One quadword per strip
#define GAMMA_LEN     (XI_LEN)
#define C_LEN         (XI_LEN)

typedef struct
{
    MdtKeaInverseMassMatrix m;
    float                   vhmf[8];
} m_and_vhmf;

typedef struct
{
    int num_strips;
    int num_blocks;
    int pad2;
    int pad3;
} JMparam;

typedef struct
{
    /* Fixed length stuff */
    JMparam    parameters;
    int        j_bl2body[J_BL2BODY_LEN];
    float      gamma[GAMMA_LEN];
    float      c[C_LEN];

    sceDmaTag  rhs_tag;               // used when storing rhs by destination chain
    float      xi_rhs[XI_LEN];
    sceDmaTag  jm_tag;                // used when storing jm by destination chain
    
    /* Variable length stuff */

    float      j_m_and_vhmf[J_M_VHMF_LEN];   // variable length

} newCalc_JM_buf;

#define RHSTAG_OFFSET_BYTES ((4+J_BL2BODY_LEN + GAMMA_LEN + C_LEN)*4)

void jm_block(int   calc_buf, /* Input / output */
              float invh,     /* Input */
              float invhh,    /* Input */
              float gamma);   /* Input */

#endif // _JMBLOCK_HPP