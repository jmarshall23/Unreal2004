/* -*-c++-*-
 *===============================================================
 * File:        Jacobi.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *================================================================
 */

#ifndef _Jacobi_H_
#define _Jacobi_H_

#include <MePrecision.h>

typedef MeReal Float;
typedef MeReal Matrix3x3[3][3];

void      sym_schur2(Matrix3x3 a, int p, int q, Float *s, Float *c);

void      jacobi_update_cols(Matrix3x3 a, int i, int j, Float c, Float s);

void      jacobi_update_rows(Matrix3x3 a, int i, int j, Float c, Float s);

int       jacobi3x3(Matrix3x3 a, Matrix3x3 r);

#endif              /* _Jacobi_H_ */
