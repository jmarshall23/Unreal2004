/* -*-c++-*-
 *===============================================================
 * File:        McdTable.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.7.10.1 $
 * $Date: 2002/04/04 15:28:58 $
 *
 *================================================================
 */

#include "McdTable.h"
#include "McdCheck.h"

/*----------------------------------------------------------------
 * McdTable implementation
 *----------------------------------------------------------------
 */

#define MCDTABLE_GETELEMENT(table,i,j)\

MeBool
McdTableInit( McdTable* t, int size )
{
  MCD_ASSERT( t );
  if( !t )  return 0;

  t->size = size;

  t->table = (void**)MeMemoryAPI.create
    ( t->size * t->size * sizeof( void* ));

  MCD_ASSERT( t->table );
  if( !t->table )  return 0;

  int i,j;
  for( j = 0 ; j < t->size; ++j )
  for( i = 0 ; i < t->size; ++i )
    {
      t->table[ i + j * t->size ] = 0;
    }

  return 1;
}

void
McdTableTerm( McdTable* t)
{
  MCD_ASSERT( t );
  MCD_ASSERT( t->table );
  MeMemoryAPI.destroy( t->table );
  MeMemoryAPI.destroy( t );
}

void
McdTableSetElement( McdTable* t, int i, int j , void* element)
{
  MCD_ASSERT( t );
  MCD_ASSERT( t->table );
  t->table[ i + j * t->size ] = element;
}

void*
McdTableGetElement_fnCall( McdTable* t, McdModelPair* p )
{
  int i = McdGeometryGetTypeId( McdModelGetGeometry( p->model1));
  int j = McdGeometryGetTypeId( McdModelGetGeometry( p->model2));
  return t->table[ i + j * t->size ];
}
