#ifndef _CURSOR_H_
#define _CURSOR_H_
/*
 * $Id: Cursor.h,v 1.1 2001/11/08 14:54:35 harveyt Exp $
 */

/*
 * Constants for the cursor.
 */
#define CURSOR_SIZE		(BALL_RADIUS * TWO)
#define CURSOR_TRANSLATE_STEP	((MeReal)1.0)
#define CURSOR_ROTATE_STEP	((MeReal)5.0 * (MeReal)ME_PI / (MeReal)180.0)
#define CURSOR_POWER_STEP	((MeReal)1.0)
#define CURSOR_POWER_MIN	((MeReal)10.0)
#define CURSOR_POWER_MAX	((MeReal)100.0)
#define CURSOR_POWER_SCALE	((MeReal)3.0)
#define CURSOR_POWER_X		((AcmeReal)5.0)
#define CURSOR_POWER_Y		((AcmeReal)360.0)
#define CURSOR_POWER_MOUSE	((AcmeReal)5.0)

#define CURSOR_STIFFNESS	((MeReal)15.0)
#define CURSOR_DAMPING		((MeReal)6.0)
#define CURSOR_FORCE_LIMIT	((MeReal)20000.0)

// #define CURSOR_TEXTURE	/* use a texture for the cursor */
// #define CURSOR_POWER_TEXT	/* show power as text value */
#define CURSOR_MOUSE_FORCE	/* all cursor mouse force */
// #define CURSOR_USE_KEYBOARD	/* use keyboard for cursor movement */
#define CURSOR_USE_MOUSE	/* use mouse for cursor movement */

/*
 * How to move a cursor.
 */
typedef struct cursor_movement {
	int		t_index;	/* MeVector3 index to translate */
	int		r_index;	/* MeVector3 index to rotate */
	int		p_index;	/* change power if 0 */
	MeReal		amount;		/* amount to move by */
} cursor_movement_t;

#define CURSOR_UP		(0)
#define CURSOR_DOWN		(1)
#define CURSOR_LEFT		(2)
#define CURSOR_RIGHT		(3)
#define CURSOR_CCW		(4)
#define CURSOR_CW		(5)
#define CURSOR_POWER_UP		(6)
#define CURSOR_POWER_DOWN	(7)

#define NCURSOR_MOVEMENTS	(8)

/*
 * Globals.
 */
extern cursor_movement_t	cursor_movement[NCURSOR_MOVEMENTS];
extern int			cursor_holding_ball;

/*
 * Prototypes.
 */
void cursor_init(void);
void cursor_finish(void);
void cursor_tick(void);
void cursor_throw_ball(void);
void cursor_grab_ball(void);
void cursor_reset(void);
void MEAPI_STDCALL cursor_move_keyboard(RRender *, cursor_movement_t *);
void MEAPI cursor_move_mouse(RRender *, int, int, int, RMouseButtonWhich,
			     RMouseButtonEvent, void *);

#endif /* _CURSOR_H_ */
