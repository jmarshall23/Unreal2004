#ifndef _VIEWER_H_
#define _VIEWER_H_
/*
 * $Id: Viewer.h,v 1.1 2001/11/08 14:54:35 harveyt Exp $
 */

/*
 * Globals.
 */
extern RRender		*rc;
extern MeApp		*meapp;
extern MeVector4	color_white;
extern MeVector4	color_green;
extern MeVector4	color_blue;

/*
 * Prototypes.
 */
void viewer_init(void);
void viewer_finish(void);
void viewer_tick(void);

#endif /* _VIEWER_H_ */
