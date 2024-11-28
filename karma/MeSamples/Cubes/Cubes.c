/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name$

   Date: $Date$ - Revision: $Revision$

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

#ifdef _MSC_VER
#pragma warning( disable : 4305 4244 )
#endif


#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <Mdt.h>
#include <MeMath.h>
#include <MeViewer.h>

/* Global declarations */
#if PS2
    /* The ps2gl renderer cannot cope with GEN1BOXES >= 8 */
#   define GEN1BOXES 4
#else
#   define GEN1BOXES 20
#endif
#define GEN2BOXES (8*GEN1BOXES)
#define GEN3BOXES (8*GEN2BOXES)

#define MAXBOXES  GEN3BOXES

MeReal boxDensity = (MeReal)(0.7);

/* Initial radius of each box. */
MeReal boxRadius = 0.5;

MeReal restitution = 0.8;

MeReal explodeSpeed = 3.5;
MeReal centerOffset = 12;
MeReal centerAngle = 0;
MeReal centerElevation = 0.65;

float boxColor[4] = {0.0 , 0.73, 0.73, 1};

int boxCount = 0;

/* World for the Dynamics Toolkit simulation */
MdtWorldID world;

MdtKeaParameters params;

/* Physics representations */
MdtBodyID gen1box[GEN1BOXES];
MdtBodyID gen2box[GEN2BOXES];
MdtBodyID gen3box[GEN3BOXES];

/* Graphical representations */
RGraphic *groundG;

RGraphic *gen1boxG[GEN1BOXES];
RGraphic *gen2boxG[GEN2BOXES];
RGraphic *gen3boxG[GEN3BOXES];

int       gen3bounceCount[GEN3BOXES];

int       maxBounceCount = 2;

int       frame = 0;

MeReal gravity[3] = { 0, -5, 0 };

/* Render context */
RRender *rc;

MeReal step = (MeReal)(0.03);
MeBool evolve = 1;

MeReal groundRenderTransform[16] =
{
    1, 0, 0, 0,
    0, 0, -1, 0,
    0, 1, 0, 0,
    0, -0.05, 0, 1
};

MeReal boxCorner[8][3] =
{
    {1, 1, 1},
    {1, 1, -1},
    {1, -1, 1},
    {1, -1, -1},
    {-1, 1, 1},
    {-1, 1, -1},
    {-1, -1, 1},
    {-1, -1, -1}
};

void makeInertiaTensor(MeReal moi, MeMatrix3 I)
{
    I[0][0] = moi;
    I[0][1] = 0;
    I[0][2] = 0;

    I[1][0] = 0;
    I[1][1] = moi;
    I[1][2] = 0;

    I[2][0] = 0;
    I[2][1] = 0;
    I[2][2] = moi;
}

void MEAPI killAll(RRender *rc, void *user_data)
{
    evolve = 0;
}

void MEAPI wakeAll(RRender *rc, void *user_data)
{
    evolve = 1;
}

/* Fast box-plane collision test - only generates 1 contact though! */
int doCollision(MdtBodyID b, MeReal radius)
{
    MeVector3 pos;
    MeReal boundingRadiusSquared = 3 * radius * radius;

    MdtBodyGetPosition(b, pos);

    /* Farfield! */
    if((pos[1]*pos[1]) <= boundingRadiusSquared)
    {
        MeReal penetration;
        MeMatrix4Ptr tm;

        /* vectors from center of box to edge */
        MeVector3 boxX, boxY, boxZ, lowestCorner;
        MeReal corner[3];

        tm = MdtBodyGetTransformPtr(b);

        boxX[0] = tm[0][0]; boxX[1] = tm[0][1]; boxX[2] = tm[0][2];
        MeVector3Scale(boxX, radius);

        boxY[0] = tm[1][0]; boxY[1] = tm[1][1]; boxY[2] = tm[1][2];
        MeVector3Scale(boxY, radius);

        boxZ[0] = tm[2][0]; boxZ[1] = tm[2][1]; boxZ[2] = tm[2][2];
        MeVector3Scale(boxZ, radius);

        if(boxX[1] > 0) corner[0] = -1;
        else corner[0] = 1;

        if(boxY[1] > 0) corner[1] = -1;
        else corner[1] = 1;

        if(boxZ[1] > 0) corner[2] = -1;
        else corner[2] = 1;

        lowestCorner[0] = pos[0] + (boxX[0] * corner[0]) +
            (boxY[0] * corner[1]) + (boxZ[0] * corner[2]);
        lowestCorner[1] = pos[1] + (boxX[1] * corner[0]) +
            (boxY[1] * corner[1]) + (boxZ[1] * corner[2]);
        lowestCorner[2] = pos[2] + (boxX[2] * corner[0]) +
            (boxY[2] * corner[1]) + (boxZ[2] * corner[2]);

        penetration = -lowestCorner[1];

        /* If box has hit ground */
        if(penetration > 0)
        {
            MeVector3 vel;

            pos[1] += penetration;

            MdtBodySetPosition(b, pos[0], pos[1], pos[2]);

            MdtBodyGetLinearVelocity(b, vel);
            if(vel[1] < 0)
            {
                MeVector3 angvel, hitOffset;

                vel[1] = -restitution * vel[1];
                MdtBodySetLinearVelocity(b, vel[0], vel[1], vel[2]);
                MeVector3Subtract(hitOffset, lowestCorner, pos);

                angvel[0] = -hitOffset[2] * vel[1];
                angvel[1] = 0;
                angvel[2] = hitOffset[0] * vel[1];

                MdtBodySetAngularVelocity(b, angvel[0], angvel[1], angvel[2]);
            }
            return 1;
        }
    }
    return 0;
}

void splitBox(MdtBodyID* blist, RGraphic** glist, int i, MeReal currentRadius,
              MdtBodyID* nblist, RGraphic** nglist)
{
    MeVector3 vel, velrot;
    MeMatrix4 tm;
    /*MeReal mass;*/
    /*MeMatrix3 I;*/
    float color[4];
    MeVector3 boxX, boxY, boxZ;
    int j;

    MdtBodyGetLinearVelocity(blist[i], vel);
    MdtBodyGetAngularVelocity(blist[i], velrot);

    /*mass = 0.125 * MdtBodyGetMass(blist[i]);*/

    /*MdtBodyGetInertiaTensor(blist[i], I);*/
    /*makeInertiaTensor(0.25 * I[0][0], I);*/

    MdtBodyGetTransform(blist[i], tm);

    boxX[0] = tm[0][0]; boxX[1] = tm[0][1]; boxX[2] = tm[0][2];
    MeVector3Scale(boxX, 0.5 * currentRadius);

    boxY[0] = tm[1][0]; boxY[1] = tm[1][1]; boxY[2] = tm[1][2];
    MeVector3Scale(boxY, 0.5 * currentRadius);

    boxZ[0] = tm[2][0]; boxZ[1] = tm[2][1]; boxZ[2] = tm[2][2];
    MeVector3Scale(boxZ, 0.5 * currentRadius);

    RGraphicGetColor(glist[i],color);

    RGraphicDelete(rc,glist[i],0);
    MdtBodyDisable(blist[i]);
    MdtBodyDestroy(blist[i]);
    blist[i] = 0;
    boxCount--;

    for (j=0; j<8; j++)
    {
        MeVector3 offset;
        int newIndex = (8*i)+j;

        nblist[newIndex] = MdtBodyCreate(world);
        boxCount++;

        /*MdtBodySetMass(nblist[newIndex], mass);*/
        /*MdtBodySetInertiaTensor(nblist[newIndex], I);*/

        MdtBodySetTransform(nblist[newIndex], (void *)tm);

        MdtBodySetAngularVelocity(nblist[newIndex],
            velrot[0], velrot[1], velrot[2]);

        MdtBodyEnable(nblist[newIndex]);

        /* Work out offset for each little box from big box origin */
        offset[0] = (boxX[0] * boxCorner[j][0]) +
            (boxY[0] * boxCorner[j][1]) +
            (boxZ[0] * boxCorner[j][2]);
        offset[1] = (boxX[1] * boxCorner[j][0]) +
            (boxY[1] * boxCorner[j][1]) +
            (boxZ[1] * boxCorner[j][2]);
        offset[2] = (boxX[2] * boxCorner[j][0]) +
            (boxY[2] * boxCorner[j][1]) +
            (boxZ[2] * boxCorner[j][2]);

        MdtBodySetLinearVelocity(nblist[newIndex],
            vel[0] + explodeSpeed * offset[0],
            vel[1] + explodeSpeed * offset[1],
            vel[2] + explodeSpeed * offset[2]);

        MdtBodySetPosition(nblist[newIndex],
            tm[3][0] + offset[0],
            tm[3][1] + offset[1],
            tm[3][2] + offset[2]);

        /* Size given in edge lengths */
        nglist[newIndex] =
            RGraphicBoxCreate(rc, currentRadius, currentRadius,
                currentRadius, color,
                MdtBodyGetTransformPtr(nblist[newIndex]));

    }
}

/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI tick(RRender *rc, void *user_data)
{
    int i;
    MdtBodyID body;
    MeDictNode *node;
    MeDict *dict;

    /* Swoop around */
    RCameraSetView(rc,
        centerOffset + 4*MeCos((MeReal)frame/150),
        centerAngle += 0.006,
        centerElevation + MeSin((MeReal)frame/120)*0.25
    );
    RCameraUpdate(rc);

    if (!evolve)
        return;

    /* Do collision */

    /* These timer calls are for the OpenGL performance bar. */
    MeProfileStartSection("Collision",0);
    for (i = 0; i < GEN3BOXES; i++)
    {
        if(gen3box[i])
        {
            if(doCollision(gen3box[i], boxRadius * 0.25))
            {
                if(gen3bounceCount[i] != maxBounceCount)
                    gen3bounceCount[i]++;
                else
                {
                    RGraphicDelete(rc,gen3boxG[i],0);
                    MdtBodyDisable(gen3box[i]);
                    MdtBodyDestroy(gen3box[i]);
                    gen3box[i] = 0;
                    boxCount--;
                }
            }
        }
    }

    for (i = 0; i < GEN2BOXES; i++)
    {
        if(gen2box[i])
        {
            if(doCollision(gen2box[i], boxRadius * 0.5))
            {
                splitBox(gen2box, gen2boxG, i,
                    boxRadius * 0.5, gen3box, gen3boxG);
            }
        }
    }

    for (i = 0; i < GEN1BOXES; i++)
    {
        if(gen1box[i])
        {
            if(doCollision(gen1box[i], boxRadius))
            {
                splitBox(gen1box, gen1boxG, i,
        		    boxRadius, gen2box, gen2boxG);
            }
        }
    }
    MeProfileEndSection("Collision");

    /* Evolve the world. */
    MeProfileStartSection("Dynamics",0);
    dict = &world->enabledBodyDict;
    for (node = MeDictFirst(dict); node != 0; node = MeDictNext(dict, node))
    {
        MdtKeaBody* bodyArray;

        body = (MdtBodyID)MeDictNodeGet(node);

        body->keaBody.force[1] = body->mass * gravity[1];
        body->keaBody.vel[1] +=
            body->keaBody.invmass * step * body->keaBody.force[1];

        bodyArray = &body->keaBody;

        MdtKeaIntegrateSystem(&bodyArray,
            (MdtKeaTransformation*)body->comTM,  1, params);

        memcpy(&body->bodyTM, &body->comTM, sizeof(MeMatrix4));
    }
    MeProfileEndSection("Dynamics");

    frame++;
}


void MEAPI_CDECL cleanup(void)
{
    MdtWorldDestroy(world);

    RRenderContextDestroy(rc);
}


void MEAPI reset(RRender *rc, void *user_data)
{
    MeReal mass, moi, r;
    MeMatrix3 I;
    int i;
    float color[4];
    MeVector3 pos;
    MeReal theta = 0;
    MeVector4 q;

    mass = boxDensity * 2.0 * boxRadius * 2.0 * boxRadius * 2.0 * boxRadius;
    moi = 0.4 * mass * boxRadius * boxRadius;

    /* Destroy any existing boxes first */
    for (i = 0; i < GEN3BOXES; i++)
    {
        if(gen3box[i])
        {
            RGraphicDelete(rc,gen3boxG[i],0);
            MdtBodyDisable(gen3box[i]);
            MdtBodyDestroy(gen3box[i]);
	    gen3box[i] = 0;
            boxCount--;
        }
        gen3bounceCount[i] = 0;
    }

    for (i = 0; i < GEN2BOXES; i++)
    {
        if(gen2box[i])
        {
            RGraphicDelete(rc,gen2boxG[i],0);
            MdtBodyDisable(gen2box[i]);
            MdtBodyDestroy(gen2box[i]);
	    gen2box[i] = 0;
            boxCount--;
        }
    }

    for (i = 0; i < GEN1BOXES; i++)
    {
        if(gen1box[i])
        {
            RGraphicDelete(rc,gen1boxG[i],0);
            MdtBodyDisable(gen1box[i]);
            MdtBodyDestroy(gen1box[i]);
            gen1box[i] = 0;
            boxCount--;
        }
    }

    /* Then create a new 1st generation */

    for (i = 0; i < GEN1BOXES; i++)
    {
        gen1box[i] = MdtBodyCreate(world);
        boxCount++;

        MdtBodySetMass(gen1box[i], mass);

        makeInertiaTensor(moi, I);
        MdtBodySetInertiaTensor(gen1box[i], (void *)I);

        r = 2 + (MeReal)i * 0.2;
        theta += 2/r;

        pos[0] = r * MeSin(theta);
        pos[1] = 4 + ((MeReal)i * 0.5);
        pos[2] = r * MeCos(theta);

        MdtBodySetPosition(gen1box[i], pos[0], pos[1], pos[2]);

        pos[1] = 0;
        MeVector3Normalize(pos);
        MeQuaternionMake( q, pos,  0.1);

        MdtBodySetQuaternion(gen1box[i], q[0], q[1], q[2], q[3]);
        MdtBodySetLinearVelocity(gen1box[i], 0, 0, 0);
        MdtBodySetAngularVelocity(gen1box[i], 2 * pos[0], 0, 2 * pos[2]);
        MdtBodyEnable(gen1box[i]);

        color[0] = (MeReal)rand()/RAND_MAX;
        color[1] = (MeReal)rand()/RAND_MAX;
        color[2] = (MeReal)rand()/RAND_MAX;
        color[3] = 1.0;

        gen1boxG[i] =
            RGraphicBoxCreate(rc, 2 * boxRadius, 2 * boxRadius,
                2 * boxRadius, color, MdtBodyGetTransformPtr(gen1box[i]));
    }
}

/* Main Routine */
int MEAPI_CDECL main(int argc, const char **argv)
{
    static char *help[3] =
    {
        "$RIGHT - reset",
        "$UP - stop all objects",
        "$DOWN - wake all objects",
    };


    const int helpNum = sizeof (help) / sizeof (help[0]);
    float color[4];
    MeCommandLineOptions *options;

    /* Initialise rendering attributes */

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);

    RCameraSetView(rc,centerOffset,0,centerElevation);
    RCameraUpdate(rc);
    RPerformanceBarCreate(rc);

    /* Initialise dynamics */
    world = MdtWorldCreate(MAXBOXES, 0, 1, 1);

    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    params.debug.readKeaInputData   = MEFALSE;
    params.debug.writeKeaInputData  = MEFALSE;
    params.debug.writeKeaInterData  = MEFALSE;
    params.debug.writeKeaOutputData = MEFALSE;

    params.cpu_resources = MdtKeaQueryCPUResources();
    params.epsilon = MDTWORLD_DEFAULT_EPSILON;
    params.gamma = MDTWORLD_DEFAULT_GAMMA;
    params.velocityZeroTol = MDTWORLD_DEFAULT_VELOCITY_ZERO_TOL;
    params.max_iterations = 0;
    params.stepsize = step;


    reset(rc,0);


    /* GROUND: */

    color[0] = 0.0f; color[1] = 0.75f; color[2] = 1.0f; color[3] = 1.0f;
    groundG = RGraphicGroundPlaneCreate(rc, 30.0f,30, color, -0.05);
    RGraphicSetTexture(rc, groundG, "checkerboard");

    /* KEYS: */

    RRenderSetUpCallBack(rc,killAll,0);
    RRenderSetDownCallBack(rc,wakeAll,0);
    RRenderSetRightCallBack(rc,reset,0);

    RRenderSetWindowTitle(rc, "Cubes example");
    RRenderCreateUserHelp(rc, help, helpNum);
    RRenderToggleUserHelp(rc);

    /*
      Cleanup after simulation.
    */
#ifndef PS2
    atexit(cleanup);
#endif
    /*
      Run the Simulation.
    */

    /*
      RRun() executes the main loop.

      Pseudocode: while no exit-request { Handle user input call Tick() to
      evolve the simulation and update graphic transforms Draw graphics }
    */

    RRun(rc, tick, 0);

    return 0;
}
