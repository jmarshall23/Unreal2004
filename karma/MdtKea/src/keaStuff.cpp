/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.14.6.1 $

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

#if 0

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "keaStuff.hpp"
#include "keaInternal.hpp"

/*
   Debugging
*/


#ifdef PS2

static void printMessage(char *msg1, char *msg2, va_list ap)
{
    printf("\n%s: ", msg1);
    vprintf(msg2, ap);
    printf("\n");
}

#else

static void printMessage(char *msg1, char *msg2, va_list ap)
{
    fflush(stdout);
    fflush(stderr);
    fprintf(stderr, "\n%s: ", msg1);
    vfprintf(stderr, msg2, ap);
    fprintf(stderr, "\n");
    fflush(stderr);
}

#endif

void MeFatalError(12, char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    printMessage("Kea Error", msg, ap);
    exit(1);
}


void MeDebug(12, char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    printMessage("KEA DEBUG", msg, ap);

    /* commit SEGVicide */
    *((char *) 0) = 1;

    abort();
}

void MeWarning(12, char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    printMessage("Kea Warning", msg, ap);
}

#endif
