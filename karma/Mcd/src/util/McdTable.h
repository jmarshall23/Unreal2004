/* -*-c++-*-
 *===============================================================
 * File:        McdTable.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.4.18.1 $
 * $Date: 2002/04/04 15:28:58 $
 *
 * Intended as reusable component for making geotypexgeotype
 * dispatch tables for SafeTime, Intersect, Distance, Touch, etc..
 *
 * It holds elements of type void*. The wrapper will cast this
 * to some function pointer type.
 *
 *================================================================
 */

#ifndef MCDTABLE_H
#define MCDTABLE_H

#include <McdModelPair.h>

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------
 * McdTable
 *---------------------------------------------------------------
 */

typedef struct
{
  int size;
  void **table;

} McdTable;

MeBool
McdTableInit( McdTable*, int size );

void
McdTableTerm( McdTable* );

void
McdTableSetElement( McdTable*, int i, int j , void* element );

  /* return the void* element */
#ifdef MCDCHECK

#define McdTableGetElementFromIndices( table, i, j)\
(MCD_ASSERT(table),\
MCD_ASSERT((i)>=0 && (i)<table->size &&\
(j)>=0 && (j)<table->size),\
table->table[i + j * table->size ])

#define McdTableGetElement( table, p )\
(MCD_CHECK_MODEL_PAIR(p),\
McdTableGetElementFromIndices( table, \
McdGeometryGetTypeId( McdModelGetGeometry( p->model1)),\
McdGeometryGetTypeId( McdModelGetGeometry( p->model2)))\
)

#else

#define McdTableGetElementFromIndices( table, i, j)\
table->table[ i + j * table->size ]

#define McdTableGetElement( table, p )\
(McdTableGetElementFromIndices( table, \
McdGeometryGetTypeId( McdModelGetGeometry( p->model1)),\
McdGeometryGetTypeId( McdModelGetGeometry( p->model2)))\
)

#endif

  /* temporary, function call version of macro definition */

void*
McdTableGetElement_fnCall( McdTable*, McdModelPair* );


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* MCDTABLE_H */
