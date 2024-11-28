/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:35 $ - Revision: $Revision: 1.29.8.1 $

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

  A simple pendulum, with direct calls to Kea Solver
  This example shows how to write an application that calls the Kea
  Solver directly.

  The trade-off is between optimization of size and speed (using Kea
  only) vs. simplicity and speed of development (using the top level API).

*/

#include <string.h>

#include <MdtKea.h>
#include <MeMath.h>
#include <MeViewer.h>


#define MEM_BLOCK_SIZE      10000
#define CUBOID_LENGTH       3
#define NUM_BODIES          1
#define NUM_CONSTRAINTS     1
#define DOF_CONSTRAINED_BS  3
#define NUM_PADDING_ROWS    4
/*
  Graphic-only definitions:
*/
RRender *rc;

char *help[1] = {"There is no user interaction for this example."};

/*
  Graphic-only object's transformation matrix:
*/
MeMatrix4 pivotTransform =
{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1}
};

void *memory;

/*
  N.B. Renderer colour is never double precision:
*/
const float orange[4] = { 1, 0.4f,   0.0f,   1.0f };
const float blue[4]   = { 0, 0.598f, 0.797f, 1.0f };

RGraphic *r_cuboid = 0;
RGraphic *r_pivot = 0;
RGraphic *groundG= 0;

/*
  Graphic-only object's transformation matrix:
*/
MeMatrix4 groundTransform =
{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, -3, 0, 1}
};

/* Physics object definitions: */

#define TIMESTEP        (MeReal)0.01
#define BODY_MASS       2

const MeReal gravity[3]  = { 0, -10, 0 };
const MeReal velocity[3] = { 0,   0, 1 };

/* Structure for Euler parameters, mass, moment of inertia etc. */
MdtKeaBody Cuboid;

/* Quaternion, moment of Inertia etc. */
MeReal CuboidTransform[16]
#ifdef PS2
__attribute__ ((aligned(16)))
#endif
    = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        1, 0, 1, 1
    }
;

/*
  The joint position in the cuboid reference frame is constrained to be
  constant * by the ball and socket joint:
*/
const MeReal pos1[4] = { -1, 0, 0 };

/*
  The joint position is fixed in the inertial reference frame in this
  single body demo.
*/
const MeReal pos2[4] = { 0, 0, 1 };

#define MdtBclNO_BODY  -1

/*
  Definitions for a single ball and socket joint:

  Almost all these should be 'const'.
*/

#ifndef PS2
/*
  In general, all input to kea should be quadword (16 byte) aligned.
*/

#ifdef __GNUC__
#define QALIGNED __attribute__ ((aligned (16)))

int NumConstraints QALIGNED = (NUM_CONSTRAINTS);
int NumDegreesOfFreedomConstrained QALIGNED = (NUM_CONSTRAINTS * DOF_CONSTRAINED_BS);
int NumRowsIncludingPadding QALIGNED = (NUM_CONSTRAINTS * (DOF_CONSTRAINED_BS + NUM_PADDING_ROWS) );
int JacobianRowOffset QALIGNED = (0);
MeReal WorkVector[4] QALIGNED = { 0 };
MeReal DummyVector[4] QALIGNED = { 0 };

int BodyIndex[2] QALIGNED = { 0, MdtBclNO_BODY };
MeReal ForceUpperLimit[4] QALIGNED
= { MEINFINITY, MEINFINITY, MEINFINITY, 0 };
MeReal ForceLowerLimit[4] QALIGNED
= { -MEINFINITY, -MEINFINITY, -MEINFINITY, 0 };

    /* Read-only */
MeReal ResultantForces[24] QALIGNED;
/* Read-only work vector. */
MeReal Eigenvalues[4] QALIGNED;
MeReal ErrorTerm[4] QALIGNED;

#else
#error "Alignment not yet set up for non __GNUC__ platforms.\nWill someone who knows the syntax do this for Irix, Win32, Intel compilers please!"
#endif /* __GNUC__ */

#else
/*
  On the PS2, all input to kea should be quad-quadword (64 byte) aligned, for faster DMA.
*/

#define QQALIGNED __attribute__ ((aligned (64)))

int NumConstraints QQALIGNED = (NUM_CONSTRAINTS);
int NumDegreesOfFreedomConstrained QQALIGNED = (NUM_CONSTRAINTS * DOF_CONSTRAINED_BS);
int NumRowsIncludingPadding QQALIGNED = (NUM_CONSTRAINTS * (DOF_CONSTRAINED_BS + NUM_PADDING_ROWS) );
int JacobianRowOffset QQALIGNED = (0);
MeReal WorkVector[4] QQALIGNED = { 0 };
MeReal DummyVector[4] QQALIGNED = { 0 };

int BodyIndex[2] QQALIGNED = { 0, MdtBclNO_BODY };
MeReal ForceUpperLimit[4] QQALIGNED
= { MEINFINITY, MEINFINITY, MEINFINITY, 0 };
MeReal ForceLowerLimit[4] QQALIGNED
= { -MEINFINITY, -MEINFINITY, -MEINFINITY, 0 };

/* Read-only */
MeReal ResultantForces[24] QQALIGNED;
/* Read-only work vector. */
MeReal Eigenvalues[4] QQALIGNED;
MeReal ErrorTerm[4] QQALIGNED;
#endif /* PS2 */

/*
  The following allocates space for 3 J matrices.

  This demo shows only a single body constrained by this joint, so only
  the first Jacobian is actually used here.

  Each Jacobian has 6 columns for the velocity factors (velocity x,y,z
  followed by angular velocity x,y,z).

  One degree of freedom is constrained for each non-zero row in the
  Jacobian, so we need only the first three rows in this demo.

  However, Kea expects all memory to be aligned on 4-byte boundaries, so
  we allow space for 4 rows.

  The matrix layout is row-major for each "strip" of 4 rows, but
  column-major within each strip.  See the KEAJ macro below.
*/

/* 8 constraint rows of length 12, so Jacobian size is 12*8 MeReals */

#ifdef PS2
MeReal Jacobian[12*8] __attribute__ ((aligned(16)));
#else
MeReal Jacobian[12*8];
#endif

MdtKeaConstraints BallAndSocketJoint;

#define KEAJ(i,j,k) \
    (Jacobian[ (k)*4 + ((j)/4)*18*4 + (j)%4 + (i)*24 ])

#define EPSILON 0.01f
#define GAMMA 0.2f

MdtKeaParameters EvolutionParams = { TIMESTEP, EPSILON, GAMMA, 0, 0 };


void InitBallAndSocketJoint(void)
{
    memset(&BallAndSocketJoint, 0, sizeof (MdtKeaConstraints));

    /*
      Note that the integer addresses would be arrays if there were
      multiple partitions:
    */
    BallAndSocketJoint.num_partitions = 1;
    BallAndSocketJoint.num_constraints = NumConstraints;
    BallAndSocketJoint.num_constraints_partition = &NumConstraints;
    BallAndSocketJoint.num_rows_exc_padding_partition =
        &NumDegreesOfFreedomConstrained;
    BallAndSocketJoint.num_rows_inc_padding_partition =
        &NumRowsIncludingPadding;
    BallAndSocketJoint.Jsize = &NumDegreesOfFreedomConstrained;
    BallAndSocketJoint.num_rows_exc_padding = NumDegreesOfFreedomConstrained;
    BallAndSocketJoint.num_rows_inc_padding = NumRowsIncludingPadding;
    BallAndSocketJoint.Jofs = &JacobianRowOffset;

    /*
      Input and output solver matrices, with padding for 4-byte
      alignment:
    */
    BallAndSocketJoint.Jbody = BodyIndex;
    /* Used by kea. */
    BallAndSocketJoint.xgamma = WorkVector;

    /* No slipping constraints in this example. */
    BallAndSocketJoint.slipfactor = DummyVector;

    BallAndSocketJoint.lambda = Eigenvalues;
    BallAndSocketJoint.force = ResultantForces;
    BallAndSocketJoint.hi = ForceUpperLimit;
    BallAndSocketJoint.lo = ForceLowerLimit;

    /* No joint actuation or target velocities in this  demo. */
    BallAndSocketJoint.c = DummyVector;

    BallAndSocketJoint.xi = ErrorTerm;
    BallAndSocketJoint.Jstore = Jacobian;
}

/*
  This function assumes that the cuboid may be translated, but is not
  rotated.
*/
void InitCuboid(const MeReal mass, const MeReal MoI[3])
{
    memset(&Cuboid, 0, sizeof (MdtKeaBody));
    Cuboid.invmass = ((MeReal) 1) / mass;

    /* Set the quaternion to the identity quaternion: */
    Cuboid.qrot[0] = 1;

    /* Set the moment of inertia matrix: */
    Cuboid.I0[0] = MoI[0];
    Cuboid.I1[1] = MoI[1];
    Cuboid.I2[2] = MoI[2];

    /* Set the inverted moment of inertia matrix: */
    Cuboid.invI0[0] = 1 / MoI[0];
    Cuboid.invI1[1] = 1 / MoI[1];
    Cuboid.invI2[2] = 1 / MoI[2];

    /* Set the initial velocity of the cuboid:  */
    Cuboid.vel[0] = velocity[0];
    Cuboid.vel[1] = velocity[1];
    Cuboid.vel[2] = velocity[2];
}


/*
  Set +{A} to a 3x3 matrix corresponding to the 3x1 vector +{a}, such that
  m{A*b = a x b} (x is the cross product operator).
*/
void CreateCrossMatrix(const MeReal a[3], MeReal A0[3],
    MeReal A1[3], MeReal A2[3])
{
    A0[0] = 0;
    A0[4] = a[2];
    A0[8] = -a[1];

    A1[0] = -a[2];
    A1[4] = 0;
    A1[8] = a[0];

    A2[0] = a[1];
    A2[4] = -a[0];
    A2[8] = 0;
}

void UpdateBallAndSocketJoint(void)
{
    int i;
    MeReal p[4];

    memset(ResultantForces, 0, 24 * sizeof (MeReal));
    memset(Eigenvalues, 0, 4 * sizeof (MeReal));
    memset(Jacobian, 0, 8 * 12 * sizeof (MeReal));
    memset(WorkVector, 0, 4 * sizeof (MeReal));

    /*
      Kea solves J.v = c for v.  The latter is a 6-dimensional composite
      of velocity (v) and angular velocity (w).

      The vector c is always zero in this demo.

      The J matrix must be set for each call to the solver (MdtKeaStep)
      for the appropriate values to enforce the required constraint.

      For a single-body ball and socket joint, the required condition
      is: $v - p x w = 0$ where $p$ is the joint position relative to
      the constrained body, in inertial reference frame coordinates.
    */

    /*
      Set the left 3x3 block of Jacobian[0] to the 3x3 identity matrix:
    */

    KEAJ(0, 0, 0) = 1;
    KEAJ(0, 1, 1) = 1;
    KEAJ(0, 2, 2) = 1;

    /*
      Calculate current joint position relative to cuboid in inertial
      reference frame:
    */
    MeMatrixMultiply( p, 4,  3,  1,  CuboidTransform,  pos1);

    /*
      Set the right 3x3 block of Jacobian[0] to the 3x3 cross-product
      matrix of p:
    */
    CreateCrossMatrix(p,
        &KEAJ(0, 0, 3), &KEAJ(0, 1, 3), &KEAJ(0, 2,3));

    /* Compute position error: */
    for (i = 0; i < 3; i++)
        ErrorTerm[i] = (p[i] + CuboidTransform[12 + i]) - pos2[i];
}

/*
  External forces (such as gravity) acting on physical bodies must be
  updated every tick:
*/
void UpdateCuboid(const MeReal mass)
{
    int i = 0;

    for (; i < 3; ++i)
    {
        /* No other external or damping forces */
        Cuboid.force[i] = mass * gravity[i];
        Cuboid.torque[i] = 0;
    }
}


void Tick(RRender * rc, void* userdata)
{
    const MeReal *Force_on_cuboid;
    const MeReal *Torque_on_cuboid;

    InitBallAndSocketJoint();

    UpdateBallAndSocketJoint();
    UpdateCuboid(BODY_MASS);

    MdtKeaAddConstraintForces(
        BallAndSocketJoint
        , &Cuboid
        , (MdtKeaTransformation * const) CuboidTransform
        , NUM_BODIES
        , EvolutionParams);

    MdtKeaIntegrateSystem(
        &Cuboid
        , (MdtKeaTransformation * const) CuboidTransform
        , NUM_BODIES
        , EvolutionParams);

    /*
      Read-only ResultantForces array has now been updated - this gives
      the force on the body due to the constraint.  The force and torque
      properties of the Cuboid structure have also been updated with the
      total force on the body.
    */
    Force_on_cuboid = ResultantForces;  /* x,y,z, padding */
    Torque_on_cuboid = ResultantForces + 4; /* x,y,z, padding */
}

void MEAPI_CDECL cleanup(void)
{
    RRenderContextDestroy(rc);
}

/*
    Entry Point
*/
int MEAPI_CDECL main(int argc, const char **argv)
{
    const MeReal Ratio = (10); /* Length : width ratio  > 1 */
    const MeReal IRSq = (1 / (Ratio * Ratio));
    const AcmeVector3 origin = {0, 0, 0};

    MeReal I[3];

    MeCommandLineOptions* options;

    memory = malloc(MEM_BLOCK_SIZE+16);

    EvolutionParams.memory_pool = (void*)(((int)memory+15)&~15);
    EvolutionParams.memory_pool_size = MEM_BLOCK_SIZE;
    InitBallAndSocketJoint();

    /*
      Set MoI for a solid cuboid w,h = r * x, d = x, where w,h,d are
      half-width, half-height etc.
    */
    I[0] = I[1] = BODY_MASS
        * (1 + IRSq) * CUBOID_LENGTH * CUBOID_LENGTH / 12;
    I[2] = BODY_MASS
        * 2 * IRSq * CUBOID_LENGTH * CUBOID_LENGTH / 12;

    /* Transformation matrix set by direct initialisation. */
    InitCuboid(BODY_MASS, I);

    /*
      Renderer.
    */
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 1;

    RCameraSetLookAt(rc, origin);
    RCameraSetView(rc, 3, 0, 0);

    r_pivot = RGraphicSphereCreate(rc
        ,0.5f * CUBOID_LENGTH / Ratio, orange, 0);
    RGraphicSetTransformPtr(r_pivot, pivotTransform);
    r_cuboid = RGraphicBoxCreate(rc,
        CUBOID_LENGTH / Ratio, CUBOID_LENGTH / Ratio,
        CUBOID_LENGTH, blue, 0);

    /* Warning here since we pass an MdtKea transform pointer but are
       expecting an MeMatrix4 pointer ... */
    RGraphicSetTransformPtr(r_cuboid, CuboidTransform);

    groundG = RGraphicBoxCreate(rc, 7, 0.2f, 7, blue, groundTransform);
    RGraphicSetTexture(rc, groundG, "checkerboard");

    RRenderCreateUserHelp(rc, help,1);  /* press F1 for onscreen user help */
    RRenderSetWindowTitle(rc, "KeaOnly tutorial");

    atexit(cleanup);
    RRun(rc, Tick, 0);

    return 0;
}
