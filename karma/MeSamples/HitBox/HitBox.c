/* -*- mode: C; -*- */

/*
  Copyright (c) 1997-2002 MathEngine PLC

  $Name: t-stevet-RWSpre-030110 $

  Date: $Date: 2002/04/17 19:28:03 $ - Revision: $Revision: 1.19.2.12 $

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
    A cubical box confinement volume is created out of six planes with
    their normals pointing inside.

    Various type of shapes, mostly convex, are dropped into the box, and
    they end up behaving like a ``gas''.

    The purpose is to generate a lot of collision events, mostly in order
    to benchmark the collision engine.

    Some of the code is reused from 'ConvexStairs.c' and 'ConvexPrims.c'.
*/

#include <stdlib.h>
#include <time.h>

#if defined WIN32 && defined _DEBUG
#   include <crtdbg.h>
#endif
#include <MeAssert.h>
#include <MePrecision.h>
#include <MeMath.h>
#include <MeMisc.h>

#include <MeProfile.h>
#include <McdProfile.h>

#include <MeViewer.h>
#include <RConvex.h>

#include <Mcd.h>
#include <Mdt.h>
#include <Mst.h>
#include <MeApp.h>

#ifdef PS2
#  define solverMATRIXCLAMP (36)
#endif

#define dbgPRINTCONTACTS    0

#include "HitBox.h"

/*
    TUNABLES
*/

/* more than 200 shapes causes trouble with PS2GL */
/* more than 500 shapes does not fit in PS2 Kea */

#define shapeNO         (200)
#define sideSIZE        (20.00f)

#define convexBoxREALLY 1

#define shapeMAX        (10+(unsigned) shapeNO)
#define pairMAX         (shapeMAX*8)
#define constraintMAX   (shapeMAX*10)

#define sideHALF        (sideSIZE/2.00f)
#define sideQUART       (sideSIZE/4.00f)

static const unsigned   debugLevel      = 4;

static const unsigned   cullShapeShape  = 0;
#define pairCULLER      1
#define contactCULLER   0

static struct shapeType shapeType[shapeTypesCount] =
{
    /*  k                   t                   count             v   */
#if 1
    {   shapeDynamic,       shapeBox,           shapeNO* 15 /100, 0   },
    {   shapeDynamic,       shapeBall,          shapeNO* 20 /100, 0   },
    {   shapeDynamic,       shapePipe,          shapeNO* 15 /100, 0   },
    {   shapeDynamic,       shapeConvexBox,     shapeNO* 15 /100, 0   },
    {   shapeDynamic,       shapeConvexCone,    shapeNO* 15 /100, 0   },
    {   shapeDynamic,       shapeConvexSlice,   shapeNO* 15 /100, 0   },
#else
#   if 1
    {   shapeDynamic,       shapeBox,           0, 0 },
    {   shapeDynamic,       shapePipe,          0, 0 },
    {   shapeDynamic,       shapeBall,          0, 0 },
    {   shapeDynamic,       shapeConvexBox,     0, 0 },
    {   shapeDynamic,       shapeConvexCone,    40, 0 },
    {   shapeDynamic,       shapeConvexSlice,   40, 0 },
#   else
    {   shapeDynamic,       shapeBox,           shapeNO* 90 /100, 0   },
    {   shapeDynamic,       shapePipe,          shapeNO*  5 /100, 0   },
    {   shapeDynamic,       shapeBall,          shapeNO*  5 /100, 0   },
    {   shapeDynamic,       shapeConvexBox,     shapeNO*  0 /100, 0   },
    {   shapeDynamic,       shapeConvexCone,    shapeNO*  0 /100, 0   },
    {   shapeDynamic,       shapeConvexSlice,   shapeNO*  0 /100, 0   },
#   endif
#endif
    {   shapeFixed,         shapeSide,          0,              0   },
    {   shapeFixed,         shapeSideOL,        sidesCount,     0   },
    {   shapeFixed,         shapeGround,        0,              0   },
    {   shapeVisual,        shapeAxis,          0,              0   }
};

static const unsigned   randomSeed      = 42;

static const MeReal     step            = 0.02f;

static const MeReal     gravityY        = 0.02f;

static const MeReal     defMass[2]          = {  4.50f, 19.50f };

static const MeReal     defDampLinear[2]    = {  0.01f,  0.05f };
static const MeReal     defDampAngular[2]   = {  0.01f,  0.05f };

static const MeReal     defVelLinearM[2]    = { -22.80f,  22.80f };
static const MeReal     defVelLinearS[2]    = {  -9.10f,   9.10f };

static const MeReal     defVelAngularM[2]   = { -12.10f,  12.10f };
static const MeReal     defVelAngularS[2]   = {  -2.10f,   2.10f };

static const MeReal     defFriction     = 0.001f;
static const MeReal     defRestitution  = 1.000f;
static const MeReal     defSoftness     = 0.001f;

static const float      volumeGap       = 0.00f;

#define boxSIZE1        1.00f
#define boxSIZE2        1.50f
#define boxSIZE3        1.20f

static const MeVector3  boxSizes        = { boxSIZE1, boxSIZE2, boxSIZE3 };
static const MeReal     ballRadius      = 0.70f;
static const MeReal     pipeRadius      = 0.60f;
static const MeReal     pipeLength      = 1.30f;

static unsigned         tickReport      = 0;
static unsigned         tickLimit       = 0;

/*
    CONSTANTS
*/

static MeALIGNDATA(const MeMatrix4,identityTM,MeALIGNTO) =
{
    {   1.0f,   0.0f,   0.0f,   0.0f    },
    {   0.0f,   1.0f,   0.0f,   0.0f    },
    {   0.0f,   0.0f,   1.0f,   0.0f    },

    {   0.0f,   0.0f,   0.0f,   1.0f    },
};

/*
    90d rotation about the X axis, and half a unit high on the Y.
    Because of arcane reasons related to C++ and 'Mcd' internals
    it cannot be 'const'.
*/
static MeALIGNDATA(MeMatrix4,groundTM,MeALIGNTO)  =
{
    {   1.0f,   0.0f,   0.0f,   0.0f    },
    {   0.0f,   1.0f,  -1.0f,   0.0f    },
    {   0.0f,   1.0f,   0.0f,   0.0f    },

    {   0.0f,   0.0f,   0.0f,   1.0f    },
};

/* This ought to be const, but we actually set it up later */
static MeALIGNDATA(MeMatrix4,sideTM[sidesCount],MeALIGNTO);

static const MeVector3  sideBL              = { -sideHALF, -sideHALF, 0.0f };
static const MeVector3  sideBR              = {  sideHALF, -sideHALF, 0.0f };
static const MeVector3  sideTL              = { -sideHALF,  sideHALF, 0.0f };
static const MeVector3  sideTR              = {  sideHALF,  sideHALF, 0.0f };

static const float      colorReddish[4]     = { 0.6f,   0,      0.4f,   1   };
static const float      colorGreenish[4]    = { 0.4f,   0.6f,   0,      1   };
static const float      colorBlueish[4]     = { 0,      0.4f,   0.6f,   1   };
static const float      colorWhite[4]       = { 1,      1,      1,      1   };

static const float      colorSix[6][4] =
{
    /*  R       G       B               A       */
    {   1.0f,   0.0f,   0.0f,           1.0f    },      /* red          */
    {   0.0f,   1.0f,   0.0f,           1.0f    },      /* green        */
    {   0.0f,   0.0f,   1.0f,           1.0f    },      /* blue         */
    {   0.8f,   0.8f,   0.0f,           1.0f    },      /* yellow       */
    {   0.8f,   0.0f,   0.8f,           1.0f    },      /* magenta      */
    {   0.0f,   0.8f,   0.8f,           1.0f    }       /* cyan         */
};

/* parameters used in constructing convex objects */
#define ROOT2           (1.414213562f)

#define _R1             (0.45f)
#define _R2             (0.35f)
#define _R2a            (_R2*1.35f)

#define _L2             (_R2*(ROOT2+1.0f))
#define _L2a            (_L2*1.35f )

#define _O              (_R2*0.5f)

#if convexBoxREALLY

#   define _S1          (boxSIZE1/2.0f)
#   define _S2          (boxSIZE2/2.0f)
#   define _S3          (boxSIZE3/2.0f)

#   define convexBoxVERTICES 8
    static MeALIGNDATA(const MeVector3,convexBox[convexBoxVERTICES],MeALIGNTO)
        =
    {
        {    _S1,            _S2,            _S3    },
        {    _S1,           -_S2,            _S3    },
        {   -_S1,            _S2,            _S3    },
        {   -_S1,           -_S2,            _S3    },

        /* assuming dimension 3 is height, this is the lower face */

        {    _S1,            _S2,           -_S3    },
        {    _S1,           -_S2,           -_S3    },
        {   -_S1,            _S2,           -_S3    },
        {   -_S1,           -_S2,           -_S3    }
    };
#else
#   define convexBoxVERTICES 12

    static MeALIGNDATA(const MeVector3,convexBox[convexBoxVERTICES],MeALIGNTO)
    {
        {   _R1,            _R1*2,          _R1     },
        {   -_R1,           _R1*2,          _R1     },
        {   -_R1,           _R1*2,          -_R1    },
        {   _R1,            _R1*2,          -_R1    },
        {   _R1*2,          0.0f,            _R1*2  },
        {   -_R1*2,         0.0f,           _R1*2   },
        {   -_R1*2,         0.0f,           -_R1*2  },
        {   _R1*2,          0.0f,           -_R1*2  },
        {   _R1,            -_R1*2,         _R1     },
        {   -_R1,           -_R1*2,         _R1     },
        {   -_R1,           -_R1*2,         -_R1    },
        {   _R1,            -_R1*2,         -_R1    }
    };
#endif

#define convexConeVERTICES      17

static MeALIGNDATA(const MeVector3,convexCone[convexConeVERTICES],MeALIGNTO)
    =
{
    {   _L2,            _O+_R2,         _R2     },
    {   _R2,            _O+_R2,         _L2     },
    {   -_R2,           _O+_R2,         _L2     },
    {   -_L2,           _O+_R2,         _R2     },
    {   -_L2,           _O+_R2,         -_R2    },
    {   -_R2,           _O+_R2,         -_L2    },
    {   _R2,            _O+_R2,         -_L2    },
    {   _L2,            _O+_R2,         -_R2    },
    {   _L2a,           _O+0.0f,        _R2a    },
    {   _R2a,           _O+0.0f,        _L2a    },
    {   -_R2a,          _O+0.0f,        _L2a    },
    {   -_L2a,          _O+0.0f,        _R2a    },
    {   -_L2a,          _O+0.0f,        -_R2a   },
    {   -_R2a,          _O+0.0f,        -_L2a   },
    {   _R2a,           _O+0.0f,        -_L2a   },
    {   _L2a,           _O+0.0f,        -_R2a   },
    {   0.0f,           _O+-_R2*4,      0.0f    }
};

#define convexSliceVERTICES     16

static MeALIGNDATA(const MeVector3,convexSlice[convexSliceVERTICES],MeALIGNTO)
    =
{
    {   _L2,            _R2,            _R2     },
    {   _R2,            _R2,            _L2     },
    {   -_R2,           _R2,            _L2     },
    {   -_L2,           _R2,            _R2     },
    {   -_L2,           _R2,            -_R2    },
    {   -_R2,           _R2,            -_L2    },
    {   _R2,            _R2,            -_L2    },
    {   _L2,            _R2,            -_R2    },
    {   _L2a,           -_R2,           _R2a    },
    {   _R2a,           -_R2,           _L2a    },
    {   -_R2a,          -_R2,           _L2a    },
    {   -_L2a,          -_R2,           _R2a    },
    {   -_L2a,          -_R2,           -_R2a   },
    {   -_R2a,          -_R2,           -_L2a   },
    {   _R2a,           -_R2,           -_L2a   },
    {   _L2a,           -_R2,           -_R2a   }
};

#undef _R1
#undef _R2
#undef _R2a

#undef _L2
#undef _L2a

#undef _O

static const char *const help[4] =
{
    "$LEFT2 - toggle stepping",
    "$DOWN2 - single stepping"
};

/*
    STATE
*/

static McdGeometryID    geometryBox;
static McdGeometryID    geometryBall;
static McdGeometryID    geometryPipe;

static McdGeometryID    geometryConvexBox;
static McdGeometryID    geometryConvexCone;
static McdGeometryID    geometryConvexSlice;

static McdGeometryID    geometryGround;
static McdGeometryID    geometrySide;
static McdGeometryID    geometrySideOL;

static unsigned         shapeCount = 0;
static struct shapeAny  shape[shapeMAX];

static RRender          *rc;
static MeApp            *app;
static McdFrameworkID   framework;
static MstBridgeID      bridge;
static MdtWorldID       world;
static McdSpaceID       space;

static unsigned         tickCount = 0;
static unsigned         tickStop = 0;

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

#define randomMULTIPLIER        (48271)
#define randomMODULUS           (2147483647)

static unsigned randomNow;

static void randomInit(unsigned origin)
{
    if (origin == 0)
        origin = randomSeed;

    randomNow = origin;
}

static MeReal randomNext(const MeReal start,const MeReal end)
{
    randomNow = (randomNow*randomMULTIPLIER) % randomMODULUS;

    {
        static unsigned i;
        if (i < 10000)
        {
            MeDebug(64,"randomNow %u: %u",i,randomNow);
            i++;
        }
    }

    return start + ((end - start) * randomNow) / (MeReal) (randomMODULUS-1);
}

static void debugTM(const unsigned level,const char *const s,
    const MeMatrix4Ptr m)
{
    MeDebug(level,"%s"
        " T [%8.5f %8.5f %8.5f]"
        " R [%8.5f %8.5f %8.5f"
        "|%8.5f %8.5f %8.5f"
        "|%8.5f %8.5f %8.5f]",
        s,
        m[3][0],m[3][1],m[3][2],
        m[0][0],m[0][1],m[0][2],
        m[1][0],m[1][1],m[1][2],
        m[2][0],m[2][1],m[2][2]);
}

#if 0
static void debugMeMatrix4(const unsigned level,const char *const s,
    const MeMatrix4Ptr m)
{
    MeDebug(level,"%s\n"
        "  [%8.5f %8.5f %8.5f %8.5f]\n"
        "  [%8.5f %8.5f %8.5f %8.5f]\n"
        "  [%8.5f %8.5f %8.5f %8.5f]\n"
        "  [%8.5f %8.5f %8.5f %8.5f]",
        s,
        m[0][0],m[0][1],m[0][2],m[0][3],
        m[1][0],m[1][1],m[1][2],m[1][3],
        m[2][0],m[2][1],m[2][2],m[2][3],
        m[3][0],m[3][1],m[3][2],m[3][3]);
}
#endif

/*
    From <URL:http://WWW.SRL.GATech.edu/education/ME6175/notes/xforms.html>

    Z:      cos     -sin    0
            sin     cos     0
            0       0       1

    Y:      cos     0       sin
            0       1       0
            -sin    0       cos

    X:      1       0       0
            0       cos     -sin
            0       sin     cos
*/

static void rotateAroundZ(MeMatrix4Ptr tm,const MeReal s,const MeReal c)
{
    tm[0][0] = tm[1][1] = c;
    tm[0][1] = -(tm[1][0] = s);
}

static void rotateAroundY(MeMatrix4Ptr tm,const MeReal s,const MeReal c)
{
    tm[0][0] = tm[2][2]= c;
    tm[0][2] = -(tm[2][0] = -s);
}

static void rotateAroundX(MeMatrix4Ptr tm,const MeReal s,const MeReal c)
{
    tm[1][1] = tm[2][2] = c;
    tm[1][2] = -(tm[2][1] = s);
}

static void MEAPI toggleStepping(RRender *rc,void *userData)
{
    tickStop = (tickCount < tickStop) ? tickCount : tickLimit;
}

static void MEAPI singleStepping(RRender *rc,void *userData)
{
    tickStop = tickCount+1;
}

#if (contactCULLER)
static MeBool MEAPI contactCuller(McdIntersectResult *result,
    McdContact *const colC,MdtContactID dynC)
{
    const McdModelPairID pair = result->pair;
    const McdGeometryID g1 = McdModelGetGeometry(pair->model1);
    const McdGeometryID g2 = McdModelGetGeometry(pair->model2);

    MEASSERT(geometryGround == geometrySide);
    MEASSERT(geometrySide == geometrySideOL);

    return g1 == geometrySide || g2 == geometrySide;
}
#endif

#if (pairCULLER)
static MeBool MEAPI pairCuller(McdIntersectResult *result,
    MdtContactGroupID group)
{
    const McdModelPairID pair = result->pair;
    const McdGeometryID g1 = McdModelGetGeometry(pair->model1);
    const McdGeometryID g2 = McdModelGetGeometry(pair->model2);

    MEASSERT(geometryGround == geometrySide);
    MEASSERT(geometrySide == geometrySideOL);

    return g1 == geometrySide || g2 == geometrySide;
}
#endif

#if (dbgPRINTCONTACTS)
static MeBool MEAPI contactPrinter(McdIntersectResult *result,
    McdContact *const colC,MdtContactID dynC)
{
    const McdModelPairID pair = result->pair;
    const McdGeometryID g1 = McdModelGetGeometry(pair->model1);
    const McdGeometryID g2 = McdModelGetGeometry(pair->model2);

    if (g1 == geometryBox && g2 == geometryBox)
    {
        const McdContact *const c = result->contacts;
        const unsigned n = result->contactCount;

        MeDebug(0,"\ncontact: count %u, touch %u"
            ", normal [%6.2f,%6.2f,%6.2f]",
            n,result->touch,
            result->normal[0],result->normal[1],result->normal[2]);

        {
            unsigned i;

            for (i = 0; i < n; i++)
            {
                const McdContact *const k = &c[i];

                MeDebug(0," %-2u: sep. %8.5f, dims %u"
                    ", pos. [%8.5f,%8.5f,%8.5f]"
                    ", normal [%8.5f,%8.5f,%8.5f]",
                    i,k->separation,k->dims,
                    k->position[0],k->position[1],k->position[2],
                    k->normal[0],k->normal[1],k->normal[2]);
            }
        }
    }

    return 1;
}
#endif

static void MEAPI tick(RRender *rc,void *useData)
{
    const unsigned n = tickCount++;

    if (tickLimit > 0)
    {
        if (n >= tickLimit)
            exit(0);

        if (n >= tickStop)
            return;
    }

    MeDebug(32,"-------------------------------- %3u",
        tickCount-1);

    MeAppStep(app);

    MeProfileStartSection("Collision",0);
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge,space,world);
    MeProfileEndSection("Collision");

    MeProfileStartSection("Dynamics",0);
    MdtWorldStep(world,step);
    MeProfileEndSection("Dynamics");

    if (tickReport != 0 && (n % tickReport) == 0)
    {
        unsigned i;

        for (i = 0; i < shapeCount; i++)
        {
            const struct shapeAny *const s = &shape[i];
            char f[256];

            if (s->k != shapeDynamic)
                return;

            sprintf(f,"tick %3u: shape %3u, kind %3u, type %3u:",
                n,i,s->k,s->t);
            debugTM(1,f,McdModelGetTransformPtr(s->u.d.m));
        }
    }
}

#ifndef PS2
static void MEAPI_CDECL cleanup (void)
{
    McdFrameworkDestroyAllModelsAndGeometries(framework);
    MstBridgeDestroy(bridge);

    McdSpaceDestroy(space);
    McdTerm(framework);
    MdtWorldDestroy(world);

    MeAppDestroy(app);

    RRenderContextDestroy(rc);
}
#endif

int main(int argc,const char **argv)
{
    const MstMaterialID defMaterial = MstBridgeGetDefaultMaterial();
    const MeReal        side40Pc = (MeReal) (sideSIZE*40.0f)/100.0f;

    unsigned            n;
    unsigned            i,j;

#if 0
    extern void MeMemoryInitNGC(void);
    MeMemoryInitNGC();
#endif

    MeDebugLevel = debugLevel;

    randomInit(randomSeed);
    tickStop = tickLimit;

#if defined WIN32 && defined _DEBUG
    {
        int debugFlag;

        debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        debugFlag |= _CRTDBG_ALLOC_MEM_DF;
        debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
        debugFlag |= _CRTDBG_LEAK_CHECK_DF;

#if 0
        _CrtSetDbgFlag(debugFlag);
#endif
    }
#   endif

    /*
        Initialize dynamics
    */

    MeDebug(64,"main: sizeof (MeReal) %u",sizeof (MeReal));

    MeDebug(1,"main: creating world with %u shapes, %u constraints",
        shapeMAX,constraintMAX);

    world = MdtWorldCreate(shapeMAX,constraintMAX, 1, 1);
    MdtWorldSetGravity(world,0.0f,-gravityY,0.0f);

#ifdef PS2
    MdtWorldSetMaxMatrixSize(world,solverMATRIXCLAMP);
#endif

    /*
        Initialize the collision subsystem
    */

    MeDebug(1,"main: init collision with"
        " %u interactions, %u models, %u composites",0,shapeMAX,0);

    /* # geoTypes = #primitives + convex */
    framework = McdInit(0,shapeMAX,0,1);

    McdPrimitivesRegisterTypes(framework);
    McdConvexMeshRegisterType(framework);

    McdPrimitivesRegisterInteractions(framework);
    McdConvexMeshPrimitivesRegisterInteractions(framework);

    MeDebug(1,"main: farfield axissort with"
        " %u objects, %u pairs, %u groups",shapeMAX,pairMAX,1);

    space = McdSpaceAxisSortCreate(framework,McdAllAxes,shapeMAX,pairMAX);

    /*
        Set up the bridge
    */

    bridge = MstBridgeCreate(framework,shapeMAX);
    MstSetWorldHandlers(world);

    MeDebug(32,"main: %s","command line processing");

    {
        MeCommandLineOptions *const options
            = MeCommandLineOptionsCreate(argc,argv);

        rc = RRenderContextCreate(options,0,!MEFALSE);

        MeCommandLineOptionsDestroy(options);
    }

    if (rc == 0)
        return 1;

    /*
        Create the geometries; we only do one geometry for each type of
        shape, for simplicity.
    */

    geometryBox         = (McdGeometryID)
        McdBoxCreate(framework,
            boxSizes[axisX],boxSizes[axisY],boxSizes[axisZ]);
    geometryBall        = (McdGeometryID)
        McdSphereCreate(framework,ballRadius);
    geometryPipe        = (McdGeometryID)
        McdCylinderCreate(framework,pipeRadius,pipeLength);

    geometryConvexBox   = (McdGeometryID)
        McdConvexMeshCreateHull(framework,convexBox,convexBoxVERTICES,0);
    geometryConvexCone  = (McdGeometryID)
        McdConvexMeshCreateHull(framework,convexCone,convexConeVERTICES,0);
    geometryConvexSlice = (McdGeometryID)
        McdConvexMeshCreateHull(framework,convexSlice,convexSliceVERTICES,0);

    geometryGround      = McdPlaneCreate(framework);
    geometrySide        = geometryGround;
    geometrySideOL      = geometryGround;

    MeDebug(16,"main: geometries created:\n"
        "  box 0x%08x, ball 0x%08x, pipe 0x%08x\n"
        "  convexBox 0x%08x, convexCone 0x%08x, convexSlice 0x%08x\n"
        "  ground 0x%08x, side 0x%08x, sideOL 0x%08x",
        geometryBox,geometryBall,geometryPipe,
        geometryConvexBox,geometryConvexCone,geometryConvexSlice,
        geometryGround,geometrySide,geometrySideOL
    );

    /*
        Geometries done, now create the models that use them and the
        bodies that give dynamical properties to those models.
    */

    for (i = 0; i < sidesCount; i++)
        memcpy(sideTM[i],identityTM,sizeof sideTM[i]);

    /*
        We need to create the six planes for the confinement region.
        The render's height is Y, and depth is Z; collision's are
        viceversa.

        Planes when created lie on the collision X-Y plane.

        Thus we need:

        top:    translated by 'volumeGap+sideSIZE' on the Y axis,
                rotated -90d around any the X axis.

        base:   translated by 'volumeGap' on the Y axis
                rotate 90d on the X axis

        front:  non rotated
                translated -'sideHALF' on the Z axis.

        left:   rotated 90d around the Z axis,
                translated -'sideHALF' on the X axis.

        back:   rotated 180d around the X axis,
                translated 'sideHALF' on the Z axis.

        right:  rotated -90d around the Z axis,
                translated 'sideHALF' on the X axis.
    */

    /* rotations */

    /*
        sin(180d) = 0   cos(180d) = -1
        sin(90d)  = 1   cos(90d)  = 0
        sin(-90d) = -1  cos(-90d) = 0
    */

    rotateAroundX(sideTM[sideTop],   -1,0);
    rotateAroundX(sideTM[sideBase],  1,0);

    rotateAroundZ(sideTM[sideFront], 0,1);
    rotateAroundY(sideTM[sideLeft],  -1,0);

    rotateAroundX(sideTM[sideBack],  0,-1);
    rotateAroundY(sideTM[sideRight], 1,0);

    /* translations */

    sideTM[sideTop]  [tmTranslation][axisY]  = volumeGap+sideHALF;
    sideTM[sideBase] [tmTranslation][axisY]  = volumeGap-sideHALF;
                                                  
    sideTM[sideFront][tmTranslation][axisZ]  = -sideHALF;
    sideTM[sideLeft] [tmTranslation][axisX]  = -sideHALF;
                                                  
    sideTM[sideBack] [tmTranslation][axisZ]  = sideHALF;
    sideTM[sideRight][tmTranslation][axisX]  = sideHALF;

    for (i = 0; i < sidesCount; i++)
    {
        char f[128];

        sprintf(f,"main: side %3u:",i);
        debugTM(32,f,sideTM[i]);
    }
 
    for (i = 0, n = 0;
         i < shapeTypesCount && n < shapeMAX;
         i++)
    {
        struct shapeType *const         t = &shapeType[i];

        t->v = &shape[n];

        MeDebug(32,"main: type %3u at 0x%08x"
            ", kind %3u, n %3u, v 0x%08x",i,t,t->k,n,t->v);

        for (j = 0, n;
             j < t->count && n < shapeMAX;
             j++, n++)
        {
            struct shapeAny *const      s = &t->v[j];

            s->k = t->k;
            s->t = t->t;

            MeDebug(32,"main: type %3u shape %3u 0x%08x"
                ", %3u'th of kind %3u type %3u",i,n,s,j,s->k,s->t);

            switch (s->k)
            {
            case shapeDynamic:  
            {
                const MdtBodyID b = MdtBodyCreate(world);
                McdGeometryID   g;
                McdModelID      m;

                const MeReal    mass = randomNext(defMass[0],defMass[1]);
                const MeReal    dampLinear = randomNext(
                                    defDampLinear[0],defDampLinear[1]);
                const MeReal    dampAngular = randomNext(
                                    defDampAngular[0],defDampAngular[1]);

                MeReal          posX,posY,posZ,
                                    lVelX,lVelY,lVelZ,
                                    aVelX,aVelY,aVelZ;

                posX    = randomNext(-side40Pc,side40Pc);
                posY    = randomNext(-side40Pc,side40Pc);
                posZ    = randomNext(-side40Pc,side40Pc);

                lVelX   = randomNext(defVelLinearM[0],defVelLinearM[1]);
                lVelY   = randomNext(defVelLinearM[0],defVelLinearM[1]);
                lVelZ   = randomNext(defVelLinearM[0],defVelLinearM[1]);

                aVelX   = randomNext(defVelAngularM[0],defVelAngularM[1]);
                aVelY   = randomNext(defVelAngularS[0],defVelAngularS[1]);
                aVelZ   = randomNext(defVelAngularS[0],defVelAngularS[1]);

                MeDebug(32,
                    "main: type %3u shape %3u 0x%08x initial state:\n"
                    "  mass %5.2f, dampLinear %5.2f, dampAngular %5.2f"
                    ", pos [%5.2f,%5.2f,%5.2f]\n"
                    "  velLinear [%5.2f,%5.2f,%5.2f]"
                    ", velAngular [%5.2f,%5.2f,%5.2f]",
                    i,n,s,
                    mass,dampLinear,dampAngular, posX,posY,posZ,
                    lVelX,lVelY,lVelZ, aVelX,aVelY,aVelZ
                );

                MdtBodySetMass(b,mass);
                MdtBodySetLinearVelocityDamping(b,dampLinear);
                MdtBodySetAngularVelocityDamping(b,dampAngular);

                MdtBodySetPosition(b,posX,posY,posZ);
                MdtBodySetLinearVelocity(b,lVelX,lVelY,lVelY);
                MdtBodySetAngularVelocity(b,aVelX,aVelY,aVelY);

                MdtBodyEnable(b);

                switch (s->t)
                {
                case shapeBox:          g = geometryBox;            break;
                case shapeBall:         g = geometryBall;           break;
                case shapePipe:         g = geometryPipe;           break;

                case shapeConvexBox:    g = geometryConvexBox;      break;
                case shapeConvexCone:   g = geometryConvexCone;     break;
                case shapeConvexSlice:  g = geometryConvexSlice;    break;

                default:
                    MEASSERT(!"known type of dynamic kind");
                }

                m = McdModelCreate(g);

                McdSpaceInsertModel(space,m);
                McdModelSetBody(m,b);

                MeDebug(8,"main: type %3u shape %3u, model 0x%08x"
                    ", geometry 0x%08x, body 0x%08x",i,n,m,g,b);

                s->u.d.g = g;
                s->u.d.m = m;
                s->u.d.b = b;

                s->u.d.r = 0;
                s->u.d.rCount = 0;
                break;
            }

            case shapeFixed:
            {
                McdGeometryID           g;
                McdModelID              m;

                switch (s->t)
                {
                case shapeSideOL:
                    g = geometrySideOL;
                    m = McdModelCreate(g);

                    McdModelSetTransformPtr(m,sideTM[j]);
                    McdSpaceInsertModel(space,m);
                    McdSpaceUpdateModel(m);
                    McdSpaceFreezeModel(m);
                    break;

                case shapeGround:
                    g = geometryGround;
                    m = McdModelCreate(g);

                    McdModelSetTransformPtr(m,groundTM);
                    McdSpaceInsertModel(space,m);
                    McdSpaceUpdateModel(m);
                    McdSpaceFreezeModel(m);
                    break;

                default:
                    MEASSERT(!"known type of kind 'shapeFixed'");
                }

                MeDebug(8,"main: type %3u shape %3u"
                    ", model 0x%08x, geometry 0x%08x",i,n,m,g);

                s->u.f.g = g;
                s->u.f.m = m;

                s->u.f.r = 0;
                s->u.f.rCount = 0;
                break;
            }

            case shapeInvisible:
            {
                McdGeometryID           g;
                McdModelID              m;

                MEASSERT(t->t == shapeSide);

                g = geometrySide;
                m = McdModelCreate(g);

                McdModelSetTransformPtr(m,sideTM[j]);
                McdSpaceInsertModel(space,m);
                McdSpaceUpdateModel(m);
                McdSpaceFreezeModel(m);

                MeDebug(8,"main: type %3u shape %3u"
                    ", model 0x%08x, geometry 0x%08x",i,n,m,g);

                s->u.i.g = g;
                s->u.i.m = m;
                break;
            }

            case shapeVisual:
                s->u.f.r = 0;
                s->u.f.rCount = 0;
                break;

            default:
                MEASSERT(!"known shape kind");
            }
        }
    }

    shapeCount = n;

    /*
        We have all the geometries and the models, optimize the space
    */

    McdSpaceBuild(space);

    McdFrameworkGetDefaultRequestPtr(framework)->contactMaxCount = 4;

    {
        MdtContactParamsID props = MstBridgeGetContactParams(bridge,
            defMaterial,defMaterial);

        MdtContactParamsSetType(props,MdtContactTypeFriction2D);

        MdtContactParamsSetFriction(props,      defFriction);
        MdtContactParamsSetRestitution(props,   defRestitution);
        MdtContactParamsSetSoftness(props,      defSoftness);
    }

    /*
        Create the corresponding graphics objects. Perhaps we could/should
        do this together with model/body creation, but there may be reasons
        why a separate, later, pass is useful.
    */

    /*
        We have the option of keeping all contacts or just those for the
        plane/shape pairs, which we sometimes prefer to make
        benchmarking just the collision bit easier.
    */

#if (dbgPRINTCONTACTS)
    MstBridgeSetPerContactCB(bridge,
        defMaterial,defMaterial,&contactPrinter);
#endif

    if (cullShapeShape)
    {
#       if (contactCULLER)
            MstBridgeSetPerContactCB(bridge,
                defMaterial,defMaterial,&contactCuller);
#       endif
#       if (pairCULLER)
            MstBridgeSetPerPairCB(bridge,
                defMaterial,defMaterial,&pairCuller);
#       endif
    }

    for (i = 0, n = 0;
         i < shapeTypesCount && n < shapeMAX;
         i++)
    {
        const struct shapeType *const t = &shapeType[i];

        for (j = 0, n;
             j < t->count && n < shapeMAX;
             j++, n++)
        {
            struct shapeAny     *const s = &t->v[j];

            unsigned            c;
            RGraphic            **r;

            MEASSERT(s->t == t->t && s->k == t->k);

            switch (t->k)
            {
            case shapeDynamic:
            {
                const MeMatrix4Ptr  tm = McdModelGetTransformPtr(s->u.d.m);
                float               color[4];

#if 0
                memcpy(color,colorSix[i%6],sizeof color);
#else
                {
                    const float         K = (float)
                                            (s-shape) / (float) shapeCount;
                    const MeReal        m = MdtBodyGetMass(s->u.d.b);
                    const float         s = 0.35f + 0.65f*(m-defMass[0])
                                            /(defMass[1]-defMass[0]);
                    MeHSVtoRGB(360.00f * K,0.80f,s,color);
                }
#endif

                c = 1;
                r = (RGraphic **)
                        (*MeMemoryAPI.create)(c*sizeof (RGraphic *));

                switch (t->t)
                {
                case shapeBox:
                    r[0] = RGraphicBoxCreate(rc,
                        boxSizes[axisX],boxSizes[axisY],boxSizes[axisZ],
                        color,tm);
                    break;

                case shapeBall:
                    r[0] = RGraphicSphereCreate(rc,ballRadius,color,tm);
                    break;

                case shapePipe:
                    r[0] = RGraphicCylinderCreate(rc,
                        pipeRadius,pipeLength,color,tm);
                    break;

                case shapeConvexBox:
                case shapeConvexCone:
                case shapeConvexSlice:
                    r[0] = RGraphicConvexCreate(rc,
                        (McdConvexMeshID) s->u.d.g,color,tm);
                    break;

                default:
                    MEASSERT(!"known type of dynamic kind");
                }

                MeDebug(8,"main: type %3u shape %3u"
                    ", kind %3u, graphic 0x%08x[%u]",s->t,i,s->k,r,c);

                s->u.d.r = r;
                s->u.d.rCount = c;
                break;
            }

            case shapeFixed:
                switch (s->t)
                {
                case shapeGround:
                    c = 1;
                    r = (RGraphic **)
                            (*MeMemoryAPI.create)(c*sizeof (RGraphic *));

                    r[0] = RGraphicGroundPlaneCreate(rc,
                        30.0,30,colorWhite,
                        groundTM[tmTranslation][axisY]);

                    RGraphicSetTexture(rc,r[0],"checkerboard");
                    break;

                case shapeSideOL:
                {
                    static const MeReal o[3] = { 0.0f, 0.0f, 0.0f };
                    static const MeReal n[3] = { 0.0f, 0.0f, sideSIZE/12.0f };

                    const MeVector4 *const tm = (const MeVector4 *)
                        McdModelGetTransformPtr(s->u.f.m);

                    if (s->u.f.r == 0)
                    {
                        c = t->count * 4;
                        r = (RGraphic **)
                                (*MeMemoryAPI.create)(c*sizeof (RGraphic *));
                    }

                    {
                        RGraphic            **const rj = r + j*4;

                        rj[0] = (RGraphic *)
                            RNLineCreate(rc,o,n,colorSix[j%6],tm);
                        rj[1] = (RGraphic *)
                            RNLineCreate(rc,sideBL,sideTL,colorWhite,tm);
                        rj[2] = (RGraphic *)
                            RNLineCreate(rc,sideTL,sideTR,colorWhite,tm);
                        rj[3] = (RGraphic *)
                            RNLineCreate(rc,sideBR,sideTR,colorWhite,tm);
                    }
                    break;
                }

                default:
                    MEASSERT(!"known type of kind 'shapeFixed'");
                }

                MeDebug(8,"main: type %3u shape %3u"
                    ", kind %3u, graphic 0x%08x",s->t,i,s->k,r);

                s->u.f.r = r;
                s->u.f.rCount = c;
                break;

            case shapeInvisible:
                MEASSERT(s->t == shapeSide);
                break;

            case shapeVisual:
            {
                static const float      o[3] = { 0.0f, 0.1f, 0.0f };
                static const float      u[6][3] =
                    {
                        {  9.0f,  0.1f,  0.0f },
                        {  0.0f,  9.1f,  0.0f },
                        {  0.0f,  0.1f,  9.0f },
                        { -9.0f,  0.1f,  0.0f },
                        {  0.0f, -9.1f,  0.0f },
                        {  0.0f,  0.1f, -9.0f }
                    };
                
                unsigned                j;

                c = 6;
                r = (RGraphic **)
                        (*MeMemoryAPI.create)(c*sizeof (RGraphic *));
                
                for (j = 0; j < c; j++)
                    r[j] = (RGraphic *) RNLineCreate(rc,
                        o,u[j],colorSix[j],identityTM);

                s->u.v.r = r;
                s->u.v.rCount = c;
                break;
            }

            default:
                MEASSERT(!"known shape kind");
            }
        }
    }

    /*
        Initialize the camera
    */

    RCameraRotateAngle(rc,0.3f);
    RCameraRotateElevation(rc,0.6f);
    RCameraZoom(rc,sideSIZE*1.1f);

    RCameraUpdate(rc);

    /*
        Initialize the keyboard
    */

    RRenderSetLeft2CallBack(rc,toggleStepping,0);
    RRenderSetDown2CallBack(rc,singleStepping,0);

#ifndef PS2
    atexit(cleanup);
#endif

    RPerformanceBarCreate(rc);
    RRenderSetWindowTitle(rc,"HitBox example");

    RRenderCreateUserHelp(rc,help,2);
    RRenderToggleUserHelp(rc);

    app = MeAppCreate(world,space,rc);

    /* Run the Simulation. */

    RRun(rc,tick,0);

    return 0;
}
