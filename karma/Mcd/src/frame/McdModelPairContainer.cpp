/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.23.4.1 $

   This software and its accompanying manuals have been developed
   by MathEngine PLC ("MathEngine") and the copyright and all other
   intellectual property rights in them belong to MathEngine. All
   rights conferred by law (including rights under international
   copyright conventions) are reserved to MathEngine. This software
   may also incorporate information which is confidential to
   MathEngine.

   Save to the extent permitted by law, or as otherwise expressly
   permitted by MathEngine, this software and the manuals must not
   be copied (in whole or in part), re-arranged, altered or adapted
   in any way without the prior written consent of the Company. In
   addition, the information contained in the software may not be
   disseminated without the prior written consent of MathEngine.

 */

#include <stdlib.h>
#include <stdio.h>
#include <MeMemory.h>
#include <McdModelPairContainer.h>
#include <McdCheck.h>

/*----------------------------------------------------------------
 * McdModelPairContainer implementation
 *----------------------------------------------------------------
 */

/** Create a new McdModelPairContainer object.
    @a size is the size of the entire array, which is divide into three
    sections, one for each of "hello","staying" and "goodbye".
    Goodbye pairs get inserted at the start of the model pair container,
    Hello and Staying pairs at the end.
 */

McdModelPairContainer *MEAPI
McdModelPairContainerCreate( int size )
{
  McdModelPairContainer* pairs =
  (McdModelPairContainer*)MeMemoryAPI.create( sizeof( McdModelPairContainer));
  McdModelPair **array = (McdModelPair**)MeMemoryAPI.create( size*sizeof( McdModelPair*) );
  McdModelPairContainerInit( pairs, array, size );

  return pairs;
}

/** Initialize a McdModelPairContainer object.
    This is used in place of McdModelPairContainerCreate
    when it is desired to allocate it on the stack.
*/
void MEAPI
McdModelPairContainerInit( McdModelPairContainer *a,
               McdModelPair **array, int size )
{
  a->array = array;
  a->size = size;
  McdModelPairContainerReset( a );
}

/** Reset @a a. After this point, the container is empty.
 */
void MEAPI
McdModelPairContainerReset( McdModelPairContainer* a )
{
  a->goodbyeFirst = 0;
  a->goodbyeEnd = 0;

  a->helloFirst = a->size;
  a->helloEndStayingFirst = a->size;
  a->stayingEnd = a->size;
}

/** Destroy @a a.
 */
void MEAPI
McdModelPairContainerDestroy( McdModelPairContainer* a )
{
    MeMemoryAPI.destroy(a->array);
    MeMemoryAPI.destroy(a);
}

/** Return the number of "goodbye" pairs in @a a.
    Identical to a->goodbyeEnd - a->goodbyeFirst.
*/
int MEAPI
McdModelPairContainerGetGoodbyeCount( McdModelPairContainer* a )
{
  return a->goodbyeEnd;
}
/** Return the number of "hello" pairs in @a a.
    Identical to a->helloEnd - a->helloFirst.
*/
int MEAPI
McdModelPairContainerGetHelloCount( McdModelPairContainer* a )
{
  return a->helloEndStayingFirst - a->helloFirst;
}
/** Return the number of "staying" pairs in @a a.
    Identical to a->stayingEnd - a->stayingFirst.
*/
int MEAPI
McdModelPairContainerGetStayingCount( McdModelPairContainer* a )
{
  return a->stayingEnd - a->helloEndStayingFirst;
}

/** Add a Hello pair to the ModelPairContainer. Return false if
there was no space
*/

MeBool MEAPI McdModelPairContainerInsertHelloPair(McdModelPairContainer *m, McdModelPairID p)
{
    if(m->goodbyeEnd==m->helloFirst) 
        return false;
    m->array[--m->helloFirst]=p;
    return true;
}

/** Add a Goodbye pair to the ModelPairContainer. Return false if
there was no space
*/

MeBool MEAPI McdModelPairContainerInsertGoodbyePair(McdModelPairContainer *m, McdModelPairID p)
{
    if(m->goodbyeEnd==m->helloFirst)
        return false;
    m->array[m->goodbyeEnd++]=p;
    return true;
}

/** Add a Staying pair to the ModelPairContainer. Return false if
there was no space
*/

MeBool MEAPI McdModelPairContainerInsertStayingPair(McdModelPairContainer *m, McdModelPairID p)
{
    if(m->goodbyeEnd==m->helloFirst)
        return false;
    m->array[--m->helloFirst]=m->array[--m->helloEndStayingFirst];
    m->array[m->helloEndStayingFirst]=p;
    return true;
}


/** Remove a pair from a ModelPairContainer
*/

void MEAPI McdModelPairContainerRemovePair(McdModelPairContainer *m, McdModelPairID *p)
{
    int i = p-m->array;
    if(i<0 && i>= m->size)
        return;

    if(i<m->goodbyeEnd)
    {
        m->array[i]=m->array[--m->goodbyeEnd];
    }
    else if(i>=m->helloEndStayingFirst)
    {
        m->array[i]=m->array[m->helloEndStayingFirst];
        m->array[m->helloEndStayingFirst++] = m->array[m->helloFirst++];
    }
    else if(i>=m->helloFirst)
    {
        m->array[i]=m->array[m->helloFirst++];
    }
}             



/** Return the location of the first "goodbye" pair in @a.
    Use in conjunction with McdModelPairContainerGetGoodbyeCount().
    Identical to a->array[a->goodbyeFirst]
*/
McdModelPair** MEAPI
McdModelPairContainerGetGoodbyeArray( McdModelPairContainer* a,
                      int *count )
{
  *count = McdModelPairContainerGetGoodbyeCount(a);
  return a->array;
}
/** Return the location of the first "hello" pair in @a.
    Use in conjunction with McdModelPairContainerGetHelloCount().
    Identical to a->array[a->helloFirst]
*/
McdModelPair** MEAPI
McdModelPairContainerGetHelloArray( McdModelPairContainer* a,
                      int *count )
{
  *count = McdModelPairContainerGetHelloCount(a);
  return (a->array + a->helloFirst);
}
/** Return the location of the first "staying" pair in @a.
    Use in conjunction with McdModelPairContainerGetStayingCount().
    Identical to a->array[a->stayingFirst]
*/
McdModelPair** MEAPI
McdModelPairContainerGetStayingArray( McdModelPairContainer* a,
                      int *count )
{
  *count = McdModelPairContainerGetStayingCount(a);
  return (a->array + a->helloEndStayingFirst);
}

#ifdef MCDCHECK
void MEAPI
McdModelPairContainerPrintStats( McdModelPairContainer* pc)
{
  printf("McdModelPairContainer: \n");
  printf("\t\t numHello: %d\n",
     McdModelPairContainerGetHelloCount(pc) );
  printf("\t\t numStay: %d\n",
     McdModelPairContainerGetStayingCount(pc) );
  printf("\t\t numGoodbye: %d\n",
     McdModelPairContainerGetGoodbyeCount(pc) );

  /*
  printf("\t size: %d\n", pc->size );
  printf("\t goodbyeEnd: %d\n", pc->goodbyeEnd );
  printf("\t helloFirst: %d\n", pc->helloFirst );
  printf("\t helloEndStayingFirst: %d\n", pc->helloEndStayingFirst );
  printf("\t stayingEnd: %d\n", pc->stayingEnd );
  */

}
#endif

/*----------------------------------------------------------------
 *  McdModelPairContainerIterator
 *----------------------------------------------------------------
 */

/** Initialize @a iter with @a pairs.
    Currently used only in conjunction with McdIntersectEach
*/
void MEAPI
McdModelPairContainerIteratorInit( McdModelPairContainerIterator* iter,
                   McdModelPairContainer* pairs )
{

  iter->container = pairs;
  iter->count = pairs->helloEndStayingFirst;
}

/* Notes

ContainerIterator reads "cuurent pairs" backwards,
so that stay pairs are done before hello ones.
This improves resource usage: stay MdtContacts may be freed up
when stay pairs are passed through (eg. less contacts needed )
and then available for the hello ones.
Subtle optimization..

*/

/*----------------------------------------------------------------
 *  McdModelPairContainerIterator
 *----------------------------------------------------------------
 */

#if 0
#ifdef MCDCHECK
void              MEAPI
McdPairContainerPrintStats( McdPairContainer* pairs)
{
  printf("McdPairContainer: \n");
  printf("\t\t numHello: %d\n", pairs->helloCount );
  printf("\t\t numStay: %d\n", pairs->stayingCount );
  printf("\t\t numGoodbye: %d\n", pairs->goodbyeCount );
 }
#endif
#endif

