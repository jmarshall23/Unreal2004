/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:13 $ - Revision: $Revision: 1.8.6.1 $

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
#pragma warning( disable : 4305 )
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <MeMath.h>
#include <MeViewer.h>
#include <MeApp.h>

#include <MpaCharacter.h>

/*
  Global declarations
*/
#define NBalls      4

#define NChars      1

#define Max_Segs_Per_Char   30

#define NAnims      2

#define BALL_RADIUS ((MeReal)0.5)
#define GROUND_RENDER_THICKNESS ((MeReal)0.1)
#define GROUND_HEIGHT ((MeReal)-1.75)

/* help text */

char *help[7] =
{
    "$ACTION2 - target reach",
    "$ACTION3 - shoot",
    "$UP2 - speed up",
    "$DOWN2 - slow down",
    "$LEFT2 - turn left",
    "$RIGHT2 - turn right",
    "$ACTION1 - toggle options menu",
};

/* World for the Dynamics Toolkit simulation */
MstUniverseID universe;
McdSpaceID space;
MdtWorldID world;
MstBridgeID bridge;

/*
  Physics representations
*/
MdtBodyID ball[NBalls];

/*
  Graphical representations
*/
RGraphic *groundG;
RGraphic *ballG[NBalls];
RGraphic *targetG;

/*
  Collision reps
*/
McdModelID groundCM;
McdModelID ballCM[NBalls];
McdGeometryID plane_prim, ball_prim[NBalls];

/*
  MPA objects
*/
MpaCharacter   *chars[NChars];
MpaAnimation   *anims[NAnims];

char *animList[NAnims][2] = {
    {"idle.mpa","idle"},
    {"walk.mpa","walk"},
};

/* Globals */
RRender *rc;

MeApp *meapp;

RMenu *menu;

MeReal gravity[3] = { 0, -5, 0 };

MeVector3 targetPos = { -5, 0, 0 };

MeReal blendTime = (MeReal)0.25;

unsigned int fireDelay = 4;
unsigned int fireFrames = 40;

int enableFriction = 1;

MeReal step = (MeReal)(0.02);

int nextBall = 0;

int selectedCharacter = 0;

int targetReach = 0;

float charSpeed[NChars];
MeReal walkSpeed = 4.0f;
MeReal maxSpeed = 6.0f;
float speedDelta = 1.0f;

MeU32 ROOT[NChars];
MeU32 BICEP[NChars];
MeU32 FINGERS[NChars];

MeReal groundTransform[4][4] =
{
    {1, 0, 0, 0},
    {0, 0, -1, 0},
    {0, 1, 0, 0},
    {0, GROUND_HEIGHT, 0, 1}
};

MeMatrix4 groundRenderTransform =
{
    {1, 0, 0, 0},
    {0, 0, -1, 0},
    {0, 1, 0, 0},
    {0, GROUND_HEIGHT-((MeReal)0.5)*GROUND_RENDER_THICKNESS, 0, 1}
};

MeMatrix4 targetRenderTransform =
{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

void MEAPI shoot(RRender* rc, void* userData)
{
    int i;
    MeReal v[3];
    MeVector3 pos;
    MeVector3 at;

    if (fireFrames < fireDelay)
        return;

    fireFrames = 0;

    /* Re-Enables a body in case it has been AutoDisabled */
    MdtBodyEnable(ball[nextBall]);
    RCameraGetPosition(rc,pos);
    MdtBodySetPosition(ball[nextBall],pos[0],pos[1] - 0.5f,pos[2]);
    MdtBodySetQuaternion(ball[nextBall], 1, 0, 0, 0);
    RCameraGetLookAt(rc,at);
    for (i = 0; i < 3; i++)
        v[i] = at[i] - pos[i];

    MeVector3Normalize(v);

    for (i = 0; i < 3; i++)
        v[i] *= (MeReal)20.0;

    MdtBodySetLinearVelocity(ball[nextBall], v[0], v[1], v[2]);
    MdtBodySetAngularVelocity(ball[nextBall], 0, 0, 0);

    nextBall = (nextBall + 1) % NBalls;
}

void
drawCharacterLines(MpaCharacter *character)
{
    MeU32 i;
    for (i = 0; i < MpaPhysicalObjectGetBodyCount(character); ++i) {
        MdtBodyID body = MpaPhysicalObjectGetBody(character,i);
        MeI32 parentID = character->mHierarchy[i];
        MdtBodyID parent = (parentID>=0)?MpaPhysicalObjectGetBody(character,parentID):NULL;
        RGraphic *line = (RGraphic*)MpaPhysicalObjectGetGraphics(character,i);
        if (body && parent && line) {
            MeVector4 *tm;
            AcmeReal begin[3],end[3];
            tm = MdtBodyGetTransformPtr(body);
            begin[0] = (float)tm[3][0];
            begin[1] = (float)tm[3][1];
            begin[2] = (float)tm[3][2];
            tm = MdtBodyGetTransformPtr(parent);
            end[0] = (float)tm[3][0];
            end[1] = (float)tm[3][1];
            end[2] = (float)tm[3][2];
            RGraphicLineMoveEnds(line,begin,end);
        }
    }
}

/*
  Some character control functions
*/
void playNewState(MpaCharacter *character, MeU32 stateIndex, MeReal playRate, MeReal rampTime)
{
    MpaPlayer *player = MpaCharacterGetPlayer(character);
    MpaDictionary *stateDictionary = MpaPlayerGetStateDictionary(player);
    MpaState **statePtr = (MpaState**)MpaDictionaryGetEntryData(stateDictionary,stateIndex);
    MpaState *state;

    if (!statePtr) {
        return;
    }

    state = *statePtr;

    MpaStateSetPlayRateMultiplier(state,playRate);

    if (MpaStateGetIsActive(state) && MpaStateGetRampRate(state) >= 0) {
        return;
    }

    MpaPlayerDeactivateAll(player,rampTime);
    MpaPlayerActivateState(player,state,0);
    MpaStateRampUp(state,rampTime);
}

void turn(MpaCharacter *character, MeReal delta)
{
    MeVector4 rot;
    MeReal angle = MpaCharacterGetAngle(character);
    angle += delta;
    MpaCharacterSetAngle(character,angle);
    rot[0] = MeCos(angle/2);
    rot[1] = 0;
    rot[2] = MeSin(angle/2);
    rot[3] = 0;
    MpaSpatialObjectSetRotation(character,rot);
}

void
moveCharacter(MeU32 charN)
{
    MpaCharacter *character = chars[charN];
    MeVector3 pos;
    MpaSpatialObjectGetPosition(character,pos);
    if (charSpeed[charN]) {
        MeReal angle = MpaCharacterGetAngle(character);
        pos[0] += step*charSpeed[charN]*MeSin(angle);
        pos[2] += step*charSpeed[charN]*MeCos(angle);
        MpaPhysicalObjectSetPosition(character,pos);
    }
    pos[1] = 0;
}

/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI tick(RRender * rc, void *userData)
{
    MeU32 i;

    MeAppStep(meapp);
    MstUniverseStep(universe,step);

    /*
      Update characters
    */
    for (i = 0; i < NChars; ++i)
    {
        MpaVirtual(Update, chars[i])(chars[i], step);
        moveCharacter(i);
        drawCharacterLines(chars[i]);
    }

    fireFrames++;
}


/*
  Reset to initial positions
*/
void MEAPI reset(RRender * rc, void *userData)
{
    int i;

    /*
      The MdtBodyEnable function enables physical simulation of a body.
      A disabled body takes much less CPU time, and retains its parameters
      until Enabled again.
    */
    for (i = 0; i < NBalls; ++i)
    {
        MdtBodySetPosition(ball[i], 4, GROUND_HEIGHT+BALL_RADIUS, 4 + ((MeReal)(2.0) * i));
        MdtBodySetQuaternion(ball[i], 1, 0, 0, 0);
        MdtBodySetLinearVelocity(ball[i], 0, 0, 0);
        MdtBodySetAngularVelocity(ball[i], 0, 0, 0);
        MdtBodyEnable(ball[i]);
    }

    nextBall = 0;

    for (i = 0; i < NChars; ++i)
    {
        MpaCharacter *character = chars[i];
        MeVector3 pos;
        charSpeed[i] = 0;
        pos[0] = (MeReal)i;
        pos[1] = 0;
        pos[2] = 0;
    }
}

void setCharacterWalkState(MpaCharacter *character, float speed) {
    int state;
    float rate;

    if (speed == 0) {
        state = 0;
        rate = 1;
    } else
    {
        state = 1;
        rate = speed/walkSpeed;
    }

    playNewState(character,state,rate,blendTime);
}

/*
  Character KB input handling
*/
void MEAPI speedUp(RRender * rc, void *userData)
{
    if ((charSpeed[selectedCharacter] += speedDelta) > maxSpeed) {
        charSpeed[selectedCharacter] = maxSpeed;
    }
    setCharacterWalkState(chars[selectedCharacter],charSpeed[selectedCharacter]);
}

void MEAPI slowDown(RRender * rc, void *userData)
{
    if ((charSpeed[selectedCharacter] -= speedDelta) < 0) {
        charSpeed[selectedCharacter] = 0;
    }
    setCharacterWalkState(chars[selectedCharacter],charSpeed[selectedCharacter]);
}

void MEAPI turnLeft(RRender * rc, void *userData)
{
    turn(chars[selectedCharacter],-(MeReal)0.1);
}

void MEAPI turnRight(RRender * rc, void *userData)
{
    turn(chars[selectedCharacter],(MeReal)0.1);
}

void MEAPI toggleTargetReach(RRender * rc, void *userData)
{
    MpaCharacter *character = chars[selectedCharacter];
    MdtJointID reachhook = MpaCharacterGetMeathook(character,FINGERS[selectedCharacter]);

    targetReach = !targetReach;
    if (targetReach) {
        MeVector3 armStrength = {100,100,100};
        MdtConstraintEnable(reachhook);
      MdtRPROJointSetAttachmentPosition(reachhook,targetPos[0],targetPos[1],targetPos[2],1);
        /* Turn off animation for this */
        MpaCharacterForSubtree(character,BICEP[selectedCharacter],MpaNodeCB_SetFlag,(void*)kOverride);
        /* Weak angular strength on arm */
        MpaCharacterForSubtree(character,BICEP[selectedCharacter],MpaNodeCB_SetJointAngularStrength,(void*)armStrength);
        /* Set following */
        MpaCharacterForSubtree(character,BICEP[selectedCharacter],MpaNodeCB_SetFlag,(void*)kFollowRot);
    } else {
        MeVector3 armStrength = {MEINFINITY,MEINFINITY,MEINFINITY};
        MdtConstraintDisable(reachhook);
        MpaCharacterForSubtree(character,BICEP[selectedCharacter],MpaNodeCB_ResetFlag,(void*)kOverride);
        MpaCharacterForSubtree(character,BICEP[selectedCharacter],MpaNodeCB_SetJointAngularStrength,(void*)armStrength);
        MpaCharacterForSubtree(character,BICEP[selectedCharacter],MpaNodeCB_ResetFlag,(void*)kFollowRot);
    }
}

void MEAPI ToggleGravity(MeBool on)
{
    if(on)
        MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);
    else
        MdtWorldSetGravity(world, 0, 0, 0);

}

void cleanup(void)
{
    int i;

    for (i = 0; i < NChars; ++i)
    {
        MpaCharacterDestroy(chars[i]);
    }

    MpaAnimTerm();
    MpaObjectTerm();

    MeAppDestroy(meapp);

    McdGeometryDestroy(plane_prim);
    McdModelDestroy(groundCM);
    for (i = 0; i < NBalls; i++)
    {
        McdGeometryDestroy(ball_prim[i]);
        McdModelDestroy(ballCM[i]);
    }

    MstUniverseDestroy(universe);
    RRenderContextDestroy(rc);
    RMenuDestroy(menu);
}


/*
    Main Routine
*/

int main(int argc, const char **argv)
{
    int i;
    float color[3];

    MstUniverseSizes sizes;
    MeCommandLineOptions* options;

    /* ************* MAKE PHYSICS *************** */

    sizes = MstUniverseDefaultSizes;

    sizes.dynamicBodiesMaxCount = NBalls+NChars*Max_Segs_Per_Char;
    sizes.dynamicConstraintsMaxCount = 200;
    sizes.materialsMaxCount = 1;
    sizes.collisionModelsMaxCount = 50;
    sizes.collisionPairsMaxCount = 1000;
    sizes.collisionGeometryInstancesMaxCount = 0;
    sizes.collisionUserGeometryTypesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);
    space = MstUniverseGetSpace(universe);
    world = MstUniverseGetWorld(universe);
    bridge = MstUniverseGetBridge(universe);

    MdtWorldSetEpsilon(world,0.001f);
    MdtWorldSetGamma(world,0.2f);

    /*
      Collision detection
    */

    {
        plane_prim = McdPlaneCreate();
        groundCM = McdModelCreate(plane_prim);
        groundCM = MstFixedModelCreate(universe,plane_prim,groundTransform);

        for (i = 0; i < NBalls; i++)
        {
            ball_prim[i] = McdSphereCreate(BALL_RADIUS);
            ballCM[i] = MstModelAndBodyCreate(universe, ball_prim[i], 1);
            ball[i] = McdModelGetBody(ballCM[i]);
            MdtBodySetMass(ball[i],100);
        }

        McdSpaceBuild(space);
    }

    /*
      MPA
    */

    /*
      MPA Initializations
    */
    MpaObjectInit(10);
    MpaAnimInit(NAnims);
    MpaCharacterInit();

    /*
      Load character & animation(s)
    */
    chars[0] = MpaCharacterLoad("human.cha",NULL,universe);

    for (i = 0; i < NAnims; ++i)
    {
        anims[i] = MpaAnimationLoad(animList[i][0],animList[i][1]);
    }

    /*
      This sort of setup should be encapsulated,
      but for now here's a chunk of code:
    */
    for (i = 0; i < NChars; ++i)
    {
        /*
          Get animation player for each character
        */
        MpaPlayer *player = MpaCharacterGetPlayer(chars[i]);
        MeU32 j;
        for (j = 0; j < NAnims; ++j)
        {
            /*
              Create an animation state and add it to the player,
              for each animation.  If it's animation 0, activate it.
            */
            MpaState *state = MpaStateCreate(anims[j],MpaDictionaryGetSize(MpaPlayerGetNodeDictionary(player)));
            MpaPlayerAddState(player,state,animList[j][1]);
            if (j == 0)
            {
                MpaPlayerActivateState(player,state,1);
            }
        }
        /*
          More wacky initializations... this is messy and should change
        */
        MpaCharacterSetUpJoints(chars[i]);

        ROOT[i] = MpaCharacterGetBodyIndex(chars[i],"BIP01");
        BICEP[i] = MpaCharacterGetBodyIndex(chars[i],"R_BICEP");
        FINGERS[i] = MpaCharacterGetBodyIndex(chars[i],"R_HAND");

        /* Turn on meathook */
        MdtConstraintEnable(MpaCharacterGetMeathook(chars[i],ROOT[i]));
        /* Set following */
        MpaNodeCB_SetFlag(chars[i],ROOT[i],(void*)kMHFollowPos);
        /* Weak linear strength on reachhook */
    MdtRPROJointSetLinearStrength(MpaCharacterGetMeathook(chars[i],FINGERS[i]),400,400,400);
        /* No angular strength on reachhook */
    MdtRPROJointSetAngularStrength(MpaCharacterGetMeathook(chars[i],FINGERS[i]),0,0,0);
    }

    /*
      Initialise rendering attributes
    */

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;

    RPerformanceBarCreate(rc);

    RCameraSetView(rc,5,ME_PI,0);

    /*
      GROUND:
    */

    color[0] = 0.0f;
    color[1] = 0.75f;
    color[2] = 0.1f;

    groundG =
        RGraphicBoxCreate(rc, 24.0f, 24.0f, 0.1f, color,
            groundRenderTransform);
    RGraphicSetTexture(rc, groundG, "checkerboard");

    /*
      TARGET:
    */

    color[0] = 1;
    color[1] = 0;
    color[2] = 0;

    MeVector3Copy(targetRenderTransform[3],targetPos);
    targetG = RGraphicTorusCreate(rc, 0.1f, 0.2f, color, targetRenderTransform);

    /*
      BALLS:
    */

    for (i = 0; i < NBalls; i++)
    {
        color[0] = 0.85f;
        color[1] = 0.85f;
        color[2] = 0.0f;
        ballG[i] = RGraphicSphereCreate(rc, 0.5, color, MdtBodyGetTransformPtr(ball[i]));
    }

    /*
      CHARACTERS:
    */

    for (i = 0; i < NChars; ++i)
    {
        MeU32 j;
        for (j = 0; j < MpaPhysicalObjectGetBodyCount(chars[i]); ++j)
        {
            MdtBodyID body = MpaPhysicalObjectGetBody(chars[i],j);
            float zero[3] = {0,0,0};
            color[0] = 1;
            color[1] = 0;
            color[2] = 1;
            /*
              Create ball at each body's location
            */
            RGraphicSphereCreate(rc, 0.1f, color, MdtBodyGetTransformPtr(body));
            /*
              Create a graphics line, and save in character.  This will
              need updating.
            */
            if (chars[i]->mHierarchy[j] >= 0) {
                MpaPhysicalObjectSetGraphics(chars[i], j, RGraphicLineCreate(rc, zero, zero, color, NULL));
            }
        }
    }

    reset(rc,0);

    /*
        App:
    */

    meapp = MeAppCreate(world, space, rc);

    RRenderSetActionNCallBack(rc, 2, toggleTargetReach, 0);
    RRenderSetActionNCallBack(rc, 3, shoot, 0);
    RRenderSetUp2CallBack(rc, speedUp, 0);
    RRenderSetDown2CallBack(rc, slowDown, 0);
    RRenderSetLeft2CallBack(rc, turnLeft, 0);
    RRenderSetRight2CallBack(rc, turnRight, 0);

    /* CONTROLS: */

    RRenderSetWindowTitle(rc, "MPA example");
    RRenderCreateUserHelp(rc,help,7);
    RRenderToggleUserHelp(rc);

    menu = RMenuCreate(rc, "Options Menu");
    RMenuAddToggleEntry(menu, "Gravity", ToggleGravity, 0);
    RRenderSetDefaultMenu(rc, menu);

    RRun(rc, tick, 0);

    return 0;
}
