#include <crtdbg.h>
#include <Mst.h>
#include <MeAssetDB.h>
#include <MeAssetDBXMLIO.h>
#include <MeAssetFactory.h>

void MEAPI MyPostCreateMcdGeometryCB(McdGeometryID geometry, void *userdata)
{
}

McdModelID MEAPI MyCreateMcdModel(MeFAssetPart *part, McdGeometryID g, MdtWorldID world, MeMatrix4Ptr assetTM)
{
	return 0;
}

void MEAPI MyPostCreateMcdModelCB(McdModelID model, void *userdata)
{
}

MdtConstraintID MEAPI MyCreateMdtConstraint(MeFJoint *joint, MdtWorldID world, McdModelID m1, McdModelID m2)
{
	return 0;
}

void MEAPI MyPostCreateMdtConstraintCB(MdtConstraintID joint, void *userdata)
{
}

// Example of AssetFactory usage
int main(int argc, char **argv)
{

#if defined WIN32 && defined _DEBUG && 1
    {
        int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

        debugFlag |= _CRTDBG_ALLOC_MEM_DF;
        debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
        //debugFlag |= _CRTDBG_CHECK_CRT_DF;
        debugFlag |= _CRTDBG_LEAK_CHECK_DF;
        _CrtSetDbgFlag(debugFlag);
    }
#endif    

    // First, load an asset database with data

    MeAssetDB *db = MeAssetDBCreate();
    MeAssetDBXMLInput *input = MeAssetDBXMLInputCreate(db, 0);

    MeStream stream = MeStreamOpen("testdata.ka", kMeOpenModeRDONLY);

    MeAssetDBXMLInputRead(input, stream);

    MeStreamClose(stream);
    
    MeAssetDBXMLInputDestroy(input);
    
    // db is now populated
    
    MeFAsset *crane = MeAssetDBLookupAssetByName(db, "crane");
    
    MstUniverseID universe = MstUniverseCreate(&MstUniverseDefaultSizes);
    MdtWorldID world = MstUniverseGetWorld(universe);
    McdSpaceID space = MstUniverseGetSpace(universe);
    McdFrameworkID fwk = MstUniverseGetFramework(universe);
    McdConvexMeshRegisterType(fwk);

    MeAssetFactory *factory = MeAssetFactoryCreate(world, space, fwk);

#if 0
    MeAssetFactorySetGeometryPostCreateCB(factory, MyPostCreateMcdGeometryCB);
    
	// either
	MeAssetFactorySetModelCreateFunction(factory, MyCreateMcdModel);
	// or
    MeAssetFactorySetModelPostCreateCB(factory, MyPostCreateMcdModelCB, 0);
	// or accept default

	// either
	MeAssetFactorySetJointCreateFunction(factory, MyCreateMdtConstraint);
	// or
    MeAssetFactorySetJointPostCreateCB(factory, MyPostCreateMdtConstraintCB, 0);
	// or accept default
#endif

    MeAssetInstance *ins = MeAssetInstanceCreate(factory, crane, 0, 1);

    McdGeometryID baseGeom = MeAssetInstanceGetGeometry(ins, "base");    
    McdModelID craneBase = MeAssetInstanceGetModel(ins, "base");
    MdtHingeID baseHinge = (MdtHingeID)MeAssetInstanceGetJoint(ins, "base");

	{
		MeAIGeomIt it;
		McdGeometryID geom;

		MeAssetInstanceInitGeometryIterator(ins, &it);

		while (geom = MeAssetInstanceGetNextGeometry(&it))
		{
			printf("0x%x\n", geom);
		}
	}
	{
		MeAIModelIt it;
		McdModelID model;

		MeAssetInstanceInitModelIterator(ins, &it);

		while (model = MeAssetInstanceGetNextModel(&it))
		{
			printf("0x%x\n", model);
		}
	}
	{
		MeAIJointIt it;
		MdtConstraintID joint;

		MeAssetInstanceInitJointIterator(ins, &it);

		while (joint = MeAssetInstanceGetNextJoint(&it))
		{
			printf("0x%x\n", joint);
		}
	}

    MeAssetInstanceDestroy(ins);
    MeAssetFactoryDestroy(factory);
    MeAssetDBDestroy(db);
    MstUniverseDestroy(universe);
    
    return 0;
}

