/* -*-c++-*-
 *===============================================================
 * File:        McduRouter.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.4.18.1 $
 * $Date: 2002/04/04 15:28:58 $
 *
 *================================================================
 */

#include <McduRouter.h>
#include "McduModelIndexTable.h"

/*----------------------------------------------------------------
 * McduRouter implementation
 *----------------------------------------------------------------
 */

IDTABLE_DECLARE_STRUCT( McduRouter, McdModelPairContainer* )

IDTABLE_INIT( McduRouter, McdModelPairContainer* )
IDTABLE_TERM( McduRouter )
IDTABLE_SETID( McduRouter, RouterId )
IDTABLE_GETID( McduRouter, RouterId )

void
McduRouterSetPairContainer( int id1, int id2,
                McdModelPairContainer* pc)
{
  /* CHECK < size */
  gMcduRouter->ididToElement[id1 + id2*gMcduRouter->size] = pc;
}

McdModelPairContainer*
McduRouterGetPairContainer( int id1, int id2 )
{
  /* CHECK < size */

  return gMcduRouter->ididToElement[id1 + id2*gMcduRouter->size];
}


