/*----------------------------------------------------------------------------
File name:      BBoxFitter.cpp
Class name:     BBoxFitter
Description:    Given a list of triangles (lsTriangle Soup), find the smallest
                  oriented bounding box which can hold all vertices.
Class name:     BBoxFitter
Author:           Zhaoheng Liu
Algorithm
Design:           Karsten Howes, Zhaoheng Liu
Ported:         Scott Burlington
date:             13/09/00
Company:          MathEngine Canada (C) 2000-2001
-----------------------------------------------------------------------------*/

#include <MeMemory.h>

#include <MePrecision.h>

#ifdef USE_IOSTREAM
#ifdef WIN32
#include <iostream>
using namespace stl_namespace;
#else
#include <iostream.h>
#endif
#endif

#include "BBoxFitter.h"

/*---------------------------------------------------------------------------*/
//                  L O C A L   C O N S T A N T S
/*---------------------------------------------------------------------------*/
const MeReal EPS = (MeReal)1.0E-30;
const MeReal MIN_ANG = (MeReal)1.0E-4;        // minimum angle in searching
const MeReal REDUCE_RATE = (MeReal)(1.0 / 1.5); // angle reducing rate.

/*---------------------------------------------------------------------------*/
// constructor
/*---------------------------------------------------------------------------*/
BBoxFitter::BBoxFitter(CxTriangleMesh *_triSoup, const CxObb & initBox):
iObjFunCase(1)
{
    triSoup = _triSoup;

    bBox.mBox.ext =  initBox.mBox.ext;
    bBox.mTm =  initBox.mTm;
}

/*---------------------------------------------------------------------------*/
// copy constructor
/*---------------------------------------------------------------------------*/
BBoxFitter::BBoxFitter(const BBoxFitter & oCopy):
triSoup(oCopy.triSoup), bBox(oCopy.bBox), iObjFunCase(oCopy.iObjFunCase)
{
}

/*---------------------------------------------------------------------------*/
// destructor
/*---------------------------------------------------------------------------*/
BBoxFitter::~BBoxFitter(void)
{
}

/*---------------------------------------------------------------------------*/
// Central processing module:
// The bounding box is rotated to minimize a pre-defined objective function
/*---------------------------------------------------------------------------*/
#if 0
void
BBoxFitter::process(const int iObjFunCaseIn)
{  // What we need here is the trilist [indeces] that we are actually computing over...@!
    this->iObjFunCase = iObjFunCaseIn;
    if (iObjFunCase < 1 || iObjFunCase > 3) {
    // cerr << "Wrong objective function case in BBoxFitter::process()\n";
    // probably a little harsh ... CX_ERROR
    exit(0);
    }
    // no search is necessary if there is only one triangle in the box
    if (CxTriangleMeshGetTriangleCount(triSoup) == 1) {
    computeBBoxForSinglelsTriangle();
    return;
    }

    switch (iObjFunCase) {
    case 1:
    processVolume();
    break;
    default:
    break;
    }
}
#endif

void
BBoxFitter::process(int *triList, int numTris, const int iObjFunCaseIn)
{  // What we need here is the trilist [indeces] that we are actually computing over...@!
    this->iObjFunCase = iObjFunCaseIn;
    if (iObjFunCase < 1 || iObjFunCase > 3) {
    // cerr << "Wrong objective function case in BBoxFitter::process()\n";
    // probably a little harsh ... CX_ERROR
    exit(0);
    }
    // no search is necessary if there is only one triangle in the box
    if (CxTriangleMeshGetTriangleCount(triSoup) == 1) {
    computeBBoxForSinglelsTriangle();
    return;
    }

    switch (iObjFunCase) {
    case 1:
      // processVolume();
    exit(0);
    break;
    case 2:
    processArea(triList, numTris);
    break;
    case 3:
    processLinearSpg(triList, numTris);
    break;
    default:
    break;
    }
}

/*---------------------------------------------------------------------------*/
//
/*---------------------------------------------------------------------------*/
#if 0
void
BBoxFitter::processVolume(void)
{
#if 0  //  This function doesn't seem to get used anywhere
    lsVec3    *vMin[3], *vMax[3], rotateAxis, center;
    MeReal    projMin, projMax;
    MeReal    volume = bBox.mBox.ext.v[0] * bBox.mBox.ext.v[1] * bBox.mBox.ext.v[2];
    MeReal    volume0, minVolume = volume;
    MeReal    torMag, rotateAngle = (MeReal)-0.1;   // radian
    CxObb     minBBox = bBox;
    int       numTris = CxTriangleMeshGetTriangleCount(triSoup);

    /*
    int triList[numTris];
    */

    int i;
    int *triList; // this wont work if this gets compiled
    for (i=0; i < numTris; triList[i] = i++);

    // find extremal projections on each axis.

    for (i = 0; i < 3; ++i) {
        extremalVertices(triSoup, triList, numTris,
                (lsVec3&)bBox.mTm.axis(i), &vMax[i], &vMin[i], &projMax, &projMin);
    //  extremalVertices(triSoup, bBox.mTm.axis(i), &vMax[i], &vMin[i], &projMax, &projMin);

    }
    int       stepCount = 0;

    const int NumIterations = 35;//75;  bigger, better, slower

    while (stepCount < NumIterations) {
        volume0 = volume;
        // compute rotation axis
        bBox.mTm.getTranslation((BVVec3*)&center);
        rotateAxis = determineRotationAxis(vMin, vMax, center, torMag);
        if (torMag < EPS) {
          if (stepCount == 0) {
              rotateAxis = getRandomAxis();
          } else
            break;
        }
        // rotate the bounding box (update 3 axes, box size and box center)
        rotateBBox(rotateAxis, rotateAngle, vMin, vMax);

        volume = bBox.mBox.ext.v[0] * bBox.mBox.ext.v[1] * bBox.mBox.ext.v[2];
        if (volume < minVolume) {// keep a record of smallest (vol.) box
            minVolume = volume;
            minBBox = bBox;
        }
        if (volume > volume0) {
          rotateAngle = rotateAngle * REDUCE_RATE;
          if (MeFabs(rotateAngle) < MIN_ANG)
            break;
        }
        stepCount++;
    }

    if (volume > minVolume) {
        volume = minVolume;
        bBox = minBBox;
    }

    // sort box radii in descending order
    sortRadii(bBox);
    // cerr << "size: " << bBox.r << ": " << triSoup.size() << endl ;
#endif
}
#endif

/*---------------------------------------------------------------------------*/
//
/*---------------------------------------------------------------------------*/
void
BBoxFitter::processArea(int *triList, int numTris)
{
    lsVec3    *vMin[3], *vMax[3], rotateAxis, center;
    MeReal    projMin, projMax;
    MeReal    area = bBox.mBox.ext.v[0] * bBox.mBox.ext.v[1] + bBox.mBox.ext.v[1] *
      bBox.mBox.ext.v[2] + bBox.mBox.ext.v[2] * bBox.mBox.ext.v[0];
    MeReal    area0, minArea = area;
    MeReal    torMag, rotateAngle = (MeReal)-0.1;   // radian
    CxObb     minBBox = bBox;

    int i;
    // find extremal projections on each axis.
    for (i = 0; i < 3; ++i) {
        extremalVertices(triList, numTris, bBox.mTm.axis(i), &vMax[i], &vMin[i], &projMax, &projMin);
    }
    int       stepCount = 0;
    const int NumIterations = 35;//75;  bigger, better, slower
    while (stepCount < NumIterations) {
        area0 = area;
        bBox.mTm.getTranslation((BVVec3*)&center);
        // compute rotation axis
        rotateAxis = determineRotationAxis(vMin, vMax, center, torMag);
        if (torMag < EPS) {
            if (stepCount == 0) {
            rotateAxis = getRandomAxis();
            } else
            break;
        }
        // rotate the bounding box (update 3 axes, box size and box center)
        rotateBBox(rotateAxis, rotateAngle, vMin, vMax, triList, numTris);
        area = bBox.mBox.ext.v[0] * bBox.mBox.ext.v[1] + bBox.mBox.ext.v[1] *
            bBox.mBox.ext.v[2] + bBox.mBox.ext.v[2] * bBox.mBox.ext.v[0];
        if (area < minArea) { // keep a record of the best box
            minArea = area;
            minBBox = bBox;
        }
        if (area > area0) {
            rotateAngle = rotateAngle * REDUCE_RATE;
            if (MeFabs(rotateAngle) < MIN_ANG)
            break;
        }
        stepCount++;
    }
    if (area > minArea) {
        area = minArea;
        bBox = minBBox;
    }
    // sort box radii in descending order
    sortRadii(bBox);
}

/*---------------------------------------------------------------------------*/
//
/*---------------------------------------------------------------------------*/
void
BBoxFitter::processLinearSpg(int *triList, int numTris)
{
    lsVec3    *vMin[3], *vMax[3], rotateAxis, center;
    MeReal    projMin, projMax;
    MeReal    ObjFun = bBox.mBox.ext.v[0] * bBox.mBox.ext.v[0] + bBox.mBox.ext.v[1] *
        bBox.mBox.ext.v[1] + bBox.mBox.ext.v[2] * bBox.mBox.ext.v[2];
    MeReal    ObjFun0, minObjFun = ObjFun;
    MeReal    torMag, rotateAngle = (MeReal)-0.1;   // radian
    CxObb     minBBox = bBox;

    int i;
    for (i = 0; i < 3; ++i) {
        extremalVertices(triList, numTris, bBox.mTm.axis(i), &vMax[i], &vMin[i], &projMax, &projMin);
    }
    int       stepCount = 0;
    const int NumIterations = 35;//75;  bigger, better, slower
    while (stepCount < NumIterations) {
        ObjFun0 = ObjFun;
        bBox.mTm.getTranslation((BVVec3*)&center);
        // compute rotation axis
        rotateAxis = determineRotationAxis(vMin, vMax, center, torMag);
        if (torMag < EPS) {
            if (stepCount == 0) {
            rotateAxis = getRandomAxis();
            } else
            break;
        }
        // rotate the bounding box (update 3 axes, box size and box center)
        rotateBBox(rotateAxis, rotateAngle, vMin, vMax, triList, numTris);
        ObjFun = bBox.mBox.ext.v[0] * bBox.mBox.ext.v[0] + bBox.mBox.ext.v[1] *
            bBox.mBox.ext.v[1] + bBox.mBox.ext.v[2] * bBox.mBox.ext.v[2];
        if (ObjFun < minObjFun) {   // keep a record ofthe best box
            minObjFun = ObjFun;
            minBBox = bBox;
        }

        if (ObjFun > ObjFun0) {
            rotateAngle = rotateAngle * REDUCE_RATE;
            if (MeFabs(rotateAngle) < MIN_ANG)
            break;
        }

        stepCount++;
    }

    if (ObjFun > minObjFun) {
        ObjFun = minObjFun;
        bBox = minBBox;
    }
    // sort box radii in descending order
    sortRadii(bBox);
}

/*---------------------------------------------------------------------------*/
// sort radii of a box in descending order and update corresponding axes
/*---------------------------------------------------------------------------*/
void
BBoxFitter::sortRadii(CxObb & aBox)
{
    MeReal    rTmp;
    BVVec3    axisTmp;

    if (aBox.mBox.ext.v[2] > aBox.mBox.ext.v[1]) {
        rTmp = aBox.mBox.ext.v[1];
        aBox.mBox.ext.v[1] = aBox.mBox.ext.v[2];
        aBox.mBox.ext.v[2] = rTmp;

        axisTmp = aBox.mTm.axis(1);
        aBox.mTm.axis(1) = aBox.mTm.axis(2);
        aBox.mTm.axis(2) = axisTmp;
    }

    if (aBox.mBox.ext.v[1] > aBox.mBox.ext.v[0]) {
        rTmp = aBox.mBox.ext.v[0];
        aBox.mBox.ext.v[0] = aBox.mBox.ext.v[1];
        aBox.mBox.ext.v[1] = rTmp;

        axisTmp = aBox.mTm.axis(0);
        aBox.mTm.axis(0) = aBox.mTm.axis(1);
        aBox.mTm.axis(1) = axisTmp;
    }

    if (aBox.mBox.ext.v[2] > aBox.mBox.ext.v[1]) {
        rTmp = aBox.mBox.ext.v[1];
        aBox.mBox.ext.v[1] = aBox.mBox.ext.v[2];
        aBox.mBox.ext.v[2] = rTmp;

        axisTmp = aBox.mTm.axis(1);
        aBox.mTm.axis(1) = aBox.mTm.axis(2);
        aBox.mTm.axis(2) = axisTmp;
    }
}

/*---------------------------------------------------------------------------*/
//
/*---------------------------------------------------------------------------*/
lsVec3
BBoxFitter::getRandomAxis(void)
{
    lsVec3    v;

    v[0] = (MeReal)(2.0 * rand()) / RAND_MAX - (MeReal)1.0;
    v[1] = (MeReal)(2.0 * rand()) / RAND_MAX - (MeReal)1.0;
    v[2] = (MeReal)(2.0 * rand()) / RAND_MAX - (MeReal)1.0;

    v.normalize();

    return v;
}

/*---------------------------------------------------------------------------*/
//
/*---------------------------------------------------------------------------*/
MeReal
BBoxFitter::getRandomAngle(void)
{
    return (((MeReal) rand() / RAND_MAX)) * ME_PI;
}

/*---------------------------------------------------------------------------*/
//  Given an axis about which the box is rotated and the corresponding rotation
//  angle, this routine rotates the box.
/*---------------------------------------------------------------------------*/
#if 0
void
BBoxFitter::rotateBBox(lsVec3 &rotateAxis, const MeReal angle, lsVec3 vMin[3], lsVec3 vMax[3])
{
    // Note: the vector "rotateAxis" has been normalized

    MeReal    dsin = (MeReal)sin(angle / (MeReal)2.0);

    // forming quaternion
    MeReal    q0 = (MeReal)cos(angle / (MeReal)2.0);
    MeReal    q1 = rotateAxis[0] * dsin;
    MeReal    q2 = rotateAxis[1] * dsin;
    MeReal    q3 = rotateAxis[2] * dsin;
    MeReal    rotMat[3][3];

    convertQuaternionToRotMat(rotMat, q0, q1, q2, q3);

    BVVec3    newAxis, boxCenter(0, 0, 0);
    MeReal    projMin, projMax;
    int       numTris = CxTriangleMeshGetTriangleCount((CxTriangleMesh *)&triSoup);

    /*
    int triList[numTris];
    */

    int i;
    int *triList;
    triList = (int*) MeMemoryALLOCA( numTris*sizeof(int));
    for (i=0; i < numTris; triList[i] = i++);


    // update three axes, box size and box center
    for (i = 0; i < 3; ++i) {
        MatMultiplyVector(rotMat, (lsVec3&)bBox.mTm.axis(i), (lsVec3&)newAxis);
        bBox.mTm.axis(i) = newAxis;

        extremalVertices( triList, numTris, bBox.mTm.axis(i), &vMax[i], &vMin[i], &projMax, &projMin);
        bBox.mBox.ext.v[i] = MeFabs((projMax - projMin) / 2.0);
        boxCenter += (projMin + bBox.mBox.ext.v[i]) * (bBox.mTm.axis(i));
    }

    bBox.mTm.setTranslation(boxCenter);

    MeMemoryFREEA(trilist);
}
#endif

void
BBoxFitter::rotateBBox(lsVec3 &rotateAxis, const MeReal angle, lsVec3 **vMin, lsVec3 **vMax,
               int* triList, int numTris)
{
    // Note: the vector "rotateAxis" has been normalized

    MeReal    dsin = (MeReal)sin(angle / (MeReal)2.0);

    // forming quaternion
    MeReal    q0 = (MeReal)cos(angle / (MeReal)2.0);
    MeReal    q1 = rotateAxis[0] * dsin;
    MeReal    q2 = rotateAxis[1] * dsin;
    MeReal    q3 = rotateAxis[2] * dsin;
    MeReal    rotMat[3][3];

    convertQuaternionToRotMat(rotMat, q0, q1, q2, q3);
    BVVec3    newAxis, boxCenter(0, 0, 0);
    MeReal    projMin, projMax;
    int       i;

    // update three axes, box size and box center
    for (i = 0; i < 3; ++i) {
        MatMultiplyVector(rotMat, (lsVec3&)bBox.mTm.axis(i), (lsVec3&)newAxis);
        bBox.mTm.axis(i) = newAxis;
        extremalVertices(triList, numTris, bBox.mTm.axis(i), &vMax[i], &vMin[i], &projMax, &projMin);
        bBox.mBox.ext.v[i] = (MeReal)MeFabs((projMax - projMin) / (MeReal)2.0);
        boxCenter += (projMin + bBox.mBox.ext.v[i]) * (bBox.mTm.axis(i));
    }
    bBox.mTm.setTranslation(boxCenter);
}

/*---------------------------------------------------------------------------*/
// by computing torques, we determine the axis about which the bounding box
// will be rotated.
//
//  input:  const lsVec3* vMin, const lsVec3* vMax -- containing 6 touching
//          points
//          center  -- center of box
//  output: return lsVec3 -- unit vector of rotation axis
//          torqueMagnitude -- magnitude of the torque
/*---------------------------------------------------------------------------*/
lsVec3
BBoxFitter::determineRotationAxis(lsVec3 **vMin,
                  lsVec3 **vMax, const lsVec3 &center, MeReal &torqueMagnitude)
{
    lsVec3    forces;
    computeForces(bBox, forces);

    lsVec3    rotateAxis(0, 0, 0);
    lsVec3    vDiff, vTorque;

    // ************* first touching point: force = f0 ***********************
    // relative position of "vPoint" with respect to "center"
    vDiff = *vMax[0] - center;

    lsVec3    force = forces[0] * (lsVec3&)(bBox.mTm.axis(0));
    vTorque = force.cross(vDiff);

    // increment of the rotation axis vector
    rotateAxis += vTorque;

    // ************* second touching point: force = -f0 ********************
    vDiff = *vMin[0] - center;

    force = -forces[0] * (lsVec3&)(bBox.mTm.axis(0));
    vTorque = force.cross(vDiff);

    // increment of the rotation axis vector
    rotateAxis += vTorque;

    // ****************** 3th touching point: force = f1 *******************
    vDiff = *vMax[1] - center;
    force = forces[1] * (lsVec3&)(bBox.mTm.axis(1));
    vTorque = force.cross(vDiff);
    rotateAxis += vTorque;

    // ****************** 4th touching point: force = -f1 ******************
    vDiff = *vMin[1] - center;
    force = -forces[1] * (lsVec3&)(bBox.mTm.axis(1));
    vTorque = force.cross(vDiff);
    rotateAxis += vTorque;

    // ****************** 5th touching point: force = f2 *******************
    vDiff = *vMax[2] - center;
    force = forces[2] * (lsVec3&)(bBox.mTm.axis(2));
    vTorque = force.cross(vDiff);
    rotateAxis += vTorque;

    // ****************** 6th touching point: force = -f2 ******************
    vDiff = *vMin[2] - center;
    force = -forces[2] * (lsVec3&)(bBox.mTm.axis(2));
    vTorque = force.cross(vDiff);
    rotateAxis += vTorque;

    torqueMagnitude = rotateAxis.norm();

    if (torqueMagnitude > EPS)
    rotateAxis = ((MeReal)1.0 / torqueMagnitude) * rotateAxis;

    return rotateAxis;
}

/*---------------------------------------------------------------------------*/
// Force application points (for f0, f1, f2) are chosen in such a way that
// each force contributes to positive torque about all axes.
//
// Note: This routine is based on 6 force points. If the number of forces
//       changes, we should update this routine.
/*---------------------------------------------------------------------------*/
MeReal
BBoxFitter::getMaxTorqueMagnitude(const CxObb & aBox)
{
    // forces
    lsVec3    forces;
    computeForces(aBox, forces);

    lsVec3    torque(0, 0, 0);

    // f0 force application point relative to center point
    lsVec3    farPoint =
        aBox.mBox.ext.v[0] * (lsVec3&)aBox.mTm.axis(0) - aBox.mBox.ext.v[1] *
        (lsVec3&)aBox.mTm.axis(1) + aBox.mBox.ext.v[2] * (lsVec3&)aBox.mTm.axis(2);

    lsVec3    force = forces[0] * (lsVec3&)aBox.mTm.axis(0);
    torque += force.cross(farPoint);

    // f1 force application point relative to center point
    farPoint = aBox.mBox.ext.v[2] * (lsVec3&)aBox.mTm.axis(2) - aBox.mBox.ext.v[0] *
        (lsVec3&)aBox.mTm.axis(0) - aBox.mBox.ext.v[1] * (lsVec3&)aBox.mTm.axis(1);

    // f1 is in negative direction of local y axis
    force = -forces[1] * (lsVec3&)aBox.mTm.axis(1);
    torque += force.cross(farPoint);

    // f2 force application point relative to center point
    farPoint = aBox.mBox.ext.v[1] * (lsVec3&)aBox.mTm.axis(1) + aBox.mBox.ext.v[2] *
        (lsVec3&)aBox.mTm.axis(2) - aBox.mBox.ext.v[0] * (lsVec3&)aBox.mTm.axis(0);

    force = forces[2] * (lsVec3&)aBox.mTm.axis(2);
    torque += force.cross(farPoint);

    return (2 * torque.norm());
}

/*---------------------------------------------------------------------------*/
// Search a smallest bounding volume by randomly determine rotation axes
// and rotation angles
/*---------------------------------------------------------------------------*/
#if 0
void
BBoxFitter::randomSearch(void)
{
    // cerr << "bBox axes: " << bBox.mBox.ext.v[0] << " " << bBox.mBox.ext.v[1] << " "
    //     << bBox.mBox.ext.v[2] << endl;

    // random number seed
    // srand( time( NULL ) );

    lsVec3    vMin[3], vMax[3];
    lsVec3    rotateAxis;
    MeReal    rotateAngle;

    MeReal    volume = bBox.mBox.ext.v[0] * bBox.mBox.ext.v[1] * bBox.mBox.ext.v[2];
    MeReal    minVolume;

    minVolume = volume;

    CxObb minBBox = bBox;
    int       stepCount = 0;

    while (stepCount < 1000) {
        stepCount++;

        rotateAxis = getRandomAxis();
        rotateAngle = getRandomAngle();

        // rotate the bounding box (update 3 axes, box size and box center
        rotateBBox(rotateAxis, rotateAngle, vMin, vMax);

        volume = bBox.mBox.ext.v[0] * bBox.mBox.ext.v[1] * bBox.mBox.ext.v[2];

        if (volume < minVolume) {
            minVolume = volume;
            minBBox = bBox;
        }
    }

    minVolume = minBBox.mBox.ext.v[0] * minBBox.mBox.ext.v[1] * minBBox.mBox.ext.v[2];
}
#endif

/*---------------------------------------------------------------------------*/
//
/*---------------------------------------------------------------------------*/
int
BBoxFitter::maxLength(const lsVec3 &dist) const
{
    int       i = 0;
    if (dist[1] > dist[0])
        i = 1;
    if (dist[2] > dist[i])
        i = 2;
    return i;
}

/*---------------------------------------------------------------------------*/
// compute bounding box for a single triangle
/*---------------------------------------------------------------------------*/
int
BBoxFitter::computeBBoxForSinglelsTriangle(void)
{
    lsVec3    vtx1, vtx2, vtx3, edge[3];
    CxTriangleMeshGetTriangleVertices((CxTriangleMesh*)&triSoup, 0, &vtx1, &vtx2, &vtx3);
    edge[0] = vtx1 - vtx2;
    edge[1] = vtx2 - vtx3;
    edge[2] = vtx3 - vtx1;

    // compute the squared-lengths of each edge
    lsVec3    dist;
    dist[0] = computeSquaredDist(edge[0]);
    dist[1] = computeSquaredDist(edge[1]);
    dist[2] = computeSquaredDist(edge[2]);

    // get longest edge index
    int       imax = maxLength(dist);

    lsVec3    axis0;
    MeReal    vlength = (MeReal)1.0 / (MeReal)sqrt(dist[imax]);
    axis0[0] = edge[imax][0] * vlength;
    axis0[1] = edge[imax][1] * vlength;
    axis0[2] = edge[imax][2] * vlength;

    lsVec3    axis2 = edge[0].cross(edge[1]);

    // normalizeVector( axis2 );
    axis2.normalize();

    lsVec3    axis1 = axis2.cross(axis0);

    MeReal    rotateMat[3][3];

    rotateMat[0][0] = axis0[0];
    rotateMat[0][1] = axis1[0];
    rotateMat[0][2] = axis2[0];
    rotateMat[1][0] = axis0[1];
    rotateMat[1][1] = axis1[1];
    rotateMat[1][2] = axis2[1];
    rotateMat[2][0] = axis0[2];
    rotateMat[2][1] = axis1[2];
    rotateMat[2][2] = axis2[2];

    lsVec3    minval, maxval, proj;

    MTxV(proj, rotateMat, vtx1);
    minval = proj;
    maxval = proj;

    MTxV(proj, rotateMat, vtx2);
    minmax(minval, maxval, proj);

    MTxV(proj, rotateMat, vtx3);
    minmax(minval, maxval, proj);

    lsVec3    localcenter;
    localcenter[0] = (minval[0] + maxval[0]) * (MeReal)0.5;
    localcenter[1] = (minval[1] + maxval[1]) * (MeReal)0.5;
    localcenter[2] = (minval[2] + maxval[2]) * (MeReal)0.5;

    lsVec3    center;
    center[0] = localcenter[0] * rotateMat[0][0] +
      localcenter[1] * rotateMat[0][1] + localcenter[2] * rotateMat[0][2];

    center[1] = localcenter[0] * rotateMat[1][0] +
      localcenter[1] * rotateMat[1][1] + localcenter[2] * rotateMat[1][2];

    center[2] = localcenter[0] * rotateMat[2][0] +
      localcenter[1] * rotateMat[2][1] + localcenter[2] * rotateMat[2][2];

    lsVec3    r;
    r[0] = (maxval[0] - minval[0]) * (MeReal)0.5;
    r[1] = (maxval[1] - minval[1]) * (MeReal)0.5;
    r[2] = (maxval[2] - minval[2]) * (MeReal)0.5;

    bBox.mTm.axis(0) = (BVVec3&)axis0;
    bBox.mTm.axis(1) = (BVVec3&)axis1;
    bBox.mTm.axis(2) = (BVVec3&)axis2;
    bBox.mBox.ext = r;
    bBox.mTm.setTranslation((BVVec3&)center);

    return 1;
}

void
BBoxFitter::extremalVertices(int *triList, int numTris, const BVVec3 &axis, lsVec3 **vMax, lsVec3 **vMin,
                 MeReal *projMax, MeReal *projMin)
{
    MeReal    proj;
    lsVec3   *v[3];
    lsVec3    cast;

    CxTriangleMeshGetTriangleVertexPtrs(triSoup, triList[0], &v[0], &v[1], &v[2]);

    *vMax = v[0];
    *vMin = v[0];
    axis.setLsVec(&cast);
    *projMax = *projMin = (*vMin)->dot(cast);

    for (int i = 0; i < numTris; i++) {
        CxTriangleMeshGetTriangleVertexPtrs(triSoup, triList[i], &v[0], &v[1], &v[2]);
        for (int j = 0; j < 3; j++) {
            proj = v[j]->dot(cast);
            if (proj < *projMin) {
            *projMin = proj;
            *vMin = v[j];
            }
            if (proj > *projMax) {
            *projMax = proj;
            *vMax = v[j];
            }
        }
    }
}
