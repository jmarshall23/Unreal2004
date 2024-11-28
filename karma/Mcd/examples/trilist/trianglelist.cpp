// \\Lake\E\Projects\Tests\TriangleList\TriangleList.cpp

#include <MeApp.h>
#include <stdio.h>

#include <McdTriangleList.h>
#define TRI_LIST_SIZE 4
McdUserTriangle tri_list_memory[TRI_LIST_SIZE];

typedef struct tp_s     {
	MeVector3 vertex[3];
    MeVector3 normal;
} terrain_poly;

typedef struct terrain_s {
    terrain_poly poly[4];
} terrain_spec;


MdtWorldID env;
MdtBodyID ball_dm;

RRender *rc;
RGraphic *ball_G, *terrain_G;

McdSpaceID c_space;
McdGeometryID ball_prim, terrain_prim;
McdModelID ball_cm, terrain_cm;

MeCommandLineOptions *options=0;

MstBridgeID cd_bridge;

MeVector3 gravity = {0,(MeReal)-9.81,0};

int mat_terrain, mat_ball;
MdtBclContactParams *params;

terrain_spec terrain = 
{
        { 
                {   {{-10, 0, 10},	{10, 0, 10},	{0, -10, 0}},    {0,0.707f,-0.707f} },
                {   {{10, 0, 10},   {10, 0, -10},	{0, -10, 0}},    {-0.707f,0.707f,0} },
                {   {{10, 0, -10},  {-10, 0, -10},	{0, -10, 0}},    {0,0.707f,0.707f} },
                {   {{-10, 0, -10}, {-10, 0, 10},	{0, -10, 0}},    {0.707f,0.707f,0} }
        }
};

void cleanup(void)
{
        MdtWorldDestroy(env);

        RRenderContextDestroy(rc);

        MeCommandLineOptionsDestroy(options);
        
        MstBridgeDestroy(cd_bridge);

        McdGeometryDestroy(ball_prim);
        McdModelDestroy(ball_cm);
        McdSpaceDestroy(c_space);
        McdTerm();
}


void MEAPI tick(RRender *rc, void *userdata)
{
    McdSpaceUpdateAll(c_space);
    MstBridgeUpdateContacts(cd_bridge, c_space, env);
    MdtWorldStep(env, 0.05f);
}

void MakeRObjectVertex(RObjectVertex* vertex, MeVector3 vert, MeVector3 norm, MeReal u, MeReal v)
{
    vertex->m_X = vert[0];
    vertex->m_Y = vert[1];
    vertex->m_Z = vert[2];
    vertex->m_NX = norm[0];
    vertex->m_NY = norm[1];
    vertex->m_NZ = norm[2];
    vertex->m_U = u;
    vertex->m_V = v;
}

static int MEAPI TriListGeneratorCB(McdModelPair *pair, MeVector3 pos, MeReal rad)
{
	McdTriangleList* tl = (McdTriangleList*)McdModelGetGeometry(pair->model2);
	McdUserTriangle *tri;
	MeVector3 edge1, edge2;

	for(int i=0; i<4; i++)
	{
		tri = &tl->triangleList[i];

		/* Vertices	*/
		tri->vertices[0] = &terrain.poly[i].vertex[0];
		tri->vertices[1] = &terrain.poly[i].vertex[1];
		tri->vertices[2] = &terrain.poly[i].vertex[2];

		/* Edges - Not needed if you are storing the poly Normals	*/
		MeVector3Subtract(edge1, *tri->vertices[1], *tri->vertices[0]);
		MeVector3Subtract(edge2, *tri->vertices[2], *tri->vertices[1]);

		/* Normal -- Must be related to vertices / edges using RH rule	*/
		tri->normal = &terrain.poly[i].normal;
		MeVector3Cross(*tri->normal, edge1, edge2);
		MeVector3Normalize(*tri->normal);

		/* Store Userdata relating to info about RW polygon	*/
		tri->element1.ptr = NULL;
        tri->element2.tag = NULL;

	}

	tl->triangleCount = 4;

	return tl->triangleCount;
}
void MEAPI_CDECL main(int argc, const char *argv[])
{

		argv[argc-1]="-gl";
        options = MeCommandLineOptionsCreate(argc, argv);
        rc = RRenderContextCreate(options, 0, !MEFALSE);

        //world
        env = MdtWorldCreate(10, 20);
        MdtWorldSetGravity(env,gravity[0],gravity[1],gravity[2]);

        //objects
        ball_dm = MdtBodyCreate(env);
        MdtBodyEnable(ball_dm);
		MdtBodySetPosition(ball_dm, 7, 0, 0);
		MdtBodySetLinearVelocity(ball_dm, 0, 0, 5);

        //collision initialisation
        McdInit(McdPrimitivesGetTypeCount(), 2, 1);

        McdPrimitivesRegisterTypes();
        McdPrimitivesRegisterInteractions();

        c_space = McdSpaceAxisSortCreate(McdAllAxes, 10, 20,1);
        //collision sphere
        MeReal sphere_radius = 2.0f;
        ball_prim = McdSphereCreate(sphere_radius);
        ball_cm = McdModelCreate(ball_prim);
        McdSpaceInsertModel(c_space, ball_cm);
        McdModelSetBody(ball_cm, ball_dm);

		cd_bridge = MstBridgeCreate( 4); 


		terrain_prim = McdTriangleListCreate(20,20,20, TriListGeneratorCB);
		terrain_cm = McdModelCreate(terrain_prim);
		((McdTriangleList*)McdModelGetGeometry(terrain_cm))->triangleList = tri_list_memory;
		((McdTriangleList*)McdModelGetGeometry(terrain_cm))->triangleMaxCount = TRI_LIST_SIZE;
		McdSpaceInsertModel(c_space, terrain_cm);
        McdModelSetBody(terrain_cm, NULL);

		static MeMatrix4 m = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
		McdModelSetTransformPtr(terrain_cm, m);

	    McdSpaceUpdateAll(c_space); // need to call before freeze
		McdSpaceFreezeModel(terrain_cm); // world doesn't move


        McdSpaceBuild(c_space);
        
	    mat_ball	= MstBridgeGetDefaultMaterial();
		mat_terrain = MstBridgeGetNewMaterial(cd_bridge);

	    params = MstBridgeGetContactParams(cd_bridge,mat_ball,mat_terrain);
		MdtContactParamsSetType(params,MdtContactTypeFrictionZero); 
		MdtContactParamsSetRestitution(params, 0.5f);
		
		McdModelSetMaterial(terrain_cm, mat_terrain);
		McdModelSetMaterial(ball_cm, mat_ball);

        //render environment
        const float red[] = {1.f, 0.f, 0}, yellow[] = {1.f, 1.f, 0}, green[] = {0,1, 0};
        ball_G = RGraphicSphereCreate(rc, sphere_radius, red, MdtBodyGetTransformPtr(ball_dm));

		terrain_G = RGraphicCreateEmpty(4 * 3);

		for(int i=0; i<4; i++)
		{
			for(int j=0; j<3; j++)
			{
				MakeRObjectVertex(&terrain_G->m_pVertices[(i*3)+j], terrain.poly[i].vertex[j], terrain.poly[i].normal, 0, 0);
			}
		}
		terrain_G->m_pObject->m_nTextureID = -1; /* no texture */
		RGraphicSetColor(terrain_G,yellow);
		terrain_G->m_pObject->m_bIsWireFrame = 0;
		terrain_G->m_pLWMatrix = NULL;
		MeMatrix4TMMakeIdentity((MeMatrix4Ptr)terrain_G->m_pObject->m_Matrix);
		RGraphicAddToList(rc, terrain_G, 0);

		RCameraSetView(rc,(float)25,(float)0.5,(float)0.6);

		rc->m_bUseAmbientLight = 0;
		rc->m_DirectLight1.m_Direction[0] = 0.5f;
		rc->m_DirectLight1.m_Direction[1] = -1;
		rc->m_DirectLight1.m_Direction[2] = 0.866f;
		rc->m_DirectLight1.m_rgbAmbient[0] = 0.5f;
		rc->m_DirectLight1.m_rgbAmbient[1] = 0.5f;
		rc->m_DirectLight1.m_rgbAmbient[2] = 0.5f;
//		rc->m_DirectLight1.m_rgbDiffuse[0] = 1;
//		rc->m_DirectLight1.m_rgbDiffuse[1] = 1;
//		rc->m_DirectLight1.m_rgbDiffuse[2] = 1;

        RRun(rc, tick, 0);

        //
        cleanup();

}
