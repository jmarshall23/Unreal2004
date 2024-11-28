/* -*- mode: C; -*- */
/*
    Copyright (c) 1997-2002 MathEngine PLC

    $Name: t-stevet-RWSpre-030110 $
  
    Date: $Date: 2002/04/04 15:29:08 $ - Revision: $Revision: 1.1.2.2 $
    
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

#include <stdlib.h>

#include <Mst.h>
#include <MeMisc.h>
#include <MeViewer.h>
#include <MeMath.h>
#include <MeApp.h>
#include <MeDebugDraw.h>
#include <McduDebugDraw.h>
#include <ConeGeometry.h>       /* User defined geometry */

#define MAXCOUNT 30
#define GRAVITY -1
#define DRAW_LINES 1

/* Render context */
RRender *rc;

/* graphics */
RGraphic *coneG[MAXCOUNT];
RGraphic *planeG;

MeApp* meapp;

McdFrameworkID framework;
MstUniverseID universe;

McdGeometryID coneGeom;
McdModelID cone[MAXCOUNT];

McdGeometryID planeGeom;
McdModelID plane;

MeMatrix4Ptr groundTM;
MeMatrix4 baseGroundTM= 
{
    {1, 0,  0, 0}, 
    {0, 0, -1, 0}, 
    {0, 1,  0, 0}, 
    {0, 0,  0, 1}
};

MeReal step = (MeReal)(0.03);

MeReal coneRadius = 1;
MeReal coneHeight = 1.5f;
MeReal spacing = 1.5f;

char *help[] = {
    "$ACTION2 - reset", 
    "$MOUSE - mouse force"
};

#if DRAW_LINES
MeReal green[] = { 0, 1, 0, 1 };
RGraphic **sline;      // static lines for DrawLine
int max_sline, num_sline;
#endif

/* per timestep simulation function */

void MEAPI tick(RRender * rc, void* userData)
{
    MeAppStep(meapp);
#if DRAW_LINES
    num_sline = 0;
    MeAppDrawContacts(meapp);
    McduDebugDrawSpace(universe->space, 7, green);
#endif
    MstUniverseStep(universe, step);
}


#if DRAW_LINES

/****************************************************************************
 *  This function returns an identity transformation.
 */
MeMatrix4Ptr IdentityTM()
{
    MeMatrix4Ptr tm = (MeMatrix4Ptr) MeMemoryAPI.createAligned(sizeof(MeMatrix4), 16);
    MeMatrix4TMMakeIdentity(tm);
    return tm;
}
/****************************************************************************
    This draws a line.  It is a callback registered in MeDebugDrawAPI.line.
*/
void MEAPI DrawLine(MeVector3 start, MeVector3 end, 
                    MeReal r, MeReal g, MeReal b)
{
    int i;
    AcmeReal z[3] = { 0,0,0 };
    AcmeReal s[3] = { start[0], start[1], start[2] };
    AcmeReal e[3] = { end[0], end[1], end[2] };
    AcmeReal c[4] = { r, g, b, 1 };

    if (num_sline == max_sline)    // need to grow
    {
        max_sline += 10;
        sline = (RGraphic**) realloc(sline, max_sline * sizeof *sline);
        for (i = num_sline; i < max_sline; ++i)
            sline[i] = RGraphicLineCreate(rc,z,z,z,IdentityTM());
    }
    i = num_sline++;
    RGraphicLineMoveEnds(sline[i], s, e);
    RGraphicSetColor(sline[i], c);
}
#endif

/* put all the cones in their initial position */

void MEAPI Reset(RRender * rc, void* userData)
{
    int i;
    
    for(i=0; i<MAXCOUNT; i++)
    {
        McdModelDynamicsReset(cone[i]);
        
        McdModelDynamicsSetPosition(cone[i], 
            spacing*(i%5)-3, i+2.f, spacing*(i/5)-3);
        
        McdModelDynamicsEnable(cone[i]);
    }
    
    McdModelDynamicsSetLinearVelocity(cone[0], 
        (MeReal)0.1, (MeReal)0.2, (MeReal)0.1);
}

/* Cleanup and exit */

void MEAPI_CDECL cleanup(void)
{
    int i;
    McdModelDestroy(plane);
    for(i=0;i<MAXCOUNT;i++)
        McdModelDestroy(cone[i]);
    
    McdGeometryDestroy(coneGeom);
    McdGeometryDestroy(planeGeom);
    MstUniverseDestroy(universe);
    MeAppDestroy(meapp);
    RRenderContextDestroy(rc);
    MeMemoryAPI.destroyAligned(groundTM);
}

/* Main Routine */
int main(int argc, char *argv[])
{
    int i;
    MstUniverseSizes sizes;
    float color[4] = { 0, 1, 0, 1 };  /* RGB color */
    MeCommandLineOptions* options;
    
    /* Create the universe */
    
    sizes = MstUniverseDefaultSizes;
    sizes.collisionModelsMaxCount = MAXCOUNT + 3;
    sizes.collisionPairsMaxCount = MAXCOUNT * 6;
    sizes.collisionUserGeometryTypesMaxCount = 1;
    sizes.dynamicBodiesMaxCount = MAXCOUNT + 3;
    sizes.dynamicConstraintsMaxCount = MAXCOUNT * 6;
    
    universe = MstUniverseCreate(&sizes);
    framework = MstUniverseGetFramework(universe);

    MdtWorldSetGravity(MstUniverseGetWorld(universe), 0, GRAVITY, 0);

    /* Register the Cone user geometry */

    ConeRegisterType(framework);
    ConePrimitivesRegisterInteractions(framework);
    
    /* Create the plane */
    planeGeom = McdPlaneCreate(framework);
    groundTM = (MeMatrix4Ptr)MeMemoryAPI.createAligned(sizeof(MeMatrix4), 16);
    MeMatrix4Copy(groundTM, baseGroundTM);
    plane = MstFixedModelCreate(universe, planeGeom, groundTM);
    
    /* Create the cones */
    
    coneGeom = ConeCreate(framework, coneRadius, coneHeight);
    
    for(i=0; i<MAXCOUNT; i++)
        cone[i] = MstModelAndBodyCreate(universe, coneGeom, 1);
    
    /* Put them in the right place to be strung together */
    
    Reset(rc, 0);
    
    /* Initialise rendering attributes. */
      
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, 1);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 1;

    RCameraRotateElevation(rc, .4f);
    RCameraRotateAngle(rc, 2.5f);

    RPerformanceBarCreate(rc);
    
    RRenderSetActionNCallBack(rc, 2, Reset, 0);
    
    RRenderSetWindowTitle(rc, "Cone geometry example");
    RRenderCreateUserHelp(rc, help, 2);
    RRenderToggleUserHelp(rc);
    
    /* Create graphics. */
    
    for(i=0; i<MAXCOUNT; i++)
    {
        MeHSVtoRGB(((float)i/(float)MAXCOUNT)*360, (float)0.8, 1, color);
        coneG[i] = RGraphicConeCreate(rc, coneRadius, coneHeight*3/4, coneHeight/4, color, McdModelGetTransformPtr(cone[i]));
    }
    
    color[0] = color[1] = color[2] = color[3] = 1;
    planeG = RGraphicGroundPlaneCreate(rc, 24, 2, color, 0);
    RGraphicSetTexture(rc, planeG, "checkerboard");
    
    /* Set up MeApp so we get mouse picking */
    
    meapp = MeAppCreateFromUniverse(universe, rc);
    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);

#if DRAW_LINES
    MeAppDrawContactsInit(meapp, color, 300);
    MeAppSetContactDrawLength(meapp, 0.1f);   // 0.02 is actual 1:1
    MeDebugDrawAPI.line = DrawLine;
#else
    RRenderSkydomeCreate(rc, "skydome", 2, 1);
#endif
    
#ifndef PS2
    atexit(cleanup);
#endif
    RRun(rc, tick, 0);
    
    return 0;
}
