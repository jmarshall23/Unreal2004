#ifndef _UNIVERSE_H_
#define _UNIVERSE_H_
/*
 * $Id: Universe.h,v 1.1 2001/11/08 14:54:35 harveyt Exp $
 */

/*
 * Universe sizes.
 */
#define NCOLLISION_MODELS \
	(NOBJECTS + NROPES)
#define NCOLLISION_PAIRS \
	(NCOLLISION_MODELS * 16)
#define NDYNAMIC_OBJECTS \
	(NCOLLISION_MODELS)
#define NCONTACTS \
	(NDYNAMIC_OBJECTS * 16)

/*
 * Globals.
 */
extern MstUniverseID	universe;
extern MdtWorldID	world;
extern MeVector3	gravity;
extern MeReal		step;

/*
 * Prototypes.
 */
void universe_init(void);
void universe_finish(void);
void universe_tick(void);

#endif /* _UNIVERSE_H_ */
