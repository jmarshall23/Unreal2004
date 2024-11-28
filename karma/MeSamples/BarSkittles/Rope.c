/*
 * $Id: Rope.c,v 1.1 2001/11/08 14:54:35 harveyt Exp $
 */
#include "MeMath.h"
#include "MeViewer.h"
#include "MeApp.h"
#include "Mst.h"

#include "BarSkittles.h"
#include "Universe.h"
#include "Viewer.h"
#include "Game.h"
#include "Object.h"
#include "Rope.h"

/*
 * A rope segment.
 */
typedef struct rope {
#if defined(ROPE_SIMPLE)
	MdtBSJointID	joint;
	RGraphic *	graphic;
#else
	object_t	object;		/* the object for this rope segment */
#  if defined(ROPE_SPRING)
	MdtSpringID	joint;		/* previous segment to join to */
#  elif defined(ROPE_BSJOINT)
	MdtBSJointID	joint;		/* previous segment to join to */
#  elif defined(ROPE_UNIVERSAL)
	MdtUniversalID	joint;		/* previous segment to join to */
#  endif
#endif
} rope_t;

rope_t		rope[NROPES];

#ifdef ROPE_SIMPLE

static void
rope_segment_init_simple(rope_t *	rp,
			 object_t *	ballp,
			 object_t *	towerp)
{
	MeMatrix4Ptr	tmp;
	AcmeReal	start[3], end[3];

	/*
	 * Join the ball to the top of the tower.
	 */
	rp->joint = MdtBSJointCreate(world);
	MdtBSJointSetBodies(rp->joint, ballp->body, 0);
	MdtBSJointSetPosition(rp->joint,
			      towerp->reset_position[0],
			      towerp->reset_position[1] + (TOWER_HEIGHT / TWO),
			      towerp->reset_position[2]);
	MdtBSJointEnable(rp->joint);

	/*
	 * Create the line for the rope.
	 */
	start[0] = start[1] = start[2] = (AcmeReal)0.0;
	end[0] = (AcmeReal)(towerp->reset_position[0]
			    - ballp->reset_position[0]);
	end[1] = (AcmeReal)(towerp->reset_position[1] + (TOWER_HEIGHT / TWO)
			    - ballp->reset_position[1]);
	end[2] = (AcmeReal)(towerp->reset_position[2]
			    - ballp->reset_position[2]);
	tmp = McdModelGetTransformPtr(ballp->cm);
	rp->graphic = RGraphicLineCreate(rc, start, end, ROPE_COLOR, tmp);
}

#else /* !ROPE_SIMPLE */

McdGeometryID	rope_geom;		/* rope geometry */
MeReal		rope_length;		/* length of each rope segment */
MeVector3	rope_dir;		/* normalised rope direction */
MeVector4	rope_q;			/* quaternion to rotate rope by */

/*
 * Initialise a rope segment.
 */
static void
rope_segment_init(rope_t *	rp,
		  int		i)
{
	MeReal		s;
	MeVector3	pos, jpos1;
#ifdef ROPE_SPRING
	MeVector3	jpos2;
#endif
	MeMatrix4Ptr	tmp;
	object_t	*op, *pop;

	/*
	 * Calculate the joint position required for the rope joint to
	 * the previous object.
	 */
	s = BALL_RADIUS	+ rope_length * (MeReal)i;
#ifdef ROPE_SPRING
	s -= ROPE_SEPERATION / TWO;
	MeVector3Copy(jpos1, object[BALL0].reset_position);
	MeVector3MultiplyAdd(jpos1, s, rope_dir);
	s += ROPE_SEPERATION;
	MeVector3Copy(jpos2, object[BALL0].reset_position);
	MeVector3MultiplyAdd(jpos2, s, rope_dir);
#else
	MeVector3Copy(jpos1, object[BALL0].reset_position);
	MeVector3MultiplyAdd(jpos1, s, rope_dir);
#endif

	/*
	 * Calculate the centre of the rope segment.
	 */
	s += rope_length / TWO;
	MeVector3Copy(pos, object[BALL0].reset_position);
	MeVector3MultiplyAdd(pos, s, rope_dir);

	/*
	 * Check for joint from last rope segment to tower.
	 */
	if (i == NROPES) {
#if defined(ROPE_SPRING)
		MdtSpringID	joint;

		joint = MdtSpringCreate(world);
		MdtSpringSetBodies(joint, rope[NROPES-1].object.body, 0);
		MdtSpringSetPosition(joint,
				     0, jpos1[0], jpos1[1], jpos1[2]);
		MdtSpringSetPosition(joint,
				     1,
				     object[TOWER0].reset_position[0],
				     object[TOWER0].reset_position[1]
				     + TOWER_HEIGHT / TWO,
				     object[TOWER0].reset_position[2]);
		MdtSpringSetStiffness(joint, ROPE_STIFFNESS);
		MdtSpringSetDamping(joint, ROPE_DAMPING);
		MdtSpringEnable(joint);
#elif defined(ROPE_BSJOINT)
		MdtBSJointID	joint;

		joint = MdtBSJointCreate(world);
		MdtBSJointSetBodies(joint, rope[NROPES-1].object.body, 0);
		MdtBSJointSetPosition(joint, jpos1[0], jpos1[1], jpos1[2]);
		MdtBSJointEnable(joint);
#elif defined(ROPE_UNIVERSAL)
		MdtUniversalID	joint;

		joint = MdtUniversalCreate(world);
		MdtUniversalSetBodies(joint, rope[NROPES-1].object.body, 0);
		MdtUniversalSetPosition(joint, jpos1[0], jpos1[1], jpos1[2]);
		MdtUniversalEnable(joint);
#endif
		return;
	}

	/*
	 * Initialise the rope segment.
	 */
	op = &(rp->object);
	MeVector3Copy(op->reset_position, pos);
	MeVector4Copy(op->reset_rotation, rope_q);
	op->geom = rope_geom;
	op->cm = MstModelAndBodyCreate(universe, rope_geom, ROPE_DENSITY);
	op->body = McdModelGetBody(op->cm);
	tmp = MdtBodyGetTransformPtr(op->body);
	op->graphic = RGraphicCylinderCreate(rc, ROPE_RADIUS,
#ifdef ROPE_SOLID
					     rope_length,
#else
					     rope_length - ROPE_SEPERATION,
#endif
					     ROPE_COLOR, tmp);

	/*
	 * Last rope segment doesn't collide with tower top.
	 */
	if (i == NROPES-1)
		McdSpaceDisablePair(op->cm, object[TOWER0].cm);

	/*
	 * Reset to correct position before we join it up.
	 */
	object_reset_item_damping(op,
				  ROPE_DAMPING_LINEAR_VEL,
				  ROPE_DAMPING_ANGULAR_VEL);

	/*
	 * Now tie the rope segment to the previous object.
	 */
	if (i == 0)
		pop = &(object[BALL0]);
	else
		pop = &(rope[i-1].object);

#if defined(ROPE_SPRING)
	rp->joint = MdtSpringCreate(world);
	MdtSpringSetBodies(rp->joint, pop->body, op->body);
	MdtSpringSetPosition(rp->joint, 0, jpos1[0], jpos1[1], jpos1[2]);
	MdtSpringSetPosition(rp->joint, 1, jpos2[0], jpos2[1], jpos2[2]);
	MdtSpringSetStiffness(rp->joint, ROPE_STIFFNESS);
	MdtSpringSetDamping(rp->joint, ROPE_DAMPING);
	MdtSpringEnable(rp->joint);
#elif defined(ROPE_BSJOINT)
	rp->joint = MdtBSJointCreate(world);
	MdtBSJointSetBodies(rp->joint, pop->body, op->body);
	MdtBSJointSetPosition(rp->joint, jpos1[0], jpos1[1], jpos1[2]);
	MdtBSJointEnable(rp->joint);
#elif defined(ROPE_UNIVERSAL)
	rp->joint = MdtUniversalCreate(world);
	MdtUniversalSetBodies(rp->joint, pop->body, op->body);
	MdtUniversalSetPosition(rp->joint, jpos1[0], jpos1[1], jpos1[2]);
	MdtUniversalEnable(rp->joint);
#endif
}

static void
rope_preinit()
{
	MeVector3	ball, tower;

	/*
	 * Compute the vector from the ball centre to the tower top centre,
	 * offset by the rope and tower radius.
	 */
	MeVector3Copy(ball, object[BALL0].reset_position);
	tower[0] = object[TOWER0].reset_position[0];
	tower[1] = object[TOWER0].reset_position[1] + (TOWER_HEIGHT / TWO);
	tower[2] = object[TOWER0].reset_position[2];
	MeVector3Subtract(rope_dir, tower, ball);

	/*
	 * Compute the length of the rope, and normalise the direction.
	 */
	rope_length = MeVector3Magnitude(rope_dir);
	// MeSqrt(MeSqr(dir[0]) + MeSqr(dir[1]) + MeSqr(dir[2]));
	MeVector3Scale(rope_dir, 1 / rope_length);

	/*
	 * Now work out how long a segment of rope will be.
	 */
	rope_length -= BALL_RADIUS;
	rope_length /= (MeReal)NROPES;

	/*
	 * Calculate rope_q - for rotating ropes correctly.
	 */
	MeQuaternionForRotation(rope_q, v_flat, rope_dir);

	/*
	 * Create the geometry we will use.  We reduce this by the seperation
	 * so that they do not collide with each other.
	 */
	rope_geom = McdCylinderCreate(ROPE_RADIUS,
				      rope_length - ROPE_SEPERATION);
}

#endif /* !ROPE_SIMPLE */

/*
 * Initialise the rope.
 */
void
rope_init()
{
#ifdef ROPE_SIMPLE
	rope_segment_init_simple(&(rope[0]), &(object[BALL0]),
				 &(object[TOWER0]));
#else
	int	i;

	/*
	 * Now create all the rope segments in the correct positions.
	 */
	rope_preinit();
	for (i = 0; i <= NROPES; i++)
		rope_segment_init(&(rope[i]), i);
#endif
}

/*
 * Reset the rope.
 */
void
rope_reset()
{
#ifndef ROPE_SIMPLE
	int	i;

	for (i = 0; i < NROPES; i++)
		object_reset_item_damping(&(rope[i].object),
					  ROPE_DAMPING_LINEAR_VEL,
					  ROPE_DAMPING_ANGULAR_VEL);
#endif
}

/*
 * Finish using rope.
 */
void
rope_finish()
{
#ifndef ROPE_SIMPLE
	McdGeometryDestroy(rope_geom);
#endif
}
