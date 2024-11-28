/*
 * $Id: Viewer.c,v 1.2 2001/11/22 17:51:36 harveyt Exp $
 */
#include "MeMath.h"
#include "MeViewer.h"
#include "MeApp.h"
#include "Mst.h"

#include "BarSkittles.h"
#include "Universe.h"
#include "Viewer.h"
#include "Game.h"
#include "Cursor.h"

/*
 * Globals.
 */
RRender		*rc;
MeApp		*meapp;
MeVector4	color_white = { 1, 1, 1, 1 };
MeVector4	color_green = { 0, 1, 0, 1 };
MeVector4	color_blue = { 0, 0, 1, 1 };

/*
 * Viewer information.
 */
MeVector3	camera_lookat = { 0, 0, 20 };
MeVector3	camera_position = { 0, -75, -25 };
char		*help[] = {
#ifdef CURSOR_USE_KEYBOARD
	"$UP, $DOWN, $LEFT, $RIGHT - move start cursor",
	"$LEFT2, $RIGHT2 - rotate launch direction",
	"$UP2, $DOWN2 - change launch power",
#endif /* CURSOR_USE_KEYBOARD */
	"$ACTION3 - launch ball",
	"$ACTION2 - reset game",
	"$ACTION4 - toggle performance bar",
#ifdef CURSOR_USE_MOUSE
	"Ctrl + Left Mouse - move cursor",
	"Ctrl + Middle Mouse - launch ball",
	"Ctrl + Right Mouse - direct/power cursor",
#endif
#ifdef CURSOR_MOUSE_FORCE
	"Shift + Mouse - force move",
#endif

};
#define NHELP	(sizeof(help) / sizeof(char *))

/*
 * Delete the performance bar.
 */
void
RPerformanceBarDelete(RRender *	rc)
{
	if (!rc->m_PerformanceBar)
		return;
	RGraphicDelete(rc, rc->m_PerformanceBar->m_ColBar, 1);
	RGraphicDelete(rc, rc->m_PerformanceBar->m_DynBar, 1);
	RGraphicDelete(rc, rc->m_PerformanceBar->m_RenBar, 1);
	RGraphicDelete(rc, rc->m_PerformanceBar->m_IdleBar, 1);
	RGraphicDelete(rc, rc->m_PerformanceBar->m_Ticks, 1);
	RGraphicDelete(rc, rc->m_PerformanceBar->m_ColText, 1);
	RGraphicDelete(rc, rc->m_PerformanceBar->m_DynText, 1);
	RGraphicDelete(rc, rc->m_PerformanceBar->m_RenText, 1);
	RGraphicDelete(rc, rc->m_PerformanceBar->m_Scale0, 1);
	RGraphicDelete(rc, rc->m_PerformanceBar->m_Scale4, 1);
	RGraphicDelete(rc, rc->m_PerformanceBar->m_Scale8, 1);
	RGraphicDelete(rc, rc->m_PerformanceBar->m_Scale12, 1);
	RGraphicDelete(rc, rc->m_PerformanceBar->m_Scale16, 1);
	MeMemoryAPI.destroy(rc->m_PerformanceBar);
	rc->m_PerformanceBar = NULL;
}

/*
 * Toggle the performance bar.
 */
void MEAPI
perfbar(RRender *	rc,
	void *		userData)
{
	if (rc->m_PerformanceBar)
		RPerformanceBarDelete(rc);
	else
		RPerformanceBarCreate(rc);
}

/*
 * Initialise the view.
 */
void
viewer_init()
{
	meapp = MeAppCreateFromUniverse(universe, rc);

	RCameraSetLookAtAndPosition(rc, camera_lookat, camera_position);
	RCameraUpdate(rc);
	RRenderSetActionNCallBack(rc, 4, perfbar, 0);
	RRenderSetActionNCallBack(rc, 3, launch, 0);
	RRenderSetActionNCallBack(rc, 2, reset, 0);
	RRenderSetWindowTitle(rc, "Bar Skittles");
	RRenderCreateUserHelp(rc, help, NHELP);
	RRenderToggleUserHelp(rc);
	RRenderSetMouseCallBack(rc, cursor_move_mouse, (void*)meapp);

	/*
	 * Set up the lighting.
	 */
	rc->m_rgbAmbientLightColor[0]
		= rc->m_rgbAmbientLightColor[1]
		= rc->m_rgbAmbientLightColor[2] = (AcmeReal)0.3;

	RLightSwitchOn(rc, kRDirect1);
	rc->m_DirectLight1.m_Direction[0]
		= rc->m_DirectLight1.m_Direction[1]
		= rc->m_DirectLight1.m_Direction[2] = (AcmeReal)-0.5;
	rc->m_DirectLight1.m_rgbAmbient[0]
		= rc->m_DirectLight1.m_rgbAmbient[1]
		= rc->m_DirectLight1.m_rgbAmbient[2] = (AcmeReal)0.0;
	rc->m_DirectLight1.m_rgbDiffuse[0]
		= rc->m_DirectLight1.m_rgbDiffuse[1]
		= rc->m_DirectLight1.m_rgbDiffuse[2] = (AcmeReal)1.0;
	rc->m_DirectLight1.m_rgbSpecular[0]
		= rc->m_DirectLight1.m_rgbSpecular[1]
		= rc->m_DirectLight1.m_rgbSpecular[2] = (AcmeReal)1.0;
}

/*
 * Finish the viewer.
 */
void
viewer_finish()
{
	MeAppDestroy(meapp);
	RRenderContextDestroy(rc);
}

void
viewer_tick()
{
	MeAppStep(meapp);
}
