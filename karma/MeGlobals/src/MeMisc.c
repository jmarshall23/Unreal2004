/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.8.6.1 $

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

/* EPIC CHANGE 12/29/2003
 *  !!! FIXME: Hack for linking on Win64.  --ryan.
 */
#ifdef _WIN64
__int64 _iob[4096];
#endif


/**
    Utility for making pretty colours.
*/
#include <MePrecision.h>

void MEAPI MeHSVtoRGB(float h, float s, float v, float rgb[3])
{
    int i;
    float f, p, q, t;
    if( s == 0 ) {
        /* achromatic (grey) */
        rgb[0] = rgb[1] = rgb[2] = v;
        return;
    }
    h /= 60;            /* sector 0 to 5 */
    i = (int)floor(h);
    f = h-i;            /* factorial part of h */
    p = v * (1-s);
    q = v * (1-s * f);
    t = v * (1-s * (1-f));
    switch(i) {
        case 0:
            rgb[0] = v; rgb[1] = t; rgb[2] = p; break;
        case 1:
            rgb[0] = q; rgb[1] = v; rgb[2] = p; break;
                case 2:
            rgb[0] = p; rgb[1] = v; rgb[2] = t; break;
        case 3:
            rgb[0] = p; rgb[1] = q; rgb[2] = v; break;
        case 4:
            rgb[0] = t; rgb[1] = p; rgb[2] = v; break;
        default:        /* case 5: */
            rgb[0] = v; rgb[1] = p; rgb[2] = q; break;
    }
}
