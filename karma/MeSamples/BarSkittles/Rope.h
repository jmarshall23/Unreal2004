#ifndef _ROPE_H_
#define _ROPE_H_
/*
 * $Id: Rope.h,v 1.1 2001/11/08 14:54:35 harveyt Exp $
 */

#define ROPE_RADIUS	((MeReal)0.2)
#define ROPE_SEPERATION	((MeReal)0.4)
/*
 * We have to make the rope dense, otherwise it becomes unstable.
 * #define ROPE_DENSITY	(MeReal)1.0 
 */
#define ROPE_DENSITY	((MeReal)20.0)
#define ROPE_STIFFNESS	((MeReal)50000.0)
#define ROPE_DAMPING	((MeReal)10.0)
#define ROPE_COLOR	color_white
#define NROPES		(10)

#define ROPE_DAMPING_LINEAR_VEL		((MeReal)0.3)
#define ROPE_DAMPING_ANGULAR_VEL	((MeReal)100.0)

/*
 * Define this for a very simple rigid rope.
 */
// #define ROPE_SIMPLE

/*
 * Otherwise must define one of each of these.
 * Determines how rope segments are connected.
 * They are ordered in from least to most suitable.
 */
// #define ROPE_SPRING
// #define ROPE_BSJOINT
#define ROPE_UNIVERSAL

/*
 * Define this if the rope should be represented as solid, hiding
 * the collision geometries seperation.
 */
#define ROPE_SOLID

/*
 * Prototypes.
 */
void rope_init(void);
void rope_reset(void);
void rope_finish(void);

#endif /* _ROPE_H_ */
