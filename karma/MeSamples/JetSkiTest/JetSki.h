#ifndef _JETSKI_H
#define _JETSKI_H
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:12 $ - Revision: $Revision: 1.5.6.1 $

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

#include <Mdt.h>
#include <MeViewer.h>

typedef enum
{
    kJetSkiHull = 0,
    kJetSkiHandle,
    kJetSkiCalves,
    kJetSkiThighs,
    kJetSkiGroin,
    kJetSkiTorso,
    kJetSkiUpperArmL,
    kJetSkiUpperArmR,
    kJetSkiForeArmL,
    kJetSkiForeArmR,
    kJetSkiMaxParts
} JetSkiParts;

typedef enum
{
    kJetSkiSteering = 0,
    kJetSkiAnkles,
    kJetSkiKnees,
    kJetSkiHips,
    kJetSkiWaist,
    kJetSkiShoulderL,
    kJetSkiShoulderR,
    kJetSkiElbowL,
    kJetSkiElbowR,
    kJetSkiWristL,
    kJetSkiWristR,
    kJetSkiMaxJoints
}
JetSkiJoints;

typedef struct JetSki
{
    MdtBodyID*          body;
    RGraphic**          bodyG;

    MdtConstraintID*    joint;
    MeMatrix4*          jointTM;

    MdtSpringID         bodyProwSpring;
    MdtSpringID         elbowSpring[3];

    RGraphic*           headG;
    MeMatrix4           headTM;

    RGraphic*           seatG;
    MeMatrix4           seatTM;

    RGraphic*           prowG;
    MeMatrix4           prowTM;

    int                 currentPose;
} JetSki;

#ifdef __cplusplus
extern "C"
{
#endif

JetSki* JetSkiCreate(MdtWorldID world, RRender* rc);
void    JetSkiDestroy(JetSki* jetski);

void    JetSkiUpdateMatrices(JetSki* jetski);
void    JetSkiNextPose(JetSki* jetski);

#ifdef __cplusplus
}
#endif

#endif
