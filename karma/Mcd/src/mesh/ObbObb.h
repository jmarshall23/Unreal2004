/* -*-c++-*-
 *===============================================================
 * File:        ObbObb.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.5.18.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#ifndef CX_OBBOBB_H
#define CX_OBBOBB_H

#include "BVTransform.h"
#include "BVNode.h"

// Return value is 0 for intersection, not zero otherwise.  The nonzero
// value indicates which separating axis separates the boxes.

unsigned int separation(const CxObb &box0, const BVTransform & m0,
            const CxObb &box2, const BVTransform & m1);

unsigned int separation(const BVTransform & m, MeReal *r_box0, MeReal *r_box1);

unsigned int separation_rect_rect(const CxObb &box0, const BVTransform & m0,
                  const CxObb &box2, const BVTransform & m1);

unsigned int separation_rect_box(const CxObb &box0, const BVTransform & m0,
                 const CxObb &box2, const BVTransform & m1);

#endif
