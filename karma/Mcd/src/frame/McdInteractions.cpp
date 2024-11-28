/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.53.2.3 $

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

#include <McdInteractions.h>
#include <McdInteractionTable.h>
#include <McdGeometry.h>
#include <McdCheck.h>
#include <McdModel.h>
#include <McdModelPair.h>
#include <MeMath.h>
#include <MeMessage.h>
#include <McdContact.h>

/**
    Determine quickly if two objects are potentially colliding.
    Returns non zero value if the object's axis aligned bounding
    boxes intersect.
*/
MeBool    MEAPI
McdNearby(McdModelPair *p)
{
    MeVector3 minCorner1, maxCorner1;
    MeVector3 minCorner2, maxCorner2;
    int       axis;
    MeBool    disjointNotFound = 1;

    McdModelGetAABB(p->model1, minCorner1, maxCorner1);
    McdModelGetAABB(p->model2, minCorner2, maxCorner2);

    for (axis = 0; (disjointNotFound) && (axis < 3); ++axis) {
    if (maxCorner1[axis] < minCorner2[axis])
        disjointNotFound = 0;
    else if (minCorner1[axis] > maxCorner2[axis])
        disjointNotFound = 0;
    }

    /* if disjointNotFound, then pair is nearby */
    return disjointNotFound;
}

#if 0
MeBool             MEAPI McdTouch( McdModelPair* p)
{
  should be distinct function registered in interactiontable

    McdRequest *currentRequest;
    McdRequest touchRequest;
    MeBool touch;

    touchRequest.contactMaxCount = 0;
    p->request = &touchRequest;

    touch = McdIntersect( p, 0);
    p->request = currentRequest;
}
#endif

/**
   Prepare @a p for use by pairwise queries.
   McdHello() must be called on a pair before using it in any queries, such
   as McdIntersect() or McdSafeTime().

   This call should be made on any "hello" pairs obtained from McdSpace for
   which pairwise queries are to be made.

   This call may involve allocation internal cached data, or
   preprocessing some information for use by McdIntersect() or
   McdSafeTime(), or selection of the appropriate algorithm based on
   some of the pair's McdRequest values. All data associated with
   these operations is stored (by reference) in private fields in the
   McdModelPair object.  The order of the two McdModel objects, as
   obtained from McdModelPairGetModels(), may be inverted after
   calling McdHello, in order to match the order in which the
   underlying algorithm has been written. This saves performing an
   internal "swap" operation each time a pairwise query is
   invoked. The values in the McdRequest object associated with @p may
   be examined in order to select an algorithmic specialization or
   perform some additional preprocessing.

   After McdHello has been called, the following calls are not allowed on
   @p until McdGoodbye is called. Otherwise, undefined behaviour results:
   McdModelPairReset()
   McdModelPairSetRequestPtr()

    @see McdIntersect McdSafeTime McdSpaceGetPairs, McdSpaceGetTransitions */

//#define MCDTRACE 1

void MEAPI
McdHello(McdModelPair *p)
{
    MCD_CHECKMODELPAIRID(p, "McdHello");

#ifdef MCDTRACE
    char     *s1, *s2;
    McdModelPairGetGeometryNames(p, &s1, &s2);
    printf("McdHello:  %s %s \n", s1, s2);
#endif

    McdFramework *frame = p->model1->frame;

    McdInteractions *interactions =
                       McdFrameworkGetInteractions(p->model1->frame,
                       McdGeometryGetType(p->model1->mInstance.mGeometry),
                       McdGeometryGetType(p->model2->mInstance.mGeometry));
    
    if (interactions->swap) 
    {
        McdModelID tmp = p->model1;
        p->model1 = p->model2;
        p->model2 = tmp;
    }

    p->request = McdFrameworkGetDefaultRequestPtr(frame);

    if (frame->mHelloCallbackFnPtr)
        (*(frame->mHelloCallbackFnPtr))(p);

    if (interactions->helloFn)
        (*interactions->helloFn)(p);

    return;
}


/**
    Informs the system that no more queries will be performed on this pair.
    Frees up any internal data associated with this pair, if applicable.
    Possible queries that can no longer be called on this pair are
    McdIntersect() and McdSafeTime()

   This call should be made on any "goodbye" pairs obtained from McdSpace for
   which McdHello has been previously called.

    @see McdIntersect McdSafeTime McdSpaceGetPairs, McdSpaceGetTransitions
*/

extern    "C" void MEAPI
McdGoodbye(McdModelPair *p)
{
    MCD_CHECKMODELPAIRID(p, "McdGoodbye");
    MCD_CHECKMODELPAIR(p, "McdGoodbye");

#ifdef MCDTRACE
    char     *g1 = McdGeometryGetTypeName(McdModelGetGeometry(p->model1));
    char     *g2 = McdGeometryGetTypeName(McdModelGetGeometry(p->model2));
    printf("McdGoodbye: %s %s \n", g1, g2);
#endif

    McdInteractions *interactions =
                       McdFrameworkGetInteractions(p->model1->frame,
                       McdModelGetGeometryType(p->model1),
                       McdModelGetGeometryType(p->model2));

    if (interactions->goodbyeFn)
        (*interactions->goodbyeFn)(p);

  /** should reset the internal "cached" fields of p */

}

/**
   Computes the intersection of two McdModel objects, representing it as a
   set of McdContact objects stored in @a result.
   The number of contacts computed depends on which geometry type -
   geometry type combination the pair has, and on the algorithm.

   The user must allocate @a result and the @a contacts array of the
   McdIntersectResult struct.
   The maximum number of contacts produced is guaranteed to be less than or equal to
   the number set in the @a contactMaxCount field of McdIntersectResult.
   The underlying algorithm may also examine the @a request field of
   McdModelPair, in particular the @a contactMaxCount field of
   McdRequest. This value represents a user's desired number of contacts to
   be produced, which may be less than the actual size of the McdContact array.
   The algorithm will attempt to meet this request, but it is not
   guaranteed.
   This McdRequest value is particularly important for intersections with
   non-primitive geometry types such as McdTriangleMesh and McdConvexMesh,
   which may potentially involve a large number of contact points.

   McdHello() needs to be called on @a p before using this function.
   The McdRequest object associated with @a p must not change after
   McdHello() has been called.

   The McdIntersectResult struct represents an intersection in a format
   that is appropriate for many types of response, in particular
   physically-based response. Contact data is used by
   MathEngine's Dynamics Toolkit, and this function is called directly by
   MathEngine's Simulation Toolkit.

   @see McdHello McdIntersectResult
*/

/* return 0 if fn not there? this return logic should be in McdHello, at
   least. perhpas repeated here as well.. */

extern    "C" MeBool MEAPI
McdIntersect(McdModelPair *p, McdIntersectResult *result)
{
    MCD_CHECKMODELPAIR(p, "McdIntersect");
    MCD_CHECKINTERSECTRESULT(result, "McdIntersect");
    McdInteractions *interactions;
    McdContact *outContacts;
    McdFramework *frame = p->model1->frame;
    
#ifdef MCDTRACE
    char     *g1 = McdGeometryGetTypeName(McdModelGetGeometry(p->model1));
    char     *g2 = McdGeometryGetTypeName(McdModelGetGeometry(p->model2));
    printf("McdIntersect: %s %s \n", g1, g2);
#endif

    int type1 = McdModelGetGeometryType(p->model1);
    int type2 = McdModelGetGeometryType(p->model2);
    result->pair = p;
    
    interactions = McdFrameworkGetInteractions(frame,type1,type2);
    
    if(!interactions->intersectFn && !McdFrameworkGetInteractionsWarned(frame,type1,type2))
    {
        MeWarning(1,"No test registered between %s and %s",
        McdFrameworkGetTypeName(frame,type1),McdFrameworkGetTypeName(frame,type2));
        McdFrameworkSetInteractionsWarned(frame,type1,type2,1);

        result->contactCount = result->touch = 0;
        return 0;
    }

    if(interactions->cull)
    {
        outContacts = result->contacts;
        result->contacts = (McdContact *)MeMemoryALLOCA(512*sizeof(McdContact)); // TODO: fix me!
    }

    result->touch = (*interactions->intersectFn)(p, result);
    
    if(interactions->cull)
    {
        result->contactCount = McdContactSimplify(result->normal,
            result->contacts, result->contactCount,
            outContacts, p->request->contactMaxCount,
            p->request->faceNormalsFirst, frame->mScale);
        result->contacts = outContacts;
    }

    return 1;           // intersect function found
}

/** @internal */
/**
   Performs appropriate collision test on input pair at an instant of
   time in the future specified by @time, extrapolating the current
   positions of both models using the linear and angular velocities.
   All data returned in McdIntersectResult "result."

   The return value is 1 if the proper collision function was
   available, 0 otherwise.

   McdHello needs to be called on this pair before calling this
   function.  @see McdIntersectResult McdSpaceGetStayingPairs
*/

extern    "C" MeBool MEAPI
McdIntersectAt(McdModelPair *p, McdIntersectResult *result, MeReal time)
{
    MeMatrix4 tm1, tm2;
    McdModelID m1 = p->model1, m2 = p->model2;
    MeBool    ret;
    MeMatrix4Ptr tm1save = McdModelGetTransformPtr(m1);
    MeMatrix4Ptr tm2save = McdModelGetTransformPtr(m2);

    MeMatrix4TMUpdateFromVelocities(tm1, (MeReal) 1e-4, time,
                                    (const MeReal *)m1->linearVelocity,
                                    (const MeReal *)m1->angularVelocity, tm1save);
    MeMatrix4TMUpdateFromVelocities(tm2, (MeReal) 1e-4, time,
                                    (const MeReal *)m2->linearVelocity,
                                    (const MeReal *)m2->angularVelocity, tm2save);
    McdModelSetTransformPtr(m1, tm1);
    McdModelSetTransformPtr(m2, tm2);
    ret = McdIntersect(p, result);
    McdModelSetTransformPtr(m1, tm1save);
    McdModelSetTransformPtr(m2, tm2save);
    return ret;
}

/**
    Performs appropriate SafeTime (time of impact and swept volume)
    computation on input pair.
    All data is returned in McdSafeTimeResult "result".
    Models are assumed to be synchronized, that is, their transforms
    correspond to positions at identical instants of time.
    The model's linear and angular velocities are used to describe
    the motion of the object.
    The input value of maxTime indicates the maximum time for travel
    given the model linear and angular velocities.
    The timestep for a dynamical simulation is a typical value for maxTime.
    For a value of 1, the object is assumed to translate by the
    entire linear velocity vector.

    The return value is 1 if the proper SafeTime function was available,
    0 otherwise.
    This function must be called after McdHello(p)
    @see McdSafeTimeResult for a description of the data returned.

    This function is subject to change.
*/

extern    "C" MeBool MEAPI
McdSafeTime(McdModelPair *p, MeReal maxTime, McdSafeTimeResult * result)
{
    int ret;

    
    // assume swapping happened at hello time.
    McdInteractions *interactions =
                       McdFrameworkGetInteractions(p->model1->frame,
                       McdGeometryGetType(p->model1->mInstance.mGeometry),
                       McdGeometryGetType(p->model2->mInstance.mGeometry));


    if (interactions->safetimeFn && p->model1->linearVelocity && p->model2->linearVelocity &&
        p->model1->angularVelocity && p->model2->angularVelocity)
    {
        ret = (*interactions->safetimeFn)(p, maxTime, result);
#ifdef MCDCHECK
        if (ret == 2) {     // something went wrong in the function
            char      bothNames[200];
            const char     *geo1, *geo2;
            McdModelPairGetGeometryNames(p, &geo1, &geo2);
            sprintf(bothNames, "%s %s.", geo1, geo2);
            McdCoreError(McdErrorNum_SafeTimeUnimplementedCase, bothNames,"McdSafeTime",__FILE__,__LINE__);
            result->time = maxTime;
        }
        // TODO: contact culling
#endif
    } else {
        // create a sensible default result
        result->time = maxTime;
    }

    return (interactions->safetimeFn != NULL);
}

/*----------------------------------------------------------------
 * McdLineSegIntersect implementation
 *----------------------------------------------------------------
 */

 /**
    Intersects an oriented line segment with a single model.

    @arg cm the collision model
    @arg inOrig pointer to an MeVector3 representing the first point on the line segment.
    @arg inDest pointer to an MeVector3 representing the second point on the line segment.
    @arg outOverlap structure containing the line segment intersection data.

    @see McdSpaceGetLineSegmentOverlapList
*/
extern "C"
unsigned int MEAPI
McdLineSegIntersect( const McdModelID cm, MeReal* const inOrig, MeReal* const inDest,
                     McdLineSegIntersectResult *outOverlap)
{
  MCD_CHECKMODEL(cm, "McdLineSegIntersect");

  outOverlap->model = cm;

  McdLineSegIntersectFnPtr theFn
    = McdGeometryGetLineSegIntersectFnPtr(McdModelGetGeometry(cm));

  return (theFn && (*theFn)(cm, inOrig, inDest, outOverlap));
}

/*----------------------------------------------------------------
 * McdHelloCallbackFn implementation
 *----------------------------------------------------------------
 */


/** Set the hello callback function to f.
    This function is called by McdHello, is typically used to set the
    McdRequest pointer for a particular McdModelPair object.
    If NULL, it will be ignored.

    @see McdHello.
*/
void      MEAPI
McdSetHelloCallback(McdFramework *frame, McdHelloCallbackFnPtr f)
{
    frame->mHelloCallbackFnPtr = f;
}

/** Get the current hello callback function.
    NULL by default.
 */

McdHelloCallbackFnPtr MEAPI
McdGetHelloCallback(McdFramework *frame)
{
    return frame->mHelloCallbackFnPtr;
}


/** Call McdHello on each "hello" pair in @a pairs.

    @see McdModelPairContainer, McdSpaceGetTransitions,
    McdSpaceGetPairs
*/
extern "C"
void MEAPI
McdHelloEach( McdModelPairContainer* pairs )
{
  int i;
  for( i = pairs->helloFirst ;
       i < pairs->helloEndStayingFirst; ++i )
    {
      McdHello( pairs->array[i] );
    }
}
/** Call McdGoodbye on each "goodbye" pair in @a pairs.

    @see McdModelPairContainer, McdSpaceGetTransitions,
    McdSpaceGetPairs
*/
extern "C"
void MEAPI
McdGoodbyeEach( McdModelPairContainer* pairs )
{
  int i;
  for( i = 0 ; i < pairs->goodbyeEnd; ++i )
    {
      McdGoodbye( pairs->array[i] );
    }
}

/* result array and contact array
   assume pointing to "first free element", ie indices start at zero
   If they are part of a bigger array,
   user must "bump them up" to reflect the change.
   Unlikely, since overflow condition is the problem,
   while the above is an "underflow" condition
*/

/* returns "overflow", true means that there are more pairs to
be intersected: "call again".
*/

/** Call McdIntersect on all "hello" and "staying" pairs in @a pairs.
    Must call McdHello on each of these pairs first, or simply call
    McdHelloEach() with @a pairs as argument.

    User allocates an array of McdIntersectResult structs and McdContact
    structs for the results to be written into. ( @a resultArray, @a
    resultArraySize, @a contactArray, @a contactArraySize ).

    As each pair is processed, it writes to as many of the next unwritten
    elements in @a contactArray as needed, and writes to the next unwritten
    element in @a resultArray. McdIntersectResult::contacts field will
    point to the location of the first element in @a contactArray that was
    used.

    The total number of elements in @a resultArray that were written to, and
    the total number of element in @a contactArray that were written to are
    set in the output arguments @a resultCount and @a contactCount,
    respectively.

    The return value indicates overflow:
    If either @a resultArray or @a contactArray were not large enough to
    allow all the appropriate pairs in @a pairs to be processed, a value of
    0 is returned, and @a pairsIter keeps track of which pairs in @a pairs
    have not been processed. In this case, process the results computed,
    and then call the function again until no overflow remains.

    This batch mode mechanism allows you to choose the workspace memory
    size you can afford to use, and then takes care of the bookkeeping to
    use that space effectively. A larger workspace means a larger number of
    pairs can be processed in "the same batch".
 */
extern "C"
MeBool MEAPI
McdIntersectEach(McdModelPairContainer* pairs,
                 McdModelPairContainerIterator* pairsIter,
                 McdIntersectResult* resultArray, int resultArraySize,
                 int *resultCount,
                 McdContact* contactArray, int contactArraySize,
                 int *contactCount)
{
    McdModelPair *pair;
    McdIntersectResult *result;
    int i;
    int resultIndex = 0; /* index into resultArray */
    int contactIndex = 0; /* index into contactArray */
    int freeContactCount;

    MeBool overflow = 0;

    for( i = pairsIter->count; i < pairs->stayingEnd; i++ )
    {
        pair = pairs->array[i];

        freeContactCount = contactArraySize - contactIndex;

        /* not enough contacts available to guarantee that current
        pair can be processed: overflow */
        if( freeContactCount < pair->request->contactMaxCount || resultIndex >= resultArraySize )
        {
            overflow = 1;
            break;
        }

        result = &(resultArray[resultIndex]);
        result->contacts = contactArray + contactIndex;
        result->contactMaxCount = freeContactCount;

        McdIntersect( pair, result  );

        resultIndex++;
        contactIndex+=result->contactCount;
    }

    pairsIter->count = i;

    *resultCount = resultIndex;
    *contactCount = contactIndex;

    return overflow;
}
