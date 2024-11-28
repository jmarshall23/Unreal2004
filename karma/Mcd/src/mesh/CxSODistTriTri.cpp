/* -*-c++-*-
 *===============================================================
 * File:        CxSODistTriTri.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.7.18.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#include <MePrecision.h>
#include "CxSODistTriTri.h"

// const MeReal EPS = ME_SMALL_EPSILON;

/**
 *
 */
CxSODistTriTri::CxSODistTriTri(const lsVec3 *tri00, const lsVec3 *tri01, const lsVec3 *tri02,
                   const lsVec3 *tri10, const lsVec3 *tri11, const lsVec3 *tri12,
                   const lsVec3 *N0, const lsVec3 *N1, const lsVec3 *direction)
    :
dir(*direction), dist(MEINFINITY)
{
    // input "direction" is supposed to be unit vector
    // MeReal t = dir.normalize();
    tri[0][0] = tri00;
    tri[0][1] = tri01;
    tri[0][2] = tri02;
    tri[1][0] = tri10;
    tri[1][1] = tri11;
    tri[1][2] = tri12;
    N[0] = N0;
    N[1] = N1;
}

/**
 *
 */
unsigned int
          CxSODistTriTri::process()
{
    if (buildWalls() == 0) {
    processCoplanar();
    } else if (cullTriAgainstWalls(0, 1) != 1) {
    cullTriAgainstWalls(1, 0);
    }

    return (dist != MEINFINITY);
}

/**
 *
 */
unsigned int
          CxSODistTriTri::checkCoplanar(const lsPlane * p0, const lsPlane * p1)
{
    if (MeFabs(p0->d - p1->d) > ME_SMALL_EPSILON)
    return 0;

    if ((p0->nor).checkAlmostEqual(p1->nor, ME_SMALL_EPSILON) ||
    (p0->nor).checkAlmostEqual(-p1->nor, ME_SMALL_EPSILON)) {
    return 1;
    }

    return 0;
}

/**
 *
 */
unsigned int
          CxSODistTriTri::processCoplanar()
{
    lsVec3
    w = dir.cross(ptri[0].nor);

    MeReal
    tri2d[2][3][2];
    int
    i, j, k, t0, t1, j1;

    for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
        tri2d[i][j][0] = dir.dot(*(tri[i][j]));
        tri2d[i][j][1] = w.dot(*(tri[i][j]));
    }
    }

    int
    iv, ivtri, ie, ietri;
    MeReal
    param, par, distance;

    for (k = 0; k < 2; k++) {
    t0 = k;
    t1 = !t0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
        j1 = (j + 1) % 3;
        if ((tri2d[t0][i][1] >= tri2d[t1][j][1] &&
             tri2d[t0][i][1] <= tri2d[t1][j1][1]) ||
            (tri2d[t0][i][1] <= tri2d[t1][j][1] && tri2d[t0][i][1] >= tri2d[t1][j1][1])) {
            MeReal
            den = tri2d[t1][j1][1] - tri2d[t1][j][1];
            if (den == 0)
            continue;
            par = (tri2d[t1][j1][1] - tri2d[t0][i][1]) / den;

            distance = tri2d[t1][j1][0] - (tri2d[t1][j1][0] - tri2d[t1][j][0]) * par;

            // !!!!
            /// distance = fabs(tri2d[t0][i][0]-distance);
            if (t0 == 0)
            distance = tri2d[t0][i][0] - distance;
            else
            distance = distance - tri2d[t0][i][0];

            if (distance < dist) {
            dist = distance;
            param = par;
            iv = i;
            ivtri = t0;
            ie = j;
            ietri = t1;
            }
        }
        }
    }
    }

    if (dist != MEINFINITY) {
    j1 = (ie + 1) % 3;
    pt[ivtri] = *tri[ivtri][iv];
    pt[ietri] = *tri[ietri][j1] - (*tri[ietri][j1] - *tri[ietri][ie]) * param;
    return 1;
    }

    return 0;
}

/**
 *
 */
unsigned int
          CxSODistTriTri::buildWalls()
{
    int
    i, j;
    lsVec3
    nor[2];

    ptri[0].nor = *(N[0]);  //tri[0]->unitNormal();
    ptri[1].nor = *(N[1]);  //tri[1]->unitNormal();
    ptri[0].d = (ptri[0].nor).dot(*tri[0][0]);
    ptri[1].d = (ptri[1].nor).dot(*tri[1][0]);

    if (checkCoplanar(&ptri[0], &ptri[1])) {
    return 0;
    }

    for (i = 0; i < 3; i++) {
    j = (i + 1) % 3;
    dvr[0].plane[i].nor = dir.cross(*tri[0][j] - *tri[0][i]);
    dvr[1].plane[i].nor = dir.cross(*tri[1][j] - *tri[1][i]);
    }

    if (ptri[0].nor.dot(dir) < 0) {
    for (i = 0; i < 3; i++)
        dvr[0].plane[i].nor = -dvr[0].plane[i].nor;
    }

    if (ptri[1].nor.dot(dir) < 0) {
    for (i = 0; i < 3; i++)
        dvr[1].plane[i].nor = -dvr[1].plane[i].nor;
    }

    for (i = 0; i < 3; i++) {
    dvr[0].plane[i].d = (dvr[0].plane[i].nor).dot(*tri[0][i]);
    dvr[1].plane[i].d = (dvr[1].plane[i].nor).dot(*tri[1][i]);
    }

    return 1;
}

/**
 *
 */
void
          CxSODistTriTri::indexSelect(const int &i, int &j, int &k)
{
    switch (i) {
    case 0:
    j = 1;
    k = 2;
    break;
    case 1:
    j = 0;
    k = 2;
    break;
    case 2:
    j = 0;
    k = 1;
    break;
    default:{
    }
    }
}

/**
 *
 */
// first tri against walls from second tri
unsigned int
          CxSODistTriTri::cullTriAgainstWalls(const int &f, const int &s)
{
    int
    i, j, icount = 0;
    MeReal
    distance;

    for (i = 0; i < 3; i++) {
    unsigned int
        inside = 1;
    for (j = 0; j < 3; j++) {
        if ((dvr[s].plane[j]).offset(tri[f][i]) < 0) {
        inside = 0;
        break;
        }
    }

    if (inside == 1) {
        icount++;
        lsVec3
        pt_s;

        if (distVertexToPlane(tri[f][i], &ptri[s], &pt_s, distance)) {
        if (f == 0)
            distance = -distance;
        if (distance < dist) {
            pt[f] = *tri[f][i];
            pt[s] = pt_s;
            dist = distance;
        }
        }
    }
    }

    if (icount == 3)
    return 1;

    lsVec3
    intst, pt_s;
    int
    bintst, ip0, ip1;

    for (i = 0; i < 3; i++) {
    int
        k = (i + 1) % 3;
    for (j = 0; j < 3; j++) {
        bintst = intersectLineSegWithPlane(tri[f][i], tri[f][k], &(dvr[s].plane[j]), &intst);
        if (bintst) {
        indexSelect(j, ip0, ip1);

        if ((dvr[s].plane[ip0]).offset(&intst) < 0)
            continue;
        if ((dvr[s].plane[ip1]).offset(&intst) < 0)
            continue;

        if (distVertexToPlane(&intst, &ptri[s], &pt_s, distance)) {
            if (f == 0)
            distance = -distance;

            if (distance < dist) {
            pt[f] = intst;
            pt[s] = pt_s;
            dist = distance;
            }
        }
        }
    }
    }

    return 0;
}

/**
 *
 */
unsigned int
          CxSODistTriTri::distVertexToPlane(const lsVec3 *v, const lsPlane * p,
                        lsVec3 *pt, MeReal &dist)
{
    MeReal
    L = (p->nor).dot(dir);
    if (MeFabs(L) < ME_SMALL_EPSILON)
    return 0;
    dist = (p->d - (p->nor).dot(*v)) / L;
    *pt = *v + dir * dist;
    return 1;
}

/**
 *
 */
unsigned int
          CxSODistTriTri::intersectLineSegWithPlane(const lsVec3 *v0, const lsVec3 *v1,
                            const lsPlane * p, lsVec3 *intst)
{
    lsVec3
    linedir = *v1 - *v0;
    MeReal
    L = (p->nor).dot(linedir);
    if (MeFabs(L) < ME_SMALL_EPSILON)
    return 0;
    MeReal
    t = (p->d - (p->nor).dot(*v0)) / L;

    if (t >= 0 && t <= 1) {
    (*intst) = (*v0 + linedir * t);
    return 1;
    }

    return 0;
}

// #define DIRVER_CODE_CxSODistTriTri
#ifdef DIRVER_CODE_

int
main()
{
    lsTriangle
    tri0(lsVec3 (0, 0, 0), lsVec3 (1, 0, 0), lsVec3 (0, 1, 0));

    cerr << tri0.v[0] << endl;
    cerr << tri0.v[1] << endl;
    cerr << tri0.v[2] << endl;

    lsTriangle
    tri1(lsVec3 (0.1, 0.1, 2.15), lsVec3 (0.5, 0.25, 2.3), lsVec3 (0.25, 0.5, 3));

    lsVec3
    dir(0.15, 0, 1.0);

    lsVec3
    pt0, pt1;

    CxSODistTriTri
    tt(&tri0, &tri1, &dir);
    if (tt.process() == 1) {
    cerr << "dist: " << tt.getDist(&pt0, &pt1) << endl;
    cerr << "pt0; " << pt0 << endl;
    cerr << "pt1; " << pt1 << endl;
    } else {
    cerr << "infinity......" << endl;
    }

    return 0;
}

#endif

// Note: should deal with following cases:
// (1) coplanar                                                                                                                                                                          -- DONE
// (2) direction contains in the tri's plane (one or both) -- TBD
//     can be: egde-edge
