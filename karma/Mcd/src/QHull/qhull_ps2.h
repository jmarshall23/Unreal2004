#ifndef QHULL_PS2_H
#define QHULL_PS2_H

#include <MePrecision.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
  Provide missing functionality required by QHull to allow it to work on
  the PS2
*/

/* The call to time in global.c appears simply to want a random number. */

/* Ideally, we'll have a way to pass a seed in so each run is different. For
the time being, though, at least each call to this function generates a
different result, although the sequence will always be the same!) */

#define RANDOM_SEED (0xdead)

static inline int qh_ps2_random32()
{
    static MeU8 qh_ps2_started = 0;
    int retval = 0;
    
    if(!qh_ps2_started) {
        /* Init the random number generator */
        asm __volatile__ ("
    addi %0, 0
    qmtc2 %0, vf01
    vrinit R, vf01x
" : : "r" (RANDOM_SEED) : "$8" );
        qh_ps2_started = 1;
    }
    /* Get the next couple of random floats from the vector unit, take the
       bottom 16 bits of the mantissa of each, and combine them into a 32
       bit word */
    asm __volatile__ ("
        vrnext.x vf01x, R
        vrnext.x vf02x, R
        qmfc2 $8, vf01
        qmfc2 $9, vf02
        sll $8, $8, 0x10
        and $9, $9, 0xffff
        add $8, $8, $9
        sw $8, 0(%0)
" : : "r" (&retval) : "$8", "$9", "$10" );
   
    return retval;
}

/* For the time being, the other instances of time (i.e. those which
   actually want some time data back) I've simply removed, so the messages
   that result from them will give junk on the PS2. */

/* Also, times and gettimeofday are not defined ... so, make versions of
   them to keep the linker happy. */

int times (void * t) {
    return -1;
}

int gettimeofday (void * t) {
    return -1;
}

#ifdef __cplusplus
}
#endif


#endif
