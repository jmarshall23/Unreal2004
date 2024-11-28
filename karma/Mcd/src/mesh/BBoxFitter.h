/******************************************************************************
File name:      BBoxFitter.h
Class name:     BBoxFitter
Description:      Given a list of triangles (lsTriangle Soup), find the smallest
                    oriented bounding box which can hold all vertices.
Author:           Zhaoheng Liu
Algorithm
Design:           Karsten Howes, Zhaoheng Liu
Ported:         Scott Burlington
date:             13/09/00
Company:          MathEngine Canada (C) 2000-2001
******************************************************************************/

#ifndef _BBOXFITTER_H_
#define _BBOXFITTER_H_

#include "CxTriangleMesh.h"
#include "BVNode.h"

class     BBoxFitter {
  public:
    BBoxFitter(CxTriangleMesh *triSoup, const CxObb & initBox);
    BBoxFitter(const BBoxFitter & ocopy);
  public:
    ~BBoxFitter(void);
/**************************** D A T A   M E M B E R S ************************/
  private:
    CxTriangleMesh *triSoup;
    CxObb     bBox;
    int       iObjFunCase;
/*************************** F U N C T I O N S *******************************/
  public:
    void      process(const int iObjFunCase = 1);
    void      process(int *triList, int numTris, const int iObjFunCase = 1);
    bool      getBox(CxObb & _bBox) const;
    // void      processVolume(void);
    void      processArea(int *triList, int numTris);
    void      processLinearSpg(int *triList, int numTris);
  private:
    lsVec3    determineRotationAxis(lsVec3 **vMin, lsVec3 **vMax,
                          const lsVec3 &center, MeReal &torqueMagnitude);
    lsVec3    getRandomAxis(void);
    MeReal    getRandomAngle(void);
    // void      randomSearch(void);
    // void      rotateBBox(lsVec3 &rotateAxis, const MeReal angle, lsVec3 vMin[3], lsVec3 vMax[3]);
    void      rotateBBox(lsVec3 &rotateAxis, const MeReal angle, lsVec3 **vMin, lsVec3 **vMax,
                      int* triList, int numTris);
    void      MTxV(lsVec3 &Vr, MeReal M1[3][3], lsVec3 V1);
    MeReal    computeSquaredDist(const lsVec3 &v);
    void      minmax(lsVec3 &mn, lsVec3 &mx, const lsVec3 &v);
    void      MatMultiplyVector(const MeReal mat[3][3], const lsVec3 &vect, lsVec3 &vectProd);
    void      convertQuaternionToRotMat(MeReal mat[3][3],
                            const MeReal q0,
                            const MeReal q1, const MeReal q2, const MeReal q3);
    MeReal    getMaxTorqueMagnitude(const CxObb & aBox);
    void      computeForces(const CxObb & aBox, lsVec3 &forces);
    void      sortRadii(CxObb & aBox);
    int       computeBBoxForSinglelsTriangle(void);
    int       maxLength(const lsVec3 &dist) const;
    void      extremalVertices(int*, int, const BVVec3 &axis, lsVec3 **vMax,
                      lsVec3 **vMin, MeReal *projMax, MeReal *projMin);

};

/*****************************************************************************/
//
/*****************************************************************************/
inline    bool
BBoxFitter::getBox(CxObb & _bBox) const
{
    _bBox = bBox;
    return true;
}

/*****************************************************************************/
// transpose of matrix M1 multiply vector V1
/*****************************************************************************/
inline void
BBoxFitter::MTxV(lsVec3 &Vr, MeReal M1[3][3], lsVec3 V1)
{
    Vr[0] = (M1[0][0] * V1[0] + M1[1][0] * V1[1] + M1[2][0] * V1[2]);
    Vr[1] = (M1[0][1] * V1[0] + M1[1][1] * V1[1] + M1[2][1] * V1[2]);
    Vr[2] = (M1[0][2] * V1[0] + M1[1][2] * V1[1] + M1[2][2] * V1[2]);
}

/*****************************************************************************/
inline void
BBoxFitter::minmax(lsVec3 &mn, lsVec3 &mx, const lsVec3 &v)
{
    for (int i = 0; i < 3; i++) {
        if (v[i] < mn[i])
            mn[i] = v[i];
        else if (v[i] > mx[i])
            mx[i] = v[i];
    }
}

/*****************************************************************************/
inline MeReal
BBoxFitter::computeSquaredDist(const lsVec3 &v)
{
    return (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

/*****************************************************************************/
inline void
BBoxFitter::MatMultiplyVector(const MeReal mat[3][3], const lsVec3 &vect, lsVec3 &vectProd)
{
    for (int i = 0; i < 3; i++) {
        vectProd[i] = 0;
        for (int j = 0; j < 3; j++) {
            vectProd[i] += mat[i][j] * vect[j];
        }
    }
}

/*****************************************************************************/
// Convert quaternion to rotation matrix
/*****************************************************************************/
inline void
BBoxFitter::convertQuaternionToRotMat(MeReal rotMat[3][3],
                      const MeReal q0,
                      const MeReal q1, const MeReal q2, const MeReal q3)
{
    MeReal    q0sq = q0 * q0;
    MeReal    q1sq = q1 * q1;
    MeReal    q2sq = q2 * q2;
    MeReal    q3sq = q3 * q3;
    MeReal    q0q1 = q0 * q1;
    MeReal    q0q2 = q0 * q2;
    MeReal    q0q3 = q0 * q3;
    MeReal    q1q2 = q1 * q2;
    MeReal    q1q3 = q1 * q3;
    MeReal    q2q3 = q2 * q3;

    // rotation matrix in terms of quaternion
    rotMat[0][0] = q0sq + q1sq - q2sq - q3sq;
    rotMat[0][1] = 2 * (q1q2 - q0q3);
    rotMat[0][2] = 2 * (q1q3 + q0q2);

    rotMat[1][0] = 2 * (q1q2 + q0q3);
    rotMat[1][1] = q0sq + q2sq - q1sq - q3sq;
    rotMat[1][2] = 2 * (q2q3 - q0q1);

    rotMat[2][0] = 2 * (q1q3 - q0q2);
    rotMat[2][1] = 2 * (q2q3 + q0q1);
    rotMat[2][2] = q0sq + q3sq - q1sq - q2sq;
}

/*****************************************************************************/
// objective function definition
/*****************************************************************************/
inline void
BBoxFitter::computeForces(const CxObb & aBox, lsVec3 &forces)
{
    switch (iObjFunCase) {
    case 1:
        {
            // objective function: volume
            // E = x*y*z
            forces[0] = aBox.mBox.ext.v[1] * aBox.mBox.ext.v[2];
            forces[1] = aBox.mBox.ext.v[0] * aBox.mBox.ext.v[2];
            forces[2] = aBox.mBox.ext.v[0] * aBox.mBox.ext.v[1];
            break;
        }

    case 2:
        {
            // objective function: area
            // E = x*y + y*z + z*x
            forces[0] = aBox.mBox.ext.v[1] + aBox.mBox.ext.v[2];
            forces[1] = aBox.mBox.ext.v[0] + aBox.mBox.ext.v[2];
            forces[2] = aBox.mBox.ext.v[0] + aBox.mBox.ext.v[1];
            break;
        }

    case 3:
        {
            // objective function: linear spring energy
            // E = 1/2*Kx*x^2 + 1/2*Ky*y^2 + 1/2*Kz*z^2
            forces[0] = aBox.mBox.ext.v[0];
            forces[1] = aBox.mBox.ext.v[1];
            forces[2] = aBox.mBox.ext.v[2];
            break;
        }

    default:
        {
    //  This needs a CX_WARNING ! ! ! ! *********************
    //      lsNotify(lsGlobal::notify::LS_WARNING, klsErrBBoxFitter_objFunction);
            exit(0);
            break;
        }
    }
}

#endif
