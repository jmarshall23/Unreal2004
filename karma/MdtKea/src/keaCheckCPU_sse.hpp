#ifndef _KEACHECKCPU_SSE_HPP
#define _KEACHECKCPU_SSE_HPP

/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.6.2.1 $

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

struct CPUResources
{
public:
    // Katmai New Instructions (PIII) Resources
    static bool KNIAvailable () {return s_kni_available;}
    static void DiscoverSIMDAvailablilty ();
    
private:
    static bool s_kni_available;
};

#endif // _KEACHECKCPU_SSE_HPP