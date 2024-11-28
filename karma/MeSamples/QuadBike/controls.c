/* -*- mode: c; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:13 $ - Revision: $Revision: 1.2.2.3 $

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

#include <Mst.h>
#include <MeApp.h>
#include <MeViewer.h>

/* Application headers  */
#include "main.h"
#include "vehicle.h"
#include "rider.h"
#include "utils.h"

static int turnInput;

/***********************************************************************************************
*
***********************************************************************************************/
void MEAPI Accelerate(RRender *rc, void *userData)
{
    quadBike.throttleInput += 10.0f*timeStep;

    quadBike.throttleInput = min(quadBike.throttleInput, 1.0f);

    quadBike.brakeInput = 0;
    
}

/***********************************************************************************************
*
***********************************************************************************************/
void MEAPI Brake(RRender *rc, void *userData)
{
    quadBike.brakeInput += 5.0f*timeStep;

    quadBike.brakeInput = min(quadBike.brakeInput, 1.0f);

    quadBike.throttleInput = 0;
}


/***********************************************************************************************
*
***********************************************************************************************/
void MEAPI Turn(RRender *rc, void *userData)
{
    int dir = (int)userData;    

    quadBike.steeringInput += 10.0f*timeStep*dir;
    quadBike.steeringInput = min(quadBike.steeringInput, 1.0f);
    quadBike.steeringInput = max(quadBike.steeringInput, -1.0f);

    turnInput = 10; /* This is a hack */
}

/***********************************************************************************************
*
***********************************************************************************************/
void UpdateControls(void)
{
    UpdateVehicleControls(&quadBike);
    UpdateRiderControls(&rider);

    if(!turnInput)
    {
        quadBike.steeringInput *= 0.9f;
    } else 
    {
        turnInput--;
    }

    quadBike.throttleInput -= 1.0f*timeStep;
    quadBike.throttleInput = max(quadBike.throttleInput, 0);
}
