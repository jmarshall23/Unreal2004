/* -*- mode: C; -*- */

/*
  Copyright (c) 1997-2002 MathEngine PLC

  $Name: t-stevet-RWSpre-030110 $

  Date: $Date: 2002/04/25 11:00:26 $ - Revision: $Revision: 1.72.2.11 $

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

/*
  Overview:

  Three different convex objects fall down stairs.
*/
#include <stdlib.h>
#include <time.h>

#if defined WIN32 && defined _DEBUG
#   include <crtdbg.h>
#endif

#include <MePrecision.h>
#include <MeMath.h>

#include <Mcd.h>
#include <McdConvexMesh.h>

#include <Mst.h>
#include <MeApp.h>

#include <MeViewer.h>
#include <RConvex.h>

#ifdef PS2
#  define MAX_MATRIX_SIZE       (36)
#endif

static RRender          *rc;
static MeApp            *app;
static McdFrameworkID   framework;
static MstBridgeID      bridge;
static MdtWorldID       world;
static McdSpaceID       space;

/*
    There are six bodies with six models, and then there are six models,
    the beams of the stairs, that have no body as they don't move;
    finally, we got the ground plane.
*/

#define convexBoxMODELS         2
#define convexConeMODELS        2
#define convexSliceMODELS       2
#define beamMODELS              5

#define maxMODELS               40
#define maxBODIES               maxMODELS

#if (maxMODELS < (convexBoxMODELS+convexConeMODELS+convexSliceMODELS+1))
#   error Total number of models insufficient
#endif

static McdGeometryID    convexBoxGeometry;
static McdGeometryID    convexConeGeometry;
static McdGeometryID    convexSliceGeometry;
static McdGeometryID    beamGeometry;
static McdGeometryID    groundGeometry;

static McdModelID       convexBoxModel[convexBoxMODELS];
static McdModelID       convexConeModel[convexConeMODELS];
static McdModelID       convexSliceModel[convexSliceMODELS];
static McdModelID       beamModel[beamMODELS];
static McdModelID       groundModel;

static RGraphic         *convexBoxGraphic[convexBoxMODELS];
static RGraphic         *convexConeGraphic[convexConeMODELS];
static RGraphic         *convexSliceGraphic[convexSliceMODELS];
static RGraphic         *beamGraphic[beamMODELS];
static RGraphic         *groundGraphic;

/* Beams and the ground don't move. */

static MdtBodyID        convexBoxBody[convexBoxMODELS];
static MdtBodyID        convexConeBody[convexConeMODELS];
static MdtBodyID        convexSliceBody[convexSliceMODELS];

/* parameters used in constructing convex objects */
#define ROOT2           (1.414213562f)

#define _R1             (0.45f)
#define _R2             (0.35f)
#define _R2a            (_R2*1.35f)

#define _L2             (_R2*(ROOT2+1.0f))
#define _L2a            (_L2*1.35f )

#define _O              (_R2*0.5f)

static const float colorReddish[4]      = { 0.6f,   0,      0.4f,   1   };
static const float colorGreenish[4]     = { 0.4f,   0.6f,   0,      1   };
static const float colorBlueish[4]      = { 0,      0.4f,   0.6f,   1   };
static const float colorWhite[4]        = { 1,      1,      1,      1   };

#define convexBoxVERTICES 12

static const MeVector3 convexBox[convexBoxVERTICES] =
{
    {   _R1,            _R1*2,          _R1    },
    {   -_R1,           _R1*2,          _R1    },
    {   -_R1,           _R1*2,          -_R1   },
    {   _R1,            _R1*2,          -_R1   },
    {   _R1*2,          0,              _R1*2  },
    {   -_R1*2,         0,              _R1*2  },
    {   -_R1*2,         0,              -_R1*2 },
    {   _R1*2,          0,              -_R1*2 },
    {   _R1,            -_R1*2,         _R1    },
    {   -_R1,           -_R1*2,         _R1    },
    {   -_R1,           -_R1*2,         -_R1   },
    {   _R1,            -_R1*2,         -_R1   }
};

#define convexConeVERTICES      17

static const MeVector3 convexCone[convexConeVERTICES] =
{
    {   _L2,            _O+_R2,         _R2    },
    {   _R2,            _O+_R2,         _L2    },
    {   -_R2,           _O+_R2,         _L2    },
    {   -_L2,           _O+_R2,         _R2    },
    {   -_L2,           _O+_R2,         -_R2   },
    {   -_R2,           _O+_R2,         -_L2   },
    {   _R2,            _O+_R2,         -_L2   },
    {   _L2,            _O+_R2,         -_R2   },
    {   _L2a,           _O+0,           _R2a   },
    {   _R2a,           _O+0,           _L2a   },
    {   -_R2a,          _O+0,           _L2a   },   
    {   -_L2a,          _O+0,           _R2a   },
    {   -_L2a,          _O+0,           -_R2a  },
    {   -_R2a,          _O+0,           -_L2a  },
    {   _R2a,           _O+0,           -_L2a  },
    {   _L2a,           _O+0,           -_R2a  },
    {   0,              _O+-_R2*4,      0      }
};

#define convexSliceVERTICES     16

static const MeVector3 convexSlice[convexSliceVERTICES] =
{
    {   _L2,            _R2,            _R2    },
    {   _R2,            _R2,            _L2    },
    {   -_R2,           _R2,            _L2    },
    {   -_L2,           _R2,            _R2    },
    {   -_L2,           _R2,            -_R2   },
    {   -_R2,           _R2,            -_L2   },
    {   _R2,            _R2,            -_L2   },
    {   _L2,            _R2,            -_R2   },
    {   _L2a,           -_R2,           _R2a   },
    {   _R2a,           -_R2,           _L2a   },
    {   -_R2a,          -_R2,           _L2a   },
    {   -_L2a,          -_R2,           _R2a   },
    {   -_L2a,          -_R2,           -_R2a  },
    {   -_R2a,          -_R2,           -_L2a  },
    {   _R2a,           -_R2,           -_L2a  },
    {   _L2a,           -_R2,           -_R2a  }
};

#undef _R1
#undef _R2
#undef _R2a

#undef _L2
#undef _L2a

#undef _O

#define beamH           (1)
#define beamW           (1.5)
#define beamL           (10)

static const MeReal     beamSizes[3] = { beamW, beamH, beamL };

static MeALIGNDATA(MeMatrix4,groundTransform,MeALIGNTO) =
{
    {   1,              0,              0,              0       },
    {   0,              0,              -1,             0       },
    {   0,              1,              0,              0       },
    {   0,              -beamH*0.5,     0,              1       }
};

static MeALIGNDATA(const MeMatrix4,groundTransform_G,MeALIGNTO) =
{
    {   1,              0,              0,              0       },
    {   0,              0,              -1,             0       },
    {   0,              1,              0,              0       },
    {   0,              -beamH*1.5f,    0,              1       }
};

static MeALIGNDATA(MeMatrix4,beamTransform[5],MeALIGNTO) =
{
    {
        {       1,              0,              0,              0       },
        {       0,              1,              0,              0       },
        {       0,              0,              1,              0       },
        {       0,              0,              0,              1       }
    },
    {
        {       1,              0,              0,              0       },
        {       0,              1,              0,              0       },
        {       0,              0,              1,              0       },
        {       -beamW*1,       beamH*1,        0,              1       }
    },
    {
        {       1,              0,              0,              0       },
        {       0,              1,              0,              0       },
        {       0,              0,              1,              0       },
        {       -beamW*2,       beamH*2,        0,              1       }
    },
    {
        {       1,              0,              0,              0       },
        {       0,              1,              0,              0       },
        {       0,              0,              1,              0       },
        {       -beamW*3,       beamH*3,        0,              1       }
    },
    {
        {       1,              0,              0,              0       },
        {       0,              1,              0,              0       },
        {       0,              0,              1,              0       },
        {       -beamW*4,       beamH*4,        0,              1       }
    },
};

static const MeReal step        = 0.02f;
static const char *const help[2] =
{
    "$ACTION4 - toggle AutoEvolve",
    "$ACTION5 - toggle AutoFall"
};

#if 0
    static const char *const help[2] =
    {
        "$LEFT2 - toggle AutoEvolve",
        "$DOWN2 - toggle AutoFall"
    };
#endif

static int autoEvolve           = 1;
static int autoFall             = 1;

static int stepCount            = 0;
static int whichBody            = 0;

/*
  Generate a random number between start and end.

       From Numerical Recipes in C: The Art of Scientific Computing
       (William  H.  Press, Brian P. Flannery, Saul A. Teukolsky,
       William T.  Vetterling;  New  York:  Cambridge  University
       Press, 1992 (2nd ed., p. 277)),

              "If you want to generate a random integer between 1
              and 10, you should always do it by using high-order
              bits, as in

                     j=1+(int) (10.0*rand()/(RAND_MAX+1.0));

              and never by anything resembling

                     j=1+(rand() % 10);

              (which uses lower-order bits)."
*/

static void randomInit(unsigned origin)
{
    while (origin == 0)
#if PS2
        origin = 42;
#else
        origin = (unsigned) time((time_t) 0);
#endif

    srand(origin);
}

static MeReal randomNext(const MeReal start,const MeReal end)
{
    return start + ((end - start) * rand ()) / (MeReal) RAND_MAX;
}

static void convexBodyInit(const MdtBodyID b,const unsigned i)
{
    MdtBodyEnable(b);

    MdtBodySetMass(b,1.0f);

    MdtBodySetLinearVelocity(b,3,-5,0);
    MdtBodySetLinearVelocityDamping(b,0.3f);
    MdtBodySetAngularVelocityDamping(b,0.3f);

    MdtBodySetPosition(b,-5,(MeReal) (i*3 + 4),0);
}

static void convexBodyReset(const MdtBodyID b)
{
    MdtBodySetLinearVelocity(b,2,-5,0);
    MdtBodySetAngularVelocity(b,0,0,randomNext(-2,2));

    MdtBodySetPosition(b,-5,12,0);
}

static void MEAPI tick(RRender *rc,void *useData)
{
    if (!autoEvolve)
        return;

    MeAppStep(app);

    MeProfileStartSection("Collision",0);
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge,space,world);
    MeProfileEndSection("Collision");

    MeProfileStartSection("Dynamics",0);
    MdtWorldStep(world,step);
    MeProfileEndSection("Dynamics");

    if (autoFall)
    {
        if ((stepCount + 000) % 300 == 0)
            convexBodyReset(convexBoxBody[whichBody]);

        if ((stepCount + 100) % 300 == 0)
            convexBodyReset(convexConeBody[whichBody]);

        if ((stepCount + 200) % 300 == 0)
        {
            convexBodyReset(convexSliceBody[whichBody]);
            whichBody = (whichBody+1) % convexSliceMODELS;
        }

        stepCount++;
    }
}

static void MEAPI_CDECL cleanup (void)
{
    McdFrameworkDestroyAllModelsAndGeometries(framework);
    MstBridgeDestroy(bridge);

    McdSpaceDestroy(space);
    McdTerm(framework);
    MdtWorldDestroy(world);

    MeAppDestroy(app);

    RRenderContextDestroy(rc);

#ifdef _XBOX
    
#endif

}

static void MEAPI toggleAutoEvolve(RRender *rc,void *userData)
{
    autoEvolve = !autoEvolve;
}

static void MEAPI toggleAutoFall(RRender *rc,void *userData)
{
    autoFall = !autoFall;
}

int MEAPI_CDECL main (int argc,const char **argv)
{
    randomInit(42);

    {
        MeCommandLineOptions *const options
            = MeCommandLineOptionsCreate(argc,argv);

        rc = RRenderContextCreate(options,0,!MEFALSE);

        MeCommandLineOptionsDestroy(options);
    }

    if (rc == 0)
        return 1;

#if defined WIN32 && defined _DEBUG
    {
        int debugFlag;

        debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        debugFlag |= _CRTDBG_ALLOC_MEM_DF;
        debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
        debugFlag |= _CRTDBG_LEAK_CHECK_DF;

        _CrtSetDbgFlag(debugFlag);
    }
#   endif

    /* Initialize dynamics */

    world = MdtWorldCreate(maxBODIES,300, 1, 1);
    MdtWorldSetAutoDisable(world,0);
    MdtWorldSetGravity(world,0,-9.81f,0);

#ifdef PS2
    MdtWorldSetMaxMatrixSize(world,MAX_MATRIX_SIZE);
#endif

    /*
        Initialize the collision subsystem
    */

    /* # geoTypes = #primitives + convex */
    framework = McdInit(0,200,0,1.0);

    McdPrimitivesRegisterTypes(framework);
    McdConvexMeshRegisterType(framework);

    McdPrimitivesRegisterInteractions(framework);
    McdConvexMeshPrimitivesRegisterInteractions(framework);

    space = McdSpaceAxisSortCreate(framework,McdAllAxes,maxMODELS,100);

    /*
      Set up the bridge
    */

    bridge = MstBridgeCreate(framework,maxMODELS);
        MstSetWorldHandlers(world);

    /*
      Create the geometries and associated models.
    */

    convexBoxGeometry = (McdGeometryID) McdConvexMeshCreateHull(
        framework,convexBox,convexBoxVERTICES,0);

    convexConeGeometry = (McdGeometryID) McdConvexMeshCreateHull(
        framework,convexCone,convexConeVERTICES,0);

    convexSliceGeometry = (McdGeometryID) McdConvexMeshCreateHull(
        framework,convexSlice,convexSliceVERTICES,0);

    beamGeometry = (McdGeometryID) McdBoxCreate(framework,
        beamSizes[0],beamSizes[1],beamSizes[2]);

    groundGeometry = McdPlaneCreate(framework);

    {
        unsigned i;
        unsigned n = 0;

        for (i = 0; i < convexBoxMODELS; i++)
        {
            const MdtBodyID b = MdtBodyCreate(world);
            const McdModelID m = McdModelCreate(convexBoxGeometry);

            convexBodyInit(b,n++);

            McdSpaceInsertModel(space,m);
            McdModelSetBody(m,b);

            convexBoxBody[i] = b;
        }

        for (i = 0; i < convexConeMODELS; i++)
        {
            const MdtBodyID b = MdtBodyCreate(world);
            const McdModelID m = McdModelCreate(convexConeGeometry);

            convexBodyInit(b,n++);

            McdSpaceInsertModel(space,m);
            McdModelSetBody(m,b);

            convexConeBody[i] = b;
        }

        for (i = 0; i < convexSliceMODELS; i++)
        {
            const MdtBodyID b = MdtBodyCreate(world);
            const McdModelID m = McdModelCreate(convexSliceGeometry);

            convexBodyInit(b,n++);

            McdSpaceInsertModel(space,m);
            McdModelSetBody(m,b);

            convexSliceBody[i] = b;
            convexSliceModel[i] = m;
        }
    }

    /* Beams */
    {
        unsigned i;

        for (i = 0; i < beamMODELS; i++)
        {
            McdModelID m = McdModelCreate(beamGeometry);

            McdModelSetTransformPtr(m,beamTransform[i]);

            McdSpaceInsertModel(space,m);
            McdSpaceUpdateModel(m);
            McdSpaceFreezeModel(m);

            beamModel[i] = m;
        }
    }

    /* Ground */

    groundModel = McdModelCreate(groundGeometry);

    McdModelSetTransformPtr(groundModel,groundTransform);

    McdSpaceInsertModel(space,groundModel);
    McdSpaceUpdateModel(groundModel);
    McdSpaceFreezeModel(groundModel);

    /*
      We have all the geometries and the models, optimize the space
    */

    McdSpaceBuild(space);

    McdFrameworkGetDefaultRequestPtr(framework)->contactMaxCount = 4;

    {
        /* set parameters for contacts */

        MdtContactParamsID props = MstBridgeGetContactParams(bridge,
            MstBridgeGetDefaultMaterial(),
            MstBridgeGetDefaultMaterial());

        MdtContactParamsSetType(props,MdtContactTypeFriction2D);
        MdtContactParamsSetFriction(props,(MeReal) 0.75);
        MdtContactParamsSetRestitution(props,(MeReal) 0.15);
        MdtContactParamsSetSoftness(props,(MeReal) 0.001);
    }

    /*
      Create the corresponding graphics objects
    */

    /* Can't loop here as the colours are different */

    convexBoxGraphic[0] = RGraphicConvexCreate(rc,
        (McdConvexMeshID) convexBoxGeometry,
        colorReddish,MdtBodyGetTransformPtr(convexBoxBody[0]));
    convexBoxGraphic[1] = RGraphicConvexCreate(rc,
        (McdConvexMeshID) convexBoxGeometry,
        colorGreenish,MdtBodyGetTransformPtr(convexBoxBody[1]));

    convexConeGraphic[0] = RGraphicConvexCreate(rc,
        (McdConvexMeshID) convexConeGeometry,
        colorBlueish,MdtBodyGetTransformPtr(convexConeBody[0]));
    convexConeGraphic[1] = RGraphicConvexCreate(rc,
        (McdConvexMeshID) convexConeGeometry,
        colorReddish,MdtBodyGetTransformPtr(convexConeBody[1]));

    convexSliceGraphic[0] = RGraphicConvexCreate(rc,
        (McdConvexMeshID) convexSliceGeometry,
        colorGreenish,MdtBodyGetTransformPtr(convexSliceBody[0]));
    convexSliceGraphic[1] = RGraphicConvexCreate(rc,
        (McdConvexMeshID) convexSliceGeometry,
        colorBlueish,MdtBodyGetTransformPtr(convexSliceBody[1]));

    convexBoxGraphic[0]->m_pObject->m_bIsWireFrame      = 0;
    convexBoxGraphic[1]->m_pObject->m_bIsWireFrame      = 0;
    convexConeGraphic[0]->m_pObject->m_bIsWireFrame     = 0;
    convexConeGraphic[1]->m_pObject->m_bIsWireFrame     = 0;
    convexSliceGraphic[0]->m_pObject->m_bIsWireFrame    = 0;
    convexSliceGraphic[1]->m_pObject->m_bIsWireFrame    = 0;

    {
        unsigned i;

        for (i = 0; i < beamMODELS; i++)
        {
            RGraphic *const g = RGraphicBoxCreate(rc,
                beamSizes[0],beamSizes[1],beamSizes[2],
                colorWhite,beamTransform[i]);

            RGraphicSetTexture(rc,g,"wood");

            beamGraphic[i] = g;
        }
    }

    groundGraphic = RGraphicGroundPlaneCreate(rc,
        30,30,colorWhite,-beamH*.5f);

    RGraphicSetTexture(rc,groundGraphic,"checkerboard");

    /*
        Initialize the camera
    */

    RCameraRotateAngle(rc,0.6f);
    RCameraRotateElevation(rc,0.4f);
    RCameraZoom(rc,1.6f);
    RCameraUpdate(rc);

    /*
        Initialize the keyboard
    */
/*
    RRenderSetLeft2CallBack(rc,toggleAutoEvolve,0);
    RRenderSetDown2CallBack(rc,toggleAutoFall,0);
  */  
    
    RRenderSetActionNCallBack(rc, 4, toggleAutoEvolve, 0);
    RRenderSetActionNCallBack(rc, 5, toggleAutoFall, 0);


#ifndef PS2
    atexit(cleanup);
#endif

    RPerformanceBarCreate(rc);
    RRenderSetWindowTitle(rc,"ConvexStairs example");

    RRenderCreateUserHelp(rc,help,2);
    RRenderToggleUserHelp(rc);

    app = MeAppCreate(world,space,rc);

    /* Run the Simulation. */
    RRun(rc,tick,0);

    return 0;
}
