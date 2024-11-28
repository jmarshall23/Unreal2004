/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:36 $ - Revision: $Revision: 1.7.8.2 $

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

#include <malloc.h>
#include <stdio.h>
#include <time.h>

#include <Mst.h>
#include <McdConvexMesh.h>
#include <MeViewer.h>
#include <MeMath.h>
#include <MeApp.h>

MeReal gStep = (MeReal)0.03;
MeBool gChange = 0;
MeReal height = (MeReal)0;

/*
  Data for the objects being simulated.  The user specifies the sizes of
  the boxes.  Positions are set at initialization time.
*/

typedef struct _sim_body
{
  MeVector3  sizes;   /* sizes of the box */
  MeMatrix4  xform;   /* initial transformation */
  MdtBodyID  body;    /* rigid body */
  McdGeometry *geometry;  /* mcd geometry? */
  McdModelID mcd_model;   /* collision model pointer */
  RGraphic*  gfx;   /* graphics representation */
} sim_body;



/* computes number of elements for static arrays */
#define NELEMS(a)   sizeof(a)/sizeof((a)[0])

#define UNIT_XFORM {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}}



sim_body  bodies[] = {
            {{1, 1, 1}, UNIT_XFORM},
            {{1, 1, 1}, UNIT_XFORM},
            {{1, 1, 1}, UNIT_XFORM},
           };


/* One global still needed! */
MeReal grav_mag = 10.0;

char *help[] = { "Press $ACTION2 to reset",
         "Press 'a' to increase time step",
         "Press 'd' to decrease time step",
         "$MOUSE - mouse force", NULL };


typedef struct _simulation
{
  sim_body            *ground;
  sim_body            *bodies;
  int                  n_bodies;
  MeApp*               meapp;
  MdtBclContactParams *props;
  MstBridgeID          bridge;
  MdtWorldID           world;
  McdSpaceID           space;
  MeReal               step;
  RRender             *rc;
} simulation;


void MEAPI shoot(RRender* rc, void* useData)
{
  return ;
}

void MEAPI Tick(RRender* rc, void* useData)
{
  simulation *sim = (simulation *)useData;
  MeProfileStartSection("Collision", 0);
  McdSpaceUpdateAll(sim->space);
  MstBridgeUpdateContacts(sim->bridge, sim->space, sim->world);
  MeProfileEndSection("Collision");
  MeProfileStartSection("Dynamics", 0);
  MdtWorldStep(sim->world, sim->step);
  MeProfileEndSection("Dynamics");
  return ;
};


void MEAPI incStep(RRender *rc, void *userData)

{
  if(gStep > 0.03)
  {
    printf("Time Step isn't increased %g,\t\n", gStep);
    return;
  }
  gChange ++;
  gStep += (MeReal)0.001;
  printf("Time Step is increased to %g,\t\n", gStep);
}

void MEAPI decStep(RRender *rc, void *userData)

{
  if(gStep < 0.001)
  {
    return;
    printf("Time Step isn't decreased %g,\n", gStep);
  }
  gChange ++;
  gStep -= (MeReal)0.001;
  printf("Time Step is descreased to %g,\n", gStep);
}


void MEAPI reset(RRender *rc, void *userData)
{

  simulation *sim = (simulation *)userData;
  MeReal height = (MeReal)0;
  int i = 0;

  MdtContactParamsSetType(sim->props,MdtContactTypeFriction2D);
  MdtContactParamsSetPrimaryFriction(sim->props, (MeReal)0.50);
  MdtContactParamsSetSecondaryFriction(sim->props, (MeReal)0.50);
  MdtContactParamsSetRestitution(sim->props,(MeReal)0.3);
  MdtContactParamsSetSoftness(sim->props,(MeReal)0.0001);

  if(gChange)
  {
    MdtWorldSetGammaWithRefTimeStep(sim->world, (MeReal)0.2, (MeReal)0.03, gStep);
    sim->step = gStep;
    gChange = 0;
  }

  MdtWorldSetGravity(sim->world, 0, -grav_mag, 0);
  for(i=0;i<sim->n_bodies;++i)
    {
    MeMatrix3 I = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    MeVector3 w;
    int j;
    MdtBodyEnable(sim->bodies[i].body);
      for(j=0;j<3;++j)
    {
      w[j] = MeRealRandomInRange(-1, 1);

    }
    MdtBodySetLinearVelocity(sim->bodies[i].body, 0, 0, 0);
    MdtBodySetAngularVelocity(sim->bodies[i].body, w[0], w[1], w[2]);
    MdtBodySetMass(sim->bodies[i].body, 1);
    MdtBodySetInertiaTensor(sim->bodies[i].body, I);
    MdtBodySetTransform(sim->bodies[i].body, sim->bodies[i].xform);
    }

}

/*
  memory allocation, object construction etc.
  Number of bodies is passed on the command line.  Number of constraints is
  an estimate.  Since we can only have contacts between the bodies, the
  total number of constraints is less than 10*(number of pairs).  10 is a
  high upper bound here since the user should set less than 4 contacts per
  colliding pair.
 */

void MEAPI init_sim(simulation *sim,  sim_body *bodies, int n_bodies,
        MeCommandLineOptions* options)
{

  int total_bodies;
  int n_pairs;

  /* this sets the transformation for the ground */
  static sim_body ground = {{0, 0, 0}, {{1,0,0,0}, {0,0,-1,0}, {0,1,0,0},
                            {0,0,0,1}},  NULL,      NULL,       NULL,     NULL};
  int i;

  height = ground.xform[3][1] - 2; 

  /* set global pointer*/
  sim->step     = gStep;
  sim->n_bodies = n_bodies;
  total_bodies  = 1+n_bodies; /* add 1 for the ground plane */
  n_pairs       = total_bodies*(total_bodies-1)/2;
  sim->bodies   =  bodies;
  sim->ground   = &ground;

  /*
    get a rendering context
   */

  sim->rc = RRenderContextCreate(options, 0, !MEFALSE);

  /*
     Create the dynamics objects.
   */
  /*
    overestimate the max number of constraint as 10 times the number of
    pairs.  This is a bit much but since a good 3-6 contact points can be
    generated, it's a safe protection.
   */
  sim->world = MdtWorldCreate(total_bodies, 10*n_pairs);
  if(!sim->world)
  {
    fprintf(stderr, "Problem with initialyzation of sim->world\n");
    exit(-1);
  }

/* reset Gamma parameters */
  MdtWorldSetGammaWithRefTimeStep(sim->world, (MeReal)0.2, (MeReal)0.03, gStep);

  /*
    init collision detection and get a space object.
   */

  McdInit(McdPrimitivesGetTypeCount(), total_bodies);

  /* only working with cubes and planes. */
  McdPrimitivesRegisterTypes();

  /* register basic primitive interactions */
  McdPrimitivesRegisterInteractions();

  /* materials table is only one entry */
  sim->bridge = MstBridgeCreate(1);

  sim->space = McdSpaceAxisSortCreate(McdAllAxes, total_bodies, n_pairs,1);
  if(!sim->space)
    {
    fprintf(stderr, "sim_init: can't get a valid handle for the collision space\n");
    exit(-1);
    }

  McdSpaceBuild(sim->space);

  /* Material properties */

  sim->props = MstBridgeGetContactParams(sim->bridge, MstBridgeGetDefaultMaterial(),
                            MstBridgeGetDefaultMaterial());

  if(!sim->props)
  {
    fprintf(stderr, "init_sim: couldn't create material properties\n");
    exit(-1);
    }


  /* Create all graphics representations */

  /* Ground plane */

  {
    float color[4] = {1, 1, 1, 1};
    sim->ground->gfx   = RGraphicGroundPlaneCreate(sim->rc, 30.0f,30, color, 0);
    if(!sim->ground->gfx)
    {
      fprintf(stderr, "Can't construct rendering geometry\n");
      exit(-1);
    }
  }
  RGraphicSetTexture(sim->rc, sim->ground->gfx, "checkerboard");

  /* Plane */
  sim->ground->geometry = McdPlaneCreate();
  if(!sim->ground->geometry)
    {
    fprintf(stderr, "Can't create mcd geometry for ground plane \n");
    exit(-1);
    }
  sim->ground->mcd_model = McdModelCreate(sim->ground->geometry);
  McdModelSetTransformPtr(sim->ground->mcd_model, sim->ground->xform);
  McdSpaceInsertModel(sim->space, sim->ground->mcd_model);
  McdSpaceUpdateModel(sim->ground->mcd_model);
  McdSpaceFreezeModel(sim->ground->mcd_model);


  for(i=0;i<sim->n_bodies;++i)
    {
    float color[4];
    int j;

    for(j=0;j<3;++j) sim->bodies[i].sizes[j] += 1;

    height += (MeReal)(sim->bodies[i].sizes[1]);

    for (j=0;j<4;++j) color[j] = MeRealRandomInRange((MeReal)0.0, (MeReal)1.0);
    sim->bodies[i].body = MdtBodyCreate(sim->world);
    sim->bodies[i].xform[3][1] = (MeReal)height;

    if(!sim->bodies[i].body)
    {
      fprintf(stderr, "can't create new body with index %d\n", i);
      exit(-1);
    }
    sim->bodies[i].gfx = RGraphicBoxCreate(sim->rc,
                                 sim->bodies[i].sizes[0],
                                 sim->bodies[i].sizes[1],
                                 sim->bodies[i].sizes[2],
                                 color,
                                               MdtBodyGetTransformPtr(sim->bodies[i].body));
    sim->bodies[i].geometry = McdBoxCreate( sim->bodies[i].sizes[0],
                                                sim->bodies[i].sizes[1],
                                                sim->bodies[i].sizes[2]);
    sim->bodies[i].mcd_model = McdModelCreate(sim->bodies[i].geometry);
    McdSpaceInsertModel(sim->space, sim->bodies[i].mcd_model);
    McdModelSetBody(sim->bodies[i].mcd_model, sim->bodies[i].body);
    }

}


int MEAPI_CDECL main(int argc, const char **argv)
{
    MeCommandLineOptions* options;
    simulation sim;

    options = MeCommandLineOptionsCreate(argc, argv);

    init_sim(&sim, bodies, NELEMS(bodies), options);

    MeCommandLineOptionsDestroy(options);

    reset(sim.rc, (void *)&sim);

    RRenderSetActionNCallBack(sim.rc, 2, reset,   &sim);
    RRenderSetActionNKey(sim.rc,3,'a');
    RRenderSetActionNCallBack(sim.rc, 3, incStep, 0);
    RRenderSetActionNKey(sim.rc,4,'d');
    RRenderSetActionNCallBack(sim.rc, 4, decStep, 0);

    RPerformanceBarCreate(sim.rc);
    RRenderSetWindowTitle(sim.rc,"Pile example");
    RRenderCreateUserHelp(sim.rc, help, 3);
    RRenderToggleUserHelp(sim.rc);

    RRun(sim.rc, Tick, &sim);

    return 0;
}
