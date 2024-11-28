/* -*-c++-*-
 *===============================================================
 * File:        CxSODistTriTri.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.5.18.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#ifndef CxSODistTriTri_H_
#define CxSODistTriTri_H_

#include "lsVec3.h"

class     CxSODistTriTri {
  public:
    CxSODistTriTri(const lsVec3 *tri00, const lsVec3 *tri01, const lsVec3 *tri02,
           const lsVec3 *tri10, const lsVec3 *tri11, const lsVec3 *tri12,
           const lsVec3 *N1, const lsVec3 *N2, const lsVec3 *direction);

    unsigned int process();

    MeReal    getDist(lsVec3 *pt0, lsVec3 *pt1) const;
    MeReal    getDist() const;
    MeReal    computeDist(lsVec3 *pt0, lsVec3 *pt1);
    MeReal    computeDist();

  private:
    struct lsPlane      // nor*pt - d = 0
    {
    lsVec3    nor;
    MeReal    d;

    MeReal    offset(const lsVec3 *v) {
        return (nor.dot(*v) - d);
    }};

    struct lsDVR        // Directed Voronoi-like Region for triangle
    {
    lsPlane   plane[3];
    };

  private:
    unsigned int buildWalls();

    unsigned int cullTriAgainstWalls(const int &f, const int &s);

    unsigned int distVertexToPlane(const lsVec3 *v, const lsPlane * p, lsVec3 *pt, MeReal &dist);

    unsigned int
              intersectLineSegWithPlane(const lsVec3 *v0, const lsVec3 *v1,
                    const lsPlane * p, lsVec3 *intst);

    unsigned int checkCoplanar(const lsPlane * p0, const lsPlane * p1);

    unsigned int processCoplanar();

    void      indexSelect(const int &i, int &j, int &k);

  private:

    const lsVec3 *tri[2][3];
    const lsVec3 *N[2];
    lsVec3    dir;

    lsDVR     dvr[2];
    lsPlane   ptri[2];
    lsVec3    pt[2];        // two min distance points
    MeReal    dist;
};

inline MeReal
          CxSODistTriTri::getDist(lsVec3 *pt0, lsVec3 *pt1) const
{
    *pt0 = pt[0];
    *pt1 = pt[1];
    return dist;
}

inline MeReal
          CxSODistTriTri::getDist() const
{
    return dist;
}

inline MeReal
          CxSODistTriTri::computeDist(lsVec3 *pt0, lsVec3 *pt1)
{
    if (process()) {
    *pt0 = pt[0];
    *pt1 = pt[1];
    }
    return dist;
}

inline MeReal
          CxSODistTriTri::computeDist()
{
    process();
    return dist;
}

#endif              // CxSODistTriTri_H_
