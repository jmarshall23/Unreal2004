
/*
  cdDominoes.c
  Copyright (c) 1997-2002 MathEngine PLC
*/

/*
  This example shows a set of long boxes standing on a flat surface.
  Press 'a' start and stop physics.  With physics stopped, 's' single-steps
  the physics.  One box can be toppled by pressing 'g' momentarily.  Balls
  can also be shot at the boxes by pressing the space bar.

  This example uses MathEngine Dynamics and Collision Toolkits
*/


#ifdef _DEBUG
#   define _MECHECK
#else
#   define _MERELEASE
#endif

/*#include "McdIntersect.t.cpp"*/
#include "McdSpace.t.cpp"
