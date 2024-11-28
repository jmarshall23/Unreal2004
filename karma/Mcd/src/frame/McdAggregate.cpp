/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/17 13:44:34 $ - Revision: $Revision: 1.25.2.2 $

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

#include <MeMath.h>
#include <MeMemory.h>
#include <McdCheck.h>
#include <McdGeometry.h>
#include <McdGeometryTypes.h>
#include <McdGeometryInstance.h>
#include <McdAggregate.h>
#include <McdModel.h>
#include <McdModelPair.h>
#include <McdInteractions.h>
#include <McdInteractionTable.h>
#include <McdContact.h>

MCD_IMPLEMENT_GEOMETRY_TYPE( McdAggregate, "McdAggregate" , Aggregate);

/*----------------------------------------------------------------
 * McdAggregate implementation
 *----------------------------------------------------------------
 */

/**
  Create an aggregate with at most maxChildren children.
*/
McdAggregateID MEAPI
McdAggregateCreate(McdFramework *frame, int maxChildren)
{
    McdAggregate *a;
    int i;

    a = (McdAggregate *)
        MeMemoryAPI.createAligned(sizeof(McdAggregate),MeALIGNTO);
    if (!a) return 0;
    McdGeometryInit((McdGeometryID)a, frame, kMcdGeometryTypeAggregate);
    a->elementTable = (McdAggregateElement *)
        MeMemoryAPI.create(maxChildren * sizeof(McdAggregateElement));
    a->elementCountMax = maxChildren;
    a->elementCount = 0;
    for(i=0;i<maxChildren;i++)
        a->elementTable[i].mGeometry=0;

    return (McdAggregateID)a;
}

/**
    Add a geometry element with a relative transform to the aggregate.
*/
int MEAPI
McdAggregateAddElement(McdAggregateID g, McdGeometryID element, MeMatrix4Ptr relTM)
{
    McdAggregate* a = (McdAggregate*)g;
    int i;

    MCD_CHECKGEOMETRY(g, "McdAggregateInsertElement");
    MCD_ASSERT(a->elementCount < a->elementCountMax,
	"McdAggregateInsertElement");

    for(i=0;i<a->elementCountMax && a->elementTable[i].mGeometry;i++)
    MCD_ASSERT(i < a->elementCountMax, "McdAggregateInsertElement");

    if (i >= a->elementCountMax)
        return -1;

    MeMatrix4Copy(a->elementTable[i].mRelTM, relTM);
    a->elementTable[i].mGeometry = element;
    a->elementCount++;
    McdGeometryIncrementReferenceCount(element);
    return i;
}

/**
    Remove the nth aggregate element.
*/
void MEAPI 
McdAggregateRemoveElement(McdAggregateID g, int element)
{
    McdAggregate* a = (McdAggregate*)g;
    MCD_CHECKGEOMETRY(g, "McdAggregateInsertElement");
    MCD_ASSERT(element<a->elementCountMax && element>=0,
	"McdAggregateRemoveElement");
    
    McdGeometryDecrementReferenceCount(a->elementTable[element].mGeometry);
    a->elementTable[element].mGeometry = 0;
    MCD_ASSERT(element<a->elementCountMax && element>=0,
	"McdAggregateRemoveElement");

    a->elementCount--;
}

/**
    Get the number of elements in the aggregate.
*/
int MEAPI
McdAggregateGetElementCount(McdAggregateID g)
{
    McdAggregate* a = (McdAggregate*)g;

    MCD_CHECKGEOMETRY(g, "McdAggregateGetElementCount");
    return a->elementCount;
}

/**
    Get the maximum number of children the aggregate can hold.
*/
int MEAPI
McdAggregateGetElementCountMax(McdAggregateID g)
{
    McdAggregate* a = (McdAggregate*)g;

    MCD_CHECKGEOMETRY(g, "McdAggregateGetElementCountMax");
    return a->elementCountMax;
}

/**
    Get the n'th child's geometry from the aggregate.
*/
McdGeometryID MEAPI
McdAggregateGetElementGeometry(McdAggregateID g, int i)
{

    McdAggregate* a = (McdAggregate*)g;
    MCD_CHECKGEOMETRY(a, "McdAggregateGetElementCountMax");
    if(i>=a->elementCountMax)
        return 0;
    return a->elementTable[i].mGeometry;
}

/**
 *   Get the n'th child's transform from the aggregate.
 */
MeMatrix4Ptr MEAPI
McdAggregateGetElementTransformPtr(McdAggregateID g,int i)
{
    McdAggregate* a = (McdAggregate*)g;
    MCD_CHECKGEOMETRY(a, "McdAggregateGetElementCountMax");
    if(i>=a->elementCountMax)
        return 0;
    return a->elementTable[i].mRelTM;
}


/*--- McdAggregate polymorphic functions ---*/

void MEAPI
McdAggregateDestroy( McdGeometry* g)
{

    MCD_CHECKGEOMETRY(g, "McdAggregateDestroy");
    if ( g != NULL )
    {
        int i;
        McdAggregate* a = (McdAggregate*)g;

        for(i=0;i<a->elementCountMax && a->elementTable[i].mGeometry;i++)
        {
            if(a->elementTable[i].mGeometry)
                McdGeometryDecrementReferenceCount(a->elementTable[i].mGeometry);
        }

        MeMemoryAPI.destroy(a->elementTable);
        McdGeometryDeinit(g);
    }
}

/****************************************************************************
  This computes the AABB.  If tight=True, use a slower more accurate algorithm
*/
void MEAPI
McdAggregateUpdateAABB(McdGeometryInstanceID ins, MeMatrix4Ptr finalTM, MeBool tight)
{
    McdAggregate* a;
    MeMatrix4 elementTM, elementFinalTM;
    MeMatrix4Ptr elementFinalTMPtr = 0;
    int i;

    MCD_CHECKGEOMETRYINSTANCE(ins, "McdAggregateGetAABB");
    MCD_CHECKTRANSFORM(ins->mTM, "McdAggregateGetAABB");

    a = (McdAggregate*)McdGeometryInstanceGetGeometry(ins);
    MCD_CHECKGEOMETRY(a, "McdAggregateGetAABB");

    MeVector3Set(ins->min,MEINFINITY,MEINFINITY,MEINFINITY);
    MeVector3Set(ins->max,-MEINFINITY,-MEINFINITY,-MEINFINITY);

    McdGeometryInstanceID elementIns = ins->child;

    for(i=0;i<a->elementCountMax;i++)
    {
        if(a->elementTable[i].mGeometry)
        {
            if(!elementIns->mTM)
                elementIns->mTM = elementTM;
            MeMatrix4MultiplyMatrix(elementIns->mTM,
        		a->elementTable[i].mRelTM,ins->mTM);
            if(finalTM)
            {
                elementFinalTMPtr = elementFinalTM;
                MeMatrix4MultiplyMatrix(elementFinalTM,
		            a->elementTable[i].mRelTM, finalTM);
            }
            McdGeometryInstanceUpdateAABB(elementIns,elementFinalTMPtr, tight);
            MeVector3Min(ins->min,ins->min,elementIns->min);
            MeVector3Max(ins->max,ins->max,elementIns->max);

            if(elementIns->mTM == elementTM)
                elementIns->mTM=0;
        }
        elementIns = elementIns->next;
    }
}

// just put a bounding sphere around the bounding box...

void MEAPI
McdAggregateGetBSphere(McdGeometryID g, MeVector3 center, MeReal *radius )
{

    MCD_CHECKGEOMETRYINSTANCE(0, "McdAggregateGetAABB"); // this doesn't work
/*
    MeVector3 tmp;
    MeVector3Add(center,ins->max,ins->min);
    MeVector3Scale(center,(MeReal)0.5);
    MeVector3Subtract(tmp,ins->max,center);
    *radius = MeVector3Magnitude(tmp);
    */
}


void MEAPI
McdAggregateMaximumPoint(McdGeometryInstanceID ins,
                   MeReal * const inDir, MeReal * const outPoint)
{
    MCD_ASSERT(0,"McdAggregateMaximumPoint");
}

static void MEAPI
SkewVector(MeVector3 inVec, MeMatrix3 outMat)
{
    outMat[0][0] = 0;
    outMat[1][1] = 0;
    outMat[2][2] = 0;

    outMat[0][1] = inVec[2];
    outMat[1][0] = -inVec[2];

    outMat[0][2] = -inVec[1];
    outMat[2][0] = inVec[1];

    outMat[1][2] = inVec[0];
    outMat[2][1] = -inVec[0];
}

/* outI = R inI R^T */
static void MEAPI
RotateTensor(MeMatrix3 R, MeMatrix3 inI, MeMatrix3 outI)
{
    MeMatrix3 transR, tmpI;

    MeMatrix3Copy(transR, R);
    MeMatrix3Transpose(transR);
    MeMatrix3MultiplyMatrix(tmpI, inI, transR);
    MeMatrix3MultiplyMatrix(outI, R, tmpI);
}

/* Take two inertia tensors and masses and their transforms, and return a
 combined inertia tensor/mass at a new position but with identity (ie. no)
 rotation part. */
static void MEAPI
CombineInertiaTensors(MeMatrix4 TM1, MeMatrix3 I1, MeReal mass1,
                      MeMatrix4 TM2, MeMatrix3 I2, MeReal mass2,
                      MeMatrix4 outTM, MeMatrix3 outI, MeReal *outMass)
{
    MeMatrix3 skew1, skew2, R1, R2, rotI1, rotI2, massSkew1, massSkew2;
    MeVector3 comRel1, comRel2;

    /* Output has no rotation part */
    MeMatrix4TMMakeIdentity(outTM);

    *outMass = mass1 + mass2;

    /* COM is simple weighted sum! */
    outTM[3][0] = ((TM1[3][0] * mass1) + (TM2[3][0] * mass2))/(*outMass);
    outTM[3][1] = ((TM1[3][1] * mass1) + (TM2[3][1] * mass2))/(*outMass);
    outTM[3][2] = ((TM1[3][2] * mass1) + (TM2[3][2] * mass2))/(*outMass);

    /*  Do crazyness (Ask Will Wray!):
        I = R1T I1 R1 + R2T I2 R2 - m1(p-p1)^(p-p1)^ - m2(p-p2)^(p-p2)^
               a            b              c                 d
        p/p1/p2 = COM position (overall and individually)
        R1/R2 = transforms rotation matrix
        R1T/R2T = transpose of above
        m1/m2 = mass
        ^ = 'Skew' operation (see above)
    */

    /*  We rotate each inertia tensor by the inverse of its transform, to get
        it back into the identity reference frame.
    */

    /* a */
    MeMatrix4TMGetRotation(R1, TM1);
    MeMatrix3Transpose(R1);
    RotateTensor(R1, I1, rotI1);

    /* b */
    MeMatrix4TMGetRotation(R2, TM2);
    MeMatrix3Transpose(R2);
    RotateTensor(R2, I2, rotI2);

    /* c */
    MeVector3Subtract(comRel1, outTM[3], TM1[3]);
    SkewVector(comRel1, skew1);
    MeMatrix3MultiplyMatrix(massSkew1, skew1, skew1);
    MeMatrix3Scale(massSkew1, mass1);

    /* d */
    MeVector3Subtract(comRel2, outTM[3], TM2[3]);
    SkewVector(comRel2, skew2);
    MeMatrix3MultiplyMatrix(massSkew2, skew2, skew2);
    MeMatrix3Scale(massSkew2, mass2);

    /* Total! */
    MeMatrix3Copy(outI, rotI1);
    MeMatrix3Add(outI, outI, rotI2);
    MeMatrix3Subtract(outI, outI, massSkew1);
    MeMatrix3Subtract(outI, outI, massSkew2);
}

/**
 *  Unit mass assumed. Note that this function does not
 *  take into account overlaps among elements of the
 *  aggregate. If you create an aggregate of 50 identical
 *  spheres at the same location, the mass properties will
 *  be those of a sphere of 50 times the density.
 */
MeI16 MEAPI
McdAggregateGetMassProperties(McdGeometry *g,
                        MeMatrix4 relTM,
                        MeMatrix3 m,
                        MeReal *volume)
{
    McdAggregate* a = (McdAggregate*)g;
    int i;
    MeReal density, massSoFar = 0;

    /* Zero inertia tensor initially */
    (*volume) = 0;
    MeMatrix4TMMakeIdentity(relTM);
    MeMatrix3MakeIdentity(m);
    m[0][0] = 0; m[1][1] = 0; m[2][2] = 0;


    /*  Because we need to return an inertia tensor assuming a mass of 1, we
        need the equivalent density.. so we calculate total volume first. */
    for(i=0; i<a->elementCount; i++)
    {
        MeReal subVol;
        MeMatrix3 subI;
        MeMatrix4 subTM;
        McdGeometryID subgeom = a->elementTable[i].mGeometry;

        McdGeometryGetMassProperties(subgeom, subTM, subI, &subVol);
        (*volume) += subVol;
    }

    density = (MeReal)1/(*volume);

    massSoFar = 0;
    for(i=0; i<a->elementCount; i++)
    {
        McdGeometryID subgeom = a->elementTable[i].mGeometry;
        MeMatrix3 subI, newI;
        MeMatrix4 subTM, totalSubTM, newTM;
        MeReal subVol;
        MeMatrix4TMMakeIdentity(subTM);
        McdGeometryGetMassProperties(subgeom, subTM, subI, &subVol);

        /* Compound primitive inertia tensor transform (usually identity),
            and transform from aggregate origin. */
        MeMatrix4MultiplyMatrix(totalSubTM, subTM, a->elementTable[i].mRelTM);

        CombineInertiaTensors(relTM, m, massSoFar,
            totalSubTM, subI, subVol*density,
            newTM, newI, &massSoFar);

        MeMatrix4Copy(relTM, newTM);
        MeMatrix3Copy(m, newI);
    }

#ifdef MCDCHECK
    {
        MeBool nonSymm = 0;

		/*	Little hacky - but doing the old percentage difference way just didn't work.
			Need to discard values < 1e-3 */
        if(MeFabs(m[0][1]-m[1][0]) > (MeReal)0.005)
            nonSymm = 1;
        if(MeFabs(m[0][2]-m[2][0]) > (MeReal)0.005)
            nonSymm = 1;
        if(MeFabs(m[1][2]-m[2][1]) > (MeReal)0.005)
            nonSymm = 1;

        if(nonSymm)
        {
            MCDCHECK_GENERICWARNING(0,
                "Non-symmetric result inertia-tensor result.",
                "McdAggregateGetMassProperties");
        }
    }
#endif

    /* HACK(ish) Force symmetric - we can get small differences because of
        all the floating point work. */
    m[0][1] = m[1][0];
    m[0][2] = m[2][0];
    m[2][1] = m[1][2];

    return 0;
}


int MEAPI McdAggregateGenericIntersect(McdModelPairID p, McdIntersectResult *result)
{
    McdModelPair dummyPair;
    McdModel dummyModel;
    McdIntersectResult dummyResult;
    MeMatrix4 elementTM;
    MeVector3 avgNormal;
    int i,j;
    McdGeometryInstanceID element, instance2;

    McdGeometryInstanceID ins = McdModelGetGeometryInstance(p->model1);
    McdAggregate *g = (McdAggregate *)McdGeometryInstanceGetGeometry(ins);

    MCD_ASSERT(McdGeometryGetType(&g->m_g)==kMcdGeometryTypeAggregate,
	"McdAggregateGenericIntersect");

    McdContact contactBuffer[400];
    int contactCount = 0;

    result->touch = 0;
    result->contactCount = 0;

    dummyModel = *p->model1;
    dummyPair.model1 = &dummyModel;
    dummyPair.model2 = p->model2;
    dummyPair.request = p->request;

    dummyResult.pair = &dummyPair;

    instance2 = McdModelGetGeometryInstance(p->model2);

    MeVector3Set(avgNormal,0,0,0);

    for(i=0, element = ins->child;
        i<g->elementCountMax;
        i++, element = element->next)
    {
        if (g->elementTable[i].mGeometry
            && McdGeometryInstanceOverlap(instance2,element))
        {
            dummyModel.mInstance = *element;
            if(!dummyModel.mInstance.mTM)
            {
                dummyModel.mInstance.mTM = elementTM;
                MeMatrix4MultiplyMatrix(elementTM,
                    g->elementTable[i].mRelTM, ins->mTM);
            }
            dummyResult.touch = 0;
            dummyResult.contacts = contactBuffer + contactCount;
            dummyResult.contactMaxCount = 400 - contactCount;

            McdHello(&dummyPair);
            McdIntersect(&dummyPair,&dummyResult);
            McdGoodbye(&dummyPair);

            if(dummyResult.touch)
            {
                result->touch = 1;
                McdGeometryType gtype = McdModelGetGeometryType(&dummyModel);
                /*
                * should we fillin detail in the user data,
                * or is there already some there?
                */
                MeBool fillin = gtype!=kMcdGeometryTypeTriangleList &&
                                gtype!=kMcdGeometryTypeAggregate;
                
                if(dummyPair.model2==p->model2)
                {
                    if(fillin)
                        for(j=0;j<dummyResult.contactCount;j++)
                            dummyResult.contacts[j].element1.ptr = element;
                }
                else
                {   // models were flipped by Hello
                    MeVector3Scale(dummyResult.normal,-1);
                    for(j=0;j<dummyResult.contactCount;j++)
                    {
                        MeVector3Scale(dummyResult.contacts[j].normal,-1);
                        dummyResult.contacts[j].element2.ptr =
                            dummyResult.contacts[j].element1.ptr;
                        if(fillin)
                            dummyResult.contacts[j].element1.ptr = element;
                    }
                }
                MeVector3MultiplyAdd(avgNormal,
                    (MeReal)dummyResult.contactCount,dummyResult.normal);
                contactCount += dummyResult.contactCount;
            }
        }
    }
    MeVector3Normalize(avgNormal);
    return result->touch;
}


int MEAPI
IxAggregateLineSegment( const McdModelID model,
                 MeReal* const inOrig, MeReal* const inDest,
                 McdLineSegIntersectResult * overlap )
{
    McdGeometryID g = McdModelGetGeometry(model);
    McdAggregate *a = (McdAggregate *)g;
    MeMatrix4 elementTM;
    McdGeometryInstanceID ins = McdModelGetGeometryInstance(model), element;
    int i;

    McdLineSegIntersectResult dummyOverlap;
    McdModel dummyModel;

    MCD_CHECK_ASSERT_(inOrig != NULL && inDest != NULL && a != NULL
	&& overlap != NULL, "IxAggregateLineSegment");
    MCD_CHECK_ASSERT_(McdGeometryGetTypeId(g) == kMcdGeometryTypeAggregate,
	"IxAggregateLineSegment");

    MeMatrix4Ptr tm = McdModelGetTransformPtr(model);

    overlap->distance = MEINFINITY;

    dummyModel = *model;

    for(i=0, element = ins->child;
	i<a->elementCountMax;i++, element = element->next)
    {
        if(a->elementTable[i].mGeometry)
        {
            dummyModel.mInstance = *element;
            if(!dummyModel.mInstance.mTM)
            {
                dummyModel.mInstance.mTM = elementTM;
                MeMatrix4MultiplyMatrix(elementTM,
		    a->elementTable[i].mRelTM, ins->mTM);
            }
            if(McdLineSegIntersect(&dummyModel,inOrig, inDest, &dummyOverlap)
		&& dummyOverlap.distance < overlap->distance)
            {
                    MeVector3Copy(overlap->normal, dummyOverlap.normal);
                    MeVector3Copy(overlap->position, dummyOverlap.position);
                    overlap->model = model;
                    overlap->distance = dummyOverlap.distance;
                }
            }
    }

    return overlap->distance!=MEINFINITY ? 1 : 0;
}

MeBool MEAPI
McdAggregateGenericRegisterInteractions(McdFrameworkID frame) {
    McdInteractions interactions;
    interactions.helloFn = 0;
    interactions.goodbyeFn = 0;
    interactions.intersectFn = McdAggregateGenericIntersect;
    interactions.safetimeFn = 0;
    interactions.cull = 1;

    int i;
    for( i = 0 ; i < frame->geometryRegisteredCountMax ; ++i )
    {
        McdFrameworkSetInteractions(frame, kMcdGeometryTypeAggregate, i, &interactions);
    }

    return 1;
}

/** 
    Draw an aggregate for debugging purposes. Assumes the MeDebugDraw line function has been set up 
*/

void MEAPI McdAggregateDebugDraw(const McdGeometryID geom, const MeMatrix4 tm, const MeReal colour[3])
{
    if(McdGeometryGetTypeId(geom) != kMcdGeometryTypeAggregate)
        return;
    
    McdAggregateID agg = (McdAggregateID) geom;
    int i;

    for(i=0; i<McdAggregateGetElementCount(agg); i++)
    {
        McdGeometryID instgeom = McdAggregateGetElementGeometry(agg, i);
        MeMatrix4Ptr insttm = McdAggregateGetElementTransformPtr(agg, i);
        MeMatrix4 totaltm;

        MeMatrix4MultiplyMatrix(totaltm, insttm, tm);

        McdGeometryDebugDraw(instgeom, totaltm, colour);
    }
}


MCD_IMPLEMENT_LINESEG_REGISTRATION(Aggregate);

/**
    Register the aggregate interactions.
*/
void MEAPI
McdAggregateRegisterInteractions(McdFrameworkID frame)
{
    McdAggregateGenericRegisterInteractions(frame);
    McdAggregateLineSegmentRegisterInteraction(frame);
}

