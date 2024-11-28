/* -*-c++-*-
 *===============================================================
 * File:        TriTriIsctTest.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 * $Revision: 1.6.18.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

/*
 * int tri_tri_intersect(MeReal V0[3],MeReal V1[3],MeReal V2[3],
 *                         MeReal U0[3],MeReal U1[3],MeReal U2[3])
 *
 * parameters: vertices of triangle 1: V0,V1,V2
 *             vertices of triangle 2: U0,U1,U2
 * result    : returns 1 if the triangles intersect, otherwise 0
 *
 */

#ifndef _TRI_TRI_ISCT_TEST_H_
#define _TRI_TRI_ISCT_TEST_H_

#include "lsVec3.h"

enum {
    DISJOINT = 0,
    INTERSECT,
    INTERSECT_1,        // two intersection points lie on edges of first triangle
    INTERSECT_2,        // two intersection points lie on edges of second triangle
    INTERSECT_3,        // one intersection point lie on an edge of first triangle, and
    // the other intersection point lie on an edge of second triangle
    INTERSECT_COPLANAR
};

unsigned int
          TriTriIsect(const MeReal *V0, const MeReal *V1, const MeReal *V2,
              const MeReal *U0, const MeReal *U1, const MeReal *U2);

unsigned int
          TriTriIsect(const MeReal *V0, const MeReal *V1, const MeReal *V2,
              const MeReal *U0, const MeReal *U1, const MeReal *U2,
              lsVec3 *IsectPt, int numMaxPts, int &numPts);

#endif              // _TRI_TRI_ISCT_TEST_H_
