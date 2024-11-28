/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/25 09:24:00 $ - Revision: $Revision: 1.3.2.2 $

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


/* Mathengine headers   */
#include <Mst.h>
#include <MeApp.h>
#include <MeViewer.h>

/* Application headers  */
#include "main.h"
#include "utils.h"
#include "vehicle.h"

/* disable unused variable warnings */
#ifdef _MSC_VER
#pragma warning( disable : 4101 )
#endif
    
FourWheeledVehicleData quadBikeData =
{
    0.25f,                                  /* wheelRadius */
    {{-0.6f,-0.25f,-0.5f},                  /* wheelOffset[BACK_LEFT] */
    {-0.6f,-0.25f,0.5f},                    /* wheelOffset[BACK_RIGHT] */
    {0.6f,-0.25f,-0.5f},                    /* wheelOffset[FRONT_LEFT] */
    {0.6f,-0.25f,0.5f}},                    /* wheelOffset[FRONT_RIGHT] */
    {1.85f,0.5f,0.35f},                     /* chassisDims */
    {0.19f,0.57f,0.00f}                     /* handlebar offset */
};

FourWheeledVehicle quadBike;

MeVector3 startPos = {0, 10, -10};


/***********************************************************************************************
*
***********************************************************************************************/
MeBool MEAPI WheelGroundCB(McdIntersectResult* result, McdContact* colC, MdtContactID dynC)
{
    MdtBodyID body;
    MdtCarWheelID wj;
    MeVector3 normal, haxis, dir;
    MeReal slip;
    MdtBclContactParams *params;

    /* Add wheel callback code here */
    /* PASTE_15 */

    return 1; /* keep contact */
}

/***********************************************************************************************
*
***********************************************************************************************/
void UpdateVehicleControls(FourWheeledVehicle *veh)
{
    MeReal pgap = 0.4f;
    MeReal width = 0.5f;
    MeReal maxSpeed = 5.0f;
    MeReal maxForce = MEINFINITY;
    MeReal theta, desired_vel;
    MeReal torque;
    int i;

    for (i = 0; i < 4; i++)
    {
        MeVector3 haxis;

        if(!veh->wheelJoint[i]) continue; /* Do nothing if wheel joint hasn't been created */
        
        MdtCarWheelGetHingeAxis(veh->wheelJoint[i], haxis);
        MdtBodySetFastSpinAxis(veh->wheelBody[i], haxis[0],haxis[1],haxis[2]);

        /* Add Suspension joint controller here */
        /* Wheel torque calculations. */
        if (i == BACK_LEFT || i == BACK_RIGHT)
        {
            if(veh->brakeInput == 0)
            {
                /* Add throttle input here  */
                /* PASTE_10 */

                /* Ensure brakes are zeroed */
                MdtCarWheelSetHingeLimitedForceMotor(veh->wheelJoint[i], 0, 0);
            }

            /* Add Braking input here   */
            /* PASTE_11 */
        }
        else
        {

            /* Add Front wheel steering controller here */
            /* PASTE_12 */

            /* Braking on front wheels as well  */
            if(veh->brakeInput == 0)
            {
                /* Ensure brakes are zeroed */
                MdtCarWheelSetHingeLimitedForceMotor(veh->wheelJoint[i], 0, 0);
            }

            /* Add braking input here   */
            /* PASTE_11 */
        }
    }

    /* Add Handle Bar twist controller here */
    /* PASTE_13 */
}

/***********************************************************************************************
*
***********************************************************************************************/
void CreateVehicleGraphics(FourWheeledVehicle *veh)
{
    int i;

    /*
        Create chassis graphic and associate it with chassis body 
    */
    if(veh->chassisBody)
    {
        veh->chassisGraphic = RGraphicLoadASE(rc,"chassis.ase",1.0f,1.0f,1.0f,white,MdtBodyGetTransformPtr(veh->chassisBody));
        RGraphicSetTexture(rc, veh->chassisGraphic, "chassis");
    }
    /*
        Create each wheel graphic and associate it with chassis body 
    */
    for(i = 0; i < 4; i++)
    {
        if(veh->wheelBody[i])
        {
            if ((i%2)==0){
                veh->wheelGraphic[i] = RGraphicLoadASE(rc,"rightWheel.ase",1.0f,1.0f,1.0f,white,MdtBodyGetTransformPtr(veh->wheelBody[i]));
                RGraphicSetTexture(rc, veh->wheelGraphic[i], "wheel");
            }else{
                veh->wheelGraphic[i] = RGraphicLoadASE(rc,"leftWheel.ase",1.0f,1.0f,1.0f,white,MdtBodyGetTransformPtr(veh->wheelBody[i]));
                RGraphicSetTexture(rc, veh->wheelGraphic[i], "wheel");
            }
        }

    }

    /* Handle Bars  */
    if(veh->hBarBody)
    {
        veh->chassisGraphic = RGraphicBoxCreate(rc, 
                                            (MeReal)0.03, (MeReal)0.03,(MeReal)0.7,
                                            white, MdtBodyGetTransformPtr(veh->hBarBody));
    }
}


/***********************************************************************************************
*
***********************************************************************************************/
void InitialiseVehicle(FourWheeledVehicle *veh)
{
    int i,j;
    MeVector3 pos;
    MeMatrix3 Ixyz = {1,0,0, 0,1,0, 0,0,1};
    MeMatrix4 comTM = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,-0.3,0,1};

    memset(veh, 0, sizeof(FourWheeledVehicle));

    /*
        Set the data parameters
    */
    veh->data = &quadBikeData;

    /* Add Chassis initialization here  */
    /* PASTE_06 */

    /* Add Wheel initialization here    */
    /* PASTE_07 */

    /* Add Suspension joint initialization here */
    /* PASTE_08 */

    /* Add Handlebar initialization here    */
    /* PASTE_09 */

    /* Create graphics. */
    CreateVehicleGraphics(veh);
}

/***********************************************************************************************
*
***********************************************************************************************/
void ResetVehicle(FourWheeledVehicle *veh)
{
    MeVector3 pos;
    int i;

    /* Reset chassis position & velocity    */
    if(veh->chassisBody)
    {
        MdtBodySetPosition(veh->chassisBody, startPos[0], startPos[1], startPos[2]);
        MdtBodySetLinearVelocity(veh->chassisBody, 0, 0, 0);
        MdtBodySetAngularVelocity(veh->chassisBody, 0, 0, 0);
        MdtBodySetQuaternion(veh->chassisBody, 1, 0, 0, 0);
    }

    /* Reset each wheel */
    for(i = 0; i < 4; i++)
    {
        if(veh->wheelBody[i])
        {
            MeVector3Add(pos, startPos, veh->data->wheelOffset[i]);
            MdtBodySetPosition(veh->wheelBody[i], pos[0], pos[1], pos[2]);
            MdtBodySetLinearVelocity(veh->wheelBody[i], 0, 0, 0);
            MdtBodySetAngularVelocity(veh->wheelBody[i], 0, 0, 0);
            MdtBodySetQuaternion(veh->wheelBody[i], 1, 0, 0, 0);
        }
    }

    /* Reset Handle bars    */
    if(veh->hBarBody)
    {
        MeVector3Add(pos, startPos, veh->data->hBarOffset);
        MdtBodySetPosition(veh->hBarBody, pos[0], pos[1], pos[2]);
        MdtBodySetLinearVelocity(veh->hBarBody, 0, 0, 0);
        MdtBodySetAngularVelocity(veh->hBarBody, 0, 0, 0);
        MdtBodySetQuaternion(veh->hBarBody, 1, 0, 0, 0);
    }
}
