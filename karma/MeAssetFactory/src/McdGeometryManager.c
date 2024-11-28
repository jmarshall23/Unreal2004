/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   $Date: 2002/05/24 14:18:24 $ $Revision: 1.22.2.6.4.3 $

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

#include "MeAssetFactory.h"
#include "Mst.h"
#include "McdQHullTypes.h"


/**
 * Create empty Geometry Manager. The geometry manager is responsible
 * for converting MeFGeometrys to McdGeometrys and for tracking the
 * created McdGeometrys. Converting the same MeFGeometry more than once
 * will just return a reference to the original McdGeometry.
 *
 */
McdGeomMan* MEAPI McdGMCreate(McdFrameworkID fwk)
{
    McdGeomMan* gm;
    
    MEASSERT(fwk);

    gm = (McdGeomMan*)MeMemoryAPI.create(sizeof(McdGeomMan));

    gm->fwk = fwk;

    /* Create both hash tables. */
    gm->name2geom = MeHashCreate(97);
    gm->geom2name = MeHashCreate(97);

    MeHashSetKeyCompareFunc(gm->geom2name, MeHashIntCompare);
    MeHashSetHashFunc(gm->geom2name, MeHashInt);
    MeHashSetKeyFreeFunc(gm->name2geom, MeMemoryAPI.destroy);

	gm->nullGeom = McdNullCreate(fwk);
    return gm;
}

/**
 * Destroy Geometry Manager.
 */
void MEAPI McdGMDestroy(McdGeomMan* gm)
{
    MeHashDestroy(gm->name2geom);
    MeHashDestroy(gm->geom2name);

    MeMemoryAPI.destroy(gm);
}

/**
 * Return number of McdGeometrys currently in Geometry Manager.
 */
int MEAPI McdGMGetGeomCount(McdGeomMan* gm)
{
    return MeHashPopulation(gm->geom2name);
}

/**
 * Return McdFrameworkID used by Geometry Manager.
 */
McdFrameworkID MEAPI McdGMGetFramework(McdGeomMan* gm)
{
    MEASSERT(gm);
    return gm->fwk;
}

/**
 * Return the geometry specified by the supplied MeFGeometry. If this geometry
 * has already been created and is part of the Geometry Manager, it will
 * return the existing McdGeometry.
 */
McdGeometryID MEAPI McdGMCreateGeometry(McdGeomMan* gm, const MeFGeometry* fg, const char *assetName)
{
    McdGeometryID agGeom;
    MeFPrimitiveIt it;
    int nPrimitives = MeFGeometryGetPrimitiveCount(fg);
    char* agHashName;
    char* agName = MeFGeometryGetName(fg);
    /* agHashName is assetName::agName */
    agHashName = (char*)MeMemoryAPI.create(strlen(assetName) + strlen(agName) + 3);
    agHashName[0]=0;
    strcat(agHashName, assetName);
    strcat(agHashName, "::");
    strcat(agHashName, agName);
    agGeom = (McdGeometryID)MeHashLookup(agHashName, gm->name2geom);
    
    /* If we didn't find the geometry - we need to create it. */
    if(!agGeom)
    {
        MeFPrimitive *prim;

        agGeom = McdAggregateCreate(gm->fwk, nPrimitives);
        MeFGeometryInitPrimitiveIterator(fg, &it);
        
        while (prim = MeFGeometryGetPrimitive(&it))
        {
            McdGeometryID primGeom;
            char* hashName;
            char* primName = MeFPrimitiveGetName(prim);

            /* hashName is assetName::agName::primName */
            hashName = (char*)MeMemoryAPI.create(strlen(assetName) + strlen(agName) + strlen(primName) + 5);
            hashName[0]=0;
            strcat(hashName, assetName);
            strcat(hashName, "::");
            strcat(hashName, agName);
            strcat(hashName, "::");
            strcat(hashName, primName);            
            
            primGeom = (McdGeometryID)MeHashLookup(hashName, gm->name2geom);
            
            if(!primGeom)
            {
                MeFPrimitiveType type = MeFPrimitiveGetType(prim);
                if (type == kMeFPrimitiveTypeBox)
                {
                    MeVector3 dims;
                    MeFPrimitiveGetDimensions(prim, dims);
                    primGeom = McdBoxCreate(gm->fwk, dims[0], dims[1], dims[2]);
                } 
                else if (type == kMeFPrimitiveTypeSphere)
                {
                    primGeom = McdSphereCreate(gm->fwk, MeFPrimitiveGetRadius(prim));
                }
                else if (type == kMeFPrimitiveTypeCylinder)
                {
                    primGeom = McdCylinderCreate(gm->fwk, 
                        MeFPrimitiveGetRadius(prim), 
                        MeFPrimitiveGetHeight(prim));
                }
                else if (type == kMeFPrimitiveTypeSphyl)
                {
                    primGeom = McdSphylCreate(gm->fwk, 
                        MeFPrimitiveGetRadius(prim), 
                        MeFPrimitiveGetHeight(prim));
                }
                else if (type == kMeFPrimitiveTypePlane)
                {
                    primGeom = McdPlaneCreate(gm->fwk);
                }
                else if (type == kMeFPrimitiveTypeConvex)
                {
                    if(MeFPrimitiveGetVertexCount(prim) >  0)
                        primGeom = McdConvexMeshCreateHull(gm->fwk, MeFPrimitiveGetVertexArray(prim), MeFPrimitiveGetVertexCount(prim), 0);
                    else
                        primGeom = 0;
                }
                else
                {
                    MeWarning(0, "McdGMCreateGeometry: Unknown Geometry type.");
                    return 0;
                }
                
                /* Add this new geometry to the geometry manager. */
                if(primGeom)
                {
                    /* Add this geometry to the hashes for future use. */
                    
                    MeHashInsert(hashName, primGeom, gm->name2geom);
                    MeHashInsert(primGeom, hashName, gm->geom2name);
                }
                else
                {
                    MeWarning(0, "McdGMCreateGeometry: Failed to create primitive.");
                    return 0;
                }

            }
            else
                MeMemoryAPI.destroy(hashName);
            
            McdAggregateAddElement(agGeom, primGeom, MeFPrimitiveGetTransformPtr(prim));
        }

        /* Add the new aggregate geometry to the geometry manager. */
        if(agGeom)
        {
            /* Add this geometry to the hashes for future use. */
            MeHashInsert(agHashName, agGeom, gm->name2geom);
            MeHashInsert(agGeom, agHashName, gm->geom2name);
        }
        else
        {
            MeWarning(0, "McdGMCreateGeometry: Failed to create aggregate.");
            return 0;
        }
    }
    else
    {
	    MeMemoryAPI.destroy(agHashName);
    }
    return agGeom;
}

/**
 * Destroy the geometry if it is not referenced.
 */
void MEAPI McdGMDestroyGeometry(McdGeomMan* gm, McdGeometryID geom)
{
    if(McdGeometryGetReferenceCount(geom) > 0)
        return;

	if(McdGeometryGetTypeId(geom) == kMcdGeometryTypeAggregate)   
	{   
		int g, gCount = McdAggregateGetElementCount(geom);   
		
		for(g=0; g<gCount; g++)   
		{   
			McdGeometryID partGeom = McdAggregateGetElementGeometry(geom, g);   
			
			if(partGeom)
			{   
                /*  We need to remove this element from the aggregate to 
                    decrement its ref count. */
                McdAggregateRemoveElement(geom, g);
                McdGMDestroyGeometry(gm, partGeom);   
			}   
		}    
	} 

	if(McdGeometryGetTypeId(geom) != kMcdGeometryTypeNull)
   	{
		 /*  Use geom2name has table to find name, and remove from both hash
			tables before destroying. */
		char *name = (char*)MeHashLookup(geom, gm->geom2name);
    
		if(name)
		{
			/* Remove from both hash tables. */
			MeHashDelete(name, gm->name2geom); 
			MeHashDelete(geom, gm->geom2name);
			McdGeometryDestroy(geom);
		}
	}
}


/**
 * Returns a null geometry
 */
McdNullID MEAPI McdGMGetNullGeometry(McdGeomMan* gm)
{
	return gm->nullGeom;
}
