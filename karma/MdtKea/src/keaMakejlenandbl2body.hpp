#ifndef _KEAMAKEJLENANDBL2BODY_HPP
#define _KEAMAKEJLENANDBL2BODY_HPP
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.10.6.1 $

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

void makejlenandbl2body(int *      jlen,
                        int *      bl2body,
                        int *      bl2cony,
                        const int * Jbody,
                        const int * Jsize,
                        int         num_strips_inc_padding,
                        int         num_strips,
                        int         num_constraints);

void makejlenandbl2body_PS2(int           jlen[],                            /* Output */
                            int           jlen_unpadded[],                   /* Output */
                            int           bl2body[],                         /* Output */
                            int           bl2body_unpadded[],                /* Output */
                            int           bl2cony[],                         /* Output */
                            const int     Jbody[],                           /* Input */
                            const int     Jsize[],                           /* Input */
                            const int     num_rows_inc_padding_partition[],  /* Input */
                            const int     num_rows_exc_padding_partition[],  /* Input */
                            const int     num_constraints_partition[],       /* Input */
                            int           num_partitions);                    /* Input */

#endif /* _KEAMAKEJLENANDBL2BODY_HPP */
