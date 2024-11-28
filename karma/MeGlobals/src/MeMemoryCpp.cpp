/* -*- C++ -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/17 16:22:25 $ - Revision: $Revision: 1.1.2.2 $

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

#include <new>

#include <MeMemory.h>

/*
    This should really be enabled on every platform, but it is
    just essential on NGC.
*/

#ifdef NGC
void *::operator new(const size_t bytes)
{
    return (*MeMemoryAPI.create)(bytes);
}

void *::operator new[](const size_t bytes)
{
    return (*MeMemoryAPI.create)(bytes);
}

void ::operator delete(void *block)
{
    (*MeMemoryAPI.destroy)(block);
}

void ::operator delete[](void *block)
{
    (*MeMemoryAPI.destroy)(block);
}
#endif
