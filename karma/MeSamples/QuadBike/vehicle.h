/* -*- mode: c; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:13 $ - Revision: $Revision: 1.2.2.2 $

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
typedef enum
{
    BACK_LEFT=0,
    BACK_RIGHT,
    FRONT_LEFT,
    FRONT_RIGHT,
    NUM_WHEELS
} WheelNumber;

typedef struct
{
    MeReal wheelRadius;
    MeVector3 wheelOffset[4];
    MeVector3 chassisDims;
    MeVector3 hBarOffset;
} FourWheeledVehicleData;

typedef struct 
{
    /* wheels (bl,br,fl,fr) */
    MdtBodyID wheelBody[4];
    McdGeometryID wheelGeom[4];
    McdModelID wheelCM[4];
    RGraphic *wheelGraphic[4];

    /* chassis */
    MdtBodyID chassisBody;
    McdGeometryID chassisGeom;
    McdModelID chassisCM;
    RGraphic *chassisGraphic;

    /* handlebars */
    MdtBodyID hBarBody;
    McdGeometryID hBarGeom;
    McdModelID hBarCM;
    RGraphic *hBarGraphic;

    /* Wheel suspension joints */
    MdtCarWheelID wheelJoint[4];

    /* Handlebar joint  */
    MdtHingeID hBarJoint;

    /* parameters specifying dynamics and dimensions */
    FourWheeledVehicleData *data;

    /* Control inputs   */
    MeReal steeringInput;
    MeReal throttleInput;
    MeReal brakeInput;

} FourWheeledVehicle;

extern FourWheeledVehicle quadBike;
extern MeVector3 startPos;

extern void UpdateVehicleControls(FourWheeledVehicle *veh);
extern void InitialiseVehicle(FourWheeledVehicle *veh);
extern void DestroyVehicle(FourWheeledVehicle *veh);
extern void ResetVehicle(FourWheeledVehicle *veh);
extern MeBool MEAPI WheelGroundCB(McdIntersectResult* result, McdContact* colC, MdtContactID dynC);
