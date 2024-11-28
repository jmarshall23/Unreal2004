/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/29 10:21:38 $ - Revision: $Revision: 1.121.2.16 $

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

#include <stdio.h>

#include <MePrecision.h>
#include <MeAssert.h>
#include <MeMath.h>
#include <MdtTypes.h>
#include <MdtBody.h>

#ifdef _MECHECK
#include <MeMessage.h>
#endif

/*
  Set a = b x c, where a is a pointer to a row in jstore.

  Note that adjacent row elements are 4 elements apart in jstore.
*/
static inline void MdtBclMathCrossToJstore(const MeVector3 b,
    const MeVector3 c, MeReal a[12])
{
    a[0 * 4] = b[1] * c[2] - b[2] * c[1];
    a[1 * 4] = b[2] * c[0] - b[0] * c[2];
    a[2 * 4] = b[0] * c[1] - b[1] * c[0];
}

/*
  Set +{A} to a 3x3 matrix corresponding to the 3x1 vector +{a}, such
  that m{A*b = a x b} (x is the cross product operator).

  The result is multiplied by +{factor}.  There are +{colskip} elements
  between the adjacent columns of +{A}.  If +{A} is stored by rows then
  simply multiply +{factor} by -1.
*/
static inline void MdtBclMathCrossMatrixToJstore(const MeVector3 a,
    const MeReal factor, MeVector3 A0, MeVector3 A1, MeVector3 A2)
{
    A0[0] = 0;
    A0[4] = a[2] * factor;
    A0[8] = -a[1] * factor;

    A1[0] = -a[2] * factor;
    A1[4] = 0;
    A1[8] = a[0] * factor;

    A2[0] = a[1] * factor;
    A2[4] = -a[0] * factor;
    A2[8] = 0;
}

/*
  Internal constraint math function.
*/
static void GetQMatrix_NOT_USED(MeReal Q[4][4], const MeVector4 q)
{
    unsigned int i;
    MeMatrix3CrossFromVector(&Q[1][1], &Q[2][1], &Q[3][1], q, (MeReal)-1);

    Q[0][0] = q[0];
    for (i = 1; i < 4; ++i)
    {
        Q[i][0] = q[i];
        Q[0][i] = -q[i];
        Q[i][i] = q[0];
    }
}

/*
  Internal constraint math function.
*/
static void GetPMatrix_NOT_USED(MeReal P[4][4], const MeVector4 q)
{
    unsigned int i;
    MeMatrix3CrossFromVector(&P[1][1], &P[2][1], &P[3][1], q, (MeReal) 1);

    P[0][0] = q[0];
    for (i = 1; i < 4; ++i)
    {
        P[i][0] = q[i];
        P[0][i] = -q[i];
        P[i][i] = q[0];
    }
}

/*
  Internal constraint math function.
  Constructs the 3x4 matrix E specified in section 6.10 of "Kinematic
  Constraints for Rigid Body Dynamics" by C. Lacoursière.
*/
static void GetEMatrix(MeReal E[3][4], const MeVector4 q)
{
  // first column
  E[0][0] = -q[1];
  E[1][0] = -q[2];
  E[2][0] = -q[3];
  // Upper diagonal
  E[0][1] = E[1][2] = E[2][3] = q[0];
  // Cross matrix
  E[1][3] = -(E[2][2] = q[1]);
  E[2][1] = -(E[0][3] = q[2]);
  E[0][2] = -(E[1][1] = q[3]);
}



/*
  Internal constraint math function.

  Constructs the 3x4 matrix G specified in section 6.10 of "Kinematic
  Constraints for Rigid Body Dynamics" by C. Lacoursière.
*/
static void GetGMatrix(MeReal G[3][4], const MeVector4 q)
{
  /*
    CL thinks that there's been a transpose change in
    MeMatrix3CrossFromVector since Bryan and Mike's code was written.
   */

  /*
    unsigned int i;
    MeMatrix3CrossFromVector(vq, &G[0][1], &G[1][1], &G[2][1], (MeReal)1);

    for (i = 0; i < 3; ++i)
    {

        G[i][0] = -q[i + 1];
        G[i][i + 1] = q[0];
    }
  */
  // first column
  G[0][0] = -q[1];
  G[1][0] = -q[2];
  G[2][0] = -q[3];
  // Upper diagonal
  G[0][1] = G[1][2] = G[2][3] = q[0];
  // Cross matrix
  G[1][3] = -(G[2][2] = -q[1]);
  G[2][1] = -(G[0][3] = -q[2]);
  G[0][2] = -(G[1][1] = -q[3]);
}





static void
MyMultiply_NOT_USED(int p, int q, int r, const MeReal *const A, const MeReal *const B, MeReal *C)
{
    int i,j,k;
    for (i = 0; i < p; ++i) {
        for (j = 0; j < r; ++j) {
            MeReal e = 0;
            for (k = 0; k < q; ++k) {
                e += A[i*q+k]*B[k*r+j];
            }
            C[i*r+j] = e;
        }
    }
}

static void
MyMultiplyT1_NOT_USED(int p, int q, int r, const MeReal *const A, const MeReal *const B, MeReal *C)
{
    int i,j,k;
    for (i = 0; i < p; ++i) {
        for (j = 0; j < r; ++j) {
            MeReal e = 0;
            for (k = 0; k < q; ++k) {
                e += A[k*p+i]*B[k*r+j];
            }
            C[i*r+j] = e;
        }
    }
}

static void
MyMultiplyT2(int p, int q, int r, const MeReal *const A, const MeReal *const B, MeReal *C)
{
    int i,j,k;
    for (i = 0; i < p; ++i) {
        for (j = 0; j < r; ++j) {
            MeReal e = 0;
            for (k = 0; k < q; ++k) {
                e += A[i*q+k]*B[j*q+k];
            }
            C[i*r+j] = e;
        }
    }
}
/**
 * General accessor macro for Jacobians.
 */
//#define KEAJN(i,n,k,J)  ((J)[(k)*4+((n)/4)*12*4+(n)%4+(i)*24])

#define KEAJN(i,n,k,J)  ((J)[n/4][i].col[k][n%4])

/**
 * Accessor macro for Jacobians in ConstraintInfo.
 * 'i'th body [0..2), 'j'th row [0..NUMROWS) and 'k'th element [0..6).
 */
#define KEAJ(i,j,k)     KEAJN((i),(clist->num_rows_inc_padding+(j)), (k), clist->Jstore )

/* Should be defined in "MdtKea.h" ? */
typedef struct
{
    /* Constraint (position) error. */
    MeReal *xi;
    /* RHS (velocity) value in the constraint equation J*v=c. */
    MeReal *c;
    /* Low (force) limit on Lagrange multiplier. */
    MeReal *lo;
    /* High (force) limit on Lagrange multiplier. */
    MeReal *hi;
    /* First order constraint slipping vector. */
    MeReal *slipfactor;
    /* Projection constant. */
    MeReal *xgamma;
}
MdtKeaInputRowFactors;

/**
 * Macro for finding the row to start adding this constraint at.
 */
#define FINDSTARTROW \
    MeReal *c          = (clist->c) + clist->num_rows_exc_padding; \
    MeReal *xi         = (clist->xi) + clist->num_rows_exc_padding; \
    MeReal *lo         = (clist->lo) + clist->num_rows_exc_padding; \
    MeReal *hi         = (clist->hi) + clist->num_rows_exc_padding; \
    MeReal *slipfactor = (clist->slipfactor) + clist->num_rows_exc_padding; \
    MeReal *xgamma     = (clist->xgamma) + clist->num_rows_exc_padding;  \
    MdtKeaInputRowFactors factors; \
    factors.c          = c;  \
    factors.xi         = xi; \
    factors.lo         = lo; \
    factors.hi         = hi; \
    factors.slipfactor = slipfactor; \
    factors.xgamma     = xgamma

/**
 * Macro for adding a constraints bodies to the 'keaConstraints' 'struct'.
 */
#define ADDBODIES(clist, c) \
    ((clist)->Jbody[(clist)->num_constraints][0] = (c)->head.bodyindex[0], \
    (clist)->Jbody[(clist)->num_constraints][1] = (c)->head.bodyindex[1])



/**
 * Initialises the MdtKeaConstraints struct.
 *
 * Call this before starting a partition
 * and adding any constraints.
 */
void MEAPI MdtBclInitConstraintRowList(MdtKeaConstraints *const clist)
{
    clist->num_rows_inc_padding = 0;
    clist->num_rows_exc_padding = 0;
    clist->num_constraints      = 0;
    clist->num_partitions       = 0;
}

/**
 * Ends a partition in the MdtKeaConstraints struct.
 *
 * Call this after adding all constraints, or starting a new partition.
 *
 * Note - Each partition must contain at least 1 constraint row
 */
void MEAPI MdtBclEndPartition(MdtKeaConstraints *const clist)
{

    while(clist->num_rows_exc_padding&3)
    {
        clist->xi[clist->num_rows_exc_padding]=0.0f;
        clist->slipfactor[clist->num_rows_exc_padding]=0.0f;
        clist->c[clist->num_rows_exc_padding]=0.0f;
        clist->xgamma[clist->num_rows_exc_padding]=0.0f;
        clist->num_rows_exc_padding++;
    }
    
    if(clist->num_rows_inc_padding_partition[clist->num_partitions]!=0)
    {


//#ifdef PS2
        if( (clist->num_rows_inc_padding_partition[clist->num_partitions]%4)!=0)
        {
            clist->num_rows_inc_padding = MeMathCEIL4(clist->num_rows_inc_padding-4);
            clist->num_rows_inc_padding_partition[clist->num_partitions] =
                MeMathCEIL4(clist->num_rows_inc_padding_partition[clist->num_partitions]-4);
        }/*
#else
  
        clist->num_rows_inc_padding = MeMathCEIL4(clist->num_rows_inc_padding-4);
        clist->num_rows_inc_padding_partition[clist->num_partitions] =
            MeMathCEIL4(clist->num_rows_inc_padding_partition[clist->num_partitions]-4);
#endif */

    }

    clist->num_partitions++;
}

/**
 * Starts a partition in the MdtKeaConstraints struct.
 *
 * Call this before adding any constraints..
 */
void MEAPI MdtBclStartPartition(MdtKeaConstraints *const clist)
{
    clist->num_rows_exc_padding_partition[clist->num_partitions] = 0;
    clist->num_rows_inc_padding_partition[clist->num_partitions] = 0;
    clist->num_constraints_partition[clist->num_partitions] = 0;
}

MeReal MEAPI MdtBclDotJ(MeReal v[6], MdtKeaConstraints *const clist, int row, int body)
{
    MeReal dot = 0;

    dot += v[0] * KEAJ(body, row, 0);
    dot += v[1] * KEAJ(body, row, 1);
    dot += v[2] * KEAJ(body, row, 2);
    dot += v[3] * KEAJ(body, row, 3);
    dot += v[4] * KEAJ(body, row, 4);
    dot += v[5] * KEAJ(body, row, 5);

    return dot;
}

/**
 * MdtBclEndConstraint is used internally by MdtBcl.
 *
 * It is the last thing called by MdtBclAddBSJoint etc, so you don't
 * need it unless you are writing your own constraints.
 *
 * On entry, the rows of the current constraint have been added to the list
 * 4 rows of padding are added
 * Note that Jsize is set to the number of rows excluding padding
 * Jofs, is the offset of the constraint if all the padding is removed
 * num_rows_partition is the number of rows excluding padding
 *
 * Note that if the constraint is the last in a partition, some of the
 * padding may be removed when MdtBclEndPartition is called
 */
void MEAPI MdtBclEndConstraint(MdtKeaConstraints *const clist,
    const unsigned rows_added)
{
    int i,rows_added_plus_padding;
    if( rows_added > 0 )
    {
        for(i=0;i!=6;i++) { KEAJ(0,rows_added+0,i)=0; KEAJ(1,rows_added+0,i)=0; }
        for(i=0;i!=6;i++) { KEAJ(0,rows_added+1,i)=0; KEAJ(1,rows_added+1,i)=0; }
        for(i=0;i!=6;i++) { KEAJ(0,rows_added+2,i)=0; KEAJ(1,rows_added+2,i)=0; }
        for(i=0;i!=6;i++) { KEAJ(0,rows_added+3,i)=0; KEAJ(1,rows_added+3,i)=0; }

        for(i=0;i!=3;i++) clist->force[clist->num_constraints].primary_body.force[i] = 0;
        for(i=0;i!=3;i++) clist->force[clist->num_constraints].primary_body.torque[i] = 0;
        for(i=0;i!=3;i++) clist->force[clist->num_constraints].secondary_body.force[i] = 0;
        for(i=0;i!=3;i++) clist->force[clist->num_constraints].secondary_body.torque[i] = 0;

//#ifdef PS2
        if( ((clist->num_rows_inc_padding+rows_added)%4) == 0) rows_added_plus_padding = rows_added;
        else                                                   rows_added_plus_padding = rows_added+4;
//#else
//        rows_added_plus_padding = rows_added+4;
//#endif
        clist->num_rows_inc_padding += rows_added_plus_padding;
        clist->num_rows_inc_padding_partition[clist->num_partitions] += rows_added_plus_padding;
        clist->num_rows_exc_padding += rows_added;
        clist->Jsize[clist->num_constraints] = (rows_added);
        clist->Jofs[clist->num_constraints] =
            clist->num_rows_exc_padding_partition[clist->num_partitions];
        clist->num_rows_exc_padding_partition[clist->num_partitions] += (rows_added);
        clist->num_constraints_partition[clist->num_partitions]++;
        clist->num_constraints++;
    }
}

/* Convert the relative constraint transformation in the constraint header
   into the world reference frame. */
static inline void ConvertRefFramesToWorld(const MdtConstraintHeader * const head,
                             const MdtKeaTransformation * const tlist,
                             MeMatrix4 ref1world, MeMatrix4 ref2world)
{
    MeMatrix4MultiplyMatrix( ref1world, head->ref1, *(MeMatrix4*)&tlist[head->bodyindex[0]].R0);

    if(head->bodyindex[1] != MdtBclNO_BODY)
        MeMatrix4MultiplyMatrix( ref2world, head->ref2, *(MeMatrix4*)&tlist[head->bodyindex[1]].R0);
    else
        MeMatrix4Copy( ref2world, head->ref2);
}

/*
  Local service to set the epsilon and gamma position-projection factors
  to correspond to the required spring constant and damping terms.

  Note that this service assumes that stepsize and kp are both positive,
  and kd is non-negative.

  This service returns false if the constraint should be deactivated
  (e.g. if the overshoot has effectively already been corrected) and
  true if the constraint must be maintained.
*/
static bool SetLimitFactors(MdtKeaInputRowFactors * const factors,
    const unsigned int RowIndex, MdtBclLimit * const limit,
    const MdtBclSingleLimit * const sl,
    const MdtBclSolverParameters *const params)
{
    if (sl->stiffness < limit->damping_thresh)
    {
        /*
          The joint will remain beyond this limit for more than three
          steps, so we can try to simulate a soft bounce, using the
          projection parameters to model a damped spring:
        */
        const MeReal hepsilon =
            1 / ((params->stepsize * sl->stiffness) + sl->damping);

        factors->slipfactor[RowIndex] = hepsilon - params->epsilon;
        if(factors->slipfactor[RowIndex] < 0)
        {
#ifdef _MECHECK
            //MeWarning(0, "SetLimitFactors: Negative slipfactor.");
#endif
            factors->slipfactor[RowIndex] = 0;
        }

        if (sl->damping == 0)
            factors->xgamma[RowIndex] = 1 - params->gamma;
        else
            factors->xgamma[RowIndex] =
                (hepsilon * params->stepsize * sl->stiffness) -
                params->gamma;

        factors->xi[RowIndex] = limit->overshoot;
    }
    else
    {
        /*
          ...for a hard bounce, apply restitution by setting a target
          rebound velocity:

          N.B. We do not change the default projection parameters in
          this case.
        */
        const MeReal ReboundVelocity = -sl->restitution * limit->velocity;
        limit->bRelaxingToLimit = (MeFabs( (ReboundVelocity * params->stepsize) /
                                           (params->gamma * limit->overshoot) )  <  1);

        if( limit->velocity * limit->overshoot >= 0 )
        {
            /*
              This is the first step beyond the limit, so reverse the
              velocity:
            */
            factors->c[RowIndex] = ReboundVelocity;
        }
        else if( !limit->bRelaxingToLimit )
        {
            /*
              ...we didn't bounce back inside the limit in a single tick,
              but we're bouncing out anyway, so no need for the constraint.
            */
            return false;
        }
        /*
          If the rebound velocity is less than the relaxation
          velocity, we have effectively "stuck" to this limit
          (i.e. restitution will be zero).
        */
        if( limit->bRelaxingToLimit )
        {
            factors->c[RowIndex] = 0;
            factors->xi[RowIndex] = limit->overshoot;
        }
    }
    return true;
}


/*
  Local service which tests whether either of the limits on the given
  single axis have been overshot and, if so, sets the appropriate
  factors for this constraint row.  This assumes that the corresponding
  row of the Jacobian has been set to this axis.
*/
static bool LimitSingleAxis(MdtBclLimit * const limit,
    MdtKeaInputRowFactors * const factors, const unsigned int RowIndex,
    const MdtBclSolverParameters *const params)
{
    if (limit->overshoot < 0)
    {
        /*
          Lower limit overshot:
        */
        const MdtBclSingleLimit *const lolimit = limit->limit;

        if (lolimit->stiffness > 0)
        {
            if (!SetLimitFactors(factors, RowIndex, limit, lolimit, params))
                /*
                  Return *without* writing extra constraint to Jacobian.
                */
                return false;

            factors->lo[RowIndex] = 0;
            factors->hi[RowIndex] = MEINFINITY;
        }
    }
    else if (limit->overshoot > 0)
    {
        /*
          Upper limit overshot:
        */
        const MdtBclSingleLimit *const hilimit = limit->limit + 1;

        if (hilimit->stiffness > 0)
        {
            if (!SetLimitFactors(factors, RowIndex, limit, hilimit, params))
                /*
                  Return *without* writing extra constraint to Jacobian.
                */
                return false;

            factors->lo[RowIndex] = -MEINFINITY;
            factors->hi[RowIndex] = 0;
        }
    }

    /*
      May or may not subsequently write the extra row to the Jacobian.
    */
    return true;
}


/*
  This applies the actuator force as external force acting on the
  attached bodies.

  This service should be called only when the joint has overshot a limit
  for one of its limited degrees of freedom, and is also being actuated
  (powered) on the same axis.
*/
static void ApplyExternalForcesToAttachedBodies(MdtKeaBody *const blist[],
    const int *const BodyIndex, const MdtBclLimit * const limit,
    const MeVector3 LinearAxis, const MeVector3 RotationalAxis1,
    const MeVector3 RotationalAxis2)
{
    const MeReal scale =
        (limit->desired_vel > limit->velocity ? limit->fmax : -limit->fmax);

    blist[BodyIndex[0]]->force[0] += scale * LinearAxis[0];
    blist[BodyIndex[0]]->force[1] += scale * LinearAxis[1];
    blist[BodyIndex[0]]->force[2] += scale * LinearAxis[2];
    blist[BodyIndex[0]]->torque[0] += scale * RotationalAxis1[0];
    blist[BodyIndex[0]]->torque[1] += scale * RotationalAxis1[1];
    blist[BodyIndex[0]]->torque[2] += scale * RotationalAxis1[2];

    if (BodyIndex[1] != MdtBclNO_BODY)
    {
        blist[BodyIndex[1]]->force[0] += (-scale * LinearAxis[0]);
        blist[BodyIndex[1]]->force[1] += (-scale * LinearAxis[1]);
        blist[BodyIndex[1]]->force[2] += (-scale * LinearAxis[2]);
        blist[BodyIndex[1]]->torque[0] += (-scale * RotationalAxis2[0]);
        blist[BodyIndex[1]]->torque[1] += (-scale * RotationalAxis2[1]);
        blist[BodyIndex[1]]->torque[2] += (-scale * RotationalAxis2[2]);
    }
}


/*
  This service tests whether actuation or limitation about the given
  axis is currently required.  If so, it implements this and returns
  true.

  Otherwise, it returns false with no operations.
*/
static bool ActuateJointAxes(const MdtBclLimit * const limit,
                             const int *const BodyIndex, const MeVector3 LinearAxis,
                             const MeVector3 RotationalAxis1, const MeVector3 RotationalAxis2,
                             MdtKeaBody *const blist[], MdtKeaInputRowFactors * const factors,
                             const unsigned int RowIndex, MdtKeaConstraints *const clist)
{
    if ((limit->overshoot != 0) || limit->bPowered ||  limit->is_locked )
    {
        unsigned int i = 0;
        
        for (i = 0; i < 3; i++)
        {
            KEAJ(0, RowIndex, i) = LinearAxis[i];
            KEAJ(0, RowIndex, 3 + i) = RotationalAxis1[i];
        }
        
        if (BodyIndex[1] != MdtBclNO_BODY)
            for (i = 0; i < 3; i++)
            {
                KEAJ(1, RowIndex, i) = -LinearAxis[i];
                KEAJ(1, RowIndex, 3 + i) = -RotationalAxis2[i];
            }
            
            if (limit->bPowered || limit->is_locked)
            {
                if (limit->overshoot != 0)
                    ApplyExternalForcesToAttachedBodies(blist, BodyIndex,
                    limit, LinearAxis, RotationalAxis1, RotationalAxis2);
                else
                {
                    factors->lo[RowIndex] = -limit->fmax;
                    factors->hi[RowIndex] = limit->fmax;
                    
                    if ( limit->bPowered )
                        factors->c[RowIndex] = limit->desired_vel;
                    else
                    {
                        factors->xi[RowIndex] = limit->position - limit->position_lock;
                    }
                }
            }
            
            return true;
    }
    
    return false;
}

/*
  This service tests whether actuation or limitation about the given
  axis is currently required.  If so, it implements this and returns
  true.

  Otherwise, it returns false.
*/

static bool SetSingleAxisActuation( MdtBclLimit * const limit,
    const int *const BodyIndex, const MeVector3 LinearAxis,
    const MeVector3 RotationalAxis1, const MeVector3 RotationalAxis2,
    MdtKeaBody *const blist[], MdtKeaInputRowFactors * const factors,
    const unsigned int RowIndex, MdtKeaConstraints *const clist,
    const MdtBclSolverParameters *const params )
{
    if (LimitSingleAxis(limit, factors, RowIndex, params))
    {
        /*
          Set Jacobian to appropriate axis, and add actuation terms if
          the axis is powered:
        */
        if (ActuateJointAxes( limit, BodyIndex, LinearAxis, RotationalAxis1,
                              RotationalAxis2, blist, factors, RowIndex, clist ))
        {
            return true;
        }
    }
    return false;
}

/*
  This service calculates the limit coordinate (the "position") with reference
  to the coordinate at the previous timestep: this prevents angular coordinates
  from winding discontinuously from +pi to -pi or vv.

  This service then calculates the overshoot, if any, of either of
  the stops set on the limit.  If the "limited" flag has been
  cleared, the overshoot value is cleared by this service.
*/
static void CalculateLimitPositionAndOvershoot( MdtBclLimit * const limit,
                                                const MeReal stepsize,
                                                const MeBool bRotary )
{
    /* The change in coordinate is expected usually to be within a few percent
    of (velocity * stepsize).  However, if either velocity or stepsize are very
    large, the fractional error will grow - hence the large "safety" factor.
    For the rotational position algorithm to work, MaxExpectedDelta must
    be significantly less than 2 pi. */
    MEASSERT( limit->bCalculatePosition );
#ifdef _MECHECK
    const MeReal MaxExpectedDelta = (MeReal) MeFabs( 10 * limit->velocity * stepsize );
    if( bRotary && (MaxExpectedDelta >= ME_PI) )
    {
        /*MeWarning(0, "MdtBcl: CalculateLimitPositionAndOvershoot: "
                     "The angular velocity is too high to be defined "
                     "outside the -pi to +pi range.\n");*/
    }
#endif

    /* Initialise the limit coordinate if appropriate:  */
    if( !limit->bPositionInitialised )
    {
        /* This relies on the position attribute being initialised to zero on creation. */
        limit->offset -= limit->position;
        limit->bPositionInitialised = true;
        limit->previous_position = limit->position;
    }

    if( ME_IS_ZERO( limit->velocity ) )
    {
        /* Any change in coordinate will be due to a transition through a pole. */
        limit->position = limit->previous_position;
    }
    else if( bRotary )
    {
        while( limit->position > limit->previous_position + ME_PI )
        {
            limit->position -= (ME_PI + ME_PI);
        }

        while( limit->position + ME_PI < limit->previous_position )
        {
            limit->position += (ME_PI + ME_PI);
        }
    }
    /* Now update "previous position":  */
    limit->previous_position = limit->position;

    /* see if the joint should be limited */
    limit->overshoot = 0;

    if( limit->bLimited )
    {
        const MeReal UserCoordinate = limit->position + limit->offset;
        if( UserCoordinate < limit->limit[0].stop )
            limit->overshoot = UserCoordinate - limit->limit[0].stop;
        else if( UserCoordinate > limit->limit[1].stop )
            limit->overshoot = UserCoordinate - limit->limit[1].stop;
        else
            /*  ... overshoot remains zero */
            limit->bRelaxingToLimit = false;
    }
}

/*
  Utilities for adding rows to the constraint row list.
*/
#define ZEROROW(r) { int z; for(z=0;z!=6;z++) {KEAJ(0, (r), z)=0;KEAJ(1, (r), z)=0;}} \
    xi[r]=0.0f; \
    slipfactor[r]=0.0f; \
    c[r]=0.0f;\
    xgamma[r]=0.0f;

/**
 * Version of 'AddSphericalRows' that takes constraint position in the
 * world frame instead of the body local frame. If body2Present is 0 (false),
 * body2pos can be null as it is not used.
 */
void MEAPI MdtBclAddSphericalRowsWorld(MdtKeaConstraints *const clist,
                const MeVector3 body1pos, const MeVector3 body2pos,
                const MeVector3 cpos1world, const MeVector3 cpos2world,
                const int body2Present, const MeVector3 worldLinVel, 
                const MeVector3 worldAngVel)
{
    FINDSTARTROW;
    int i;
    MeVector3 cpos1rel;

    ZEROROW(0);
    ZEROROW(1);
    ZEROROW(2);

    KEAJ(0, 0, 0) = 1;
    KEAJ(0, 1, 1) = 1;
    KEAJ(0, 2, 2) = 1;

    /* work out vector from body1 to constraint (world frame) */
    MeVector3Subtract(cpos1rel, cpos1world,  body1pos);

    MdtBclMathCrossMatrixToJstore(cpos1rel, 1,
        &KEAJ(0, 0, 3), &KEAJ(0, 1, 3), &KEAJ(0, 2, 3));

    /* Compute position error */
    for(i=0; i<3; i++)
        xi[i] = cpos1world[i] - cpos2world[i];

    if (body2Present)
    {
        MeVector3 cpos2rel;

        KEAJ(1, 0, 0) = -1;
        KEAJ(1, 1, 1) = -1;
        KEAJ(1, 2, 2) = -1;

        /* vector from body2 to constraint (world frame) */
        MeVector3Subtract( cpos2rel, cpos2world,  body2pos);

        MdtBclMathCrossMatrixToJstore(cpos2rel, -1,
            &KEAJ(1, 0, 3), &KEAJ(1, 1, 3), &KEAJ(1, 2, 3));
    }
    /* JAG: World velocity for joints. Remove if you think its dodgy. */
#if 0
    else
    {
        MeReal worldVel[6];

        worldVel[0] = worldLinVel[0];
        worldVel[1] = worldLinVel[1];
        worldVel[2] = worldLinVel[2];
        worldVel[3] = worldAngVel[0];
        worldVel[4] = worldAngVel[1];
        worldVel[5] = worldAngVel[2];

        c[0] = MdtBclDotJ(worldVel, clist, 0, 0);
        c[1] = MdtBclDotJ(worldVel, clist, 1, 0);
        c[2] = MdtBclDotJ(worldVel, clist, 2, 0);
    }
#endif

    /* No limits on lagrange multipliers. */
    for (i = 0; i < 3; i++)
    {
        lo[i] = -MEINFINITY;
        hi[i] = MEINFINITY;
    }
}



/*
  Functions for adding whole constraints to the constraint row list
*/

/**
 * Add a Ball And Socket Joint constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclBSJoint struct.
 * @param tlist Array of MdtKeaTransformation structs for the array of
 * bodies.
 * @param blist Array of MdtKeaBody struct that are reference by the
 * constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */
void MEAPI MdtBclAddBSJoint(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;
    MdtBSJoint* joint = (MdtBSJoint*)constraint;

    /* unsigned int num_rows_added = 3; */
    /* unsigned int i = 0; */
    MeMatrix4 ref1world, ref2world;

    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, joint);

    /* Map constraint reference frames into world reference frame. */
    ConvertRefFramesToWorld(&joint->head, tlist, ref1world, ref2world);

    MdtBclAddSphericalRowsWorld(clist, tlist[joint->head.bodyindex[0]].pos,
        tlist[joint->head.bodyindex[1]].pos, ref1world[3], ref2world[3],
        (joint->head.bodyindex[1] != MdtBclNO_BODY), 
        joint->head.worldLinVel, joint->head.worldAngVel );

    /* Increment the rows added counter etc. */
    MdtBclEndConstraint(clist, 3);
}

static inline void MdtContactWriteRow(MdtKeaConstraints *const clist, 
                                      int row, 
                                      MeReal *vector,
                                      MeReal *pos0, 
                                      MeReal *pos1, 
                                      MeBool writeBody1)
{
    int j;
    for (j = 0; j < 3; j++)
        KEAJ(0, row, j) = vector[j];
    
    MdtBclMathCrossToJstore(pos0, vector, &KEAJ(0, row, 3));
    
    if (writeBody1)
    {
        for (j = 0; j < 3; j++)
            KEAJ(1, row, j) = -vector[j];
        
        MdtBclMathCrossToJstore(vector, pos1, &KEAJ(1, row, 3));
    }
}



/**
 * Add a Contact constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclContact struct.
 * @param tlist Array of MdtKeaTransformation structs for the array
 * of bodies.
 * @param blist Array of MdtKeaBody struct that are reference by the
 * constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */

void MEAPI MdtBclAddContact(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;

    MdtContact* contact = (MdtContact*)constraint;
    MdtBclContactParams *p = &contact->params;
    int options = p->options;

    int friction_rows = p->type == MdtContactTypeFriction1D ? 1 :
                        p->type == MdtContactTypeFriction2D ? 2 : 0;

    MeBool sliding_world = contact->head.bodyindex[1] == MdtBclNO_BODY && 
                          (options & MdtBclContactOptionUseWorldVelocity);

    MeReal one_over_root_2 = (MeReal)(0.7071067811865475244);

    MdtBodyID b0 = contact->head.mdtbody[0];
    MdtBodyID b1 = contact->head.mdtbody[1];

    /* contact pos relative to body 0,1 */
    
    MeVector3 pos0, pos1;

    /* velocities of bodies 0,1 at contact and velocity of b0 relative to b1 */
    
    MeVector3 vel0, vel1, rvel;
    
    /* primary and secondary directions of friction */
    
    MeVector3 dir1, dir2;

    /* this determines whether we need to scale the sides of the friction box
       because we auto-oriened it with the primary direction in the corner */

    MeReal boxScale = 1;

    /* Precalculate some stuff we'll need below */

    MdtBodyGetCenterOfMassPosition(b0,pos0);
    MeVector3Subtract(pos0,contact->cpos,pos0);
    MdtBodyGetVelocityAtPoint(b0, contact->cpos, vel0);

    if (b1!=0)
    {
        MdtBodyGetCenterOfMassPosition(b1,pos1);
        MeVector3Subtract(pos1,contact->cpos,pos1);
        MdtBodyGetVelocityAtPoint(b1, contact->cpos, vel1);
    }
    else if(options & MdtBclContactOptionUseWorldVelocity)
        MeVector3Copy(vel1, contact->worldVel);
    else
        vel1[0] = vel1[1] = vel1[2] = 0;
    
    MeVector3Subtract(rvel, vel0, vel1);

    MeReal rvel_normal = MeVector3Dot(rvel,contact->normal);

    /* Add bodies to MdtKeaConstraints struct */
    
    ADDBODIES(clist, contact);


    /* Do the normal-direction row */

    ZEROROW(0);

    MdtContactWriteRow(clist, 0, contact->normal,pos0,pos1,b1?1:0);

    xi[0] = -(contact->penetration);
    lo[0] = (options & MdtBclContactOptionAdhesive) ? -p->max_adhesive_force : 0;
    hi[0] = MEINFINITY;

    if (options & MdtBclContactOptionSoft)
    {
        slipfactor[0] = p->softness;
        if(slipfactor[0] < 0)
        {
#ifdef _MECHECK
            //MeWarning(0, "MdtBclAddContact: Negative slipfactor.");
#endif
            slipfactor[0] = 0;
        }

    }

    /*
      If restitution is required, set the right hand side of the
      constraint equation to nonzero.
    */

    if(options & MdtBclContactOptionBounce && rvel_normal < -p->velThreshold)
    {
        c[0] = -(p->restitution) * rvel_normal;
    }

    if(options & MdtBclContactOptionUseWorldVelocity)
    {
        c[0] += MeVector3Dot(contact->worldVel, contact->normal);
    }

    /* Find the primary (and if reqd, secondary) friction directions */

    if(friction_rows)
    {
        if(options & MdtBclContactOptionUseDirection)
        {
            MeVector3Copy(dir1, contact->direction);
            MeVector3MultiplySubtract(dir1,MeVector3Dot(dir1,contact->normal),
                                      contact->normal);
        }
        else
        {
            MeVector3Copy(dir1,rvel);
            MeVector3MultiplySubtract(dir1,rvel_normal,contact->normal);
        }

        MeReal mag = MeVector3Normalize(dir1);

        if(friction_rows==1)
        {
            if(mag < ME_MEDIUM_EPSILON)
                friction_rows = 0; 
            /* what else can we do? */
        }

        else 
        {
#ifdef _MECHECK
            if(p->friction1 != p->friction2 && 
                !(options & MdtBclContactOptionUseDirection))
            {
                MeWarning(0, "MdtBclAddContact: Using different primary and "
                    "secondary friction values without calling MdtContactSetDirection " 
                    "is not sensible!");
            }
#endif
            if(mag < ME_MEDIUM_EPSILON)
            {

                /* If we do not have a contact direction, guess... */
                
               MeVector3PlaneSpace(contact->normal, dir1, dir2);

#ifdef _MECHECK
               /* but warn the user if they set a duff direction */
             
               if((options & MdtBclContactOptionUseDirection))
               {
                   MeWarning(0, "MdtBclAddContact: User-set contact direction "
                       "is parallel to contact normal");
               }
#endif
            }
            else
            {
                MeVector3Cross(dir2, dir1, contact->normal);

                /*  If there is a relative velocity (ie dynamic case) and we are
                    auto-orienting, we orient friction box so the _corner_ points 
                    in the direction of slide. */

                if(!(options & MdtBclContactOptionUseDirection))
                {
                    MeVector3 par, perp;
                    MeVector3MultiplyScalar(par,dir1,one_over_root_2);
                    MeVector3MultiplyScalar(perp,dir2,one_over_root_2);

                    MeVector3Add(dir1,par,perp);
                    MeVector3Subtract(dir2, par, perp);
    
                    /* and make sure the friction box limits are scaled so that if
                       both directions are sliding the limit is the same as the
                       static limit */

                    boxScale = one_over_root_2;
                }
            }
        }
    }

    if(friction_rows)
    {
        ZEROROW(1);

        MdtContactWriteRow(clist, 1, dir1, pos0, pos1,b1 ? 1 : 0);

        if (options & MdtBclContactOptionSlip1)
        {
            slipfactor[1] = p->slip1;
            if(slipfactor[1] < 0)
            {
#ifdef _MECHECK
                //MeWarning(0, "MdtBclAddContact: Negative slipfactor.");
#endif
                slipfactor[1] = 0;
            }

        }

        if (options & MdtBclContactOptionSlide1)
            c[1] = p->slide1;

        if(sliding_world)
        {
            c[1] += MeVector3Dot(dir1, contact->worldVel);
            p->slide1 = c[1];
        }

        hi[1] = (p->model1 == MdtFrictionModelBox 
                 || p->model1 == MdtFrictionModelNormalForce)
                 ? p->friction1 * boxScale : MEINFINITY;

        lo[1] = -hi[1];
    }


    if (friction_rows == 2)
    {
        ZEROROW(2);

        MdtContactWriteRow(clist, 2, dir2, pos0, pos1, b1 ? 1 : 0);

        if (options & MdtBclContactOptionSlip2)
        {
            slipfactor[2] = p->slip2;
            if(slipfactor[2] < 0)
            {
#ifdef _MECHECK
                //MeWarning(0, "MdtBclAddContact: Negative slipfactor.");
#endif
                slipfactor[2] = 0;
            }
        }

        if (options & MdtBclContactOptionSlide2)
            c[2] = p->slide2;

        if(sliding_world)
        {
            c[2] += MeVector3Dot(dir2, contact->worldVel);
            p->slide2 = c[2];
        }

        hi[2] = (p->model2 == MdtFrictionModelBox 
                 || p->model2 == MdtFrictionModelNormalForce)
                 ? p->friction2 * boxScale : MEINFINITY;

        lo[2] = -hi[2];
    }

    MdtBclEndConstraint(clist,friction_rows+1);
}


#ifdef BUGGERED

/**
 * Add a Contact constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclContact struct.
 * @param tlist Array of MdtKeaTransformation structs for the array
 * of bodies.
 * @param blist Array of MdtKeaBody struct that are reference by the
 * constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */
void MEAPI MdtBclOldAddContact(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;

    ZEROROW(0);

    MdtContact* contact = (MdtContact*)constraint;
    /*
      Set the jacobian for this contact. Also set a vector 'slipfactor'
      that is used to modify the system matrix to account for first
      order slipping constraints.
    */

    int j;
    int num_rows_added;
    MeBool orientingBox = 0;

    MeReal one_over_root_2 = (MeReal)(0.7071067811865475244);
    

    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, contact);

    if (contact->params.options & MdtBclContactOptionSoft)
    {
        slipfactor[0] = contact->params.softness;
        if(slipfactor[0] < 0)
        {
#ifdef _MECHECK
            //MeWarning(0, "MdtBclOldAddContact: Negative slipfactor.");
#endif
            slipfactor[0] = 0;
        }
    }

    if (contact->params.type == MdtContactTypeFriction1D
        || contact->params.type == MdtContactTypeFriction2D)
    {
        ZEROROW(1);
        if (contact->params.options & MdtBclContactOptionSlip1)
        {
            slipfactor[1] = contact->params.slip1;
            if(slipfactor[1] < 0)
            {
#ifdef _MECHECK
                //MeWarning(0, "MdtBclOldAddContact: Negative slipfactor.");
#endif
                slipfactor[1] = 0;
            }
        }

        if (contact->params.options & MdtBclContactOptionSlide1)
            c[1] = contact->params.slide1;
    }

    if (contact->params.type == MdtContactTypeFriction2D)
    {
        ZEROROW(2);
        if (contact->params.options & MdtBclContactOptionSlip2)
        {
            slipfactor[2] = contact->params.slip2;
            if(slipfactor[2] < 0)
            {
#ifdef _MECHECK
                //MeWarning(0, "MdtBclOldAddContact: Negative slipfactor.");
#endif
                slipfactor[2] = 0;
            }
        }

        if (contact->params.options & MdtBclContactOptionSlide2)
            c[2] = contact->params.slide2;
    }

    xi[0] = -(contact->penetration);

    /*
      Get motion vectors for the number of limited DOF - the contact
      normal and one or two planespace tangent vectors.

      Frictional forces are applied along the tangent vectors to
      minimize the tangential motion.
    */

    MeVector3 mvec[3];

    MeVector3Copy(mvec[0], contact->normal);
    
    if (contact->params.type == MdtContactTypeFriction2D)
    {
        if (!(contact->params.options & MdtBclContactOptionUseDirection))
        {
            //MeVector3PlaneSpace(contact->normal, mvec[1], mvec[2]);
            
            MeVector3 rvel, vel1, vel2;
            MeVector3 vel_proj_norm, vel_in_plane;

            MdtBodyID b1 = contact->head.mdtbody[0];
            MdtBodyID b2 = contact->head.mdtbody[1];
            
#ifdef _MECHECK
            if(contact->params.friction1 != contact->params.friction2)
            {
                MeWarning(0, "MdtBclAddContact: Using different primary and "
                    "secondary friction values without calling MdtContactSetDirection " 
                    "is not sensible!");
            }
#endif

            /* First calculate relative velocities at contact point. */
            MdtBodyGetVelocityAtPoint(b1, contact->cpos, vel1);
            
            if(b2 != 0)
                MdtBodyGetVelocityAtPoint(b2, contact->cpos, vel2);
            else
            {
                if(contact->params.options & MdtBclContactOptionUseWorldVelocity)
                    MeVector3Copy(vel2, contact->worldVel);
                else
                    vel2[0] = vel2[1] = vel2[2] = 0;
            }
            
            MeVector3Subtract(rvel, vel1, vel2);

                /*  Project relative velocity into plane of contact normal. 
                    (project rvel along normal and subtract from rvel). */
            MeVector3Copy(vel_proj_norm, contact->normal);
            MeVector3Scale(vel_proj_norm, MeVector3Dot(contact->normal, rvel));
            MeVector3Subtract(vel_in_plane, rvel, vel_proj_norm);            

            /*  If there is a relative velocity (ie dynamic case) we orient 
                friction box so the corner points in the direction of slide. */
            if (MeVector3MagnitudeSqr(vel_in_plane) > ME_MEDIUM_EPSILON * ME_MEDIUM_EPSILON)
            {
                MeVector3 perp_in_plane;
                
                /* Get unit direction vector of sliding. */
                MeVector3Normalize(vel_in_plane);
                
                /*  Now we want the CORNER of the friction box not a FACE pointing
                    in the direction of travel. So we work out a perp. vector, add it
                    and divide by sqrt(2) */
                MeVector3Cross(perp_in_plane, vel_in_plane, contact->normal);

                MeVector3Add(mvec[1], vel_in_plane, perp_in_plane);
                MeVector3Scale(mvec[1], one_over_root_2);

                MeVector3Subtract(mvec[2], vel_in_plane, perp_in_plane);
                MeVector3Scale(mvec[2], one_over_root_2);

                /*  If we are orienting the box, we scalethe friction below to that
                    magnitude of dynamic friction is <= magnitude of static friction. */
                orientingBox = 1;
            }
            else
            {
                /* If we are not sliding, the best we can 'guess' at is planespace. */
               MeVector3PlaneSpace(contact->normal, mvec[1], mvec[2]); 
            }

        }
        else
        {
            /*
              Project and normalize `direction' vector so its
              perpendicular to normal.

              This will be $mvec[1]. mvec[2] = mvec[1] x normal$, so
              these vectors span the planespace. We can get mvec[2]
              before mvec[1] has been projected and normalized, as long
              as it has unit length.
            */

            MeVector3Cross( mvec[2], contact->direction,  contact->normal);
            MeVector3Cross( mvec[1], contact->normal,  mvec[2]);

            /* NOTE! we dont actually project and normalize yet, as
               below! */
        }
        num_rows_added = 3;
    }
    else if (contact->params.type == MdtContactTypeFriction1D)
    {
        /*
          Project direction vector so its perpendicular to normal and
          has unit length.
        */

        MeReal k = MeVector3Dot(contact->direction, contact->normal);

        for (j = 0; j < 3; j++)
            mvec[1][j] =
                (contact->direction)[j] - k * (contact->normal[j]);

        MeVector3Normalize(mvec[1]);
        num_rows_added = 2;
    }
    else if (contact->params.type == MdtContactTypeFrictionZero)
    {
        /* its ok, no error... */
        num_rows_added = 1;
    }
#ifdef _MECHECK
    else
        MeDebug(12, "internal error in contact jacobian code");
#endif

    /* p: contact pos relative to body 1,2 */
    MeVector3 p;

    /*
      Check for body-body contact.
    */

    if (contact->head.bodyindex[1] != MdtBclNO_BODY)
    {
        /*
          Get contact position relative to body2.
        */
        for (j = 0; j < 3; j++)
            p[j] = contact->cpos[j] - tlist[contact->head.bodyindex[1]].pos[j];

        /*
          Set body 2 constraint to disallow motion along motion vectors.
        */
        for (int t = 0; t < num_rows_added; t++)
        {
            for (j = 0; j < 3; j++)
            {
                KEAJ(1, t, j) = -mvec[t][j];
            }

            MdtBclMathCrossToJstore(mvec[t], p, &KEAJ(1, t, 3));
        }
    }

    /*
      Get contact position relative to body1.
    */
    for (j = 0; j < 3; j++)
        p[j] = contact->cpos[j] - tlist[contact->head.bodyindex[0]].pos[j];

    /*
      Set body 1 constraint to disallow motion along motion vectors.
    */
    for (int t = 0; t < num_rows_added; t++)
    {
        for (j = 0; j < 3; j++)
            KEAJ(0, t, j) = mvec[t][j];

        MdtBclMathCrossToJstore(p, mvec[t], &KEAJ(0, t, 3));
    }

    /*
      Set limits on lagrange multipliers
    */
    if (contact->params.options & MdtBclContactOptionAdhesive)
        lo[0] = -(contact->params.max_adhesive_force);
    else
        lo[0] = 0;

    hi[0] = MEINFINITY;

    MeReal max;

    /*  If we are orienting the friction box in the direction of travel (ie. in the dynamic
        friction case), we scale the friction so its <= the static case. */
    if (contact->params.type == MdtContactTypeFriction1D
        || contact->params.type == MdtContactTypeFriction2D)
    {
        if (contact->params.model1 == MdtFrictionModelBox
            || contact->params.model1 == MdtFrictionModelNormalForce)
            max = contact->params.friction1;
        else
            max = MEINFINITY;

        if(orientingBox)
        {
            lo[1] = -max * one_over_root_2;
            hi[1] = max * one_over_root_2;
        }
        else
        {
            lo[1] = -max;
            hi[1] = max;
        }
    }

    if (contact->params.type == MdtContactTypeFriction2D)
    {
        if (contact->params.model2 == MdtFrictionModelBox
            || contact->params.model2 == MdtFrictionModelNormalForce)
            max = contact->params.friction2;
        else
            max = MEINFINITY;

        if(orientingBox)
        {
            lo[2] = -max * one_over_root_2;
            hi[2] = max * one_over_root_2;
        }
        else
        {
            lo[2] = -max;
            hi[2] = max;
        }
    }

    /*
      If restitution is required, set the right hand side of the
      constraint equation to nonzero.
    */

    if (contact->params.options & MdtBclContactOptionBounce)
    {
        /*
          Find the normal velocity of the one or two bodies.
        */

        MeReal vel = 0;
        MeReal v0=0,v1=0;
/*
        for (j = 0; j < 6; j++)
            vel += KEAJ(0, 0, j) * blist[contact->head.bodyindex[0]]->vel[j];

        if (contact->head.bodyindex[1] != MdtBclNO_BODY)
            for (j = 0; j < 6; j++)
                vel += KEAJ(1, 0, j) * blist[contact->head.bodyindex[1]]->vel[j];

        for (j = 0; j < 6; j++)
            v0 += KEAJ(0, 0, j) * blist[contact->head.bodyindex[0]]->vel[j];

        if (contact->head.bodyindex[1] != MdtBclNO_BODY)
            for (j = 0; j < 6; j++)
                v1 += KEAJ(1, 0, j) * blist[contact->head.bodyindex[1]]->vel[j];
*/
        for (j = 0; j < 3; j++)
            vel += KEAJ(0, 0, j) * blist[contact->head.bodyindex[0]]->vel[j];

        for (j = 0; j < 3; j++)
            vel += KEAJ(0, 0, j+3) * blist[contact->head.bodyindex[0]]->velrot[j];

        if (contact->head.bodyindex[1] != MdtBclNO_BODY)
        {
            for (j = 0; j < 3; j++)
                vel += KEAJ(1, 0, j) * blist[contact->head.bodyindex[1]]->vel[j];
            for (j = 0; j < 3; j++)
                vel += KEAJ(1, 0, j+3) * blist[contact->head.bodyindex[1]]->velrot[j];
        }

        if (vel < -(contact->params.velThreshold))
            c[0] = -(contact->params.restitution) * vel;


    }

    /* If this is a contact to the world, and we have a 'world' velocity, use it. */
    if(contact->head.bodyindex[1] == MdtBclNO_BODY && 
        (contact->params.options & MdtBclContactOptionUseWorldVelocity))
    {
        /* normal velocity - add to restitution */
        c[0] += MeVector3Dot(mvec[0], contact->worldVel);

        /* direction velocities (add to any existing slide) */
        if(contact->params.type == MdtContactTypeFriction1D || 
            contact->params.type == MdtContactTypeFriction2D)
        {
            c[1] += MeVector3Dot(mvec[1], contact->worldVel);
            contact->params.slide1 = c[1];
        }
        
        if(contact->params.type == MdtContactTypeFriction2D)
        {
            c[2] += MeVector3Dot(mvec[2], contact->worldVel);
            contact->params.slide2 = c[2];
        }
    }
}

void MEAPI MdtBclAddContact(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    int i,j;
    
    int diff = 0;
    MeReal copy_hi[3], 
        copy_lo[3], 
        copy_xi[3], 
        copy_c[3], 
        copy_slipfactor[3], 
        copy_xgamma[3],
        copy_J[2][3][6];

    MdtContact* contact = (MdtContact*)constraint;
    MdtBclContactParams *p = &contact->params;
    int friction_rows = p->type == MdtContactTypeFriction1D ? 1 :
                        p->type == MdtContactTypeFriction2D ? 2 : 0;


    MdtBclOldAddContact(clist,constraint,tlist,blist,params);
    
    for(i=0;i<friction_rows+1;i++)
    {
        copy_hi[i]=clist->hi[i];
        copy_lo[i]=clist->lo[i];
        copy_xi[i]=clist->xi[i];
        copy_c[i]=clist->c[i];
        copy_xgamma[i]=clist->xgamma[i];
        copy_slipfactor[i] = clist->slipfactor[i];
        for(j=0;j<6;j++)
        {
            copy_J[0][i][j]=KEAJ(0,i,j);
            copy_J[1][i][j]=KEAJ(1,i,j);
        }
    }

    MdtBclNewAddContact(clist,constraint,tlist,blist,params);
    
    MeReal eps = (MeReal)1e-3;

    for(i=0;i<friction_rows+1;i++)
    {
        if(fabs(copy_hi[i]-clist->hi[i])>eps)
        {
            printf("Hi Diff in row %d: %f != %f\n",i,copy_hi[i],clist->hi[i]);
            diff = 1;
        }

        if(fabs(copy_lo[i]-clist->lo[i])>eps)
        {
            printf("Lo Diff in row %d: %f != %f\n",i,copy_lo[i],clist->lo[i]);
            diff = 1;
        }

        if(fabs(copy_xi[i]-clist->xi[i])>eps)
        {
            printf("Xi Diff in row %d: %f != %f\n",i,copy_xi[i],clist->xi[i]);
            diff = 1;
        }

        if(fabs(copy_slipfactor[i]-clist->slipfactor[i])>eps)
        {
            printf("Slipfactor Diff in row %d: %f != %f\n",i,copy_slipfactor[i],clist->slipfactor[i]);
            diff = 1;
        }

        if(fabs(copy_c[i]-clist->c[i])>eps)
        {
            printf("C Diff in row %d: %f != %f\n",i,copy_c[i],clist->c[i]);
            diff = 1;
        }

        if(fabs(copy_xgamma[i]-clist->xgamma[i])>eps)
        {
            printf("Xgamma Diff in row %d: %f != %f\n",i,copy_xgamma[i],clist->xgamma[i]);
            diff = 1;
        }

        for(j=0;j<6;j++)
        {
            if(fabs(copy_J[0][i][j]-KEAJ(0,i,j))>eps)
            {
                printf("Jacobian diff body 0, row %d, col %d: %f != %f\n",i,j,copy_J[0][i][j],KEAJ(0,i,j));
                diff = 1;
            }

            if(fabs(copy_J[1][i][j]-KEAJ(1,i,j))>eps)
            {
                printf("Jacobian diff body 1, row %d, col %d: %f != %f\n",i,j,copy_J[1][i][j],KEAJ(1,i,j));
                diff = 1;
            }
        }
    }

    if(diff)
    {
        MdtBclNewAddContact(clist,constraint,tlist,blist,params);
        MdtBclOldAddContact(clist,constraint,tlist,blist,params);
    }

    MdtBclEndConstraint(clist,friction_rows+1);
}
#endif


/**
 * Add a ContactGroup constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclContact struct.
 * @param tlist Array of MdtKeaTransformation structs for the array
 * of bodies.
 * @param blist Array of MdtKeaBody struct that are reference by the
 * constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */

void    MEAPI MdtBclAddContactGroup(MdtKeaConstraints *const clist,
                void *const constraint, const MdtKeaTransformation *const tlist,
                MdtKeaBody *const blist[], const void *const params)
{
    MdtContactID c;
    MdtContactGroupID g = (MdtContactGroupID)constraint;
    MeReal averageForce
        = (g->count == 0) ? 0 : g->normalForce/g->count;

    for(c = g->first; c; c=c->nextContact)
    {
        c->head.sortKey = g->head.sortKey;
        c->head.bodyindex[0] = g->head.bodyindex[0];
        c->head.bodyindex[1] = g->head.bodyindex[1];
        c->head.mdtbody[0] = g->head.mdtbody[0];
        c->head.mdtbody[1] = g->head.mdtbody[1];

        if(c->params.model1 == MdtFrictionModelNormalForce)
            c->params.friction1 = averageForce * c->params.frictioncoeff1;
        
        if(c->params.model2 == MdtFrictionModelNormalForce)
            c->params.friction2 = averageForce * c->params.frictioncoeff2;

        MdtBclAddContact(clist, c, tlist, blist, params);
    }
}


/**
 * Add a Hinge constraint to a MdtKeaConstraints struct
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclHinge struct.
 * @param tlist Array of MdtKeaTransformation structs for the array
 * of bodies.
 * @param blist Array of MdtKeaBody struct that are reference by the
 * constraint.
 * @param stepsize Time step that the system is going to be
 * evolved by.
 */

void MEAPI MdtBclAddHinge(MdtKeaConstraints *const clist, void *const constraint,
    const MdtKeaTransformation * const tlist, MdtKeaBody *const blist[],
    const void *const params)
{
    FINDSTARTROW;

    ZEROROW(0);
    ZEROROW(1);
    ZEROROW(2);
    ZEROROW(3);
    ZEROROW(4);
    ZEROROW(5);

    MdtHinge* joint = (MdtHinge*)constraint;
    unsigned int i;
    unsigned int j;
    MeMatrix4 ref1world, ref2world;

    /* Map constraint reference frames into world reference frame. */
    ConvertRefFramesToWorld(&joint->head, tlist, ref1world, ref2world);

    ADDBODIES(clist, joint);

    if( joint->limit.bCalculatePosition )
    {
        /* Calculate relative angular velocity. */
        MeVector3 ang_vel;

        if (joint->head.bodyindex[1] != MdtBclNO_BODY)
        {
            MeVector3Subtract( ang_vel, blist[joint->head.bodyindex[0]]->velrot,  blist[joint->head.bodyindex[1]]->velrot);
        }
        else
        {
            MeVector3Copy( ang_vel, blist[joint->head.bodyindex[0]]->velrot);
        }

        /* theta */
        joint->limit.position =
            -MeAtan2(MeVector3Dot(ref2world[1], ref1world[2]), MeVector3Dot(ref2world[1], ref1world[1]));
        
		joint->limit.velocity = MeVector3Dot(ang_vel, ref1world[0]);

        CalculateLimitPositionAndOvershoot( &joint->limit, ((MdtBclSolverParameters*)params)->stepsize, true );
    }
    else
    {
        joint->limit.overshoot = 0;
    }

    /* END OLD START STEP */

    /* Add 3 constraint rows to constrain the position. */
    MdtBclAddSphericalRowsWorld(clist, tlist[joint->head.bodyindex[0]].pos,
        tlist[joint->head.bodyindex[1]].pos, ref1world[3], ref2world[3],
        (joint->head.bodyindex[1] != MdtBclNO_BODY),
        joint->head.worldLinVel, joint->head.worldAngVel);

    /* 
        qacw: quaternion describing the constraint frame of body a in world coordinates
        qbcw: quaternion describing the constraint frame of body b in world coordinates
    */
    MeVector4 qacw, qbcw, qrel;
    MeMatrix4 qmul;

    MeQuaternionFromTM(qacw, ref1world);
    MeQuaternionFromTM(qbcw, ref2world);

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            qmul[i][j] = qacw[i]*qbcw[j];

//    qrel[0] =  qmul[0][0] + qmul[1][1] + qmul[2][2] + qmul[3][3];
//    qrel[1] = -qmul[1][0] + qmul[0][1] + qmul[3][2] - qmul[2][3];
    qrel[2] = -qmul[2][0] - qmul[3][1] + qmul[0][2] + qmul[1][3];
    qrel[3] = -qmul[3][0] + qmul[2][1] - qmul[1][2] + qmul[0][3];
    
    MeVector3 JwQ2, JwQ3;

    JwQ2[0] = (MeReal)0.5 * (qmul[2][1] - qmul[3][0] - qmul[0][3] + qmul[1][2]);
    JwQ2[1] = (MeReal)0.5 * (qmul[2][2] - qmul[3][3] + qmul[0][0] - qmul[1][1]);
    JwQ2[2] = (MeReal)0.5 * (qmul[2][3] + qmul[3][2] + qmul[0][1] + qmul[1][0]);

    JwQ3[0] = (MeReal)0.5 * (qmul[3][1] + qmul[2][0] + qmul[1][3] + qmul[0][2]);
    JwQ3[1] = (MeReal)0.5 * (qmul[3][2] + qmul[2][3] - qmul[1][0] - qmul[0][1]);
    JwQ3[2] = (MeReal)0.5 * (qmul[3][3] - qmul[2][2] - qmul[1][1] + qmul[0][0]);

    for (i = 0; i < 3; i++) {
        KEAJ(0, 3, i) = 0;
        KEAJ(0, 3, i + 3) = JwQ2[i];
        KEAJ(0, 4, i) = 0;
        KEAJ(0, 4, i + 3) = JwQ3[i];
    }

    if (joint->head.bodyindex[1] != MdtBclNO_BODY)
        for (i = 0; i < 3; i++) {
            KEAJ(1, 3, i) = 0;
            KEAJ(1, 3, i + 3) = -JwQ2[i];
            KEAJ(1, 4, i) = 0;
            KEAJ(1, 4, i + 3) = -JwQ3[i];
        }
 
    /*
      Compute angular position error for hinges.
    */
    xi[3] = -qrel[2];
    xi[4] = -qrel[3];

    /*
      No limits on lagrange multipliers.
    */
    for (i = 3; i < 5; i++)
    {
        lo[i] = -MEINFINITY;
        hi[i] = MEINFINITY;
    }

    /*
      Optional joint limit and power constraint. When we have a powered
      joint that is also at a joint limit we have a problem.  This is
      because the joint limits and power limits are incompatible with
      each other.

      I have come up with an OK solution. If we're at a low joint limit
      (say) and the desired velocity is negative, simple apply an
      *external* force of -fmax to the joint, because that corresponds
      to the motor achieving maximum power to try and fight the joint
      limit.

      If the desired velocity is positive, apply an external force of
      +fmax to the joint.  This will usually be too much to achieve the
      desired velocity, and will result in a too-fast "kick" away from
      the joint limit (but as soon as we're away from the joint limit
      we'll get the correct behavior).  This will ensure that if an
      external force (more than the force limit of the actuator) is
      attracting the joint to its joint limit then we wont get any
      motion away from that limit.

      The reason we do this: we cant simply ask for the desired velocity
      and limit the hi force, because this would prevent an arbitrarily
      high force being applied to prevent a *negative* velocity.

      Bad!  Maybe we can solve this properly with more than one
      constraint or with a more complicated LCP solution region.
    */

    /*
      Set joint power and joint limits, if a limit has been overshot:
    */
    /* Dummy linear axis */
    MeVector3 LinearAxis;
    MeVectorSetZero(LinearAxis, 3);

    if( SetSingleAxisActuation( &joint->limit, joint->head.bodyindex, LinearAxis,
            ref2world[0], ref2world[0], blist, &factors, 5, clist, (MdtBclSolverParameters*)params ) )
    {
        /*
          Limited hinge places restrictions on sixth degree of freedom.
         */
        MdtBclEndConstraint(clist, 6);
    }
    else
    {
        /*
          Default hinge joint removes 5 degrees of freedom.
        */
        MdtBclEndConstraint(clist, 5);
    }
}

/**
 * Add a Prismatic constraint to a MdtKeaConstraints 'struct'.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclPrismatic struct.
 * @param tlist Array of MdtKeaTransformation structs for the array of
 * bodies.
 * @param blist Array of MdtKeaBody struct that are reference by the
 * constraint.
 * @param params Evolution parameters specific to the numerical solver.
 */
void MEAPI MdtBclAddPrismatic(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;

    ZEROROW(0);
    ZEROROW(1);
    ZEROROW(2);
    ZEROROW(3);
    ZEROROW(4);
    ZEROROW(5);


    MdtPrismatic* joint = (MdtPrismatic*)constraint;
    int i;
    MeMatrix4 ref_Iw, ref_Jw;
    MeVector3 pos_IJw, pos_iJw, pos_jJw, tmp;
    MeVector3 RotationalAxis1, RotationalAxis2;
    MeReal vel;

    /* Map constraint reference frames I, J into world reference frame. */
    ConvertRefFramesToWorld(&joint->head, tlist, ref_Iw, ref_Jw);

    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, joint);

    /* offset of secondary body constraint reference frame (J) origin
       relative to primary body constraint reference frame (I) origin
       resolved in the world frame (w)
    */
    pos_IJw[0] = ref_Jw[3][0] - ref_Iw[3][0];
    pos_IJw[1] = ref_Jw[3][1] - ref_Iw[3][1];
    pos_IJw[2] = ref_Jw[3][2] - ref_Iw[3][2];

    /* offset of secondary body constraint reference frame (J) origin
       relative to primary body CoM reference frame (i) origin
       resolved in the world frame (w)
    */
    pos_iJw[0] = ref_Jw[3][0] - tlist[joint->head.bodyindex[0]].pos[0];
    pos_iJw[1] = ref_Jw[3][1] - tlist[joint->head.bodyindex[0]].pos[1];
    pos_iJw[2] = ref_Jw[3][2] - tlist[joint->head.bodyindex[0]].pos[2];

    MeVector3Cross( RotationalAxis1, pos_iJw,  ref_Iw[0]);

    if (joint->head.bodyindex[1] == MdtBclNO_BODY)
    {
    /* offset of secondary body constraint reference frame (J) origin
       relative to secondary body CoM reference frame (j) origin
       resolved in the world frame (w)
    */
        pos_jJw[0] = ref_Jw[3][0];
        pos_jJw[1] = ref_Jw[3][1];
        pos_jJw[2] = ref_Jw[3][2];
    }
    else
    {
        pos_jJw[0] = ref_Jw[3][0] - tlist[joint->head.bodyindex[1]].pos[0];
        pos_jJw[1] = ref_Jw[3][1] - tlist[joint->head.bodyindex[1]].pos[1];
        pos_jJw[2] = ref_Jw[3][2] - tlist[joint->head.bodyindex[1]].pos[2];
        MeVector3Cross( RotationalAxis2, pos_jJw, ref_Iw[0]);
    }

    /*  prismatic velocity calculation */

    vel =  MeVector3Dot(ref_Iw[0],       blist[joint->head.bodyindex[0]]->vel);
    vel += MeVector3Dot(RotationalAxis1, blist[joint->head.bodyindex[0]]->velrot);

    if (joint->head.bodyindex[1] != MdtBclNO_BODY)
    {
        vel -= MeVector3Dot(ref_Iw[0],       blist[joint->head.bodyindex[1]]->vel);
        vel -= MeVector3Dot(RotationalAxis2, blist[joint->head.bodyindex[1]]->velrot);
    }

    if( joint->limit.bCalculatePosition )
    {
        joint->limit.velocity = vel;
        joint->limit.position = -MeVector3Dot(ref_Iw[0], pos_IJw);
        CalculateLimitPositionAndOvershoot( &joint->limit, ((MdtBclSolverParameters*)params)->stepsize, false );
    }
    else
    {
        joint->limit.overshoot = 0;
    }

    /*
      Calculate error for positional part (e1,e2).
    */
    xi[0] = MeVector3Dot(pos_IJw, ref_Iw[1]);
    xi[1] = MeVector3Dot(pos_IJw, ref_Iw[2]);

    /*
       Calculate error for rotational part (e3,e4,e5).
     */
    xi[2] = MeVector3Dot(ref_Iw[1],ref_Jw[2]);
    xi[3] = MeVector3Dot(ref_Iw[2],ref_Jw[0]);
    xi[4] = MeVector3Dot(ref_Iw[0],ref_Jw[1]);

    /*
      Calculate Jacobians for the two positional (linear) constraints.
    */

    for (i = 0; i < 3; i++) KEAJ(0, 0, i) = -ref_Iw[1][i];
    for (i = 0; i < 3; i++) KEAJ(0, 1, i) = -ref_Iw[2][i];
    MdtBclMathCrossToJstore(ref_Iw[1], pos_iJw, &KEAJ(0, 0, 3));
    MdtBclMathCrossToJstore(ref_Iw[2], pos_iJw, &KEAJ(0, 1, 3));

    if (joint->head.bodyindex[1] != MdtBclNO_BODY)
    {
        for (i = 0; i < 3; i++) KEAJ(1, 0, i) = ref_Iw[1][i];
        for (i = 0; i < 3; i++) KEAJ(1, 1, i) = ref_Iw[2][i];
        MdtBclMathCrossToJstore(pos_jJw, ref_Iw[1], &KEAJ(1, 0, 3));
        MdtBclMathCrossToJstore(pos_jJw, ref_Iw[2], &KEAJ(1, 1, 3));
    }

    /*
      Calculate Jacobians for the three rotational (angular) constraints.
    */

    MeVector3Cross( tmp, ref_Iw[1],  ref_Jw[2]);
    for (i = 0; i < 3; i++) KEAJ(0, 2, 3 + i) = tmp[i];
    if (joint->head.bodyindex[1] != MdtBclNO_BODY)
        for (i = 0; i < 3; i++) KEAJ(1, 2, 3 + i) = -tmp[i];

    MeVector3Cross( tmp, ref_Iw[2],  ref_Jw[0]);
    for (i = 0; i < 3; i++) KEAJ(0, 3, 3 + i) = tmp[i];
    if (joint->head.bodyindex[1] != MdtBclNO_BODY)
        for (i = 0; i < 3; i++) KEAJ(1, 3, 3 + i) = -tmp[i];

    MeVector3Cross( tmp, ref_Iw[0],  ref_Jw[1]);
    for (i = 0; i < 3; i++) KEAJ(0, 4, 3 + i) = tmp[i];
    if (joint->head.bodyindex[1] != MdtBclNO_BODY)
        for (i = 0; i < 3; i++) KEAJ(1, 4, 3 + i) = -tmp[i];

    /*
      No limits on lagrange multipliers
    */

    for (i = 0; i < 5; i++)
    {
        lo[i] = -MEINFINITY;
        hi[i] = MEINFINITY;
    }

    /*
      Set joint power and joint limits, if a limit has been overshot:
    */

    if( SetSingleAxisActuation( &joint->limit, joint->head.bodyindex, ref_Iw[0],
            RotationalAxis1, RotationalAxis2, blist, &factors, 5, clist, (MdtBclSolverParameters*)params ) )
    {
        /*
          Limited prismatic joint places restrictions on sixth degree of freedom.
        */
        MdtBclEndConstraint(clist, 6);
    }
    else
    {
        /*
          Default prismatic joint removes 5 degrees of freedom.
        */
        MdtBclEndConstraint(clist, 5);
    }

}

/**
 * Add a Car Wheel constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclCarWheel struct.
 * @param tlist Array of MdtKeaTransformation structs for the array
 * of bodies.
 * @param blist Array of MdtKeaBody struct that are reference by the
 * constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */
void MEAPI MdtBclAddCarWheel(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;

    ZEROROW(0);
    ZEROROW(1);
    ZEROROW(2);
    ZEROROW(3);
    ZEROROW(4);
    ZEROROW(5);

    MeMatrix4 ref1world, ref2world;

    MdtCarWheel* joint = (MdtCarWheel*)constraint;

    /* Map constraint reference frames into world reference frame. */
    ConvertRefFramesToWorld(&joint->head, tlist, ref1world, ref2world);

    int i;
    MeVector3 a, b, tmp;
    MeVector3 at1;

    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, joint);

    MeVector3Subtract(at1,ref1world[3],tlist[joint->head.bodyindex[0]].pos);
    MeVector3PlaneSpace(ref1world[0], a, b);

    MeVector3Cross( tmp, at1,  ref1world[0]);

    for (i = 0; i < 3; i++)
        KEAJ(0, 0, i) = ref1world[0][i];
    for (i = 0; i < 3; i++)
        KEAJ(1, 0, i) = -ref1world[0][i];
    for (i = 0; i < 3; i++)
        KEAJ(0, 0, 3 + i) = tmp[i];

    MeVector3Cross( tmp, at1,  a);

    for (i = 0; i < 3; i++)
        KEAJ(0, 1, i) = a[i];
    for (i = 0; i < 3; i++)
        KEAJ(1, 1, i) = -a[i];
    for (i = 0; i < 3; i++)
        KEAJ(0, 1, 3 + i) = tmp[i];

    MeVector3Cross( tmp, at1,  b);

    for (i = 0; i < 3; i++)
        KEAJ(0, 2, i) = b[i];
    for (i = 0; i < 3; i++)
        KEAJ(1, 2, i) = -b[i];
    for (i = 0; i < 3; i++)
        KEAJ(0, 2, 3 + i) = tmp[i];

    /*
      Compute position error.
    */
    for (i = 0; i < 3; i++)
        tmp[i] = (ref1world[3][i] - ref2world[3][i]);

    xi[0] = MeVector3Dot(tmp, ref1world[0]);
    xi[1] = MeVector3Dot(tmp, a);
    xi[2] = MeVector3Dot(tmp, b);

    /*
      No limits on lagrange multipliers.
    */
    for (i = 0; i < 3; i++)
    {
        lo[i] = -MEINFINITY;
        hi[i] = MEINFINITY;
    }

    int limit = 0;

    if (xi[0] > -(joint->slo))
    {
        /*
          Hit suspension low limit.
        */
        xi[0] += joint->slo;
        hi[0] = 0;
        limit = -1;
    }
    else if (xi[0] < -(joint->shi))
    {
        /*
          Hit suspension high limit.
        */
        xi[0] += joint->shi;
        lo[0] = 0;
        limit = 1;
    }
    else
    {
        xi[0] += joint->sref;

        MeReal hepsilon =
            (MeReal)(1.0) / (((MdtBclSolverParameters*)params)->stepsize * joint->skp + joint->skd);

        slipfactor[0] = hepsilon;
        if(slipfactor[0] < 0)
        {
#ifdef _MECHECK
            //MeWarning(0, "MdtBclAddCarWheel: Negative slipfactor.");
#endif
            slipfactor[0] = 0;
        }
        xgamma[0] = hepsilon * ((MdtBclSolverParameters*)params)->stepsize * joint->skp;
    }

    if (limit)
    {
        slipfactor[0] = joint->slsoft;
        if(slipfactor[0] < 0)
        {
#ifdef _MECHECK
            //MeWarning(0, "MdtBclAddCarWheel: Negative slipfactor.");
#endif
            slipfactor[0] = 0;
        }

        MeReal f = (limit == 1)
            ? ((joint->shi - joint->sref) * joint->skp)
            : ((joint->slo - joint->sref) * joint->skp);

        for (i = 0; i < 3; i++)
            tmp[i] = ref1world[0][i] * f;

        blist[joint->head.bodyindex[0]]->force[0] += tmp[0];
        blist[joint->head.bodyindex[0]]->force[1] += tmp[1];
        blist[joint->head.bodyindex[0]]->force[2] += tmp[2];

        if( joint->head.bodyindex[1] != -1 )
        {
            blist[joint->head.bodyindex[1]]->force[0] -= tmp[0];
            blist[joint->head.bodyindex[1]]->force[1] -= tmp[1];
            blist[joint->head.bodyindex[1]]->force[2] -= tmp[2];            
        }
       
        MeVector3 torque;

        MeVector3Cross(torque, at1, tmp);

        blist[joint->head.bodyindex[0]]->torque[0] += torque[0];
        blist[joint->head.bodyindex[0]]->torque[1] += torque[1];
        blist[joint->head.bodyindex[0]]->torque[2] += torque[2];
    }

    /*
      Constraint to keep the wheel vertical.

      The position error is s'*h where s is the steering axis and h is
      the hinge axis (inertial frame). Thus s and h should be kept
      perpendicular.
    */

    MeVector3Cross( tmp, ref2world[1],  ref1world[0]);

    for (i = 0; i < 3; i++)
        KEAJ(0, 3, 3 + i) = -tmp[i];
    for (i = 0; i < 3; i++)
        KEAJ(1, 3, 3 + i) = tmp[i];

    xi[3] = MeVector3Dot(ref2world[1], ref1world[0]);
    lo[3] = -MEINFINITY;
    hi[3] = MEINFINITY;

    /*
      Actuated steering axis.
    */

    for (i = 0; i < 3; i++)
        KEAJ(0, 4, 3 + i) = ref1world[0][i];
    for (i = 0; i < 3; i++)
        KEAJ(1, 4, 3 + i) = -ref1world[0][i];

    c[4]  = -(joint->svel);
    lo[4] = -(joint->sfmax);
    hi[4] = joint->sfmax;

    if (joint->slock)
    {
        MeVector3Cross( tmp, ref2world[1],  ref1world[1]);
        xi[4] = MeVector3Dot(tmp, ref1world[0]);
    }

    if (joint->hfmax <= 0.0001f)
        /*
          Increment row counters by 5
        */
        MdtBclEndConstraint(clist, 5);
    else
    {
        /*
          Actuated wheel hinge axis.
        */
        for (i = 0; i < 3; i++)
            KEAJ(0, 5, 3 + i) = ref2world[1][i];
        for (i = 0; i < 3; i++)
            KEAJ(1, 5, 3 + i) = -ref2world[1][i];

        c[5] = -(joint->hvel);
        lo[5] = -(joint->hfmax);
        hi[5] = joint->hfmax;

        /*
          Increment row counters by 6
        */
        MdtBclEndConstraint(clist, 6);
    }
}


/**
 * Add a Fixed Path constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclFixedPath struct.
 * @param tlist Array of MdtKeaTransformation structs for the array
 * of bodies.
 * @param blist Array of MdtKeaBody struct that are reference by
 * the constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */
void MEAPI MdtBclAddFixedPath(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;

    ZEROROW(0);
    ZEROROW(1);
    ZEROROW(2);


    MdtFixedPath* joint = (MdtFixedPath*)constraint;
    MeMatrix4 ref1world, ref2world;

    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, joint);

    /* Map constraint reference frames into world reference frame. */
    ConvertRefFramesToWorld(&joint->head, tlist, ref1world, ref2world);

    /*
      Add 3 constraint rows to constrain the position.
    */
    MdtBclAddSphericalRowsWorld(clist, tlist[joint->head.bodyindex[0]].pos,
        tlist[joint->head.bodyindex[1]].pos, ref1world[3], ref2world[3],
        (joint->head.bodyindex[1] != MdtBclNO_BODY),
        joint->head.worldLinVel, joint->head.worldAngVel);

    unsigned int i;

    MeVector4 temp, negc;

    MeVector3Copy( temp, joint->vel1 ); temp[3] = 0;
    MeMatrix4MultiplyVector( negc, ref1world, temp);

    if (joint->head.bodyindex[1] == MdtBclNO_BODY)
        for (i = 0; i < 3; i++)
            negc[i] -= joint->vel2[i];
    else
    {
        MeVector4 v2;

        MeVector3Copy( temp, joint->vel2 ); temp[3] = 0;
        MeMatrix4MultiplyVector( v2, ref2world, temp);

        for (i = 0; i < 3; i++)
            negc[i] -= v2[i];
    }

    /*
      Set the kinematic constraint vector 'c' which is the right hand
      side of the constraint equation: J*v=c.
    */
    for (i = 0; i < 3; i++)
        c[i] = -negc[i];

    /*
      Increment row counters by 3.
    */
    MdtBclEndConstraint(clist, 3);
}


/**
 * Add a Universal Joint constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclUniversal struct.
 * @param tlist Array of MdtKeaTransformation structs for the array
 * of bodies.
 * @param blist Array of MdtKeaBody struct that are reference by
 * the constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */
void MEAPI MdtBclAddUniversal(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;

    ZEROROW(0);
    ZEROROW(1);
    ZEROROW(2);
    ZEROROW(3);

    MdtUniversal* joint = (MdtUniversal*)constraint;
    MeMatrix4 ref1world, ref2world;

    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, joint);

    /* Map constraint reference frames into world reference frame. */
    ConvertRefFramesToWorld(&joint->head, tlist, ref1world, ref2world);

    /* Add 3 constraint rows to constrain the position. */
    MdtBclAddSphericalRowsWorld(clist, tlist[joint->head.bodyindex[0]].pos,
        tlist[joint->head.bodyindex[1]].pos, ref1world[3], ref2world[3],
        (joint->head.bodyindex[1] != MdtBclNO_BODY),
        joint->head.worldLinVel, joint->head.worldAngVel);


    MeVector3 universalPerpendicular;

    /*  This constraint keeps the x axis in the body 1 frame and the y axis
        in the body 2 frame orthogonal. */
    MeVector3Cross(universalPerpendicular, ref1world[0],  ref2world[1]);

    unsigned int i = 0;

    for (i = 0; i < 3; i++)
        KEAJ(0, 3, i + 3) = universalPerpendicular[i];

    if (joint->head.bodyindex[1] != MdtBclNO_BODY)
        for (i = 0; i < 3; i++)
            KEAJ(1, 3, i + 3) = -universalPerpendicular[i];

    /* No limits on the lagrange multiplier for the torque term: */
    lo[3] = -MEINFINITY;
    hi[3] = MEINFINITY;

    /* Lastly, set the error term for the relative position of the hinge axes. */
    xi[3] = MeVector3Dot(ref1world[0], ref2world[1]);

    /* Increment rows added counter by 4 */
    MdtBclEndConstraint(clist, 4);
}

/**
 * Add a Skeletal Joint constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtSkeletal struct.
 * @param tlist Array of MdtKeaTransformation structs for the array
 * of bodies.
 * @param blist Array of MdtKeaBody struct that are reference by
 * the constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */
void MEAPI MdtBclAddSkeletal(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;

    ZEROROW(0);
    ZEROROW(1);
    ZEROROW(2);

    MeI32 nRows = 3; // We'll at least have the spherical rows

    MdtSkeletal* joint = (MdtSkeletal*)constraint;
    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, joint); 

    MeMatrix4 Racw, Rbcw;

    MeVector4 qaca, qbcb, qacw, qbcw, qrel;
    MeMatrix4 qmul;
    MeMatrix3 Rrel;
    unsigned int i;
    unsigned int j;

    MeQuaternionFromTM(qaca, joint->head.ref1);
    MeQuaternionProduct(qacw, joint->head.mdtbody[0]->keaBody.qrot, qaca);
    MeQuaternionToTM(Racw, qacw);
    MeMatrix4TMTransform(Racw[3],*(MeMatrix4*)&tlist[joint->head.bodyindex[0]].R0,joint->head.ref1[3]);
    if (joint->head.bodyindex[1] != MdtBclNO_BODY) {
        MeQuaternionFromTM(qbcb, joint->head.ref2);
        MeQuaternionProduct(qbcw, joint->head.mdtbody[1]->keaBody.qrot, qbcb);
        MeQuaternionToTM(Rbcw, qbcw);
        MeMatrix4TMTransform(Rbcw[3],*(MeMatrix4*)&tlist[joint->head.bodyindex[1]].R0,joint->head.ref2[3]);
    } else {
        MeMatrix4Copy(Rbcw, joint->head.ref2);
        MeQuaternionFromTM(qbcw, Rbcw);
    }
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            qmul[i][j] = qacw[i]*qbcw[j];

    qrel[0] =  qmul[0][0] + qmul[1][1] + qmul[2][2] + qmul[3][3];
    qrel[1] = -qmul[1][0] + qmul[0][1] + qmul[3][2] - qmul[2][3];
    qrel[2] = -qmul[2][0] - qmul[3][1] + qmul[0][2] + qmul[1][3];
    qrel[3] = -qmul[3][0] + qmul[2][1] - qmul[1][2] + qmul[0][3];

    MeQuaternionToR(Rrel, qrel);

    /* Add 3 constraint rows to constrain the position. */
    MdtBclAddSphericalRowsWorld(clist, tlist[joint->head.bodyindex[0]].pos,
        tlist[joint->head.bodyindex[1]].pos, Racw[3], Rbcw[3],
        (joint->head.bodyindex[1] != MdtBclNO_BODY),
        joint->head.worldLinVel, joint->head.worldAngVel);

    /* twist limit */
    if (joint->twistOption != MdtSkeletalTwistOptionFree)
    {
        unsigned int i;

        MeVector3 JwQ1;
        /* Jw = (1/2)[ -q3 q2 -q1 q0 ]acw  [....] , eq 11, p8 of Specification */
        JwQ1[0] = (MeReal)0.5 * (qmul[1][1] + qmul[0][0] - qmul[3][3] - qmul[2][2]);
        JwQ1[1] = (MeReal)0.5 * (qmul[1][2] + qmul[0][3] + qmul[3][0] + qmul[2][1]);
        JwQ1[2] = (MeReal)0.5 * (qmul[1][3] - qmul[0][2] + qmul[3][1] - qmul[2][0]);

        /* Twist limit. */
        if (joint->twistOption == MdtSkeletalTwistOptionFixed)
        {
            // Twist == 0 constraint
            ZEROROW(nRows); 

            for (i = 0; i < 3; i++) {
                KEAJ(0, nRows, i) = 0;
                KEAJ(0, nRows, i + 3) = JwQ1[i];
            }

            if (joint->head.bodyindex[1] != MdtBclNO_BODY)
                for (i = 0; i < 3; i++) {
                    KEAJ(1, nRows, i) = 0;
                    KEAJ(1, nRows, i + 3) = -JwQ1[i];
                }
            /*
               No limits on the lagrange multiplier for the torque term:
             */
            lo[nRows] = -MEINFINITY;
            hi[nRows] = MEINFINITY;

            /*
              Lastly, set the error term for the relative position of the hinge
              axes.  The axes may not have been set to be perpendicular, so the
              error term is the difference between the current relative
              orientation of the two axes and their original relative
              orientation:
            */
            xi[nRows] = -qrel[1];

            ++nRows;
        } 
        else
        {
            // -twist_max <= twist <= twist_max constraint
            MeReal CSquared = joint->cos_half_twist_limit_angle*joint->cos_half_twist_limit_angle;
            MeReal SSquared = 1-CSquared;
            MeReal PositionError = -SSquared*qrel[0]*qrel[0]+CSquared*qrel[1]*qrel[1];
            if (PositionError > 0)
            {
                MeVector3 JwQ0;
                MeVector3 Jw;

                ZEROROW(nRows); 

                /* Jw = (1/2)[ q0 q1 q2 q3 ]acw  [....] , eq 5, p6 of Specification */
                JwQ0[0] = (MeReal)0.5 * (-qmul[0][1] + qmul[1][0] - qmul[2][3] + qmul[3][2]);
                JwQ0[1] = (MeReal)0.5 * (-qmul[0][2] + qmul[1][3] + qmul[2][0] - qmul[3][1]);
                JwQ0[2] = (MeReal)0.5 * (-qmul[0][3] - qmul[1][2] + qmul[2][1] + qmul[3][0]);

                for (i = 0; i < 3; i++) {
                    KEAJ(0, nRows, i) = 0;
                    KEAJ(0, nRows, i + 3) =
                        Jw[i] = 2*SSquared*qrel[0]*JwQ0[i]-2*CSquared*qrel[1]*JwQ1[i];
                }

                if (joint->head.bodyindex[1] != MdtBclNO_BODY)
                    for (i = 0; i < 3; i++) {
                        KEAJ(1, nRows, i) = 0;
                        KEAJ(1, nRows, i + 3) = -Jw[i];
                    }

                lo[nRows] = -MEINFINITY;
                hi[nRows] = 0;

                xi[nRows] = PositionError;

                ++nRows;
            }
        }
    }

    // cone limit

    // TEMP: Ignore 'slot' option at the moment.
    if (joint->coneOption != MdtSkeletalConeOptionFree && joint->coneOption != MdtSkeletalConeOptionSlot)
    {
        unsigned int i;
        if (joint->coneOption == MdtSkeletalConeOptionCone)
        {
            /* qcla: quarter cone limit angle */
            MeReal r10_div_sq_tan_qcla1 = Rrel[1][0]*
                                         (1+joint->cos_half_cone_limit_angle_1)/
                                         (1-joint->cos_half_cone_limit_angle_1);
            MeReal r20_div_sq_tan_qcla2 = Rrel[2][0]*
                                         (1+joint->cos_half_cone_limit_angle_2)/
                                         (1-joint->cos_half_cone_limit_angle_2);
            MeReal sinhalfswing = MeSqrt((1-Rrel[0][0])/2);
            MeReal coshalfswing = MeSqrt((1+Rrel[0][0])/2);
            MeReal recip_one_plus_coshalfswing = 1/(1+coshalfswing);
            MeReal sinswing = 2*sinhalfswing*coshalfswing;
            MeReal recip_elliptic = 1/
                  (Rrel[1][0]*r10_div_sq_tan_qcla1+Rrel[2][0]*r20_div_sq_tan_qcla2);
            MeReal sqrt_recip_elliptic = MeSqrt(recip_elliptic);

            MeReal PositionError = sinhalfswing*recip_one_plus_coshalfswing -
                                       sinswing*sqrt_recip_elliptic;

            if (PositionError > 0)
            {
                MeVector3 accum;
                MeVector3 Jcone;
                MeReal ssellip3_2 = sinswing*recip_elliptic*sqrt_recip_elliptic;
                MeVector3MultiplyScalar(accum,Rbcw[0],
                  (Rrel[0][0]*sqrt_recip_elliptic-recip_one_plus_coshalfswing/2)/sinswing);
                MeVector3MultiplyAdd(accum,ssellip3_2*r10_div_sq_tan_qcla1,Rbcw[1]);
                MeVector3MultiplyAdd(accum,ssellip3_2*r20_div_sq_tan_qcla2,Rbcw[2]);
                MeVector3Cross(Jcone,accum,Racw[0]);
            
                ZEROROW(nRows);

                for (i = 0; i < 3; i++) {
                    KEAJ(0, nRows, i) = 0;
                    KEAJ(0, nRows, i + 3) = Jcone[i];
                }

                if (joint->head.bodyindex[1] != MdtBclNO_BODY)
                    for (i = 0; i < 3; i++) {
                        KEAJ(1, nRows, i) = 0;
                        KEAJ(1, nRows, i + 3) = -Jcone[i];
                    }
                /* Constraint position error */
                xi[nRows] = -PositionError;
                /* unilateral limits on the lagrange multipliers */
                lo[nRows] = 0;
                hi[nRows] = MEINFINITY;
                /* Springing and damping using epsilon and gamma */
                MeReal hepsilon = MeRecip(((MdtBclSolverParameters*)params)->stepsize * joint->cone_stiffness + joint->cone_damping);
                slipfactor[nRows]=hepsilon;
                if(slipfactor[nRows] < 0)
                {
#ifdef _MECHECK
                    //MeWarning(0, "MdtBclAddSkeletal: Negative slipfactor.");
#endif
                    slipfactor[nRows] = 0;
                }

                xgamma[nRows] = hepsilon * ((MdtBclSolverParameters*)params)->stepsize * joint->cone_stiffness;

                ++nRows;
            }
        }

        else if(joint->coneOption == MdtSkeletalConeOptionFixed)
        {
            // "Fixed" means angle == 0, at least for now.
        }

    }

    MdtBclEndConstraint(clist, nRows);
}

/**
 * Add a Linear1 constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclLinear1 struct.
 * @param tlist Array of MdtKeaTransformation structs for the array
 * of bodies.
 * @param blist Array of MdtKeaBody struct that are reference by
 * the constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */
void MEAPI MdtBclAddLinear1(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;

    ZEROROW(0);

    MdtLinear1* joint = (MdtLinear1*)constraint;

    unsigned int i = 0;

    /*
      Calculate normal and displacement vectors in world reference
      frame:
    */
    MeVector4 normal;
    MeVector3 displacement;

    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, joint);

    if (joint->head.bodyindex[1] != MdtBclNO_BODY)
    {
        MeMatrixMultiply( normal, 4,  3,  1,  (MeReal *) tlist[joint->head.bodyindex[1]].R0,  joint->pos2);

        for (i = 0; i < 3; i++)
            displacement[i] =
                tlist[joint->head.bodyindex[0]].pos[i] -
                tlist[joint->head.bodyindex[1]].pos[i];
    }
    else
    {
        for (i = 0; i < 3; i++)
        {
            normal[i] = joint->pos2[i];
            displacement[i] = tlist[joint->head.bodyindex[0]].pos[i];
        }
    }

    for (i = 0; i < 3; i++)
        KEAJ(0, 0, i) = normal[i];

    if (joint->head.bodyindex[1] != MdtBclNO_BODY)
    {
        MeVector3 CrossProduct;

        MeVector3Cross( CrossProduct, normal,  displacement);

        for (i = 0; i < 3; i++)
        {
            KEAJ(1, 0, i) = -normal[i];
            KEAJ(1, 0, i + 3) = CrossProduct[i];
        }
    }

    /*
      No limits on the lagrange multiplier:
    */
    lo[0] = -MEINFINITY;
    hi[0] = MEINFINITY;

    /*
      Set the error term:
    */
    xi[0] = MeVector3Dot(normal, displacement) - joint->displacement;

    /*
      Increment row counter by 1
    */
    MdtBclEndConstraint(clist, 1);
}

/**
 * Add a Linear2 constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclLinear2 struct.
 * @param tlist Array of MdtKeaTransformation structs for the
 * array of bodies.
 * @param blist Array of MdtKeaBody struct that are reference
 * by the constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */
void MEAPI MdtBclAddLinear2(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;

    ZEROROW(0);
    ZEROROW(1);

    MdtLinear2* joint = (MdtLinear2*)constraint;

    MeVector4 vector1, vector2, rel_pos, initial_pos;
    MeVector3 abs_pos, displacement;

    unsigned int i = 0;

    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, joint);

    /*
      Calculate vec1, vec2 and joint initial position in inertial
      reference frame:
    */
    if (joint->head.bodyindex[1] == MdtBclNO_BODY)
        for (i = 0; i < 3; i++)
        {
            vector1[i] = joint->vec1[i];
            vector2[i] = joint->vec2[i];
            initial_pos[i] = joint->pos2[i];
        }
    else
    {
        MeMatrixMultiply( vector1, 4,  3,  1,  (MeReal *) tlist[joint->head.bodyindex[1]].R0,  joint->vec1);
        MeMatrixMultiply( vector2, 4,  3,  1,  (MeReal *) tlist[joint->head.bodyindex[1]].R0,  joint->vec2);
        MeMatrixMultiply( initial_pos, 4,  3,  1,  (MeReal *) tlist[joint->head.bodyindex[1]].R0,  joint->pos2);

        for (i = 0; i < 3; i++)
            initial_pos[i] += tlist[joint->head.bodyindex[1]].pos[i];
    }

    /*
      Calculate position of joint relative to body[0], in inertial frame
      coordinates.
    */
    MeMatrixMultiply( rel_pos, 4,  3,  1,  (MeReal *) tlist[joint->head.bodyindex[0]].R0,  joint->pos1);

    /*
      Calculate absolute joint position, using bodyindex[0] data, and
      also the displacement of the joint from its original position
      (using body[1] data):
    */
    for (i = 0; i < 3; i++)
    {
        abs_pos[i] = rel_pos[i] + tlist[joint->head.bodyindex[0]].pos[i];
        displacement[i] = abs_pos[i] - initial_pos[i];
    }

    MeVector3 Perpendicular1, Perpendicular2;

    MeVector3Cross( Perpendicular1, rel_pos,  vector1);
    MeVector3Cross( Perpendicular2, rel_pos,  vector2);

    for (i = 0; i < 3; i++)
    {
        KEAJ(0, 0, i) = vector1[i];
        KEAJ(0, 1, i) = vector2[i];
        KEAJ(0, 0, i + 3) = Perpendicular1[i];
        KEAJ(0, 1, i + 3) = Perpendicular2[i];
    }

    if (joint->head.bodyindex[1] != MdtBclNO_BODY)
    {
        for (i = 0; i < 3; i++)
            abs_pos[i] -= tlist[joint->head.bodyindex[1]].pos[i];

        MeVector3Cross( Perpendicular1, vector1,  abs_pos);
        MeVector3Cross( Perpendicular2, vector2,  abs_pos);

        for (i = 0; i < 3; i++)
        {
            KEAJ(1, 0, i) = -vector1[i];
            KEAJ(1, 1, i) = -vector2[i];
            KEAJ(1, 0, i + 3) = Perpendicular1[i];
            KEAJ(1, 1, i + 3) = Perpendicular2[i];
        }
    }

    /*
      No limits on the lagrangian multipliers:
    */
    lo[0] = -MEINFINITY;
    hi[0] = MEINFINITY;
    lo[1] = -MEINFINITY;
    hi[1] = MEINFINITY;

    /*
      Set the error terms:
    */
    xi[0] = MeVector3Dot(displacement, vector1);
    xi[1] = MeVector3Dot(displacement, vector2);

    /*
      Increment row counter by 2
    */
    MdtBclEndConstraint(clist, 2);
}

/**
 * Add an Angular3 constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclAngular3 struct.
 * @param tlist Array of MdtKeaTransformation structs for the
 * array of bodies.
 * @param blist Array of MdtKeaBody struct that are reference
 * by the constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */
void MEAPI MdtBclAddAngular3(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;

    ZEROROW(0);
    ZEROROW(1);

    MdtAngular3* joint = (MdtAngular3*)constraint;
    MeMatrix3 Racw, Rbcw;
    MeReal hepsilon;

//    ConvertRefFramesToWorld(&joint->head, tlist, ref1world, ref2world);

    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, joint);

    MeVector4 qaca, qbcb, qacw, qbcw, qrel;
    MeMatrix4 qmul;
    MeMatrix3 Rrel;
    unsigned int i;
    unsigned int j;

    MeQuaternionFromTM(qaca, joint->head.ref1);
    MeQuaternionProduct(qacw, joint->head.mdtbody[0]->keaBody.qrot, qaca);
    if (joint->head.bodyindex[1] != MdtBclNO_BODY) {
        MeQuaternionFromTM(qbcb, joint->head.ref2);
        MeQuaternionProduct(qbcw, joint->head.mdtbody[1]->keaBody.qrot, qbcb);
        MeQuaternionToR(Rbcw, qbcw);
    } else {
        MeQuaternionFromTM(qbcw, joint->head.ref2);
        MeMatrix4TMGetRotation(Rbcw, joint->head.ref2);
    }
    MeQuaternionToR(Racw, qacw);

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            qmul[i][j] = qacw[i]*qbcw[j];

    qrel[0] =  qmul[0][0] + qmul[1][1] + qmul[2][2] + qmul[3][3];
    qrel[1] = -qmul[1][0] + qmul[0][1] + qmul[3][2] - qmul[2][3];
    qrel[2] = -qmul[2][0] - qmul[3][1] + qmul[0][2] + qmul[1][3];
    qrel[3] = -qmul[3][0] + qmul[2][1] - qmul[1][2] + qmul[0][3];

    MeQuaternionToR(Rrel, qrel);

    MeReal two_r00p1 = 2*(Rrel[0][0]+1);

    if (two_r00p1 > ME_SMALL_EPSILON) {

        MeVector3 t0, t1, t2;
        MeReal Sqrt2_r00p1 = MeSqrt(two_r00p1);
        MeReal Recip_roop1pSqrt2_r00p1 = 1/((Rrel[0][0]+1) + Sqrt2_r00p1);
        MeReal multfac =
            (1+Sqrt2_r00p1)/((Rrel[0][0]+1)*(2*two_r00p1+(3+Rrel[0][0])*Sqrt2_r00p1));

        /* Set the error terms: */
        xi[0] = Rrel[1][0]*Recip_roop1pSqrt2_r00p1;
        xi[1] = Rrel[2][0]*Recip_roop1pSqrt2_r00p1;

        MeVector3Cross(t0, Racw[0], Rbcw[0]);
        MeVector3Cross(t1, Racw[0], Rbcw[1]);
        MeVector3Cross(t2, Racw[0], Rbcw[2]);
        
        MeVector3Scale(t1, Recip_roop1pSqrt2_r00p1);
        MeVector3MultiplySubtract(t1, Rrel[1][0]*multfac, t0);

        MeVector3Scale(t2, Recip_roop1pSqrt2_r00p1);
        MeVector3MultiplySubtract(t2, Rrel[2][0]*multfac, t0);
        
        for (i = 0; i < 3; i++) {
            KEAJ(0, 0, i + 3) = t1[i];
            KEAJ(0, 1, i + 3) = t2[i];
        }

        if (joint->head.bodyindex[1] != MdtBclNO_BODY)
        {
            for (i = 0; i < 3; i++) {
                KEAJ(1, 0, i + 3) = -t1[i];
                KEAJ(1, 1, i + 3) = -t2[i];
            }
        }
        
        /* No limits on the lagrangian multipliers: */
        lo[0] = -MEINFINITY;
        hi[0] = MEINFINITY;
        lo[1] = -MEINFINITY;
        hi[1] = MEINFINITY;

        hepsilon = 1 / ((((MdtBclSolverParameters*)params)->stepsize * joint->stiffness) + joint->damping);

        slipfactor[0] = hepsilon;
        if(slipfactor[0] < 0)
        {
#ifdef _MECHECK
            //MeWarning(0, "MdtBclAddAngular3: Negative slipfactor.");
#endif
            slipfactor[0] = 0;
        }
        
        slipfactor[1] = hepsilon;
        if(slipfactor[1] < 0)
        {
#ifdef _MECHECK
            ////MeWarning(0, "MdtBclAddAngular3: Negative slipfactor.");
#endif
            slipfactor[1] = 0;
        }


        xgamma[0] = hepsilon * ((MdtBclSolverParameters*)params)->stepsize * joint->stiffness;
        xgamma[1] = hepsilon * ((MdtBclSolverParameters*)params)->stepsize * joint->stiffness;
    }

    /*
      Constrain rotation about the joint axis only if the bEnableRotation
      flag is cleared.
    */
    if( joint->bEnableRotation )
    {
        MdtBclEndConstraint(clist, 2);
    }
    else
    {
        ZEROROW(2);

        MeVector3 JwQ1;
        JwQ1[0] = (MeReal)0.5 * (qmul[1][1] + qmul[0][0] - qmul[3][3] - qmul[2][2]);
        JwQ1[1] = (MeReal)0.5 * (qmul[1][2] + qmul[0][3] + qmul[3][0] + qmul[2][1]);
        JwQ1[2] = (MeReal)0.5 * (qmul[1][3] - qmul[0][2] + qmul[3][1] - qmul[2][0]);

        for (i = 0; i < 3; i++) {
            KEAJ(0, 2, i + 3) = JwQ1[i];
        }

        if (joint->head.bodyindex[1] != MdtBclNO_BODY)
        {
            for (i = 0; i < 3; i++) {
                KEAJ(1, 2, i + 3) = -JwQ1[i];
            }
        }
        lo[2] = -MEINFINITY;
        hi[2] = MEINFINITY;
        xi[2] = -qrel[1];

        slipfactor[2] = hepsilon;
        if(slipfactor[2] < 0)
        {
#ifdef _MECHECK
            //MeWarning(0, "MdtBclAddAngular3: Negative slipfactor.");
#endif
            slipfactor[2] = 0;
        }

        xgamma[2] = hepsilon * ((MdtBclSolverParameters*)params)->stepsize * joint->stiffness;

        MdtBclEndConstraint(clist, 3);
    }
 }


/**
 * Add an Spring6 constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclAngular3 struct.
 * @param tlist Array of MdtKeaTransformation structs for the
 * array of bodies.
 * @param blist Array of MdtKeaBody struct that are reference
 * by the constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */
void MEAPI MdtBclAddSpring6(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    MdtSpring6* spring;
    unsigned int i = 0;
    MeMatrix4 ref1world, ref2world;
    MeVector3 cpos1rel, xiWorld;
    MdtBclSolverParameters* bclParams = (MdtBclSolverParameters*)params;
    FINDSTARTROW;
    
    ZEROROW(0);
    ZEROROW(1);
    ZEROROW(2);
    ZEROROW(3);
    ZEROROW(4);
    ZEROROW(5);

    spring = (MdtSpring6*)constraint;

    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, spring);

    /* Map constraint reference frames into world reference frame. */
    ConvertRefFramesToWorld(&spring->head, tlist, ref1world, ref2world);

	/*	Use the second bodies reference frame to define springy directions */
	for(i=0; i<3; i++)
	{
		KEAJ(0, i, 0) = ref2world[i][0];
		KEAJ(0, i, 1) = ref2world[i][1];
		KEAJ(0, i, 2) = ref2world[i][2];
	}

    /* work out vector from body1 to constraint (world frame) */
#if 0
    MeVector3Subtract(cpos1rel, ref1world[3],  tlist[spring->head.bodyindex[0]].pos);
#else
    MeVector3Set(cpos1rel, 0, 0, 0);
#endif

    /* cross product to form Jacobean */
    MdtBclMathCrossMatrixToJstore(cpos1rel, 1, &KEAJ(0, 0, 3), &KEAJ(0, 1, 3), &KEAJ(0, 2, 3));

    /* Compute position error in world ref frame. */
    MeVector3Subtract(xiWorld, ref1world[3], ref2world[3]);

	/* Convert to Jacobean ref frame. */
    MeMatrix4TMInverseRotate(xi, ref2world, xiWorld);
    MeMatrix4TMInverseRotate(c, ref2world, spring->worldLinearVel);

#if 0
    /* See if we have a second body (ie. body-body joint not body-world). */
    if(spring->head.bodyindex[1] != MdtBclNO_BODY)
    {
        MeVector3 cpos2rel;

		for(i=0; i<3; i++)
		{
			KEAJ(1, i, 0) = -ref2world[i][0];
			KEAJ(1, i, 1) = -ref2world[i][1];
			KEAJ(1, i, 2) = -ref2world[i][2];
		}

        /* vector from body2 to constraint (world frame) */
        MeVector3Subtract( cpos2rel, ref2world[3],  tlist[spring->head.bodyindex[1]].pos);

        MdtBclMathCrossMatrixToJstore(cpos2rel, -1, &KEAJ(1, 0, 3), &KEAJ(1, 1, 3), &KEAJ(1, 2, 3));
    }
#endif 
    
    /* No limits on lagrange multipliers (no force limits). */
    for (i = 0; i < 3; i++)
    {
        lo[i] = -MEINFINITY;
        hi[i] = MEINFINITY;
    }

	MeVector3 linHEpsilon;
	linHEpsilon[0] = 1 / ((bclParams->stepsize * spring->linearStiffness[0]) + spring->linearDamping[0]);
	linHEpsilon[1] = 1 / ((bclParams->stepsize * spring->linearStiffness[1]) + spring->linearDamping[1]);
	linHEpsilon[2] = 1 / ((bclParams->stepsize * spring->linearStiffness[2]) + spring->linearDamping[2]);

    slipfactor[0] = linHEpsilon[0];
    slipfactor[1] = linHEpsilon[1];
    slipfactor[2] = linHEpsilon[2];

    for(i=0; i<3; i++)
    {
        if(slipfactor[i] < 0)
        {
#ifdef _MECHECK
            //MeWarning(0, "MdtBclAddSpring6: Negative slipfactor.");
#endif
            slipfactor[i] = 0;
        }
    }

    xgamma[0] = linHEpsilon[0] * bclParams->stepsize * spring->linearStiffness[0];
    xgamma[1] = linHEpsilon[1] * bclParams->stepsize * spring->linearStiffness[1];
    xgamma[2] = linHEpsilon[2] * bclParams->stepsize * spring->linearStiffness[2];
    
	/*** BEGIN ANGULAR 3 ***/
	MdtBclMathCrossToJstore(ref1world[0], ref2world[2], &KEAJ(0, 3, 3));
    MdtBclMathCrossToJstore(ref1world[0], ref2world[1], &KEAJ(0, 4, 3));
    MdtBclMathCrossToJstore(ref1world[1], ref2world[2], &KEAJ(0, 5, 3));

    if( spring->head.bodyindex[1] != MdtBclNO_BODY )
    {
        MdtBclMathCrossToJstore(ref2world[2], ref1world[0], &KEAJ(1, 3, 3));
        MdtBclMathCrossToJstore(ref2world[1], ref1world[0], &KEAJ(1, 4, 3));
		MdtBclMathCrossToJstore(ref2world[2], ref1world[1], &KEAJ(1, 5, 3));
    }
    /* No limits on the lagrangian multipliers: */
    lo[3] = -MEINFINITY;
    hi[3] = MEINFINITY;
    lo[4] = -MEINFINITY;
    hi[4] = MEINFINITY;
    lo[5] = -MEINFINITY;
    hi[5] = MEINFINITY;

    /* Set the error terms: */
    xi[3] = MeVector3Dot(ref1world[0], ref2world[2]);
    xi[4] = MeVector3Dot(ref1world[0], ref2world[1]);
    xi[5] = MeVector3Dot(ref1world[1], ref2world[2]);

	for(i=0; i<3; i++)
		c[3+i] = MeVector3Dot(spring->worldAngularVel, ref2world[i]);

	MeVector3 angHEpsilon;
	angHEpsilon[0] = 1 / ((bclParams->stepsize * spring->angularStiffness[0]) + spring->angularDamping[0]);
	angHEpsilon[1] = 1 / ((bclParams->stepsize * spring->angularStiffness[1]) + spring->angularDamping[1]);
	angHEpsilon[2] = 1 / ((bclParams->stepsize * spring->angularStiffness[2]) + spring->angularDamping[2]);

    slipfactor[3] = angHEpsilon[0];
    slipfactor[4] = angHEpsilon[1];
    slipfactor[5] = angHEpsilon[2];

    for(i=3; i<6; i++)
    {
        if(slipfactor[i] < 0)
        {
#ifdef _MECHECK
            //MeWarning(0, "MdtBclAddSpring6: Negative slipfactor.");
#endif
            slipfactor[i] = 0;
        }
    }

    xgamma[3] = angHEpsilon[0] * bclParams->stepsize * spring->angularStiffness[0];
    xgamma[4] = angHEpsilon[1] * bclParams->stepsize * spring->angularStiffness[1];
    xgamma[5] = angHEpsilon[2] * bclParams->stepsize * spring->angularStiffness[2];

    /* Increment the rows added counter etc. */
    MdtBclEndConstraint(clist, 6);
}

/**
 * Add a Spring constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclAngular3 struct.
 * @param tlist Array of MdtKeaTransformation structs for the
 * array of bodies.
 * @param blist Array of MdtKeaBody struct that are reference
 * by the constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */
void MEAPI MdtBclAddSpring(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;

    ZEROROW(0);

    MdtSpring* joint = (MdtSpring*)constraint;

    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, joint);

    /*
      Calculate spring attachment position and relative velocity
      in inertial reference frame:
    */
    if( joint->limit.bCalculatePosition )
    {
        MeVector4 abs_offset1;
        MeVector3 abs_pos1, SpringAxis, velocity;
        MeVector3 RotationalAxis1, RotationalAxis2;

        MeMatrixMultiply( abs_offset1, 4,  3,  1,  (MeReal *) tlist[joint->head.bodyindex[0]].R0,  joint->pos1);
        MeVector3Add( abs_pos1 ,  abs_offset1,  (MeReal *) tlist[joint->head.bodyindex[0]].pos);

        if( joint->head.bodyindex[1] == MdtBclNO_BODY )
        {
            /* RotationalAxis2 is required only if there is a second body.  */
            MeVector3Subtract( SpringAxis ,  abs_pos1,  joint->pos2);
            MeVector3Copy( velocity ,  blist[ joint->head.bodyindex[0] ]->vel);
        }
        else
        {
            MeVector4 abs_offset2;
            MeVector3 abs_pos2;
            MeMatrixMultiply( abs_offset2, 4,  3,  1,  (MeReal *) tlist[joint->head.bodyindex[1]].R0,  joint->pos2);
            MeVector3Add( abs_pos2 ,  abs_offset2,  (MeReal *) tlist[joint->head.bodyindex[1]].pos);
            MeVector3Subtract( SpringAxis ,  abs_pos1,  abs_pos2);
            MeVector3Cross( RotationalAxis2 ,  abs_offset2,  SpringAxis);
            MeVector3Subtract( velocity ,  blist[ joint->head.bodyindex[0] ]->vel,  blist[ joint->head.bodyindex[1] ]->vel);
        }
        MeVector3Cross( RotationalAxis1 ,  abs_offset1,  SpringAxis);

        joint->limit.position = MeSqrt( MeVector3MagnitudeSqr( SpringAxis ) );
        MeVector3Normalize( SpringAxis );
        joint->limit.velocity = MeVector3Dot( SpringAxis, velocity );
        CalculateLimitPositionAndOvershoot( &joint->limit, ((MdtBclSolverParameters*)params)->stepsize, false );

        if( SetSingleAxisActuation( &joint->limit, joint->head.bodyindex, SpringAxis,
                RotationalAxis1, RotationalAxis2, blist, &factors, 0, clist, (MdtBclSolverParameters*)params ) )
        {
            MdtBclEndConstraint(clist, 1);
        }
        else
        {
            /* Add bodies to MdtKeaConstraints struct */
            ADDBODIES(clist, joint);
            
            KEAJ(0, 0, 0) = 0;
            KEAJ(0, 0, 1) = 0;
            KEAJ(0, 0, 2) = 0;
            KEAJ(0, 0, 3) = 0;
            KEAJ(0, 0, 4) = 0;
            KEAJ(0, 0, 5) = 0;
            
            if (joint->head.bodyindex[1] != MdtBclNO_BODY)
            {
                KEAJ(1, 0, 0) = 0;
                KEAJ(1, 0, 1) = 0;
                KEAJ(1, 0, 2) = 0;
                KEAJ(1, 0, 3) = 0;
                KEAJ(1, 0, 4) = 0;
                KEAJ(1, 0, 5) = 0;
            }

            xi[0] = 0;

            lo[0] = -MEINFINITY;
            hi[0] = MEINFINITY;
            
            MdtBclEndConstraint(clist, 1);
        }
    }
 }


/**
 * Add a Cone Limit constraint to a MdtKeaConstraints struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclUniversal struct.
 * @param tlist Array of MdtKeaTransformation structs for the array
 * of bodies.
 * @param blist Array of MdtKeaBody struct that are reference by
 * the constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */
void MEAPI MdtBclAddConeLimit(MdtKeaConstraints *const clist,
    void *const constraint, const MdtKeaTransformation * const tlist,
    MdtKeaBody *const blist[], const void *const params)
{
    MeMatrix4 ref1world, ref2world;
    MeVector3 cross;
    FINDSTARTROW;

    ZEROROW(0);

    MdtConeLimit* joint = (MdtConeLimit*)constraint;


    /* Map constraint reference frames into world reference frame. */
    ConvertRefFramesToWorld(&joint->head, tlist, ref1world, ref2world);

    MeReal coneDot = MeVector3Dot(ref1world[0], ref2world[0]);

    if(joint->cos_cone_half_angle > coneDot)
    {
       /* Add bodies to MdtKeaConstraints struct */
       ADDBODIES(clist, joint);

       coneDot = MeCLAMP(coneDot, -1, 1);
       MeReal angle = MeAcos(coneDot);

       MeVector3Cross(cross, ref1world[0],  ref2world[0]);
       MeVector3Normalize(cross);

       KEAJ(0, 0, 0) = 0;
       KEAJ(0, 0, 1) = 0;
       KEAJ(0, 0, 2) = 0;
       KEAJ(0, 0, 3) = cross[0];
       KEAJ(0, 0, 4) = cross[1];
       KEAJ(0, 0, 5) = cross[2];

       if (joint->head.bodyindex[1] != MdtBclNO_BODY)
       {
         KEAJ(1, 0, 0) = 0;
         KEAJ(1, 0, 1) = 0;
         KEAJ(1, 0, 2) = 0;
         KEAJ(1, 0, 3) = -cross[0];
         KEAJ(1, 0, 4) = -cross[1];
         KEAJ(1, 0, 5) = -cross[2];
       }
       /*
          Constraint error is angular difference:
       */
       xi[0] = joint->cone_half_angle - angle;
       /*
          No limits on the lagrange multiplier for the torque term:
       */
       lo[0] = 0;
       hi[0] = MEINFINITY;
       /*
          Springing and damping using epsilon and gamma:
       */
       MeReal hepsilon =
           (MeReal)(1.0) / (((MdtBclSolverParameters*)params)->stepsize * joint->stiffness + joint->damping);
       slipfactor[0] = hepsilon;
       if(slipfactor[0] < 0)
       {
#ifdef _MECHECK
           //MeWarning(0, "MdtBclAddConeLimit: Negative slipfactor.");
#endif
           slipfactor[0] = 0;
       }

       xgamma[0] = hepsilon * ((MdtBclSolverParameters*)params)->stepsize * joint->stiffness;

       /*
         Increment rows added counter by 1
       */
       MdtBclEndConstraint(clist, 1);
    }
    else
    {
       /* Add bodies to MdtKeaConstraints struct */
       ADDBODIES(clist, joint);

       KEAJ(0, 0, 0) = 0;
       KEAJ(0, 0, 1) = 0;
       KEAJ(0, 0, 2) = 0;
       KEAJ(0, 0, 3) = 0;
       KEAJ(0, 0, 4) = 0;
       KEAJ(0, 0, 5) = 0;

       if (joint->head.bodyindex[1] != MdtBclNO_BODY)
       {
         KEAJ(1, 0, 0) = 0;
         KEAJ(1, 0, 1) = 0;
         KEAJ(1, 0, 2) = 0;
         KEAJ(1, 0, 3) = 0;
         KEAJ(1, 0, 4) = 0;
         KEAJ(1, 0, 5) = 0;
       }
       /*
          Constraint error is angular difference:
       */
       xi[0] = 0;
       /*
          No limits on the lagrange multiplier for the torque term:
       */
       lo[0] = -MEINFINITY;
       hi[0] = MEINFINITY;

       /*
         Increment rows added counter by 1
       */
       MdtBclEndConstraint(clist, 1);
    }
}



/*
  Add rows necessary to completely contrain the position of a body
  relative to another. Adds 3 rows (x,y,z).
*/
void MdtBclAddSphericalRows(MdtKeaConstraints *const clist,
    const MeReal *const body0trans, const MeReal *const body1trans,
    const MeReal *const pos0, const MeReal *const pos1,
    const int body1Present)
{
    FINDSTARTROW;
    int i;

    ZEROROW(0);
    ZEROROW(1);
    ZEROROW(2);

    KEAJ(0, 0, 0) = 1;
    KEAJ(0, 1, 1) = 1;
    KEAJ(0, 2, 2) = 1;

    MeVector4 at1;

    MeMatrixMultiply(at1, 4, 3, 1, body0trans, pos0);

    MdtBclMathCrossMatrixToJstore(at1, 1,
        &KEAJ(0, 0, 3), &KEAJ(0, 1, 3), &KEAJ(0, 2, 3));

    if (!body1Present)
        /* Compute position error. */
        for (i = 0; i < 3; i++)
            xi[i] = (at1[i] + body0trans[12 + i]) - pos1[i];
    else
    {
        KEAJ(1, 0, 0) = -1;
        KEAJ(1, 1, 1) = -1;
        KEAJ(1, 2, 2) = -1;

        MeVector4 at2;

    MeMatrixMultiply(at2, 4, 3, 1, body1trans, pos1);
        MdtBclMathCrossMatrixToJstore(at2, -1,
            &KEAJ(1, 0, 3), &KEAJ(1, 1, 3), &KEAJ(1, 2, 3));

        /* Compute position error */
        for (i = 0; i < 3; i++)
            xi[i] = (at1[i] + body0trans[12 + i])
                - (at2[i] + body1trans[12 + i]);
    }

    /*
      No limits on lagrange multipliers.
    */
    for (i = 0; i < 3; i++)
    {
        lo[i] = -MEINFINITY;
        hi[i] = MEINFINITY;
    }
}



/**
 * Add a Relative Position Relative Orientation constraint to a MdtKeaConstraints
 * struct.
 *
 * @param clist The MdtKeaConstraints struct to add constraint to.
 * @param constraint Pointer to a MdtBclRPROJoint struct.
 * @param tlist Array of MdtKeaTransformation structs for the array
 * of bodies.
 * @param blist Array of MdtKeaBody struct that are reference by
 * the constraint.
 * @param stepsize Time step that the system is going to be evolved by.
 */

void MEAPI MdtBclAddRPROJoint(MdtKeaConstraints *const clist,
                              void *const constraint, const MdtKeaTransformation * const tlist,
                              MdtKeaBody *const blist[], const void *const params)
{
    FINDSTARTROW;

    ZEROROW(0);
    ZEROROW(1);
    ZEROROW(2);
    ZEROROW(3);
    ZEROROW(4);
    ZEROROW(5);

    
    MdtRPROJoint* joint = (MdtRPROJoint*)constraint;
    MeReal EMatrixB1[3][4];
    MeReal GMatrixB2[3][4];

    MeReal GEt[3][3];
    MeReal Gr[3][4];
    MeReal Eq[3][4];
    MeReal GrEqt[3][3];
    MeVector4 qr;
    MeVector4 ejej0;
    int i,j;

    /* Add bodies to MdtKeaConstraints struct */
    ADDBODIES(clist, joint);
    MdtBclAddSphericalRows(clist, tlist[joint->head.bodyindex[0]].R0,
        tlist[joint->head.bodyindex[1]].R0,
        joint->head.ref1[3], joint->head.ref2[3],
        joint->head.bodyindex[1] != MdtBclNO_BODY);


    /*
       should set the relative velocity here.
     */

    if(joint->use_q1)
    {
        MeQuaternionProduct(ejej0, blist[joint->head.bodyindex[0]]->qrot, joint->q1);
    }
    else
    {
        MeVector4Copy(ejej0, blist[joint->head.bodyindex[0]]->qrot);
    }

    GetEMatrix(EMatrixB1, ejej0);
    if (joint->head.bodyindex[1] != MdtBclNO_BODY)
    {
        if(joint->use_q2)
        {

            MeVector4 q2Adj;
            MeVector4 q20Adj;
            MeVector4 tmp;
            MeVector4 tmp1;
            MeVector4 eie0er;

            q2Adj[0] = blist[joint->head.bodyindex[1]]->qrot[0];
            q2Adj[1] = -blist[joint->head.bodyindex[1]]->qrot[1];
            q2Adj[2] = -blist[joint->head.bodyindex[1]]->qrot[2];
            q2Adj[3] = -blist[joint->head.bodyindex[1]]->qrot[3];

            q20Adj[0] = joint->q2[0];
            q20Adj[1] = -joint->q2[1];
            q20Adj[2] = -joint->q2[2];
            q20Adj[3] = -joint->q2[3];

            MeQuaternionProduct(tmp, joint->q2, joint->q_rel);
            MeQuaternionProduct(eie0er, blist[joint->head.bodyindex[1]]->qrot,tmp);

            MeQuaternionProduct(tmp, blist[joint->head.bodyindex[0]]->qrot, joint->q1);
            MeQuaternionProduct(tmp1, q2Adj, tmp);
            MeQuaternionProduct(qr, q20Adj, tmp1);

            GetGMatrix(GMatrixB2,eie0er);

        }
        else
        {
            MeVector4 q2Adj;
            MeVector4 eier;
            q2Adj[0] = blist[joint->head.bodyindex[1]]->qrot[0];
            q2Adj[1] = -blist[joint->head.bodyindex[1]]->qrot[1];
            q2Adj[2] = -blist[joint->head.bodyindex[1]]->qrot[2];
            q2Adj[3] = -blist[joint->head.bodyindex[1]]->qrot[3];

            MeQuaternionProduct(qr, q2Adj,blist[joint->head.bodyindex[0]]->qrot);
            MeQuaternionProduct(eier, blist[joint->head.bodyindex[1]]->qrot,joint->q_rel);
            GetGMatrix(GMatrixB2,eier);
        }
    }
    else
    {
        if(joint->use_q1)
        {
            MeQuaternionProduct(qr, blist[joint->head.bodyindex[0]]->qrot, joint->q1 );
        }
        else
        {
            MeVector4Copy(qr, blist[joint->head.bodyindex[0]]->qrot );
        }
        GetGMatrix(GMatrixB2,joint->q_rel);
    }

    MyMultiplyT2(3,4,3,(MeReal *)GMatrixB2,(MeReal *)EMatrixB1, (MeReal *)GEt);
    for(i=0;i<3;i++)
    {
        for(j=0;j<3;j++)
        {
            GEt[i][j]*=0.5;
        }
    }


    /*
    Calculate Jacobians
    */

    for (i = 0; i < 3; i++)
    {
        unsigned int j;

        for (j = 0; j < 3; ++j)
            KEAJ(0, i + 3, j + 3) = GEt[i][j];
    }

    if (joint->head.bodyindex[1] != MdtBclNO_BODY)
    {
        for (i = 0; i < 3; i++)
        {
            unsigned int j;

            for (j = 0; j < 3; ++j)
                KEAJ(1, i + 3, j + 3) = -GEt[i][j];
        }
    }

    /*
    Orientation error.
    */
    GetGMatrix(Gr,joint->q_rel);
    MeReal err[3];
    MyMultiplyT2(3,4,1,Gr[0],qr,err);
    xi[3] = err[0]; xi[4] = err[1]; xi[5] = err[2];

    /*
    Set the angular components of the kinematic constraint vector 'c'.
    */
    GetEMatrix(Eq,qr);
    MyMultiplyT2(3,4,3,Gr[0],Eq[0],GrEqt[0]);
    MyMultiplyT2(3,3,1,GrEqt[0],joint->omega,c+3);
    MeVector3Scale(&c[3],(MeReal)0.5);

    /*
    Set limits on lagrangian multipliers.
    */
    for (i = 0; i < 3; i++)
    {
        lo[i] = -joint->linear_fmax[i];
        hi[i] = joint->linear_fmax[i];
        if(hi[i] == 0.0f)
        {
            int j;
            xi[i] = 0.0f;
            for(j = 0; j < 3; j++)
            {
                KEAJ(0,i,j) = 0.0;
                if(joint->head.bodyindex[1] != MdtBclNO_BODY)
                    KEAJ(1,i,j) = 0.0;
            }
        }
    }
    for (i=3; i < 6; i++)
    {
        lo[i] = -joint->angular_fmax[i-3];
        hi[i] = joint->angular_fmax[i-3];
        if(hi[i] == 0.0f)
        {
            int j;
            xi[i] = 0.0f;
            for(j = 3; j < 6; j++)
            {
                KEAJ(0,i,j) = 0.0;
                if(joint->head.bodyindex[1] != MdtBclNO_BODY)
                    KEAJ(1,i,j) = 0.0;
            }
        }
    }


    /*
    Increment rows added counter by 6.
    */
    MdtBclEndConstraint(clist, 6);
}

