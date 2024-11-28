                       Qhull Acknowledgement
                       ---------------------

The MathEngine Collision Toolkit uses Qhull (c) to construct convex models 
from points. Qhull is a software package used for computing the convex hull 
and related geometrical structures. It is available free of charge from the 
University of Minnesota Geometry Center.

The following copyright notice applies: 

                    Qhull, Copyright (c) 1993-1998

       The National Science and Technology Research Center for
        Computation and Visualization of Geometric Structures
                        (The Geometry Center)
                       University of Minnesota
                            400 Lind Hall
                        207 Church Street S.E.
                      Minneapolis, MN 55455  USA

                       email: qhull@geom.umn.edu
               URL: http://www.geom.umn.edu/software/qhull


See COPYING.TXT for more detailed copyright information for Qhull.

==========================================================================

MathEngine modifications to QHull
---------------------------------

12/2000: MC

  Wrapping of function prototypes with C calling convention macro for
  Visual C++. Forced functions passed to stdlib functionality
  (e.g. qsort()) to __cdecl

08/2000: MT

  Wrapping of memory-related functionality, mapping calls to toolkit's
  memory management API.

08/2000: LH

  Comments forced to C-style.

07/2000: WG

  Time-related functions dsiabled on Playstation2 (doesn't support them).
