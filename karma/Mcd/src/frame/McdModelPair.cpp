/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.31.2.1 $

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
#include <McdFrame.h>
#include <McdModelPair.h>
#include <McdCheck.h>
#include <McdModel.h>
#include <McdModelPair.h>
#include <McdGeometry.h>


/*----------------------------------------------------------------
 * McdModelPair implementation
 *----------------------------------------------------------------
 */

/** Create a new McdModelPair object.
    The object is bound to a specific pair of models at creation time,
    and it is expected to retain identity over multiple time steps.

    Cannot be used with any interactions ( such McdIntersect or McdSafeTime
    ) until McdHello is called on it.

    McdModelPair objects received via McdSpaceGetPairs() were created
    internally to McdSpace, in which case there is no need for the user
    to create McdModelPair objects explicitly.

    The McdRequest struct associated with this object is set to McdGetDefaultRequestPtr().

    @see McdHello(), McdGoodbye(), McdIntersect(), McdSafeTime(),
    McdModelPairReset()
*/

extern "C"
McdModelPairID MEAPI
McdModelPairCreate( McdModelID m1, McdModelID m2 )
{
  McdModelPair* p = (McdModelPair*)MeMemoryAPI.create( sizeof( McdModelPair ) );
  MCD_CHECKMODELPAIRID(p, "McdModelPairCreate");
  McdModelPairReset( p, m1, m2 );
  return p;
}


/**
   Reuse an existing McdModelPair object to refer to a different pair of
   McdModel objects. If McdHello() has been called on @a p, then
   McdGoodbye() must be called before calling
   McdModelPairReset(). Otherwise, the behaviour is undefined.

*/
extern "C"
void MEAPI
McdModelPairReset( McdModelPair* p, McdModelID model1, McdModelID model2 )
{
    MCD_CHECKMODELPAIRID(p, "McdModelPairReset");

    p->model1 = model1;
    p->model2 = model2;
    p->request = McdFrameworkGetDefaultRequestPtr(p->model1->frame);
    p->userData = 0;
    p->responseData = 0;
}

/**
   Destroy a McdModelPair object.
   The same conditions apply here as for McdModelPairReset(), viz:
   If McdHello() has been called on @a p, then
   McdGoodbye() must be called before calling
   McdModelDestroy(). Otherwise, the behaviour is undefined.
*/

/* should we guarantee an implementation for McdModelPairIsHello'ed() ?
   eg by checking p->intersectFn
*/

extern "C"
void MEAPI
McdModelPairDestroy( McdModelPairID p )
{
    MCD_CHECKMODELPAIRID(p, "McdModelPairDestroy");
    MeMemoryAPI.destroy( p );
}

/**
   Get the two McdModel objects that @a p is bound to.
   The values written to @a m1 and @a m2 correspond to those passed in via
   McdModelPairCreate or McdModelPairReset, whichever applies.
*/

extern "C"
void             MEAPI
McdModelPairGetModels( McdModelPairID p,
               McdModelID *m1, McdModelID *m2 )
{
  *m1 = p->model1;
  *m2 = p->model2;
}

/** Set @a r to be the McdRequest struct associated with @a p.
    If may prove useful to set this value inside the McdHello callback.
    @see McdSetHelloCallback()
 */
extern "C"
void             MEAPI
McdModelPairSetRequestPtr( McdModelPairID p, McdRequest* r )
{
  p->request = r;
}

/** Return a pointer to the McdRequest struct associated
    with @a p. In McdModelPairCreate and McdModelPairReset, this value is
    set to McdGetDefaultRequestPtr().
    The value can be overridden via McdModelPairSetRequestPtr().

*/

extern "C"
McdRequest * MEAPI
McdModelPairGetRequestPtr(const McdModelPairID p )
{
  return p->request;
}

/** Get the user data associated with @a p.
    Returns the value set in the last call to McdModelPairSetUserData().
    By default set to zero.

    @see McdModelPairSetUserData
*/
extern "C"
void *            MEAPI
McdModelPairGetUserData( McdModelPairID p)
{
  return p->userData;
}

/** Sets the user data associated with @a p to @a d.

    The user data field is particularly useful because @a p's identity is
    retained across multiple time steps, and guaranteed for the duration
    over which the pair is in close proximity.

 */
extern "C"
void             MEAPI
McdModelPairSetUserData( McdModelPairID p, void* d )
{
  p->userData = d;
}


  /** @internal */
#ifdef MCDCHECK
void MEAPI
McdModelPairGetGeometryNames( McdModelPair* p,
                  const char** stringPtr1, const char** stringPtr2 )
{
  *stringPtr1 = McdGeometryGetTypeName( McdModelGetGeometry( p->model1 ) );
  *stringPtr2 = McdGeometryGetTypeName( McdModelGetGeometry( p->model2 ) );
}
#endif
