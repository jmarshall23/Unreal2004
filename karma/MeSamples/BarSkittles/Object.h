#ifndef _OBJECT_H_
#define _OBJECT_H_
/*
 * $Id: Object.h,v 1.1 2001/11/08 14:54:35 harveyt Exp $
 */

/*
 * Properties of all the objects.  All measurements are in units of g, cm,
 * and s.
 */
#define PLANE_LENGTH		((MeReal)96.0)
#define PLANE_TRIANGLES		(4)
#define PLANE_Y			((MeReal)0.0)
#define PLANE_COLOR		color_blue

#define BALL_RADIUS		((MeReal)2.0)
#define BALL_DENSITY		((MeReal)3.0)
#define BALL_COLOR		color_white
#define BALL_CLEARANCE		(BALL_RADIUS / TWO)
#define BALL_OFFSET		(TOWER_OFFSET + TOWER_RADIUS + BALL_RADIUS)

#define BASE_BORDER		(BALL_OFFSET + (MeReal)3.0)
#define BASE_HEIGHT		((MeReal)6.0)
#define BASE_DENSITY		((MeReal)2.0)
#define BASE_COLOR		color_white

#define TOWER_RADIUS		((MeReal)0.5)
#define TOWER_HEIGHT		((MeReal)50.0)
#define TOWER_DENSITY		((MeReal)0.9)
#define TOWER_COLOR		color_white
#define TOWER_OFFSET		(SKITTLE_RADIUS + (MeReal)5.0)

#define	SKITTLE_RADIUS		((MeReal)1.5)
#define SKITTLE_HEIGHT		((MeReal)10.0)
#define SKITTLE_DENSITY		((MeReal)0.9)
#define SKITTLE_COLOR		color_white
#define SKITTLE_START_X		((MeReal)0.0)
#define SKITTLE_START_Y		((MeReal)0.0)
#define SKITTLE_START_Z		((MeReal)0.0)
#define SKITTLE_SPACING		((MeReal)7.0)

/*
 * How to organise skittles given a skittle number between 0 and NSKITTLES-1.
 */
#define SKITTLE_NE(i)	((MeReal)((i) % SKITTLE_PER_SIDE) * RSQRT2)
#define SKITTLE_NW(i)	((MeReal)((i) / SKITTLE_PER_SIDE) * RSQRT2)
#define SKITTLE_IX(i)	(SKITTLE_NE(i) - SKITTLE_NW(i))
#define SKITTLE_IZ(i)	(SKITTLE_NE(i) + SKITTLE_NW(i))

#ifdef bug
#define SKITTLE_XFUDGE	((MeReal)0.0)
#else
#define SKITTLE_XFUDGE	((MeReal)0.01)
#endif
#define SKITTLE_X(i) \
	(SKITTLE_START_X + SKITTLE_SPACING * SKITTLE_IX(i) + SKITTLE_XFUDGE)
#define SKITTLE_Y(i) \
	(SKITTLE_START_Y + (SKITTLE_HEIGHT / TWO) + BASE_HEIGHT)
#define SKITTLE_Z(i) \
	(SKITTLE_START_Y + SKITTLE_SPACING * SKITTLE_IZ(i))

/*
 * Object indentities.
 */
#define PLANE0			(0)
#define NPLANES			(1)

#define BALL0			(PLANE0 + NPLANES)
#define NBALLS			(1)

#define BASE0			(BALL0 + NBALLS)
#define NBASES			(1)

#define TOWER0			(BASE0 + NBASES)
#define NTOWERS			(1)

#define SKITTLE0		(TOWER0 + NTOWERS)
#define NSKITTLES		(SKITTLE_PER_SIDE * SKITTLE_PER_SIDE)

#define NOBJECTS  \
	(NPLANES + NBALLS + NBASES + NTOWERS + NSKITTLES)

/*
 * Physical object, with geometry, collision, optional dynamics body, etc.
 */
typedef struct object {
	MeVector3	reset_position;	/* position on reset */
	MeVector4	reset_rotation;	/* rotation quaternion on reset */
	McdGeometryID	geom;		/* geometry of object */
	McdModelID	cm;		/* collision model */
	MdtBodyID	body;		/* body or 0 if none */
	RGraphic *	graphic;	/* graphic */
	int		fallen;		/* non-zero if object fallen over */
#ifdef SHOW_UP
	RGraphic *	upG;		/* graphic for up vector */
#endif /* SHOW_UP */
} object_t;

/*
 * Actual objects themselves.
 */
extern object_t object[NOBJECTS];

/*
 * All the valid geometries we will use for all the objects.
 */
#define GEOM_PLANE	(0)
#define GEOM_BALL	(1)
#define GEOM_BASE	(2)
#define GEOM_TOWER	(3)
#define GEOM_SKITTLE	(4)

#define NGEOMS		(5)

/*
 * Prototypes.
 */
void object_init(void);
void object_finish(void);
void object_reset(void);

void object_init_plane(object_t *);
void object_init_ball(object_t *);
void object_init_base(object_t *);
void object_init_tower(object_t *);
void object_init_skittle(object_t *, int);
void object_reset_item(object_t *);
void object_reset_item_damping(object_t *, MeReal, MeReal);

#endif /* _OBJECT_H_ */
