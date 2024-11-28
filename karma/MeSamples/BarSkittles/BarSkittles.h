#ifndef _BARSKITTLES_H_
#define _BARSKITTLES_H_

// #define DYNAMIC_TOWER	/* tower is dynamic object not static */
// #define DYNAMIC_BASE		/* base is dynamic object not static */
// #define SHOW_UP	/* show skittles up vector until fallen */

/*
 * Constants.
 * 
 * Note: RSQRT2 = one over square root two = cos(45).
 */
#define TWO		((MeReal)2.0)
#define RSQRT2		((MeReal)0.70710678118654752440084436210485)

/*
 * General game constants.
 */
#define SKITTLE_PER_SIDE	(3)	/* sqrt of how many skittles */
#define START_NUM_TRIES		(10)	/* how many attempts to knock over */
#define STRIKE_EXTRA_TRIES	(1)	/* extra tries if all over in one */
#define FRAME_RESET		(90)	/* how many frames before reset */

/*
 * General physical properties.
 */
#define FRICTION		((MeReal)1000.0)
#define RESTITUTION		((MeReal)0.4)
#define EPSILON			((MeReal)0.0001)
#define GRAVITY			((MeReal)-98.0)
#define DAMPING_LINEAR_VEL	((MeReal)0.03)
#define DAMPING_ANGULAR_VEL	((MeReal)0.02)

/*
 * Default frame time in seconds.
 */
#define DEFAULT_STEP		((MeReal)1.0 / (MeReal)60.0)

/*
 * Global values.
 */
extern MeVector4	q_identity;
extern MeVector4	q_standup;
extern MeVector4	q_rev;
extern MeVector4	q_standup_rev;
extern MeMatrix4	tm_identity;
extern MeVector3	v_flat;
extern MeVector3	v_up;

/*
 * Prototypes.
 */
extern void MEAPI launch(RRender *, void *);
extern void MEAPI reset(RRender *, void *);

#endif /* _BARSKITTLES_H_ */
