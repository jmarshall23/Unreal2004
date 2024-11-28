/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.4.2.2 $

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
#include <stdio.h>
#include <float.h>
#include <MePrecision.h>

/*
    finite()            GNU C
    _finite()           MS C
    isfinite()          HP C
*/

/*
    http://www.ast.cam.ac.uk/~rgm/scratch/fits/nost/users_guide/node11.html
    http://www.astro.gla.ac.uk/users/norman/star/sc13/sc13.htx/N-x1a1b2.html
    http://usgibm.nersc.gov/doc_link/en_US/xlf/html/UG43.HTM

    'float':

        positive zero           00000000
        positive denormals      00000001 -> 007FFFFF
        positive underflow      00800000
        positive numbers        00800001 -> 7F7FFFFE
        positive overflow       7F7FFFFF
        positive infinite       7F800000
        signaling not-a-number  7F800001 -> 7FBF FFFF
        quiet not-a-number      7FC00000 -> 7FFF FFFF

        negative zero           80000000
        negative denormals      80000001 -> 807FFFFF
        negative underflow      80800000
        negative numbers        80800001 -> FF7FFFFE
        negative overflow       FF7FFFFF
        negative infinite       FF800000
        signaling not-a-number  FF800001 -> FFBF FFFF
        quiet not-a-number      FFC00000 -> FFFF FFFF

    'double':

        positive zero           0000000000000000
        positive denormals      0000000000000001 -> 000FFFFFFFFFFFFF
        positive underflow      0010000000000000
        positive numbers        0010000000000001 -> 7FEFFFFFFFFFFFFE
        positive overflow       7FEFFFFFFFFFFFFF
        positive infinite       7FF0000000000000
        signaling not-a-number  7FF0000000000001 -> 7FF7FFFFFFFFFFFF
        quiet not-a-number      7FF8000000000000 -> 7FFFFFFFFFFFFFFF

        negative zero           8000000000000000
        negative denormals      8000000000000001 -> 800FFFFFFFFFFFFF
        negative underflow      8010000000000000
        negative numbers        8010000000000001 -> FFEFFFFFFFFFFFFE
        negative infinite       FFF0000000000000
        signaling not-a-number  FFF0000000000001 -> FFF7FFFFFFFFFFFF
        quiet not-a-number      FFF8000000000000 -> FFFFFFFFFFFFFFFF
        negative overflow       FFEFFFFFFFFFFFFF
*/

#define PG 0

#if PG
int MeRealIsValidFn(MeReal x)
{
#   ifndef _MSC_VER
    return 1;
#   else
    /*
        There are six valid classes and four invalid; it may look like
        it would be faster to just check the four invalid. However it is
        probably vastly quicker to just check the valid classes, as:

        * most arguments are expected to be valid;
        * as soon as a valid class is found, the number is valid,
          but all invalid classes must be checked to declared the
          number valid.

        The classes below are arranged in what I expect to be their
        popularity.

        It is an open question whether the multiple '||' or the
        'switch' will be faster.
    */
#   if 0
    const int c = _fpclass((double) x);

    return (c == _FPCLASS_PZ)   /* + 0  */
        || (c == _FPCLASS_PN)   /* positive normal      */
        || (c == _FPCLASS_NN)   /* negative normal      */
        || (c == _FPCLASS_PD)   /* positive denormal    */
        || (c == _FPCLASS_ND)   /* negative denormal    */
        || (c == _FPCLASS_NZ);  /* -0                   */
#   endif
#   if 0
    switch (_fpclass((double) x))
    {
    case _FPCLASS_PZ:	/* +0 */
    case _FPCLASS_PN:	/* positive normal */
    case _FPCLASS_NN:	/* negative normal */
    case _FPCLASS_PD:	/* positive denormal */
    case _FPCLASS_ND:	/* negative denormal */
    case _FPCLASS_NZ:	/* -0 */
        return 1;

    case _FPCLASS_NINF:	/* negative infinity */
    case _FPCLASS_PINF:	/* positive infinity */
    case _FPCLASS_QNAN:	/* quiet NaN */
    case _FPCLASS_SNAN:	/* signaling NaN */
    default:
        return 0;
    }
#   endif
#   endif
}
#else
int MeRealIsValidFn(MeReal x)
{
#   ifndef _MSC_VER
    return 1;
#   else
    int fpClass;

    fpClass = _fpclass((double) x);

    /* Illegal cases */

    if (fpClass == _FPCLASS_QNAN) /* quiet NaN */
    {
        return 1; 
    }
    else if (fpClass == _FPCLASS_SNAN) /* signaling NaN */
    {
        return 1;
    }
    else if (fpClass == _FPCLASS_NINF) /* negative infinity */
    {
        return 1;
    }
    else if (fpClass == _FPCLASS_PINF) /* positive infinity */
    {
        return 1;
    }

    /* Legal Cases */

    else if (fpClass == _FPCLASS_NN) /* negative normal */
    {
        return 0;
    }
    else if (fpClass == _FPCLASS_ND) /* negative denormal */
    {
        return 0;
    }
    else if (fpClass == _FPCLASS_NZ) /* -0 */
    {
        return 0;
    }
    else if (fpClass == _FPCLASS_PZ) /* +0 */
    {
        return 0;
    }
    else if (fpClass == _FPCLASS_PN) /* positive normal */
    {
        return 0;
    }
    else if (fpClass == _FPCLASS_PD) /* positive denormal */
    {
        return 0;
    }
    else 
    {
        /* the MeReal passed in is none of the above!! */
        return 1;
    }
#   endif
}
#endif
