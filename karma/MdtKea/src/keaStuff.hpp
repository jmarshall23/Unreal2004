#ifndef _KEASTUFF_HPP
#define _KEASTUFF_HPP
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.15.6.1 $

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

#include <MeMessage.h>
#include <MdtKea.h>


/*
  Stuff used by the j stripper jump table.
*/

typedef struct
{
    /* current offset, either 0,1,2 or 3 */
    int      offset;
    /* size of constraint to add (in rows), either 1,2,3,4,5,6 */
    int      cons_size;

    /*
      J/JM strip stuff
    */

    /* index (realtive to the current strip) of next free Jblock */
    int      Jdestindex;
    /* pointer to the next free destination J block */
    MeReal*  Jdest;
    /* pointer to the next free destination JM block */
    MeReal*  JMdest;
    /* pointer to the start of the current destination J row block */
    MeReal*  Jbase;
    /* pointer to the start of the current destination JM row block */
    MeReal*  JMbase;
    /* pointer to the first source J block containing some of the current
       constraint */
    MeReal*  Jsrc;
    /* leading dimension of the destination J and JM block array (they are
       column major) */
    int      ldJ;

    /*
      store used by calc JM
    */

    /* pointer to the constraint's zeroth body */
    MdtKeaBody* body0;
    /* pointer to the constraint's first body */
    MdtKeaBody* body1;
    /* pointer to the constraint's zeroth body's inverse inertia */
    MeReal*  body0invI;
    /* pointer to the constraint's first body's inverse inertia */
    MeReal*  body1invI;

    /*
      bodyrefersto stuff
    */

    /* index of constraint's zeroth body */
    int      body0index;
    /* index of constraint's first body */
    int      body1index;
    /* bodyrefersto[0..2) should contain the indices of the current
       constraint's bodies */
    int*     bodyrefersto;
    /* pointer to the start of the current bodyrefersto row */
    int*     bodyreferstobase;
    /* leading dimension of the bodyrefersto array (column major) */
    int      ldbrt;

    int*     body2jblock;
    int      ldb2jbl;

    int*     numinrow;
}
stripper_state;

#endif
