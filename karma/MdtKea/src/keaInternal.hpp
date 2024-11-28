#ifndef _KEAINTERNAL_HPP
#define _KEAINTERNAL_HPP
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.44.2.1 $

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

#include "keaStuff.hpp"
#include "stdio.h"

/*
  I allocate lots of stuff on the stack. Previously I was using the GNU/GCC
  and MipsPro compiler extension that allows allocation of variable sized
  local arrays, e.g. 'int n = 10; MeReal foo[n];' but because I had to port to
  windows and use Visual C++, i was forced to do this instead: 'int n = 10;
  MeReal *foo = (MeReal*) alloca (n * sizeof(MeReal));' yukky!
*/

#include <MeAssert.h>
#include <MePrecision.h>
#include <MeMemory.h>

enum MeCPUResourceID
{
    MeUnknown,
    MeX86,
    MeSSE,
};

#ifdef PS2
#define MAXPERROW 8
#else
#define MAXPERROW 10
#endif

#if 0
#   define keaStackAlloc(size) \
        do { alloca(size); printf("%d\n",size); } while (0)
#endif
#if 0
#   define keaPushStackFrame()
#   define keaPopStackFrame()
#endif

#define ALIGN_16(x) ((MeReal *) (( (MeUintPtr)x + (MeUintPtr)15) & ~(MeUintPtr)15))

#if (defined(WIN32) || defined(LINUX) || defined(IRIX) || defined(TRIMEDIA))

/*
 #   define ALIGN_16(_x) \
         ((MeReal *) (((int)(_x)+15)&~15))
*/
#define ceil16(_n) (((_n)+15)&~15)
#   define LOCAL_ARRAY(name,size) \
        MeReal *name = (MeReal*) ALIGN_16 \
            (keaStackAlloc ((size) * sizeof (MeReal) + 16))
#   define LOCAL_INT_ARRAY(name,size) \
        int *name = (int*) keaStackAlloc ((size) * sizeof (int))
#else
#   define LOCAL_ARRAY(name,size) \
        MeReal *name = (MeReal *) keaStackAlloc ((size) * sizeof (MeReal))
#   define LOCAL_INT_ARRAY(name,size) \
        int *name = (int *) keaStackAlloc ((size) * sizeof (int))
#endif

#define ALIGN_64(x) (( (MeUintPtr)x + (MeUintPtr)63) & ~(MeUintPtr)63)

#define keaStackAlloc(size) \
    MeMemoryALLOCA(size)





/*
  Vector/matrix accessors, allocators and resizers.
*/

/*
  If the vector index is out of bounds (0...size-1), cause a runtime error.
*/
static inline int keaCheckIndex(int index, int size, char *what_it_is)
{
#ifdef _MECHECK
    if (index < 0 || index >= size)
        MeDebug(12, "invalid index (%d) into %s array of size %d",
            index, what_it_is, size);
#endif
    return index;
}

/*
  If the matrix index is out of bounds (0...rows-1,0...cols-1),
  cause a runtime error.
*/
static inline int keaCheckMatrixIndex(int i, int j, int rows, int cols)
{
#ifdef _MECHECK
    if (i < 0 || j < 0 || i >= rows || j >= cols)
        MeDebug(12, "invalid index (%d,%d) into matrix of size %d*%d",
            i, j, rows, cols);
#endif
    return i + j * rows;
}

#ifdef DEBUG
#   define VEC(data,index,size) data[keaCheckIndex(index,size,"vector")]
#else
#   define VEC(data,index,size) data[index]
#endif

#ifdef DEBUG
#   define MAT(data,i,j,rows,cols) data[keaCheckMatrixIndex(i,j,rows,cols)]
#else
#   define MAT(data,i,j,rows,cols) data[(i) + (j)*(rows)]
#endif

#endif
