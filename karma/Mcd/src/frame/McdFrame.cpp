/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:54 $ - Revision: $Revision: 1.100.2.3 $

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

/* Notes for Doxyfile files:

Doxygen problems:

 Some declarations appear with no comment, when actually there is one.

 Some delcaration do not appear, whether there is a comment or not.
 This is more serious than previous one, because occurs silently.

 Preferences: keep list of declarations clean, ie. not randomly
 interspersed with "brief comments". Keep full description complete. Some
 full description do not appear because there was a "brief descrition"
 earlier. Pain in the ass for developers.

 declarations marked @internal should not appear anywhere in the doxy
 output.

 if a struct is marked @internal, then all of its data should not need to
 be marked @internal as well. The whole thing should simply not appear in
 the output.

*/

/*
  @file McdFrame.cpp
  Collision Toolkit core functions.
*/

#include <string.h>
#include <MePrecision.h>

#include <MePool.h>
#include <MeVersion.h>
#include <MeMessage.h>
#include <MeProfile.h>
#include <MeMemory.h>

#include <McdCheck.h>
#include <McdMessage.h>
#include <McdGeometry.h>
#include <McdModel.h>
#include <McdInteractions.h>
#include <McdInteractionTable.h>
#include <McdModelPair.h>
#include <McdGeometryTypes.h>

#include <McdFrame.h>

/*
 * Framework
 *
 */

/**
 * Initialize the Mcd framework.
 * @arg geoTypeMaxCount
 *      maximum number of geometry types that will be
 *      registered with the Mcd system. If you are only
 *      registering primitive geometry types via
 *      McdPrimitivesRegisterTypes(), then
 *      McdPrimitivesGetTypeCount() can be used for the @a
 *      geoTypeMaxCount
 *
 * @arg modelCount: the maximum number of collision models which will exist
 * simultaneously
 */

extern "C"
McdFrameworkID MEAPI
McdInit( int geoTypeMaxCount, int modelCount, int instanceCount, MeReal scale)
{
    McdFramework *f = (McdFramework*)
        MeMemoryAPI.create( sizeof( McdFramework ) );

    f->termActions = 0;
    f->toolkitVersionString = MeToolkitVersionString();
    
    /* init geometries */
    f->geometryRegisteredCountMax = kMcdGeometryBuiltInTypes + geoTypeMaxCount;

    MEASSERT(f->geometryRegisteredCountMax<256);
    
    f->geometryRegisteredCount = 0;
    
    f->geometryVTableTable = (McdGeometryVTable *)
        MeMemoryAPI.create( f->geometryRegisteredCountMax * sizeof(McdGeometryVTable));

    memset(f->geometryVTableTable,0,f->geometryRegisteredCountMax * sizeof(McdGeometryVTable));

    /* Init Models */
    (*MePoolAPI.init)(&f->modelPool,modelCount,sizeof(McdModel), 0);
    
    /* Init GeometryInstances */
    (*MePoolAPI.init)(&f->instancePool,instanceCount,sizeof(McdGeometryInstance), 0);
    
    /* init interaction table */
    f->interactionTable = (McdInteractions *)
        MeMemoryAPI.create(f->geometryRegisteredCountMax * 
                           f->geometryRegisteredCountMax * sizeof(McdInteractions));

    memset(f->interactionTable,0,f->geometryRegisteredCountMax * 
                           f->geometryRegisteredCountMax * sizeof(McdInteractions));

    f->mHelloCallbackFnPtr = 0;

    f->cachePool.t = MePoolNULL;    /* gjk will initialize this pool */

    f->request = (McdRequest *)MeMemoryAPI.create(sizeof(McdRequest));
    f->request->contactMaxCount = 4;
    f->request->faceNormalsFirst = 0;
    f->defaultRequest = f->request;

    f->firstGeometry = 0;
    f->firstModel = 0;

    f->modelCount = 0;
    f->geometryCount = 0;

    f->mDefaultPadding = (MeReal)0.005 * scale;
    f->mScale = scale;

    f->mScale = scale;

    return f;
}


/**

  Destroy all models and geometries associated with this framework. 
  This will delete all models, then iterate over the geometries at most 
  MAX_GUARD (default 100) times, deleting all geometries whose reference 
  count has been reduced to zero. If you have aggregates nested deeper than this, 
  you should recompile McdFramework.cpp with an appropriate value of MAX_GUARD.
*/

#define MAX_GUARD 100

extern "C"
void MEAPI
McdFrameworkDestroyAllModelsAndGeometries(McdFrameworkID f)
{
    int guard = 0;
    McdGeometryID g, nextG;

    while(f->firstModel)
        McdModelDestroy(f->firstModel);

    for(g = f->firstGeometry; f->firstGeometry; g = nextG)
    {
        nextG = g->next;

        if(McdGeometryGetReferenceCount(g)==0)
            McdGeometryDestroy(g);

        if(nextG==f->firstGeometry && guard++>MAX_GUARD)
        {
            MeWarning(1,"Failed to destroy all geometries because reference counts could not be reduced"
                 " to zero");
            return;
        }
    }
}

/**
    return the number of models allocated from the framework
*/
int MEAPI 
McdFrameworkGetModelCount(McdFrameworkID f)
{
    return f->modelCount;
}

/**
    return the number of geometries associated with the framework
*/

int MEAPI 
McdFrameworkGetGeometryCount(McdFrameworkID f)
{
    return f->geometryCount;
}


/**
Shut down the Mcd framework. Frees all memory allocated in McdInit(),
and any memory that may have been allocated by any RegisterType() or
RegisterInteraction() calls.
*/

extern "C"
void MEAPI
McdTerm(McdFrameworkID frame)
{

    /* nuke geometries */

    McdTermActionLink *actionLink;
    for( actionLink = frame->termActions; actionLink ;
    actionLink = (McdTermActionLink*)actionLink->next )
    {
        actionLink->action(frame);
    }

    if (frame->cachePool.t != MePoolNULL)
        MePoolAPI.destroy(&frame->cachePool);

    frame->geometryRegisteredCountMax = 0;
    frame->geometryRegisteredCount = 0;    

    MePoolAPI.destroy(&frame->modelPool);
    MePoolAPI.destroy(&frame->instancePool);
    MeMemoryAPI.destroy(frame->request);
    MeMemoryAPI.destroy(frame->geometryVTableTable);
    MeMemoryAPI.destroy(frame->interactionTable);

    if(frame->termActions)
        MeMemoryAPI.destroy(frame->termActions);
    MeMemoryAPI.destroy(frame);
}

/** @internal */

extern "C"
void              MEAPI
McdFrameworkRegisterTermAction(McdFrameworkID frame, McdTermAction action )
{
    McdTermActionLink *newLink
        = (McdTermActionLink*)MeMemoryAPI.create( sizeof( McdTermActionLink ));
    
    newLink->action = action;
    newLink->next = frame->termActions;
    frame->termActions = newLink;
}


/**
Sets the default contact tolerance to be used when creating an McdModel.
@see McdModelSetContactTolerance
*/
void MEAPI McdFrameworkSetDefaultContactTolerance(McdFrameworkID frame, MeReal inPadding) {
    frame->mDefaultPadding = inPadding;
}

/**
Returns the default contact tolerance.
@see McdModelSetContactTolerance
*/
MeReal MEAPI McdFrameworkGetDefaultContactTolerance(McdFrameworkID frame) {
    return frame->mDefaultPadding;
}



/**
returns 1 if the type id corresponds to a geometry that has been
registered.
*/
/** @internal */
extern "C"
void MEAPI
McdFrameworkResetTypes(McdFrameworkID frame)
{
    frame->geometryRegisteredCount = 0;
}

extern "C"
MeBool MEAPI
McdFrameworkTypeIsValid(McdFrameworkID frame, int id )
{
    return (id>=0 && id<frame->geometryRegisteredCountMax
            && frame->geometryVTableTable[id].registered);
}


/** @internal */
extern "C"
const char* MEAPI
McdFrameworkGetTypeName(McdFrameworkID frame, int id )
{
    if( !McdFrameworkTypeIsValid(frame, id ) )
    {
        return "UnregisteredGeometryType";
    }
    return frame->geometryVTableTable[id].name;
}

/** @internal */
extern "C"
void MEAPI
McdFrameworkShowTypes(McdFrameworkID frame)
{
    MeInfo(0,"McdGeometryShowTypes:");
    MeInfo(0,"      ( #types registered: %d )",frame->geometryRegisteredCount);
    
    
    int i;
    for( i = 0 ; i < frame->geometryRegisteredCount ; ++i )
    {
        MeInfo(0,"   index #%d: %s", i, frame->geometryVTableTable[i].name );
    }
    
    MeInfo(0,"");
}

MeBool            MEAPI
McdFrameworkTypeIsRegistered(McdFrameworkID frame, int typeId )
{
    if(typeId >= 0 && typeId < frame->geometryRegisteredCountMax &&
        frame->geometryVTableTable[typeId].registered!=0)
        return 1;
    return 0;
}


/** returns the number of concrete geometry types currently registered with
the system.
*/
extern "C"
MeI16 MEAPI
McdFrameworkGetRegisteredTypeCount(McdFrameworkID frame)
{ 
    return frame->geometryRegisteredCount; 
}


/** Get a pointer to the default McdRequest object.
    The default McdRequest object is the one that each McdModelPair object
    points to upon creation ( via McdModelPairCreate() ).
    This is typically used to set the maximum
    number of contacts to be generated
    by all pairs of colliding models.
    For more control the McduRequestTable can be used.
    @see McduRequestTableInit
*/

extern "C"
McdRequest * MEAPI
McdFrameworkGetDefaultRequestPtr(McdFrameworkID frame) 
{
  return frame->defaultRequest;
}

/** Set the default McdRequest. @a r will be the value returned by
    subsequent calls to McdGetDefaultRequestPtr().
*/

extern "C"
void MEAPI
McdFrameworkSetDefaultRequestPtr(McdFrameworkID frame, McdRequest *r)
{
  frame->defaultRequest = r;
}


/** @internal */

extern "C"
void MEAPI
McdFrameworkRegisterGeometryType(McdFramework *frame,
                                 McdGeometryType typeId,
                                 char *typeName,
                                 McdGeometryDestroyFnPtr f_destroy,
                                 McdGeometryGetAABBFnPtr f_getAABB,
                                 McdGeometryGetBSphereFnPtr f_getBSphere,
                                 McdGeometryMaximumPointFnPtr f_maximumPoint,
                                 McdGeometryGetMassPropertiesFnPtr f_getInertiaMatrix,
                                 McdGeometryDebugDrawFnPtr f_debugDraw)
{
    if( frame->geometryRegisteredCountMax <= typeId )
    {
        McdCoreError(kMcdErrorNum_InvalidGeoType," in McdGeometryRegisterType.", "McdGeometryRegisterType", 
            __FILE__,__LINE__);
    }
    
    MCD_ASSERT(frame->geometryVTableTable[typeId].registered == 0, "McdGeometryRegisterType");
    
    frame->geometryVTableTable[typeId].registered = 1;
    frame->geometryVTableTable[typeId].destroy = f_destroy;
    frame->geometryVTableTable[typeId].getAABB = f_getAABB;
    frame->geometryVTableTable[typeId].getBSphere = f_getBSphere;
    frame->geometryVTableTable[typeId].maximumPoint = f_maximumPoint;
    frame->geometryVTableTable[typeId].getMassProperties = f_getInertiaMatrix;
    frame->geometryVTableTable[typeId].debugDraw = f_debugDraw;
    frame->geometryVTableTable[typeId].name = typeName;
    frame->geometryRegisteredCount++;

}


extern "C"
void MEAPI
McdFrameworkSetInteractions(McdFramework *frame,
                              int geoType1, int geoType2,
                              McdInteractions* interactions)
{
    McdInteractions *element = frame->interactionTable + geoType1 + frame->geometryRegisteredCountMax * geoType2;
    MCD_INTERACTIONTABLE_CHECKBOUNDS(frame, geoType1, geoType2, "McdInteractionTableSetElement" )
        
    *element = *interactions;
    element->swap = 0;

    if (geoType1 != geoType2) 
    {
        element = frame->interactionTable + geoType2 + frame->geometryRegisteredCountMax * geoType1;
        *element = *interactions;
        element->swap = 1;
    }
}


McdInteractions* MEAPI
McdFrameworkGetInteractions(McdFramework *frame, int geoType1, int geoType2)
{
    MCD_INTERACTIONTABLE_CHECKBOUNDS(frame, geoType1, geoType2, "McdInteractionTableGetInteractions" );        
    return &frame->interactionTable[geoType1 + frame->geometryRegisteredCountMax * geoType2 ];
}


MeBool MEAPI 
McdFrameworkGetInteractionsWarned(McdFramework *framework,
    int geoType1,int geoType2)
{
    MCD_INTERACTIONTABLE_CHECKBOUNDS(framework,
        geoType1, geoType2, "McdInteractionTableGetInteractions");

    return framework->interactionTable
        [geoType1 + framework->geometryRegisteredCountMax * geoType2].warned;
}

void MEAPI 
McdFrameworkSetInteractionsWarned(McdFramework *framework,
                                  int geoType1,
                                  int geoType2,
                                  MeBool warned)
{
    MCD_INTERACTIONTABLE_CHECKBOUNDS(framework, geoType1, geoType2, "McdInteractionTableGetInteractions" );        
    framework->interactionTable[geoType1 + framework->geometryRegisteredCountMax * geoType2 ].warned = warned;
}
    

#ifdef MCDCHECK
#include <stdio.h>  
void MEAPI McdFrameworkPrintInteractionTable(McdFramework *frame)
{
    int i,j;

    printf("McdInteractionTable\n");
    printf("\t size: %d\n",frame->geometryRegisteredCountMax );
    
    for( i = 0 ; i < frame->geometryRegisteredCountMax ; i++ )
        for( j = 0 ; j < frame->geometryRegisteredCountMax ; j++ )
        {
            McdInteractions *element = &frame->interactionTable[j + frame->geometryRegisteredCountMax * i ];
            printf("i,j %d %d %s %s \n",i,j,
                McdFrameworkGetTypeName(frame,i),
                McdFrameworkGetTypeName(frame,j) );
            printf("\t\t interactions.swap: %x \n", element->swap);
            printf("\t\t interactions.helloFn: %x \n", (unsigned int)element->helloFn);
            printf("\t\t interactions.goodbyeFn: %x \n", (unsigned int) element->goodbyeFn);
            printf("\t\t interactions.intersectFn: %x \n",
                (unsigned int)element->intersectFn);
            printf("\t\t interactions.safetimeFn: %x \n", (unsigned int)element->safetimeFn);
        }
        
}
#endif


/* line segment */

void MEAPI
McdFrameworkSetLineSegInteraction(McdFramework *frame, int geoType, McdLineSegIntersectFnPtr isectfn )
{
    MCD_INTERACTIONARRAY_CHECKBOUNDS(frame, geoType, "McdLineSegRegisterInteraction")
    frame->geometryVTableTable[geoType].lineSegIntersect = isectfn;
}

/* line segment */

McdLineSegIntersectFnPtr MEAPI
McdFrameworkGetLineSegInteraction(McdGeometryID geometry )
{
    McdGeometryType geoType = McdGeometryGetTypeId(geometry);
    McdFrameworkID frame = geometry->frame;

    MCD_INTERACTIONARRAY_CHECKBOUNDS(frame,geoType, "McdLineSegRegisterInteraction");
    return frame->geometryVTableTable[geoType].lineSegIntersect;
}

