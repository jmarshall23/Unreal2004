/*********************************************************
	vectormath.h
	Written by Bryan Galdrikian
	(c) 1999 Mathengine, inc.
 *********************************************************/

#ifndef _vectormath_h_
#define _vectormath_h_

#include "mesfcore.h"
#include "lsTransform.h"
#include "lsVec4.h"

// #include "EEMath.inl"

#ifdef PS2
#define ALIGNED(a) ((unsigned int)(a)%0x10==0)
#endif

inline void Sort3(int *outIndex, const lsVec3 &inN) {
	const char kSort3Table[8] = {0x06,0x09,0x00,0x21,0x12,0x00,0x18,0x24};
	const char order = kSort3Table[((inN[0]>inN[1])<<2)|((inN[0]>inN[2])<<1)|(inN[1]>inN[2])];
	outIndex[0] = order>>4;
	outIndex[1] = (order>>2)&3;
	outIndex[2] = order&3;
}

inline void SortAbs3(int *outIndex, const lsVec3 &inN) {
	const char kSort3Table[8] = {0x06,0x09,0x00,0x21,0x12,0x00,0x18,0x24};
	lsVec3 a;
	a[0] = inN[0]*inN[0];
	a[1] = inN[1]*inN[1];
	a[2] = inN[2]*inN[2];
	const char order = kSort3Table[((a[0]>a[1])<<2)|((a[0]>a[2])<<1)|(a[1]>a[2])];
	outIndex[0] = order>>4;
	outIndex[1] = (order>>2)&3;
	outIndex[2] = order&3;
}

//	Vec x Axis
inline lsVec3 Vec3CrossAxis(const lsVec3 &inVec, const int inAxisN) {
	lsVec3 outVec;
	const int axisN1 = NextMod3(inAxisN);
	const int axisN2 = NextMod3(axisN1);
	outVec[inAxisN] = (MeReal)(0.0);
	outVec[axisN1] = inVec[axisN2];
	outVec[axisN2] = -inVec[axisN1];
	return outVec;
}

//	 Axis x Vec
inline lsVec3 AxisCrossVec3(const int inAxisN, const lsVec3 &inVec) {
	lsVec3 outVec;
	const int axisN1 = NextMod3(inAxisN);
	const int axisN2 = NextMod3(axisN1);
	outVec[inAxisN] = (MeReal)(0.0);
	outVec[axisN1] = -inVec[axisN2];
	outVec[axisN2] = inVec[axisN1];
	return outVec;
}

//	Scalar triple product A*(BxC)
inline lsReal ScalarTripleProduct(const lsVec3 &inVecA,
								  const lsVec3 &inVecB, const lsVec3 &inVecC) {
	return inVecA[0]*(inVecB[1]*inVecC[2]-inVecB[2]*inVecC[1])+
		   inVecA[1]*(inVecB[2]*inVecC[0]-inVecB[0]*inVecC[2])+
		   inVecA[2]*(inVecB[0]*inVecC[1]-inVecB[1]*inVecC[0]);
}

//	Act on vector by permutation matrices
inline void Permute120Vec3(lsVec3 &outVec, const lsVec3 &inVec) {
	outVec[0] = inVec[1];
	outVec[1] = inVec[2];
	outVec[2] = inVec[0];
}

inline void Permute201Vec3(lsVec3 &outVec, const lsVec3 &inVec) {
	outVec[0] = inVec[2];
	outVec[1] = inVec[0];
	outVec[2] = inVec[1];
}

inline void SPermute012Vec3(lsVec3 &outVec, const lsVec3 &inVec, const lsReal &inS) {
	outVec[0] = inVec[0];
	outVec[1] = inS*inVec[1];
	outVec[2] = inS*inVec[2];
}

inline void SPermute120Vec3(lsVec3 &outVec, const lsVec3 &inVec, const lsReal &inS) {
	outVec[0] = inVec[1];
	outVec[1] = inS*inVec[2];
	outVec[2] = inS*inVec[0];
}

inline void SPermute201Vec3(lsVec3 &outVec, const lsVec3 &inVec, const lsReal &inS) {
	outVec[0] = inVec[2];
	outVec[1] = inS*inVec[0];
	outVec[2] = inS*inVec[1];
}

//	Act on vector by permutation matrices (in place)
inline void Permute120Vec3(lsVec3 &outVec) {
	lsReal temp = outVec[0];
	outVec[0] = outVec[1];
	outVec[1] = outVec[2];
	outVec[2] = temp;
}

inline void Permute201Vec3(lsVec3 &outVec) {
	lsReal temp = outVec[2];
	outVec[2] = outVec[1];
	outVec[1] = outVec[0];
	outVec[0] = temp;
}

inline void lsAbsMat(const lsTransform& m, lsTransform *out) {
#ifdef PS2

    unsigned int a=(unsigned int)&(m.axis(0)[0]);
    unsigned int b=(unsigned int)&(out->axis(0)[0]);

    if(ALIGNED(a) && ALIGNED(b)){

#error commented out for gcc3.
/*
        __asm__ __volatile__("
        __expression_asm

        axis0
        axis1
        axis2

        'lqc2  @axis0,0x0(%0)
        'lqc2  @axis1,0x10(%0)
        'lqc2  @axis2,0x20(%0)

        'vabs  @axis0,@axis0
        'vabs  @axis1,@axis1
        'vabs  @axis2,@axis2

        'sqc2  @axis0,0x0(%1)
        'sqc2  @axis1,0x10(%1)
        'sqc2  @axis2,0x20(%1)

        ~axis0
        ~axis1
        ~axis2

        __end_expression_asm
        " : : "r" (&(m.axis(0)[0])),"r" (&(out->axis(0)[0])));
*/
    } else{

#else
        out->axis(0)[0] = fabs(m.axis(0)[0]);
        out->axis(0)[1] = fabs(m.axis(0)[1]);
        out->axis(0)[2] = fabs(m.axis(0)[2]);
        out->axis(1)[0] = fabs(m.axis(1)[0]);
        out->axis(1)[1] = fabs(m.axis(1)[1]);
        out->axis(1)[2] = fabs(m.axis(1)[2]);
        out->axis(2)[0] = fabs(m.axis(2)[0]);
        out->axis(2)[1] = fabs(m.axis(2)[1]);
        out->axis(2)[2] = fabs(m.axis(2)[2]);
#endif // PS2

#ifdef PS2
    }
#endif
}

inline void lsAbsTransform(const lsTransform &m, const lsVec3 &inVec, lsVec3 * outVec) {
	outVec->v[0] = inVec[0]*fabs(m.axis(0)[0])+inVec[1]*fabs(m.axis(1)[0])+inVec[2]*fabs(m.axis(2)[0]);
	outVec->v[1] = inVec[0]*fabs(m.axis(0)[1])+inVec[1]*fabs(m.axis(1)[1])+inVec[2]*fabs(m.axis(2)[1]);
	outVec->v[2] = inVec[0]*fabs(m.axis(0)[2])+inVec[1]*fabs(m.axis(1)[2])+inVec[2]*fabs(m.axis(2)[2]);
}

//	Rotate about different axes
inline void QuatRotX(lsVec4 &outQuat, const lsReal theta) {
	const lsReal c = cos((MeReal)(0.5)*theta);
	const lsReal s = sin((MeReal)(0.5)*theta);
	outQuat.setValue(c,s,(MeReal)(0.0),(MeReal)(0.0));
}

inline void QuatRotY(lsVec4 &outQuat, const lsReal theta) {
	const lsReal c = cos((MeReal)(0.5)*theta);
	const lsReal s = sin((MeReal)(0.5)*theta);
	outQuat.setValue(c,(MeReal)(0.0),s,(MeReal)(0.0));
}

inline void QuatRotZ(lsVec4 &outQuat, const lsReal theta) {
	const lsReal c = cos((MeReal)(0.5)*theta);
	const lsReal s = sin((MeReal)(0.5)*theta);
	outQuat.setValue(c,(MeReal)(0.0),(MeReal)(0.0),s);
}

inline void QuatRotN(lsVec4 &outQuat, const lsReal theta, const lsVec3 &inDir) {
	const lsReal c = cos((MeReal)(0.5)*theta);
	const lsReal s = sin((MeReal)(0.5)*theta);
	outQuat.setValue(c,s*inDir[0],s*inDir[1],s*inDir[2]);
}

inline void MulQuat(lsVec4 &outQuat, const lsVec4 &inQuatL, const lsVec4 &inQuatR) {
	outQuat[0] = inQuatL[0]*inQuatR[0]-inQuatL[1]*inQuatR[1]-inQuatL[2]*inQuatR[2]-inQuatL[3]*inQuatR[3];
	outQuat[1] = inQuatL[0]*inQuatR[1]+inQuatL[1]*inQuatR[0]+inQuatL[2]*inQuatR[3]-inQuatL[3]*inQuatR[2];
	outQuat[2] = inQuatL[0]*inQuatR[2]+inQuatL[2]*inQuatR[0]+inQuatL[3]*inQuatR[1]-inQuatL[1]*inQuatR[3];
	outQuat[3] = inQuatL[0]*inQuatR[3]+inQuatL[3]*inQuatR[0]+inQuatL[1]*inQuatR[2]-inQuatL[2]*inQuatR[1];
}

//	Create rotation matrix by specifying one of its rows.
inline void AxisToRotMat(lsTransform &outMat, const lsVec3 &inAxis, const int inAxisN,
						 const lsVec3 &inUpAxis) {
	outMat.axis(inAxisN) = inAxis;
	outMat.axis(inAxisN).normalize();
	int axis1 = NextMod3(inAxisN);
	int axis2 = NextMod3(axis1);
	outMat.axis(axis2) = outMat.axis(inAxisN).cross(inUpAxis);
	if (outMat.axis(axis2).square_norm() < (MeReal)(1.e-6)*inUpAxis.square_norm()) {
		int index[3];
		SortAbs3(index,inAxis);
		outMat.axis(axis2) = Vec3CrossAxis(outMat.axis(inAxisN),index[0]);
	}
	outMat.axis(axis2).normalize();
	outMat.axis(axis1) = outMat.axis(axis2).cross(outMat.axis(inAxisN));
}

inline void QuatFromMat(lsVec4 &q, const lsTransform &m) {
	lsReal trace = m.axis(0)[0]+m.axis(1)[1]+m.axis(2)[2];

	if (trace > (MeReal)(0.0)) {
		lsReal s = sqrt(trace+(MeReal)(1.0));
		q[0] = (MeReal)(0.5)*s;
		s = (MeReal)(0.5)/s;
		q[1] = s*(m.axis(2)[1]-m.axis(1)[2]);
		q[2] = s*(m.axis(0)[2]-m.axis(2)[0]);
		q[3] = s*(m.axis(1)[0]-m.axis(0)[1]);
	} else {
		int i = (int)(m.axis(1)[1] > m.axis(0)[0]);
		int j;
		int k;
		if (m.axis(2)[2] > m.axis(i)[i]) {
			i = 2;
			j = 0;
			k = 1;
		} else if (i == 1) {
			j = 2;
			k = 0;
		} else {
			j = 1;
			k = 2;
		}
		lsReal s = -sqrt((MeReal)(1.0)+m.axis(i)[i]-m.axis(j)[j]-m.axis(k)[k]);
		q[i+1] = (MeReal)(0.5)*s;
		s = (MeReal)(0.5)/s;
		q[0] = s*(m.axis(k)[j]-m.axis(j)[k]);
		q[j+1] = s*(m.axis(i)[j]+m.axis(j)[i]);
		q[k+1] = s*(m.axis(i)[k]+m.axis(k)[i]);
	}
}

inline void MatFromQuat(lsTransform &m, const lsVec4 &q) {
	float xx = q[1]*q[1];
	float yy = q[2]*q[2];
	float zz = q[3]*q[3];
	float xy = q[1]*q[2];
	float yz = q[2]*q[3];
	float zx = q[3]*q[1];
	float wx = q[0]*q[1];
	float wy = q[0]*q[2];
	float wz = q[0]*q[3];

	xx += xx;
	yy += yy;
	zz += zz;
	xy += xy;
	yz += yz;
	zx += zx;
	wx += wx;
	wy += wy;
	wz += wz;

	m.axis(0).setValue((MeReal)(1.0)-yy-zz,xy-wz,zx+wy);
	m.axis(1).setValue(xy+wz,(MeReal)(1.0)-zz-xx,yz-wx);
	m.axis(2).setValue(zx-wy,yz+wx,(MeReal)(1.0)-xx-yy);
}

#endif // ifndef _vectormath_h_
