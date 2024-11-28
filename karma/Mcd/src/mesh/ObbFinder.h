/* -*-c++-*-
 *===============================================================
 * File:        ObbFinder.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.5.18.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#ifndef _ObbFinder_H_
#define _ObbFinder_H_

#include "CxTriangleMesh.h"
#include "BVNode.h"

/**
 * ObbFinder
 */
class     ObbFinder {
private:
    CxTriangleMesh *mMesh;
    CxObb    *mObb;
    int      *mTriList;
    int       mNumTris;
public:
    ObbFinder(void);
    ~ObbFinder(void);

public:
    void      process(CxTriangleMesh *mesh, CxObb *obb, int *triList, int numTris);
    void      tightenObb(CxTriangleMesh *mesh, CxObb *obb, int *triList, int numTris);
private:
    void      generateCovarianceMatrixPts(MeReal covmat[3][3]);
    void      generateCovarianceMatrixTris(MeReal covmat[3][3]);
    int       computeSortedEigenVector(MeReal eigVec[3][3], MeReal cov[3][3]);
    int       computeObbOneTriangle(void);
    void      findextrems(const lsVec3 r, MeReal &min, MeReal &max);
};

#endif              // _ObbFinder_H_
